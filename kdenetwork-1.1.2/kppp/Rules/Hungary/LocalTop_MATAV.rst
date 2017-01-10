###################################################################
#
#		B I G  F A T  W A R N I N G ! ! ! 
#
#	T h i s  i s  n o t  a  r e a l l y  f u n c t i o n a l
#			r s t  f i l e
#
###################################################################

################################################################
#
# Local calls in Hungary in MATAV districts
# tarifs based on secundums!!!!! (processor eating settings)
# Valid all way in 1999 (I hope)
# by Gabor Jant <jamm@mail.interware.hu> (this will change soon)
#
################################################################
name=Hungary_MATAV_local

################################################################
# currency settings
################################################################

currency_symbol=Ft
currency_position=left
currency_digits=2

################################################################
# connection settings
################################################################
per_connection=8.25
minimum_costs=0

#######################default=(0.175, 1)

on (monday..friday) between (0:00..4:59) use (0.04375, 1) max (150)
on (monday..friday) between (5:00..6:59) use (0.06875, 1) max (150)

# in this peek hours interwall there is not a maximum per call....
on (monday..friday) between (7:00..17:59) use (0.175, 1) max ()

on (monday..friday) between (18:00..21:59) use (0.06875, 1) max (150)
on (monday..friday) between (22:00..23:59) use (0.04375, 1) max (150)

on (saturday..sunday) between (0:00..4:59) use (0.04375, 1) max (150)
on (saturday..sunday) between (5:00..21:59) use (0.06875, 1) max (150)
on (saturday..sunday) between (22:00..23:59) use (0.04375, 1) max (150)

#############################################################
#  hours   |2   2#0        |   |    1              |1   2  |#
# of day   |2 3 4#1 2 3 4 5|6 7|8 9 0 1 2 3 4 5 6 7|8 9 0 1|#
#############################################################
#monday    |               |   |                   |       |#
#..........|                   |                   |        #
#tuesday   |   8.25/call   |   |   8.25 per call   |       |#
#..........|                   |                   |        #
#wednesday |       +       |   |        +          |       |#
#..........|                   |                   |        #
#thursday  |  2.63/60sec   |   |  10.50 / 60 sec   |       |#
#..........|                   |                   |        #
#friday    |     but       |   |                   |       |#
#..........|     max           ---------------------        #
#saturday  |     150       |      8.25 per call +          |#
#..........|    per call  --->    4.13 / 60 sec             #
#sunday    |               |    but max 150 per call  ------>
#############################################################
