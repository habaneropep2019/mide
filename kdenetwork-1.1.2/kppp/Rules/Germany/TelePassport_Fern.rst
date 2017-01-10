################################################################
# TelePassport Ruleset - Fern (ab 50 km)
#
# created on 10.05.1998 by Tobias Toedter
# <Tobias_Toedter@public.uni-hamburg.de>
#
################################################################

name=TelePassport_Fern
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0

# default cost auf Maximum setzen
default=(0.00633, 1)

# Ferntarif (ab 50 km)
on (monday..friday)   between (9:00..18:00)   use (0.00633, 1)
on (monday..friday)   between (18:00..21:00)  use (0.00367, 1)
on (monday..friday)   between (21:00..5:00)   use (0.003,   1)
on (monday..friday)   between (5:00..9:00)    use (0.00367, 1)
on (saturday..sunday) between (5:00..21:00)   use (0.00367, 1)
on (saturday..sunday) between (21:00..5:00)   use (0.003,   1)

# Bundesweite Feiertage
on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/25, 12/26) between (5:00..21:00) use (0.00367, 1)
on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/25, 12/26) between (21:00..5:00) use (0.003,   1)

# Bundesweite Feiertage:
# 01/01     Neujahr
# easter-2  Karfreitag
# easter    Ostersonntag
# easter+1  Ostermontag
# 05/01     Tag der Arbeit
# easter+39 Christi Himmelfahrt
# easter+49 Pfingstsonntag
# easter+50 Pfingstmontag
# 10/03     Tag der Deutschen Einheit
# 12/25     1. Weihnachtstag
# 12/26     2. Weihnachtstag



