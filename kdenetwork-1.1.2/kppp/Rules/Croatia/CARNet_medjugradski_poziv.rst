################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=CARNet_medjugradski_poziv.rst

################################################################
# currency settings
################################################################

currency_symbol=HRK
currency_position=right 
currency_digits=2

################################################################
# connection settings
################################################################

per_connection=0.0
minimum_costs=0.0

# You pay .74 for the first 180 secons ( 3minutes) no matter
# whether you are connected for 1 second or 180 seconds.
# This rule will take priority during the first 180 seconds
# over any other rule, in particular the 'default' rule.
# have a look at costgraphs.gif in the docs directory
# of the kppp distribution for a graphic illustration.

flat_init_costs=(0.1677,36)

# This is the default rule which is used when no other rule
# applies. The first component "0.1" is the price of one
# "unit", while "72" is the duration in seconds.
# Therefore the following rule means: "Every 72 seconds 0.1 
# ATS are added to the bill"
default=(0.1677, 36)

# more complicated rules:

# "on monday until sunday from 12:00 am until 11:59 pm the costs
# are 0.2 each 72 seconds"
#on () between () use (0.2, 2)

# same as above. You must use 24 hour notation, or the accounting
# will not work correctly. (Example: write 15:00 for 3 pm)
on (monday..saturday) between (0:00..6:59) use (0.1677, 72)
on (monday..saturday) between (7:00..15:59) use (0.1677, 36)
on (monday..saturday) between (16:00..21:59) use (0.1677, 48)
on (monday..saturday) between (22:00..23:59) use (0.1677, 72)
on (sunday) between () use (0.1677, 72)

# Sluzbeni praznici, najjeftinija tarifa
# 1.1. Nova Godina
# Easter - Uskrs
# 1.5. - 
# 30.5. - Dan drzavnosti
# 22.6. - Dan borbe protiv fasizma
# 4.8. - Dan domovinske zahvalnosti
# 15.8. - Velika Gospa
# 1.11. - Dan svih svetih
# 25.12. - Bozic
on (01/01, easter, 05/01, 05/30, 06/22, 08/04, 08/15, 11/01, 12/25) between () use (0.1677,360)
