#####################################################################
# German NetCologne rate ruleset
# ------------------------------
# (probably only usefull for anybody living in Cologne)
# Does NOT cover NetColognes "Time-Line" or "Power-Line" Internet 
# Internet package !
#
# *** for calls inside Germany *** - Use at your own risk.
#
# edited 20.5.99 <Roland Pabel (pabel@cip.physik.uni-bonn.de)>)
#
# Obwohl NetCologne sekundengenau abrechnet, ist es viel einfacher
# in 6 Sekundenschritten abzurechnen. Der errechnete Betrag duerfte
# nur wenig ueber dem Realen liegen. Bisher nicht grossartig getestet!
#
# Alle Angaben ohne Gewaehr (besonders nicht von mir :-). 
# Infos zu NetCologne unter http://www.netcologne.de .
#
#####################################################################

name=German_NetCologne_Germany
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0

# default : Cost of 6 seconds in the time periods (Monday..Friday, 8:00..18:00)
default=(0.016, 6)

# (other time periods / Weekend)
on (monday..friday)   between (18:00..21:59) use (0.009, 6)
on (monday..sunday)   between (22:00..7:59) use (0.006, 6)
on (saturday..sunday) between (8:00..21:59) use (0.009, 6)

# Spezialtarif an bundeseinheitlichen Feiertagen
# Feiertagstarif = Nebentarif (8-22 Uhr) & Nachttarif (22-8)

on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/25, 12/26) between (8:00..21:59) use (0.009, 6)
on (01/01, easter-2, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 10/03, 12/25, 12/26) between (22:00..7:59) use (0.006, 6) 
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

# Feiertage in *** Kvln ***

on (easter-48, easter-52) between (8:00..21:59) use (0.009, 6)
on (easter-48, easter-52) between (22:00..7:59) use (0.006, 6)

# NetCologne bietet den Feiertagstarif auch an diesen Tagen an :
# easter-48 Rosenmontag
# easter-52 Weiberfastnacht
