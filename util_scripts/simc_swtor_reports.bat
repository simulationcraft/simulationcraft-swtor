set simc_path=..
set iterations=1000
set calculate_scale_factors=1
set threads=4
set center_scale_delta=1

set simulation1=Sage_Sorcerer_Raid
set simulation1_options=

call %simc_path%\engine\simc.exe %simulation1%.simc %simulation1_options% iterations=%iterations% calculate_scale_factors=%calculate_scale_factors% center_scale_delta=%center_scale_delta% threads=%threads% html=%simc_path%\%simulation1%%simulation1_options%.html

set simulation1=Sage_Sorcerer_Raid
set simulation1_options="fight_style=HelterSkelter"

call %simc_path%\engine\simc.exe %simulation1%.simc %simulation1_options% iterations=%iterations% calculate_scale_factors=%calculate_scale_factors% center_scale_delta=%center_scale_delta% threads=%threads% html=%simc_path%\%simulation1%%simulation1_options%.html

set simulation1=Sage_Sorcerer_Raid
set simulation1_options="fight_style=HeavyMovement"

call %simc_path%\engine\simc.exe %simulation1%.simc %simulation1_options% iterations=%iterations% calculate_scale_factors=%calculate_scale_factors% center_scale_delta=%center_scale_delta% threads=%threads% html=%simc_path%\%simulation1%%simulation1_options%.html

set simulation1=Assassin_Raid
set simulation1_options=

call %simc_path%\engine\simc.exe %simulation1%.simc %simulation1_options% iterations=%iterations% calculate_scale_factors=%calculate_scale_factors% center_scale_delta=%center_scale_delta% threads=%threads% html=%simc_path%\%simulation1%%simulation1_options%.html
