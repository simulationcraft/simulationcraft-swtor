###########################################
#                                         #
# A COPY OF simc_swtor_reports.bat        #
#                                         #
# CHECK FOR ANY CHANGES BEFORE RERUNNING  #
#                                         #
###########################################

PATH="../engine:engine:$PATH"
echo $PATH

iterations=10000
threads=7
calculate_scale_factors=1
center_scale_delta=1
statistics_level=3
ptr=0

runsim ()
{
  echo $1$2
  simc $1.simc $3 output=/dev/null statistics_level=$statistics_level iterations=$iterations calculate_scale_factors=$calculate_scale_factors center_scale_delta=$center_scale_delta threads=$threads ptr=$ptr html=$1$2.html
}

runsim Sage_Sorcerer_Raid
runsim Sage_Sorcerer_Raid -HelterSkelter "fight_style=HelterSkelter"
runsim Sage_Sorcerer_Raid -HeavyMovement "fight_style=HeavyMovement"
runsim Assassin_Raid
#runsim Assassin_Raid -HelterSkelter "fight_style=HelterSkelter"
#runsim Assassin_Raid -HeavyMovement "fight_style=HeavyMovement"
runsim Mercenary_Raid
runsim Scoundrel_Operative_Raid
runsim Gunslinger_Sniper_Raid
