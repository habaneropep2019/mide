################################################################
#
# Austria City rate ruleset (special "online" numbers)
#
# created by Mario Weilguni <mweilguni@sime.com>
# 	   
################################################################

name=Austria_Local_Area
currency_symbol=ATS
currency_position=right
currency_digits=2
default=(1.116, 120)

# tagestarif 1
on (monday..friday) between (08:00..12:00) use (1.116, 120)
on (monday..thursday) between (13:00..16:00) use (1.116, 120)

# tagestarif 2
on (monday..thursday) between (12:00..13:00) use (1.116, 120)
on (monday..thursday) between (16:00..18:00) use (1.116, 120)
on (friday) between (12:00..18:00) use (1.116, 120)

# spartarif
on (monday..friday) between (06:00..08:00) use (1.116, 360)
on (monday..friday) between (18:00..20:00) use (1.116, 360)
on (saturday,sunday) between (06:00..20:00) use (1.116, 360)

# superspartarif
on () between (00:00..06:00) use (1.116, 360)
on () between (20:00..23:59) use (1.116, 360)

# Feiertage
on (5/1,12/25,12/26,11/1,easter+1,easter+39,easter+50,1/1,10/26,1/6,12/8) between (00:00..23:59) use (1.116, 360)

