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
if ($LASTEXITCODE -ne 0) { throw "git pull failed." }

& $Python (Join-Path $Repo "scripts\sync_device_share.py") --repo $Repo --device-name "dell电脑" --peer-device "联想电脑"
if ($LASTEXITCODE -ne 0) { throw "Device share sync failed." }

& $Git add "dell电脑" "shared" "skills" "scripts\sync_device_share.py" "sync-device-share.ps1" "README.md"
if ($LASTEXITCODE -ne 0) { throw "git add failed." }

& $Git commit -m "Sync dell computer shared records"
if ($LASTEXITCODE -ne 0) {
    Write-Host "No commit created, possibly no changes."
}
else {
    & $Git push
    if ($LASTEXITCODE -ne 0) { throw "git push failed." }
}
