param(
    [string]$DeviceName = "联想电脑"
)

$ErrorActionPreference = "Stop"

$Repo = Split-Path -Parent $MyInvocation.MyCommand.Path
$Python = Join-Path $env:USERPROFILE ".cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe"
$Git = Join-Path $env:USERPROFILE ".codex\tools\mingit\cmd\git.exe"

if (-not (Test-Path -LiteralPath $Python)) {
    $Python = "python"
}
if (-not (Test-Path -LiteralPath $Git)) {
    $Git = "git"
}

Set-Location -LiteralPath $Repo

& $Git pull --ff-only
if ($LASTEXITCODE -ne 0) { throw "git pull failed." }

& $Python (Join-Path $Repo "scripts\export_sidebar_chats.py") --repo $Repo --device-name $DeviceName
if ($LASTEXITCODE -ne 0) { throw "chat export failed." }

$DevicePath = Join-Path $Repo $DeviceName
$SecretScan = & $Python -c @"
import pathlib, re, sys
root = pathlib.Path(r'''$DevicePath''')
patterns = [
    re.compile(r'sk-[A-Za-z0-9_-]{12,}'),
    re.compile(r'ghp_[A-Za-z0-9_]{20,}'),
    re.compile(r'github_pat_[A-Za-z0-9_]{20,}'),
    re.compile(r'eyJ[A-Za-z0-9_-]{20,}\.[A-Za-z0-9_-]{20,}\.[A-Za-z0-9_-]{20,}'),
]
bad = []
for path in root.rglob('*'):
    if not path.is_file():
        continue
    text = path.read_text(encoding='utf-8', errors='ignore')
    for pattern in patterns:
        for match in pattern.finditer(text):
            if 'REDACTED' not in match.group(0):
                bad.append(str(path))
if bad:
    print('\n'.join(sorted(set(bad))))
    sys.exit(1)
print('secret scan ok')
"@
if ($LASTEXITCODE -ne 0) {
    Write-Host $SecretScan
    throw "secret scan failed."
}

& $Git add scripts/export_sidebar_chats.py sync-device-chats.ps1 $DeviceName
if ($LASTEXITCODE -ne 0) { throw "git add failed." }

$Status = & $Git status --porcelain
if (-not $Status) {
    Write-Host "No chat changes to sync for $DeviceName."
    exit 0
}

$Stamp = Get-Date -Format "yyyyMMdd-HHmmss"
& $Git commit -m "Sync $DeviceName Codex chat records $Stamp"
if ($LASTEXITCODE -ne 0) { throw "git commit failed." }

& $Git push
if ($LASTEXITCODE -ne 0) { throw "git push failed." }
