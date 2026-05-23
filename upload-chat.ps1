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

$ArchivePath = & $Python (Join-Path $Repo "scripts\export_chat_archive.py") --repo $Repo
if ($LASTEXITCODE -ne 0) {
    throw "Chat export failed."
}

Set-Location $Repo
& $Git add chat-archives
if ($LASTEXITCODE -ne 0) { throw "git add failed." }

$ArchiveName = Split-Path $ArchivePath -Leaf
& $Git commit -m "Upload chat transcript $ArchiveName"
if ($LASTEXITCODE -ne 0) { throw "git commit failed." }

& $Git push
if ($LASTEXITCODE -ne 0) { throw "git push failed." }

Write-Host $ArchivePath
