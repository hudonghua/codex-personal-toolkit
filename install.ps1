param(
    [switch]$IncludeData
)

$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$CodexHome = Join-Path $env:USERPROFILE ".codex"
$SkillsRoot = Join-Path $CodexHome "skills"
$McpServersRoot = Join-Path $CodexHome "mcp_servers"
$McpDataRoot = Join-Path $CodexHome "mcp_data"
$ConfigPath = Join-Path $CodexHome "config.toml"

function Copy-DirectoryContents {
    param(
        [string]$Source,
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source)) {
        return
    }

    New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    Get-ChildItem -LiteralPath $Source -Force | ForEach-Object {
        $target = Join-Path $Destination $_.Name
        if ($_.PSIsContainer) {
            if (Test-Path -LiteralPath $target) {
                Remove-Item -LiteralPath $target -Recurse -Force
            }
            Copy-Item -LiteralPath $_.FullName -Destination $target -Recurse -Force
        }
        else {
            Copy-Item -LiteralPath $_.FullName -Destination $target -Force
        }
    }
}

function Write-Utf8NoBom {
    param(
        [string]$Path,
        [string]$Text
    )

    $encoding = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($Path, $Text, $encoding)
}

function Remove-McpServerBlocks {
    param(
        [string]$Content,
        [string[]]$ServerNames
    )

    if ([string]::IsNullOrWhiteSpace($Content)) {
        return ""
    }

    $lines = $Content -split "\r?\n", -1
    $kept = New-Object System.Collections.Generic.List[string]
    $skip = $false

    foreach ($line in $lines) {
        if ($line -match '^\s*\[(?<table>[^\]]+)\]\s*(?:#.*)?$') {
            $table = $Matches["table"].Trim()
            $normalized = $table -replace '"', ''
            $normalized = $normalized -replace "'", ""
            $skip = $false

            foreach ($serverName in $ServerNames) {
                $root = "mcp_servers.$serverName"
                if ($normalized -ieq $root -or $normalized.StartsWith("$root.", [System.StringComparison]::OrdinalIgnoreCase)) {
                    $skip = $true
                    break
                }
            }
        }

        if (-not $skip) {
            $kept.Add($line)
        }
    }

    return (($kept -join "`n").TrimEnd())
}

function Get-PythonCommand {
    $bundledPython = Join-Path $env:USERPROFILE ".cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe"
    if (Test-Path -LiteralPath $bundledPython) {
        return @($bundledPython)
    }

    $python = Get-Command python -ErrorAction SilentlyContinue
    if ($python) {
        return @($python.Source)
    }

    $py = Get-Command py -ErrorAction SilentlyContinue
    if ($py) {
        return @($py.Source, "-3")
    }

    throw "Python not found; cannot validate config.toml safely."
}

function Test-TomlFile {
    param(
        [string]$Path
    )

    $validator = Join-Path ([System.IO.Path]::GetTempPath()) ("codex_toml_validate_{0}.py" -f $PID)
    Write-Utf8NoBom -Path $validator -Text @"
import sys
import tomllib

with open(sys.argv[1], "rb") as f:
    tomllib.load(f)
"@

    try {
        $pythonCommand = @(Get-PythonCommand)
        $exe = $pythonCommand[0]
        $prefixArgs = @()
        if ($pythonCommand.Count -gt 1) {
            $prefixArgs = $pythonCommand[1..($pythonCommand.Count - 1)]
        }

        & $exe @prefixArgs $validator $Path
        if ($LASTEXITCODE -ne 0) {
            throw "TOML validation failed for $Path"
        }
    }
    finally {
        Remove-Item -LiteralPath $validator -Force -ErrorAction SilentlyContinue
    }
}

New-Item -ItemType Directory -Path $CodexHome -Force | Out-Null
Copy-DirectoryContents -Source (Join-Path $RepoRoot "skills") -Destination $SkillsRoot
Copy-DirectoryContents -Source (Join-Path $RepoRoot "mcp_servers") -Destination $McpServersRoot

if ($IncludeData) {
    Copy-DirectoryContents -Source (Join-Path $RepoRoot "records") -Destination $McpDataRoot
}
else {
    New-Item -ItemType Directory -Path $McpDataRoot -Force | Out-Null
}

$SnippetPath = Join-Path $RepoRoot "mcp_config_snippets/memory-sqlite.toml"
if (Test-Path -LiteralPath $SnippetPath) {
    $userName = Split-Path $env:USERPROFILE -Leaf
    $snippet = Get-Content -LiteralPath $SnippetPath -Raw
    $snippet = $snippet.Replace("__USER__", $userName)

    $hadConfig = Test-Path -LiteralPath $ConfigPath
    $timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $backupPath = "$ConfigPath.bak-personal-toolkit-$timestamp"
    $tempPath = "$ConfigPath.tmp-personal-toolkit"

    if ($hadConfig) {
        $config = Get-Content -LiteralPath $ConfigPath -Raw
        Copy-Item -LiteralPath $ConfigPath -Destination $backupPath -Force
    }
    else {
        $config = ""
    }

    try {
        $config = Remove-McpServerBlocks -Content $config -ServerNames @("codex_memory", "codex_sqlite")
        $newConfig = $config.TrimEnd() + "`n`n" + $snippet.Trim() + "`n"

        Write-Utf8NoBom -Path $tempPath -Text $newConfig
        Test-TomlFile -Path $tempPath

        Move-Item -LiteralPath $tempPath -Destination $ConfigPath -Force
        Test-TomlFile -Path $ConfigPath
    }
    catch {
        if ($hadConfig -and (Test-Path -LiteralPath $backupPath)) {
            Copy-Item -LiteralPath $backupPath -Destination $ConfigPath -Force
        }
        elseif (-not $hadConfig) {
            Remove-Item -LiteralPath $ConfigPath -Force -ErrorAction SilentlyContinue
        }

        Remove-Item -LiteralPath $tempPath -Force -ErrorAction SilentlyContinue
        throw
    }
}

Write-Host "Installed Codex personal toolkit."
Write-Host "Restart Codex to load new skills and MCP servers."
