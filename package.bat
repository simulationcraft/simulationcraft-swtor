
REM This script is used for packaging a release built using MINGW
REM Update the "dir" and "rev" variables appropriately

set dir=simc-swtor-114-3
set tag=release_114-3

call git pull
call git checkout %tag%
rmdir /Q /S %dir%
call git clone ./ %dir%

rmdir /Q /S "%dir%/.git"
move /Y "%dir%/engine" "%dir%/source/engine"

xcopy "engine/simc.exe" %dir% /F
xcopy "release/SimulationCraft.exe" %dir% /F

call git checkout master