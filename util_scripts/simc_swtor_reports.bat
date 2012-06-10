:: ###########################################
:: #                                         #
:: # A COPY OF simc_swtor_reports.bash       #
:: #                                         #
:: # CHECK FOR ANY CHANGES BEFORE RERUNNING  #
:: #                                         #
:: ###########################################

@echo off
setlocal
set PATH=..\engine;engine;%PATH%
set iterations=100000
set calculate_scale_factors=1
set threads=4
set center_scale_delta=1
set statistics_level=3
:: set ptr=0

call :runsim Sage_Sorcerer_Raid
call :runsim Sage_Sorcerer_Raid -HelterSkelter "fight_style=HelterSkelter"
call :runsim Sage_Sorcerer_Raid -HeavyMovement "fight_style=HeavyMovement"
call :runsim Jedi_Assassin_Raid
REM call :runsim Assassin_Raid -HelterSkelter "fight_style=HelterSkelter"
REM call :runsim Assassin_Raid -HeavyMovement "fight_style=HeavyMovement"
call :runsim Mercenary_Raid
call :runsim Scoundrel_Operative_Raid
call :runsim Gunslinger_Sniper_Raid


goto :eof

:runsim
echo.%1%2
call simc.exe %1.simc output=NUL: %3 statistics_level=%statistics_level% iterations=%iterations% calculate_scale_factors=%calculate_scale_factors% center_scale_delta=%center_scale_delta% threads=%threads% html=%1%2.html
echo.
goto :eof
