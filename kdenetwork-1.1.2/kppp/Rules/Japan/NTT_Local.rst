################################################################
# This is the rule set for Japan, local NTT charge
#
# Jacek Cwielong <cwielong@annie.co.jp>
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=NTT-Local

################################################################
# currency settings
################################################################

# defines JPY (Japanese Yen) to be used as currency
currency_symbol=JPY

# Define the position of the currency symbol.
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

# minimum costs per connection. If the costs of a phone
# call are less than this value, this value is used instead
minimum_costs=0.0

# This is the default rule which is used when no other rule
# applies. The first component "10" is the price of one
# "unit", while "180" is the duration in seconds.
# Therefore the following rule means: "Every 180 seconds
# 10 YPJ are added to the bill"
default=(10,180)

# applies 8am until 11pm: every 180 seconds 10 JPY
# are added to the bill
on () between (8:00..22:59) use(10,180)

# applies 23pm until 8am: every 240 seconds 10 JPY
# are added to the bill
on () between (23:00..7:59) use(10,240)
