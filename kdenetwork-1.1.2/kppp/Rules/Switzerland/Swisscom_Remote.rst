# Calls out of 10 km in Switzerland
# Created by J. Wezel (jwezel@access.ch)
# Sun Sep 14 01:30:33 1997
# Changed by Daniel Brönnimann (dbreonni@g26.ethz.ch)
# Thu Nov 19 1998
# Valid from 1-Aug-97

name=Swisscom_Remote_2

currency_symbol=CHF

currency_position=left
currency_digits=2
per_connection=0.00
minimum_costs=0.00
default=(0.10, 90)

# Normal tarif
on (monday..friday) between (8:00..17:00) use (0.10, 24)
on (monday..sunday) between (19:00..21:00) use (0.10, 24)

# Low tarif
on (monday..friday) between (6:00..8:00) use (0.10, 48)
on (monday..friday) between (17:00..19:00) use (0.10, 48)
on (monday..friday) between (21:00..23:00) use (0.10, 48)
on (saturday..sunday) between (6:00..23:00) use (0.10, 48)
on (01/01, 02/01, easter-2, easter+1) between (6:00..23:00) use (0.10, 48)
on (easter+39, easter+50) between (6:00..23:00) use (0.10, 48)
on (01/08, 12/25, 12/26) between (6:00..23:00) use (0.10, 48)
# Night tarif
on (monday..sunday) between (23:00..0:00) use (0.10, 96)
on (monday..sunday) between (0:00..6:00) use (0.10, 96)

