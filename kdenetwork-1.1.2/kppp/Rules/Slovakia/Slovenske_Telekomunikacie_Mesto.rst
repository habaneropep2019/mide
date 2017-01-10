################################################################
# Slovak Telecom rate ruleset
#
# created 98/01/29 by Juraj Bednár <bednar@isternet.sk>
#
# no extensive testing...
################################################################

name=Slovenske_Telekomunikacie_Mesto
currency_symbol=Sk
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(2.10, 240)

# (for CITY Tarif)
on (monday..friday)   between (7:00..19:00)  use (2.10, 180)
