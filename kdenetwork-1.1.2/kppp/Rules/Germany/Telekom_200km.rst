################################################################
# German Telekom rate ruleset
# (most complicated on Earth :-)
#
# created 97/07/18 by Stefan Markgraf <S_Markgraf@hotmail.com>
#
# no extensive testing...
################################################################

name=German_Telekom_R200
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(0.12, 13)

# (for 200km Tarif)
on (monday..friday)   between (2:00..5:00)   use (0.12, 120)
on (monday..friday)   between (5:00..9:00)   use (0.12, 22.5)
on (monday..friday)   between (9:00..12:00)  use (0.12, 13)
on (monday..friday)   between (12:00..18:00) use (0.12, 14)
on (monday..friday)   between (18:00..21:00) use (0.12, 22.5)
on (monday..friday)   between (21:00..2:00)  use (0.12, 36)
on (saturday..sunday) between (5:00..21:00)  use (0.12, 22.5)
on (saturday..sunday) between (21:00..5:00)  use (0.12, 36)

# Spezialtarif an bundeseinheitlichen Feiertagen
on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/25, 12/26) between (5:00..21:00) use (0.12, 36)
on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/25, 12/26) between (21:00..5:00) use (0.12, 36)

# bundeseinheitliche Feiertage:
# 01/01     Neujahr
# easter-2  Karfreitag
# easter    Ostersonntag
# easter+1  Ostermontag
# 05/01     Maifeiertag Tag der Arbeit
# easter+39 Christi Himmelfahrt
# easter+49 Pfingstsonntag
# easter+50 Pfingstmontag
# 10/03     Tag der Deutschen Einheit
# 12/25     Weihnachten
# 12/26     Weihnachten


