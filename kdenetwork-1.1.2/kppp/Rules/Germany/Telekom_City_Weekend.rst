#################################################################################
# German Telekom accounting rules
#
# City_Weekend
#
# derived 31/12/98 by Joachim Wesner <Joachim.Wesner@frankfurt.netsurf.de>
#         from CityCall by Stefan Markgraf and Stefan Troeger
#
#################################################################################

name=German_Telekom_City_Weekend
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(0.121, 90)

# Montag bis Freitag
on (monday..friday) between (5:00..8:59)   use (0.121, 150)
on (monday..friday) between (18:00..20:59) use (0.121, 150) 
on (monday..friday) between (21:00..4:59)  use (0.121, 240)

# Sonnabend, Sonntag
on (saturday..sunday) between () use (0.121, 240)

# Bundeseinheitliche Feiertage, Heiligabend und Silvester
on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/24, 12/25, 12/26, 12/31) between () use (0.121, 240)

# Bundeseinheitliche Feiertage:
# 01/01     Neujahr
# easter-2  Karfreitag
# easter    Ostersonntag
# easter+1  Ostermontag
# 05/01     Maifeiertag Tag der Arbeit
# easter+39 Christi Himmelfahrt
# easter+49 Pfingstsonntag
# easter+50 Pfingstmontag
# 10/03     Tag der Deutschen Einheit
# 12/25     1. Weihnachtstag
# 12/26     2. Weihnachtstag

# Zusaetzliche Tage mit Feiertagstarif:
# 12/24     Heiligabend
# 12/31     Silvester
