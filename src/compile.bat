@echo off
SET MSYS2_PATH_TYPE=inherit

echo Compiling %1...

C:\msys64\ucrt64.exe bash -lc "gcc -v -g '%1' -o '%~n1.exe' -lglut -lopengl32 -lglu32" 2>&1
if %errorlevel% neq 0 (
    echo Compilation failed with error code %errorlevel%
    exit /b %errorlevel%
)

echo Checking for executable...
if exist "%~n1.exe" (
    echo Executable created successfully: %~n1.exe
) else (
    echo Failed to create executable %~n1.exe
)

echo Compilation process completed.