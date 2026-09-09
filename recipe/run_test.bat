@echo on
gdk-pixbuf-query-loaders > test-loaders.cache
if errorlevel 1 exit /b 1
set "GDK_PIXBUF_MODULE_FILE=%CD%\test-loaders.cache"
rsvg-convert --output converted.png test.svg
if errorlevel 1 exit /b 1
cmake -S . -B consumer-build -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build consumer-build
if errorlevel 1 exit /b 1
consumer-build\rsvg-consumer.exe
if errorlevel 1 exit /b 1
set "EXPECTED_MACHINE=8664 machine (x64)"
if "%target_platform%" == "win-arm64" set "EXPECTED_MACHINE=AA64 machine (ARM64)"
for %%F in ("consumer-build\rsvg-consumer.exe" "%LIBRARY_BIN%\rsvg-convert.exe" "%LIBRARY_BIN%\rsvg-2-2.dll" "%LIBRARY_BIN%\rsvg-2.0-vs%VS_MAJOR%.dll" "%LIBRARY_LIB%\gdk-pixbuf-2.0\2.10.0\loaders\libpixbufloader_svg.dll") do (
    dumpbin /headers "%%~F" | findstr /c:"%EXPECTED_MACHINE%"
    if errorlevel 1 exit /b 1
)
