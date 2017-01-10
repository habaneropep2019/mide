################################################################
# Denmark Get2net 1001 internet call pr 1-apr-98
#
################################################################
name=get2net
currency_symbol=DKK
currency_position=right 
currency_digits=2
per_connection=0.25
default=(0.10, 60)
on (monday..friday) between (8:00..16:00) use (0.27, 60)
on (monday..friday) between (16:00..19:00) use (0.20, 60)
on (saturday) between (8:00..16:00) use (0.20, 60)
on (12/25..12/27, 12/31) between () use (0.1, 60)
on (easter) between () use (0.1,60)
on (easter+60) between () use (0.1,60)

