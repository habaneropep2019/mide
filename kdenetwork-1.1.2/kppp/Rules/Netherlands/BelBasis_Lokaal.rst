################################################################
#
#  kppp rules voor KPN BelBasis abonnement
#
#  Alleen voor lokale gesprekken!!!
#
#  Kosten:
#  Lokaal:      duur 6.5 cpm, goedkoop 3.25 cpm
#  Interlokaal: duur 20 cpm,  goedkoop 10 cpm
#  10 cent verbindingskosten
#
#  Erik Hensema
#  hensema@worldaccess.nl
#
################################################################

name=Nederlands BelBasis Lokaal
currency_symbol=Hfl
currency_position=left
currency_digits=2
per_connection=0.10
minimum_costs=0.0

default=(0.0325, 60)
on (monday..friday) between (8:00..18:00) use (0.065, 60)


