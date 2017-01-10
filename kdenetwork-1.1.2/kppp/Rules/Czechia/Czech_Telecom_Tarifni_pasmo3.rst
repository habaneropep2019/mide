################################################################
# Czech Telecom rate ruleset
#
# created 98/06/18 Miroslav Flídr <flidr@kky.zcu.cz>
#
# no extensive testing...
################################################################

name=Czech_Telecom_Tarifni_pasmo3
currency_symbol=Kc
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(2.60, 90)

on (monday..friday)   between (7:00..19:00)  use (2.60, 36)
