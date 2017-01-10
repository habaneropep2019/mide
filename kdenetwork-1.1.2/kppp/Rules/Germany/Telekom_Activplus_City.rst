
#################################################################################
# German Telekom accounting rules
#
# City mit Activ Plus Option
#
# created  99/08/27 Moritz Moeller-Herrmann mmh@gmx.net (NO WARRANTY!)
#
#################################################################################

name=German_Telekom_Activplus_City
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(0.06, 60)

# Einfache Regeln gell?
on (monday..sunday) between (9:00..17:59)   use (0.06, 60)
on (monday..sunday) between (18:00..8:59)   use (0.03, 60)
