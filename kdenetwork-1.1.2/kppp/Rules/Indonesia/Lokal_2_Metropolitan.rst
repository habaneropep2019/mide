################################################################
# This is the rule set for Indonesia local calls,
# including tax, according to Petunjuk Telepon Bandung
# Juni 1998-1999
# by Priyadi Iman Nurcahyo <priyadi@priyadi.ml.org>
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=Lokal-non-metropolitan

################################################################
# currency settings
################################################################

# defines IDR (Indonesian Rupiah) to be used as currency
currency_symbol=IDR

# Define the position of the currency symbol.
currency_position=right 

# Define the number of significat digits.
# (not absolutely needed, default is "2"
currency_digits=0



################################################################
# connection settings
################################################################

# NOTE: rules are applied from top to bottom - the
#       LAST matching rule is the one used for the
#       cost computations.


# minimum costs per per connection. If the costs of a phone
# call are less than this value, this value is used instead
minimum_costs=160


# This is the default rule which is used when no other rule
# applies. The first component "160" is the price of one
# "unit", while "120" is the duration in seconds.
# Therefore the following rule means: "Every 120 seconds 160 
# IDR are added to the bill"
default=(160, 120)

#
# more complicated rules:
#

on () between (00:00..08:59) use (160, 120)
on () between (09:00..14:59) use (160, 90)
on () between (15:00..23:59) use (160, 120)

