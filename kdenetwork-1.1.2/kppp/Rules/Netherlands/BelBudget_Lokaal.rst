################################################################
#
#  kppp rules voor KPN BelBudget abonnement
#
#  Alleen voor lokale gesprekken!!!
#
#  Kosten:
#  Lokaal:      duur 20 cpm, goedkoop 10 cpm
#  Interlokaal: duur 60 cpm, goedkoop 30 cpm
#  10 cent verbindingskosten
#
#  Erik Hensema
#  hensema@worldaccess.nl
#
################################################################

name=Nederlands BelBudget Lokaal
currency_symbol=Hfl
currency_position=left
currency_digits=2
per_connection=0.10
minimum_costs=0.0

default=(0.01, 6)
on (monday..friday) between (8:00..18:00) use (0.01, 3)



