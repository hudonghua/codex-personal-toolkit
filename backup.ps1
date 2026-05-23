param(
    [switch]$IncludeData
)

$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$CodexHome = Join-Path $env:USERPROFILE ".codex"

function Reset-Copy {
    param(
        [string]$Source,
        [string]$Destination
    )

    if (Test-Path -LiteralPath $Destination) {
        Remove-Item -LiteralPath $Destination -Recurse -Force
    }
    New-Item -ItemType Directory -Path $Destination -Force | Out-Null

    if (Test-Path -LiteralPath $Source) {
        Get-ChildItem -LiteralPath $Source -Force | ForEach-Object {
            if ($_.Name -eq ".system") {
                return
            }
            Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $Destination $_.Name) -Recurse -Force
        }
    }
}

Reset-Copy -Source (Join-Path $CodexHome "skills") -Destination (Join-Path $RepoRoot "skills")
Reset-Copy -Source (Join-Path $CodexHome "mcp_servers") -Destination (Join-Path $RepoRoot "mcp_servers")

if ($IncludeData) {
    Reset-Copy -Source (Join-Path $CodexHome "mcp_data") -Destination (Join-Path $RepoRoot "records")
}

Write-Host "Backed up Codex personal toolkit."
if (-not $IncludeData) {
    Write-Host "Skipped mcp_data. Run .\\backup.ps1 -IncludeData if you intentionally want to sync local records."
}
