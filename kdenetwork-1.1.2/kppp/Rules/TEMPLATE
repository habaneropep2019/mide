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
name=default

################################################################
# currency settings
################################################################

# defines ATS (Austrian Schilling) to be used as currency
# symbol (not absolutely needed, default = "$")
currency_symbol=ATS

# Define the position of the currency symbol.
# (not absolutely needed, default is "right")
currency_position=right 

# Define the number of significant digits.
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


# You pay .74 for the first 180 seconds ( 3 minutes) no matter
# whether you are connected for 1 second or 180 seconds.
# This rule will take priority during the first 180 seconds
# over any other rule, in particular the 'default' rule.
# have a look at costgraphs.gif in the docs directory
# of the kppp distribution for a graphic illustration.
flat_init_costs=(0.74,180)

# This is the default rule which is used when no other rule
# applies. The first component "0.1" is the price of one
# "unit", while "72" is the duration in seconds.
# Therefore the following rule means: "Every 72 seconds 0.1 
# ATS are added to the bill"
default=(0.1, 72)

#
# more complicated rules:
#

# "on monday until sunday from 12:00 am until 11:59 pm the costs
# are 0.2 each 72 seconds"
on () between () use (0.2, 2)

# same as above
on (monday..sunday) between () use (0.2, 2)

# same as above. You must use 24 hour notation, or the accounting
# will not work correctly. (Example: write 15:00 for 3 pm)
on (monday..sunday) between (0:00..23:59) use (0.2, 2)

# applies on friday, saturday, sunday and monday 8am until 1pm
on (friday..monday) between (8:00..13:00) use(0.3,72)

# ATTENTION:
on(monday..friday) between (21:00..5:00) use (0.4,2)
# does NOT include saturday 0:00-5:00, just monday..friday, as it says.

# applies on a given date (christmas)
on (12/25) between () use (0.3,72)

# a range of dates and one weekday
on (12/25..12/27, 12/31, 07/04, monday) between () use (0.4, 72)

# use this for easter
on (easter) between () use (0.3,72)

# easter + 50 days (Pfingstmontag/ Pentecost Monday )
on (easter+50) between () use (0.3,72)

on (thursday) between (20:00..21:52) use (8.2, 1)


# The "on()" rules above all relates to current time only. You can also
# make a rule depend on the number of seconds you have been connected
# by specifying this time as a third argument to "use()".
# For instance, let's say normal rate in the evening is 0.20 per minute,
# and it drops by 20% after one hour of connect time. This can be modelled
# like:

on () between (19:30..08:00) use (0.20, 60)
on () between (19:30..08:00) use (0.16, 60, 3600)

# Note that these rules, just like other rules, are sensitive to the 
# order in which they appear.

