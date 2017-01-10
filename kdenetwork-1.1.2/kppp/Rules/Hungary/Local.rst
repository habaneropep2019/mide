################################################################
#
# Local calls in Hungary
# Valid all way in 1999 before 31Th March (I really hope)
# by Laszlo Megyer <abulla@mail.elender.hu> from Hungary, Szekesfehervar
# I like motorcycles, hacking and Linux.
#
# Please DON'T erase anything from this document, instead of correcting
# mistakes in the sentences and rules. Thanks. 
#
################################################################
name=Hungary_local
################################################################
# currency settings
################################################################

currency_symbol=Ft
currency_position=right
currency_digits=2

################################################################
# connection settings
################################################################

per_connection=8.25
minimum_costs=0

default=(1.75, 10)

on (monday..friday) between (0:00..4:59) use (0.4383, 10)
on (monday..friday) between (5:00..6:59) use (0.43833, 10)
on (monday..friday) between (7:00..17:59) use (1.75, 10)
on (monday..friday) between (18:00..21:59) use (0.43833, 10)
on (monday..friday) between (22:00..23:59) use (0.4383, 10)

on (saturday..sunday) between (0:00..4:59) use (0.4383, 10)
on (saturday..sunday) between (5:00..21:59) use (0.6883, 10)
on (saturday..sunday) between (22:00..23:59) use (0.4383, 10)

# Well, there is a maximum cost of 150 Ft if the call is started any day after
# 18:00 and finished before 7:00 or it is started after 5:00 at weekend and
# finished before 22:00.
