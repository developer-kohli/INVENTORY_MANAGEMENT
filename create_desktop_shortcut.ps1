$ErrorActionPreference = "Stop"

$targetPath = "D:\INVENTORY_MANAGEMENT\InventoryManagement\inventory_app.exe"
$workingDir = "D:\INVENTORY_MANAGEMENT\InventoryManagement"
$shortcutName = "Inventory Management System.lnk"

if (-not (Test-Path $targetPath)) {
    Write-Error "Target executable not found at $targetPath"
    exit 1
}

$desktopPaths = [System.Collections.Generic.List[string]]::new()

# 1. Registry redirected Desktop (e.g. OneDrive Desktop)
$userShellFolders = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders'
$regDesktop = (Get-ItemProperty $userShellFolders -ErrorAction SilentlyContinue).Desktop
if ($regDesktop) {
    $expandedReg = [System.Environment]::ExpandEnvironmentVariables($regDesktop)
    if (Test-Path $expandedReg) {
        $desktopPaths.Add($expandedReg)
    }
}

# 2. Standard user profile desktop
$userProfileDesktop = Join-Path $env:USERPROFILE "Desktop"
if ((Test-Path $userProfileDesktop) -and (-not $desktopPaths.Contains($userProfileDesktop))) {
    $desktopPaths.Add($userProfileDesktop)
}

# 3. Fallback to Common Desktop if needed
$commonDesktop = [System.Environment]::GetFolderPath([System.Environment+SpecialFolder]::CommonDesktopDirectory)
if ($commonDesktop -and (Test-Path $commonDesktop) -and (-not $desktopPaths.Contains($commonDesktop))) {
    # Only as fallback if no user desktop found
    if ($desktopPaths.Count -eq 0) {
        $desktopPaths.Add($commonDesktop)
    }
}

if ($desktopPaths.Count -eq 0) {
    Write-Error "Could not resolve a valid Desktop path."
    exit 1
}

$wshShell = New-Object -ComObject WScript.Shell

foreach ($dPath in $desktopPaths) {
    $shortcutFile = Join-Path $dPath $shortcutName
    $shortcut = $wshShell.CreateShortcut($shortcutFile)
    $shortcut.TargetPath = $targetPath
    $shortcut.WorkingDirectory = $workingDir
    $shortcut.Description = "Inventory Management System"
    $shortcut.IconLocation = "$targetPath,0"
    $shortcut.Save()
    Write-Host "Successfully created desktop shortcut at: $shortcutFile"
}

