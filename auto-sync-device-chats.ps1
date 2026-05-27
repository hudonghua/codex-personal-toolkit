param(
    [string]$DeviceName = "联想电脑",
    [int]$IntervalSeconds = 300,
    [int]$CooldownSeconds = 120
)

$ErrorActionPreference = "Stop"

$Repo = Split-Path -Parent $MyInvocation.MyCommand.Path
$CodexHome = Join-Path $env:USERPROFILE ".codex"
$SessionsRoot = Join-Path $CodexHome "sessions"
$SessionIndex = Join-Path $CodexHome "session_index.jsonl"
$SyncScript = Join-Path $Repo "sync-device-chats.ps1"
$StateFile = Join-Path $CodexHome "auto-chat-sync-state.json"
$LogFile = Join-Path $CodexHome "auto-chat-sync.log"

function Write-Log {
    param([string]$Message)
    $line = "$(Get-Date -Format s) $Message"
    Add-Content -LiteralPath $LogFile -Value $line -Encoding UTF8
}

function Get-ChatState {
    $latest = 0
    $count = 0

    if (Test-Path -LiteralPath $SessionIndex) {
        $item = Get-Item -LiteralPath $SessionIndex
        $latest = [Math]::Max($latest, [int64]$item.LastWriteTimeUtc.Ticks)
        $count++
    }

    if (Test-Path -LiteralPath $SessionsRoot) {
        $files = Get-ChildItem -LiteralPath $SessionsRoot -Recurse -File -Filter "*.jsonl" -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -notmatch "\.bak" }
        foreach ($file in $files) {
            $latest = [Math]::Max($latest, [int64]$file.LastWriteTimeUtc.Ticks)
            $count++
        }
    }

    return "$latest/$count"
}

function Read-LastSyncedState {
    if (-not (Test-Path -LiteralPath $StateFile)) {
        return ""
    }
    try {
        $state = Get-Content -LiteralPath $StateFile -Raw -Encoding UTF8 | ConvertFrom-Json
        return [string]$state.chat_state
    } catch {
        return ""
    }
}

function Write-LastSyncedState {
    param([string]$ChatState)
    $payload = @{
        device = $DeviceName
        chat_state = $ChatState
        synced_at = (Get-Date).ToString("s")
    } | ConvertTo-Json -Compress
    Set-Content -LiteralPath $StateFile -Value $payload -Encoding UTF8
}

Write-Log "auto sync started device=$DeviceName interval=$IntervalSeconds cooldown=$CooldownSeconds"

while ($true) {
    try {
        $current = Get-ChatState
        $last = Read-LastSyncedState
        if ($current -and $current -ne $last) {
            Write-Log "change detected state=$current waiting cooldown"
            Start-Sleep -Seconds $CooldownSeconds
            $afterCooldown = Get-ChatState
            powershell -NoProfile -ExecutionPolicy Bypass -File $SyncScript -DeviceName $DeviceName
            if ($LASTEXITCODE -eq 0) {
                Write-LastSyncedState -ChatState $afterCooldown
                Write-Log "sync ok state=$afterCooldown"
            } else {
                Write-Log "sync failed exit=$LASTEXITCODE"
            }
        }
    } catch {
        Write-Log "sync exception $($_.Exception.Message)"
    }

    Start-Sleep -Seconds $IntervalSeconds
}
