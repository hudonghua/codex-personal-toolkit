param(
    [Parameter(Mandatory=$true)]
    [string]$Incoming
)

$ErrorActionPreference = "Stop"

$Repo = Split-Path -Parent $MyInvocation.MyCommand.Path
$Python = Join-Path $env:USERPROFILE ".cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe"
$Git = Join-Path $env:USERPROFILE ".codex\tools\mingit\cmd\git.exe"

if (-not (Test-Path -LiteralPath $Python)) {
    throw "Bundled Codex Python not found: $Python"
}
if (-not (Test-Path -LiteralPath $Git)) {
    $Git = "git"
}

Set-Location $Repo
& $Git pull --ff-only

$Report = & $Python (Join-Path $Repo "scripts\merge_toolkit.py") --repo $Repo --incoming $Incoming
if ($LASTEXITCODE -ne 0) {
    throw "Merge failed."
}

& $Git add skills mcp_servers mcp_config_snippets work-states records merge-reports scripts merge-toolkit.ps1
& $Git commit -m "Merge toolkit from another computer"
if ($LASTEXITCODE -ne 0) {
    Write-Host "No merge commit created, possibly no changes."
}
else {
    & $Git push
}

Write-Host "Merge report: $Report"
