################################################################
# This is the Ruleset for Jamaica.
# 8 Mar 1998 (Valid until CWJamaica disides to do some deaper
# price gaoging ?)
#
# I TAKE NO RESPONSIBILITY FOR THE VALIDITY AND ACCURACY OF
# THIS INFORMATION, IF YOU ARE SO CONCERNED PLEASE READ THE
# RELEVANT SECTION OF THE PHONE BOOK AND CONFIGURE THIS YOURSELF.
# AND DO NOT USE THIS FILE.
#
# Kevin Forge
# <forgeltd@usa.net>
#
# If you use an ISP in the same parish, these rules aply
#
# CWJ_Local.rst
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
currency_symbol=$

# Define the position of the currency symbol.
# (not absolutely needed, default is "right")
currency_position=left

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
minimum_costs=0.15


# You pay .76 for the first 60 secons ( 1minute ) no matter
# whether you are connected for 1 second or 60 seconds.
# This rule will take priority during the first 60 seconds
# over any other rule, in particular the 'default' rule.
# have a look at costgraphs.gif in the docs directory
# of the kppp distribution for a graphic illustration.
flat_init_costs=(0.15, 60)

# This is the default rule which is used when no other rule
# applies. The first component "0.15" is the price of one
# "unit", while "60" is the duration in seconds.
# Therefore the following rule means: "Every 60 seconds 0.15 
# Cents are added to the bill"
default=(0.15, 60)

#
# more complicated rules:
# do not aply since for you there is NO MERCY!! 

# "on monday until sunday from 7:00 pm until 6:59 am the costs
# are 0.38 each 60 seconds"
# on () between () use (0.15, 60)

# same as above
# on (monday..sunday) between () use (0.2, 2)

# same as above. You must use 24 hour notation, or the accounting
# will not work correctly. (Example: write 15:00 for 3 pm)
# on (monday..sunday) between (0:00..23:59) use (0.2, 2)

# applies on sunday 
# on (sunday) between () use(0.38, 60)

# ATTENTION:
# on(monday..friday) between (21:00..5:00) use (0.4,2)
# does NOT include saturday 0:00-5:00, just monday..friday, as it says.

# applies on a given date (christmas)
# on (12/25) between () use (0.3,72)

# This is most of the holidays
# on (1/1, 8/4, 12/25, 12/26, ) between () use (0.38, 60)

# use this for easter
# on (easter) between () use (0.38, 60)

# easter + 60 days (Pfingstmontag/ Pentecost Monday )
# easter - 44 days ( Ash Wedensday )
# on (easter-44) between () use (0.38, 60)

# ATTENTION:
# Enable this if within your program easter is just good friday
# and change "(easter+3)" to "(easter-3)" if it's Easter Monday
# on (easter+3) between () use (0.38, 60)

# on (thursday) between (20:00..21:52) use (8.2, 1)
