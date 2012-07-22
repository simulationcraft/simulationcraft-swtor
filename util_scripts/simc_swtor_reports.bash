#!/bin/bash
###########################################
#                                         #
# A COPY OF simc_swtor_reports.bat        #
#                                         #
# CHECK FOR ANY CHANGES BEFORE RERUNNING  #
#                                         #
###########################################

PATH="../engine:engine:$PATH"
echo $PATH

iterations=100000
threads=7
calculate_scale_factors=0
center_scale_delta=0
statistics_level=3
positive_scale_delta=1
#ptr=0

runsim ()
{
  echo $1$2
  simc $1.simc $3 output=/dev/null statistics_level=$statistics_level iterations=$iterations calculate_scale_factors=$calculate_scale_factors center_scale_delta=$center_scale_delta positive_scale_delta=$positive_scale_delta threads=$threads  html=$1$2.html # ptr=$ptr
}

#runsim Raid_Campaign
# runsim Sage_Sorcerer_Raid_Campaign
# runsim Sage_Sorcerer_Raid_Campaign -HelterSkelter "fight_style=HelterSkelter"
# runsim Sage_Sorcerer_Raid_Campaign -HeavyMovement "fight_style=HeavyMovement"
# runsim Shadow_Assassin_Raid_Campaign
# ## runsim Assassin_Raid -HelterSkelter "fight_style=HelterSkelter"
# ## runsim Assassin_Raid -HeavyMovement "fight_style=HeavyMovement"
# runsim Mercenary_Raid_Campaign
# runsim Scoundrel_Operative_Raid_Campaign
# runsim Gunslinger_Sniper_Raid_Campaign
# runsim Sorcerer_Raid_Campaign_BiS
runsim Empire_Raid_Campaign_BiS
#iterations=200000
#runsim Mercenary_Arsenal_Campaign_BiS

# runsim Sage_Sorcerer_Raid
# runsim Sage_Sorcerer_Raid -HelterSkelter "fight_style=HelterSkelter"
# runsim Sage_Sorcerer_Raid -HeavyMovement "fight_style=HeavyMovement"
# runsim Shadow_Assassin_Raid
# ## runsim Assassin_Raid -HelterSkelter "fight_style=HelterSkelter"
# ## runsim Assassin_Raid -HeavyMovement "fight_style=HeavyMovement"
# runsim Mercenary_Raid
# runsim Scoundrel_Operative_Raid
# runsim Gunslinger_Sniper_Raid
