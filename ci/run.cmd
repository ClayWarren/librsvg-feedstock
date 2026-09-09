@echo on
set "CONDA_SUBDIR=win-64"
if not exist C:\rsvg-tools\python.exe (
    "%RUNNER_TEMP%\micromamba.exe" create -y -p C:\rsvg-tools -c conda-forge conda-build conda-index rattler-build
    if errorlevel 1 exit /b 1
)
call C:\rsvg-tools\condabin\conda.bat activate C:\rsvg-tools
if errorlevel 1 exit /b 1
python ci\build.py %1 %2
if errorlevel 1 exit /b 1
