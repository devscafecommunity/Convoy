@echo off
set CONFIG=Release
if "%1"=="Debug" set CONFIG=Debug
if "%1"=="debug" set CONFIG=Debug

where cmake >nul 2>&1
if not %errorlevel%==0 set "PATH=C:\Program Files\CMake\bin;%PATH%"

cmake --version >nul 2>&1
if not %errorlevel%==0 (
    echo [ERROR] CMake not found
    pause
    exit /b 1
)

echo.
echo Building Convoy - %CONFIG%
echo.

if not exist "build" mkdir build
cd build

echo [CONFIGURE] Running CMake...
cmake .. -DCMAKE_BUILD_TYPE=%CONFIG% -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md

if errorlevel 1 (
    echo [ERROR] CMake configure failed, wiping cache and retrying...
    cd ..
    rmdir /s /q build 2>nul
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=%CONFIG% -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static-md
    if errorlevel 1 (
        echo [ERROR] CMake configure failed
        pause
        exit /b 1
    )
)

echo [BUILD] Compiling...
cmake --build . --config %CONFIG% --parallel

if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build complete!
echo Executable: build\Release\Convoy.exe
pause