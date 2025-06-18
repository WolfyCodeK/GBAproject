path=%CD%\lib\bin
@ECHO OFF

echo Building GBA Project...

REM Change to source directory
cd src

REM Clean previous build files
if exist main.o del main.o
if exist main.elf del main.elf

REM Compile
echo Compiling main.c...
gcc -c -O3 -mthumb main.c 
if "%errorlevel%"=="1" (
    echo Compilation failed!
    pause
    exit /b 1
)

REM Link
echo Linking...
gcc -mthumb -o main.elf main.o -lm
if "%errorlevel%"=="1" (
    echo Linking failed!
    pause
    exit /b 1
)

REM Convert to GBA format and move to bin
echo Converting to GBA format...
objcopy -O binary main.elf ..\bin\main.gba
if "%errorlevel%"=="1" (
    echo Conversion failed!
    pause
    exit /b 1
)

REM Clean up
del main.o 
del main.elf

REM Return to root
cd ..

echo Build successful! Output: bin\main.gba

REM Launch emulator if available
if exist "tools\VisualBoyAdvance.exe" (
    echo Launching emulator...
    start "" "tools\VisualBoyAdvance.exe" "bin\main.gba"
) else (
    echo Emulator not found. Run: tools\VisualBoyAdvance.exe bin\main.gba
)

pause 