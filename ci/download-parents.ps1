$ErrorActionPreference = 'Stop'
if (-not $env:PARENT_ARTIFACTS) { throw 'Successful prerequisite artifacts are required' }
foreach ($id in $env:PARENT_ARTIFACTS.Split(',')) {
    if ($id -notmatch '^\d+$') { throw 'Expected numeric artifact IDs' }
    $zip = "$env:RUNNER_TEMP\parent-$id.zip"
    cmd /c "gh api repos/ClayWarren/librsvg-feedstock/actions/artifacts/$id/zip > $zip"
    if ($LASTEXITCODE) { throw 'Parent artifact download failed' }
    $dest = "$env:RUNNER_TEMP\parent-$id"
    Expand-Archive $zip $dest
    $manifest = Join-Path $dest sha256.json
    if (-not (Test-Path $manifest)) { throw "No successful build manifest for $id" }
    foreach ($item in (Get-Content $manifest -Raw | ConvertFrom-Json)) {
        $path = Join-Path $dest $item.file
        if ((Get-FileHash $path -Algorithm SHA256).Hash.ToLower() -ne $item.sha256) { throw "Wrong hash: $path" }
        Write-Host "Verified parent $($item.file): $($item.sha256)"
        Copy-Item $path C:\rsvg-local\win-arm64 -Force
    }
}
