@echo off
REM Rhea Type Registry Scanner
REM Scans the project for RECLASS macros and generates type_registry.json

setlocal

set PROJECT_ROOT=%~dp0..
set CACHE_DIR=%PROJECT_ROOT%\.rhea\cache
set OUTPUT_FILE=%CACHE_DIR%\type_registry.json
set PYTHON_SCRIPT=%PROJECT_ROOT%\tools\scan_types.py

echo ========================================
echo Rhea Type Registry Scanner
echo ========================================
echo.
echo Project Root: %PROJECT_ROOT%
echo Cache Dir: %CACHE_DIR%
echo Output: %OUTPUT_FILE%
echo.

REM Check if Python is available
python --version >nul 2>&1
if errorlevel 1 (
	echo ERROR: Python not found in PATH
	echo Please install Python 3.6+ or add it to PATH
	pause
	exit /b 1
)

REM Check if scan script exists
if not exist "%PYTHON_SCRIPT%" (
	echo ERROR: Scanner script not found: %PYTHON_SCRIPT%
	pause
	exit /b 1
)

REM Create cache directory if it doesn't exist
if not exist "%CACHE_DIR%" (
	echo Creating cache directory...
	mkdir "%CACHE_DIR%"
)

REM Run the scanner
echo Running type scanner...
echo.
python "%PYTHON_SCRIPT%" "%PROJECT_ROOT%" "%OUTPUT_FILE%"

if errorlevel 1 (
	echo.
	echo ERROR: Scanner failed
	pause
	exit /b 1
)

echo.
echo ========================================
echo Type registry updated successfully!
echo ========================================
echo.
echo Cache file: %OUTPUT_FILE%
echo.

REM Optional: Show statistics
if exist "%OUTPUT_FILE%" (
	for %%A in ("%OUTPUT_FILE%") do (
		echo File size: %%~zA bytes
	)
)

pause
endlocal
