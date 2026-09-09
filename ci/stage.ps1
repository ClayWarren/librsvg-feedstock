param([string]$Subdir)
New-Item -ItemType Directory -Path native-results -Force | Out-Null
Get-ChildItem "C:\rsvg-local\$Subdir\*.conda", "C:\rsvg-local\$Subdir\sha256.json" -ErrorAction SilentlyContinue | Copy-Item -Destination native-results
$logs = @()
if (Test-Path C:\rsvg-build) {
    $logs = @(Get-ChildItem C:\rsvg-build -Recurse -Filter testlog.txt -ErrorAction SilentlyContinue)
}
for ($i = 0; $i -lt $logs.Count; $i++) {
    Copy-Item $logs[$i].FullName "native-results\testlog-$i.txt"
}
