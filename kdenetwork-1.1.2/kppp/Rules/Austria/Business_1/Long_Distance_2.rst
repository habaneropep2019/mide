################################################################
#
# Austria Zone 2 rate ruleset (national, above 200km)
#
# created by Mario Weilguni <mweilguni@sime.com>
# 	   
################################################################

name=Austria_Local_Area
currency_symbol=ATS
currency_position=right
currency_digits=2
default=(0.996, 19.2)

# tagestarif 1
on (monday..friday) between (08:00..12:00) use (0.996, 15)
on (monday..thursday) between (13:00..16:00) use (0.996, 15)

# tagestarif 2
on (monday..thursday) between (12:00..13:00) use (0.996, 19.2)
on (monday..thursday) between (16:00..18:00) use (0.996, 19.2)
on (friday) between (12:00..18:00) use (0.996, 19.2)

# spartarif
on (monday..friday) between (06:00..08:00) use (0.996, 40)
on (monday..friday) between (18:00..20:00) use (0.996, 40)
on (saturday,sunday) between (06:00..20:00) use (0.996, 40)

# superspartarif
on () between (00:00..06:00) use (0.996, 50)
on () between (20:00..23:59) use (0.996, 50)

