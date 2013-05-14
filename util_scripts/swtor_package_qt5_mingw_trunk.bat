:: Small SimC release script created by philoptik@gmail.com
:: It sits in my /git folder, above the simulationcraft folder
:: After tagging for a release, I edit the variables 'version'
:: and 'rev' and execute it.

IF (%1) == () exit /b

set version=%1

:: REQUIRES: qmake, make, g++ in your PATH, as well as a archiver

set target_dir=simc_swtor-%version%
set target_source_dir=simc_swtor-%version%-source
set build_dir=simc_swtor-%version%-build
set tag=release-%version%
set MAKE=mingw32-make
set QMAKE=qmake
set ARCHIVER=7z a

set QMAKE_PATH=C:\Qt\Qt5.0.2\5.0.2\mingw47_32\bin
set GCC_PATH=C:\Qt\Qt5.0.2\Tools\MinGW\bin

set PATH=%QMAKE_PATH%;%GCC_PATH%;%PATH%

call git clone https://simulationcraft-swtor.googlecode.com/git/ %target_dir%/source

:: Create source archive
xcopy /E /I /H %target_dir%\source %target_source_dir%
rd /S /Q %target_source_dir%\.svn
call %ARCHIVER% simc_swtor-%version%-src.zip %target_source_dir%

:: copy things over to the temporary build dir, and build cli + gui there
xcopy /E /I /H %target_dir%\source %build_dir%
cd %build_dir%
cd engine
%MAKE% -j4 NO_DEBUG=1
cd ..
%QMAKE% simcqt.pro
%MAKE% -j4 release
cd ..

xcopy /E /I /H %target_dir%\source\profiles %target_dir%\profiles
rd /Q /S %target_dir%\source\profiles\

move %target_dir%\source\Examples.simc %target_dir%\
move %target_dir%\source\Legend.html %target_dir%\
move %target_dir%\source\READ_ME_FIRST.txt %target_dir%\
move %target_dir%\source\Welcome.html %target_dir%\
move %target_dir%\source\SimC_Logo.png %target_dir%\


xcopy %target_dir%\source\mingw_qt5_dll_setup.bat %target_dir%\
cd %target_dir%
call mingw_qt5_dll_setup.bat
cd ..
del %target_dir%\mingw_qt5_dll_setup.bat

del /F /Q /S %target_dir%\source\
rd /Q /S %target_dir%\source\

xcopy %build_dir%\engine\simc.exe %target_dir%\
xcopy %build_dir%\release\SimulationCraft.exe %target_dir%\
xcopy Win32OpenSSL_Light-1_0_1c.exe %target_dir%\

call %ARCHIVER% simc_swtor-%version%-win32.zip %target_dir%

:: remove tmp build dir
rd /Q /S %build_dir%

