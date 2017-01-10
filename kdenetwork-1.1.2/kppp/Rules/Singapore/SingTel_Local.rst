################################################################
# This is the rule set for Singapore, local SingTel charge
#
# Boh Cheh Wee <bohcw@singnet.com.sg>
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=SingTel_Local

################################################################
# currency settings
################################################################

# defines Sing Dollar to be used as currency
currency_symbol=S$

# Define the position of the currency symbol.
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

# minimum costs per connection. If the costs of a phone
# call are less than this value, this value is used instead
minimum_costs=0.0

# This is the default rule which is used when no other rule
# applies.
default=(0.00721,60)

# applies 8am until 6pm: every 30 seconds 0.721 cents
# are added to the bill
on () between (8:00..18:00) use(0.00721,30)

# applies 6pm until 8am: every 60 seconds 0.721 cents
# are added to the bill
on () between (18:00..7:59) use(0.00721,60)

# applies weekends: every 60 seconds 0.721 cents
# are added to the bill
on (saturday..sunday) between (00:00..23:59) use (0.00721, 60)