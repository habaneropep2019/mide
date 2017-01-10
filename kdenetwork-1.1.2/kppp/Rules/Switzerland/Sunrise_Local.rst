################################################################
#
#  Sunrise local
#  Created by Daniel Brönnimann (dbroenni@g26.ethz.ch)
#  Nov 16 1998
#
################################################################

name=Sunrise_Local

################################################################
# currency settings
################################################################

currency_symbol=CHF
currency_position=right 
currency_digits=2

################################################################
# connection settings
################################################################

per_connection=0.0
minimum_costs=0.0

# Nachttarif, tarif de nuit
default=(0.00027083, 1)

# Normaltarif, tarif normal
on (monday..friday) between (8:00..17:00) use (0.0010833, 1)

# Niedertarif, tarif réduit
# easter+39 = Ascension Day (Auffahrt), easter+50 = Whit Monday (Pfingstmontag)
on (monday..friday) between (17:00..23:00) use (0.00054166, 1)
on (saturday..sunday) between (8:00..23:00) use (0.00054166, 1)
on (01/01, 02/01, easter-2, easter+1) between (8:00..23:00) use (0.00054166, 1)
on (easter+39, easter+50) between (8:00..23:00) use(0.00054166, 1)
on (01/08, 12/25, 12/26) between (8:00..23:00) use (0.00054166, 1)
