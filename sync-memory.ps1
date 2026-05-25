param([switch]$Quiet)
$ErrorActionPreference = "Stop"
$Repo = "C:\Users\t250c\Documents\Codex\codex-personal-toolkit"
$Git  = "C:\Users\t250c\.codex\tools\mingit\cmd\git.exe"
if (-not (Test-Path $Git)) { $Git = "git" }

Set-Location $Repo
& $Git pull --ff-only --quiet 2>$null | Out-Null

$status = & $Git status --porcelain -- memory
if (-not $status) {
    if (-not $Quiet) { Write-Host "memory: no changes" }
    exit 0
}

& $Git add memory
$ts = Get-Date -Format "yyyyMMdd-HHmmss"
& $Git commit -m "memory sync $ts" --quiet | Out-Null
& $Git push --quiet 2>$null | Out-Null
if (-not $Quiet) { Write-Host "memory: synced $ts" }
