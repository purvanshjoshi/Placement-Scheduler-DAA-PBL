@echo off
REM Compile all C++ source files for the Placement Scheduler
REM Requires g++ (MinGW) to be installed and in PATH

echo Compiling C++ source files...

REM Create output directory if it doesn't exist
if not exist "..\cpp_executables" mkdir "..\cpp_executables"

REM Compile company_manager
echo Compiling company_manager.cpp...
g++ -o ..\cpp_executables\company_manager.exe company_manager.cpp -std=c++11
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile company_manager.cpp
    pause
    exit /b 1
)

REM Compile student_manager
echo Compiling student_manager.cpp...
g++ -o ..\cpp_executables\student_manager.exe student_manager.cpp -std=c++11
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile student_manager.cpp
    pause
    exit /b 1
)

REM Compile worksheet_manager
echo Compiling worksheet_manager.cpp...
g++ -o ..\cpp_executables\worksheet_manager.exe worksheet_manager.cpp -std=c++11
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile worksheet_manager.cpp
    pause
    exit /b 1
)

REM Compile interview_manager
echo Compiling interview_manager.cpp...
g++ -o ..\cpp_executables\interview_manager.exe interview_manager.cpp -std=c++11
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile interview_manager.cpp
    pause
    exit /b 1
)

echo.
echo ========================================
echo All files compiled successfully!
echo Executables are in: ..\cpp_executables\
echo ========================================
pause
