################################################################
#
# Austria City rate ruleset
#
# created by Mario Weilguni <mweilguni@sime.com>
# 	   
################################################################

name=Austria_Local_Area
currency_symbol=ATS
currency_position=right
currency_digits=2
default=(0.936, 12)

# tagestarif 1
on (monday..friday) between (08:00..12:00) use (0.936, 12)
on (monday..thursday) between (13:00..16:00) use (0.936, 12)

# tagestarif 2
on (monday..thursday) between (12:00..13:00) use (0.936, 12)
on (monday..thursday) between (16:00..18:00) use (0.936, 12)
on (friday) between (12:00..18:00) use (0.936, 12)

# spartarif
on (monday..friday) between (06:00..08:00) use (0.936, 19.2)
on (monday..friday) between (18:00..20:00) use (0.936, 19.2)
on (saturday,sunday) between (06:00..20:00) use (0.936, 19.2)

# superspartarif
on () between (00:00..06:00) use (0.936, 19.2)
on () between (20:00..23:59) use (0.936, 19.2)

