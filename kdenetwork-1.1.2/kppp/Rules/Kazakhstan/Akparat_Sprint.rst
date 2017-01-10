################################################################
#
# This is a sample rule set for kppp. You can use it as a 
# template when you have to create your own ruleset. If you do
# so, remove all comments and add your own. This will allow
# other users to check your ruleset more easily.
# 
# Please sign the the tarif file with your name an email address
# so that I can contact you if necessary.
#
# NOTE: the rules in this rule set do not make much sense and
#       are only for demonstration purposes
#
# NOTE ON FILENAMES:
#	when you create your own ruleset, use "_" in filename
#	instead of spaces and use ".rst  as extension
#	   i.e. "Austria city calls"
#          --> file should be saved as "Austria_city_calls.rst"
#
# Thanks, Bernd Wuebben
# wuebben@math.cornell.edu / wuebben@kde.org
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=Akparat Sprint

################################################################
# currency settings
################################################################

# defines ATS (Austrian Schilling) to be used as currency
# symbol (not absolutely needed, default = "$")
currency_symbol="$"

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

# This is charged whenever you connect. If you don't have to
# pay per-connection, use "0" here or comment it out.
per_connection=0.0


# minimum costs per per connection. If the costs of a phone
# call are less than this value, this value is used instead
minimum_costs=0.0


# You pay .74 for the first 180 secons ( 3minutes) no matter
# whether you are connected for 1 second or 180 seconds.
# This rule will take priority during the first 180 seconds
# over any other rule, in particular the 'default' rule.
# have a look at costgraphs.gif in the docs directory
# of the kppp distribution for a graphic illustration.
# flat_init_costs=(0.74,180)

# This is the default rule which is used when no other rule
# applies. The first component "0.1" is the price of one
# "unit", while "72" is the duration in seconds.
# Therefore the following rule means: "Every 72 seconds 0.1 
# ATS are added to the bill"
default=(0.1333, 60)

#
# more complicated rules:
#

# "on monday until sunday from 12:00 am until 11:59 pm the costs
# are 0.2 each 72 seconds"
# on () between () use (0.2, 2)

# same as above
on (monday..sunday) between (0:00..10:00) use (0.016, 60)
on (monday..sunday) between (23:00..23:59) use (0.016, 60)
on (monday..friday) between (20:00..23:00) use (0.0666, 60)
on (saturday..sunday) between (10:00..23:00) use (0.0666, 60)
