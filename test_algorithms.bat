@echo off
REM Test script to demonstrate Merge Sort and Binary Search algorithms
REM in the Placement Management System

echo ========================================
echo Testing Algorithm Implementations
echo ========================================
echo.

echo [TEST 1] List All Students - Sorted by Roll Number (Merge Sort)
echo Command: backend\cpp_executables\student_manager.exe backend\data\students.json list
echo.
backend\cpp_executables\student_manager.exe backend\data\students.json list
echo.
echo ----------------------------------------
echo.

echo [TEST 2] Binary Search - Exact Roll Number "2418721"
echo Command: backend\cpp_executables\student_manager.exe backend\data\students.json search 2418721
echo.
backend\cpp_executables\student_manager.exe backend\data\students.json search 2418721
echo.
echo ----------------------------------------
echo.

echo [TEST 3] Binary Search - Exact Roll Number "2418722"
echo Command: backend\cpp_executables\student_manager.exe backend\data\students.json search 2418722
echo.
backend\cpp_executables\student_manager.exe backend\data\students.json search 2418722
echo.
echo ----------------------------------------
echo.

echo [TEST 4] Partial Search - Substring "24187" (Fallback to Linear)
echo Command: backend\cpp_executables\student_manager.exe backend\data\students.json search 24187
echo.
backend\cpp_executables\student_manager.exe backend\data\students.json search 24187
echo.
echo ----------------------------------------
echo.

echo [TEST 5] Search Non-existent Roll Number "9999999"
echo Command: backend\cpp_executables\student_manager.exe backend\data\students.json search 9999999
echo.
backend\cpp_executables\student_manager.exe backend\data\students.json search 9999999
echo.
echo ----------------------------------------
echo.

echo ========================================
echo Algorithm Tests Completed!
echo ========================================
echo.
echo Summary:
echo - Test 1: Demonstrates Merge Sort (students sorted by roll number)
echo - Test 2 ^& 3: Demonstrate Binary Search (exact match)
echo - Test 4: Demonstrates partial match fallback (linear search)
echo - Test 5: Demonstrates not found case
echo.
echo For detailed algorithm documentation, see ALGORITHMS.md
echo.
pause
