################################################################
# Czech Telecom rate ruleset
#
# created 98/06/18 by Miroslav Flídr <flidr@kky.zcu.cz>
#
# no extensive testing...
################################################################

name=Czech_Telecom_Tarifni_pasmo2
currency_symbol=Kc
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(2.60, 95)

on (monday..friday)   between (7:00..19:00)  use (2.60, 40)
