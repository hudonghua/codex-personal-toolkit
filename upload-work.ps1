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

$ArchivePath = & $Python (Join-Path $Repo "scripts\export_work_state.py") --repo $Repo
if ($LASTEXITCODE -ne 0) {
    throw "Work-state export failed."
}

Set-Location $Repo
& $Git add work-states skills scripts upload-work.ps1 README.md
if ($LASTEXITCODE -ne 0) { throw "git add failed." }

$ArchiveName = Split-Path $ArchivePath -Leaf
& $Git commit -m "Upload work state $ArchiveName"
if ($LASTEXITCODE -ne 0) { throw "git commit failed." }

& $Git push
if ($LASTEXITCODE -ne 0) { throw "git push failed." }

Write-Host $ArchivePath
