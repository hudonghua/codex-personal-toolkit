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

    if (Test-Path -LiteralPath $ConfigPath) {
        $config = Get-Content -LiteralPath $ConfigPath -Raw
        Copy-Item -LiteralPath $ConfigPath -Destination "$ConfigPath.bak-personal-toolkit" -Force
    }
    else {
        $config = ""
    }

    $config = [regex]::Replace(
        $config,
        "(?ms)^\\[mcp_servers\\.codex_memory\\].*?(?=^\\[mcp_servers\\.|^\\[desktop\\]|^\\[features\\]|\\z)",
        ""
    )
    $config = [regex]::Replace(
        $config,
        "(?ms)^\\[mcp_servers\\.codex_sqlite\\].*?(?=^\\[mcp_servers\\.|^\\[desktop\\]|^\\[features\\]|\\z)",
        ""
    )

    $newConfig = $config.TrimEnd() + "`r`n`r`n" + $snippet.Trim() + "`r`n"
    Set-Content -LiteralPath $ConfigPath -Value $newConfig -Encoding UTF8
}

Write-Host "Installed Codex personal toolkit."
Write-Host "Restart Codex to load new skills and MCP servers."
