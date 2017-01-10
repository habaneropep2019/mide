################################################################
#
# Local calls in Hungary in MATAV districts
# tarifs based on 10 seconds
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

default=(1.75, 10)

on (monday..friday) between (0:00..4:59) use (0.4375, 10)
on (monday..friday) between (5:00..6:59) use (0.6875, 10)
on (monday..friday) between (7:00..17:59) use (1.75, 10)
on (monday..friday) between (18:00..21:59) use (0.6875, 10)
on (monday..friday) between (22:00..23:59) use (0.4375, 10)

on (saturday..sunday) between (0:00..4:59) use (0.4375, 10)
on (saturday..sunday) between (5:00..21:59) use (0.6875, 10)
on (saturday..sunday) between (22:00..23:59) use (0.4375, 10)
