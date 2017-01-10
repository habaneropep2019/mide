################################################################
#
# NOTES: 
# This rules are for the so called "digital" phones in Greece
# The costs for a city phone-call are: 1 Unit(11.5+18%FPA=13.57Drs) every 3 minutes 
# (180secs), and between 11:00pm and 08:00am 1Unit every 8 minutes (480secs)
# Pay attention Greeks Users to the frequent changes of the Unit's price (sigh!).
#
# Nikos Kokras, senior medical student, Aristotle Univ. of Thessaloniki
# email: nikos@biogate.com, kkrnls@med.auth.gr
#
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=Hellas_local.rts

################################################################
# currency settings
################################################################

# defines DRS (Hellenic draxmi) to be used as currency
# symbol (not absolutely needed, default = "$")
currency_symbol=Drs

# Define the position of the currency symbol.
# (not absolutely needed, default is "right")
currency_position=right 

# Define the number of significat digits.
# (not absolutely needed, default is "2"
currency_digits=2



################################################################
# connection settings
################################################################

# NOTE: rules are applied from top to bottom - the
#       LAST matching rule is the one used for the
#       cost computations.


# This is the default rule which is used when no other rule
# applies. The first component "13.57" is the price of one
# "unit", while "180" is the duration in seconds.
# Therefore the following rule means: "Every 180 seconds 13.57 
# DRS are added to the bill"
default=(13.57, 180)

#
# more complicated rules:
#

# "on monday until sunday from 23:00 pm until 08:00 pm the costs
# are 0.2 each 72 seconds"
on (monday..sunday) between (23:00..08:00) use (13.57, 480)


