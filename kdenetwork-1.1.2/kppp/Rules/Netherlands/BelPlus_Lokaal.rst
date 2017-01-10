################################################################
#
#  kppp rules voor KPN BelBasis abonnement
#
#  Alleen voor lokale gesprekken!!!
#
#  Kosten:
#  Lokaal:      duur 6.5 cpm, goedkoop 2.5 cpm
#  Interlokaal: duur 20 cpm,  goedkoop 7.5 cpm
#  10 cent verbindingskosten
#
#  Erik Hensema
#  hensema@worldaccess.nl
#
################################################################

name=Nederlands BelPlus Lokaal
currency_symbol=Hfl
currency_position=left
currency_digits=2
per_connection=0.10
minimum_costs=0.0

default=(0.025, 60)
on (monday..friday) between (8:00..18:00) use (0.065, 60)

