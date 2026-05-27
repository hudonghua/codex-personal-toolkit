$ErrorActionPreference = "Stop"

$Repo = Split-Path -Parent $MyInvocation.MyCommand.Path
$Python = Join-Path $env:USERPROFILE ".cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe"
$BundledGit = Join-Path $env:USERPROFILE ".codex\tools\mingit\cmd\git.exe"

function Resolve-Git {
    $systemGit = Get-Command git -ErrorAction SilentlyContinue
    if ($systemGit) {
        return $systemGit.Source
    }
    if (Test-Path -LiteralPath $BundledGit) {
        return $BundledGit
    }
    throw "Git not found in PATH and bundled Git not found: $BundledGit"
}

function Get-GitOutput {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Args
    )

    $result = & $Git @Args
    if ($LASTEXITCODE -ne 0) {
        throw ("git {0} failed." -f ($Args -join " "))
    }
    return ($result | Out-String).Trim()
}

function Get-HasWorkingTreeChanges {
    $status = & $Git status --porcelain
    if ($LASTEXITCODE -ne 0) { throw "git status --porcelain failed." }
    return [bool]($status | Select-Object -First 1)
}

function Resolve-DeviceDirectories {
    $deviceDirs = Get-ChildItem -LiteralPath $Repo -Directory | Where-Object {
        $_.Name -like "*电脑" -or $_.Name -like "*鐢佃剳"
    }
    $deviceDir = $deviceDirs | Where-Object { $_.Name -match "^[Dd]ell" } | Select-Object -First 1
    if (-not $deviceDir) {
        throw "Could not find local dell device directory in repo."
    }
    $peerDir = $deviceDirs | Where-Object { $_.FullName -ne $deviceDir.FullName } | Select-Object -First 1
    if (-not $peerDir) {
        throw "Could not find peer device directory in repo."
    }
    return @{
        DeviceName = $deviceDir.Name
        PeerDeviceName = $peerDir.Name
    }
}

if (-not (Test-Path -LiteralPath $Python)) {
    throw "Bundled Codex Python not found: $Python"
}

$Git = Resolve-Git
$devices = Resolve-DeviceDirectories

Set-Location $Repo
& $Git fetch origin
if ($LASTEXITCODE -ne 0) { throw "git fetch failed." }

$localHead = Get-GitOutput -Args @("rev-parse", "HEAD")
$remoteHead = Get-GitOutput -Args @("rev-parse", "origin/master")
$mergeBase = Get-GitOutput -Args @("merge-base", "HEAD", "origin/master")
$autoStashed = $false

if ($localHead -ne $remoteHead) {
    if ($mergeBase -eq $remoteHead) {
        Write-Host "Remote has no new commits; local branch is ahead."
    }
    else {
        if (Get-HasWorkingTreeChanges) {
            Write-Host "Working tree has local changes. Stashing before rebase."
            & $Git stash push --include-untracked -m "sync-device-share auto-stash"
            if ($LASTEXITCODE -ne 0) { throw "git stash push failed." }
            $autoStashed = $true
        }
        Write-Host "Rebasing current branch onto origin/master."
        & $Git rebase origin/master
        if ($LASTEXITCODE -ne 0) { throw "git rebase origin/master failed." }
        if ($autoStashed) {
            Write-Host "Restoring stashed changes after rebase."
            & $Git stash pop
            if ($LASTEXITCODE -ne 0) { throw "git stash pop failed after rebase. Resolve conflicts manually." }
        }
    }
}
else {
    Write-Host "Local branch already matches origin/master."
}

& $Python (Join-Path $Repo "scripts\sync_device_share.py") --repo $Repo --device-name $devices.DeviceName --peer-device $devices.PeerDeviceName
if ($LASTEXITCODE -ne 0) { throw "Device share sync failed." }

& $Git add $devices.DeviceName "shared" "skills" "scripts\sync_device_share.py" "sync-device-share.ps1" "README.md"
if ($LASTEXITCODE -ne 0) { throw "git add failed." }

& $Git commit -m "Sync dell computer shared records"
if ($LASTEXITCODE -ne 0) {
    Write-Host "No commit created, possibly no changes."
}
else {
    & $Git push origin HEAD:master
    if ($LASTEXITCODE -ne 0) { throw "git push failed." }
}
