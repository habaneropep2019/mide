##########################################################################
# Ruleset file for the Belgium Interzonal calls applicable since 98/03/15.
# 
# Rules:
#  - Peak hours: 08:00 -> 18:00.
#  - Off-peak  : the rest + national holidays.
#  - Minimum entry costs (flat_init_costs) are halfed when off-peak. 
#    (how to implement?) 
#  - Calls longer than 10' have 30% reduction when off-peak to a bbs.
#    Calls longer than 10' have 50% reduction when off-peak to the net!
#    (how to implement?) 
#
# Guy Zelck, guy.zelck@eds.com 
################################################################

# NAME OF THE RULESET. This is NEEDED for accounting purposes.
name=Belgium_zonal

################################################################
# currency settings
################################################################

# Currency symbol, default = "$".
# define Bf (Begische Frank) to be used as currency
currency_symbol=Bf

# Position of the currency symbol, default is "right".
currency_position=right 

# Define the number of significant digits.
# (not absolutely needed, default is "2"
currency_digits=3

################################################################
# connection settings
################################################################

# NOTE: rules are applied from top to bottom - the
#       LAST matching rule is the one used for the
#       cost computations.

# This is charged whenever you connect. If you don't have to
# pay per-connection, use "0" here or comment it out.
per_connection=0.0


# Minimum costs per connection. If the costs of a phone
# call are less than this value, this value is used instead
minimum_costs=0.0


# This is the minimum time slice you allways have to pay, here its 180 sec.
# It has priority over the default rule.
# Trouble here is that during peak-hours it's 6 Bf, during off-peak hours it's 3 Bf.
# How can this be implemented?
flat_init_costs=(6,180)

# Let's take the off-peak hour rate as the default.
default=(1,60)

# During off-peak hours when a call takes > 10' you have 30% reduction.
# ?

# Peak hours.
on (monday..friday) between (08:00..18:00) use (2,60)

# Holidays (mm/dd) in '98 have the off-peak rate.
on (01/01, easter, easter+1, 05/01, easter+39, easter+49, easter+50, 07/21, 08/15, 11/01, 11/11, 12/25) between () use (1,60)

# 01/01     Nieuwjaar.
# easter    Pasen
# easter+1  Paasmaandag
# 05/01     Feest van de arbeid
# easter+39 O.H.-Hemelvaart
# easter+49 Pinksteren
# easter+50 Pinkstermaandag
# 07/21     Nationale Feestdag
# 08/15     O.L.V.-Hemelvaart
# 11/01     Allerheiligen
# 11/11     Wapenstilstand 1918
# 12/25     Kerstmis
