################################################################
#
# 27 Oct 1997 (Valid until who knows?)
#
# I TAKE NO RESPONSIBILITY FOR THE VALIDITY AND ACCURACY OF
# THIS INFORMATION, IF YOU ARE SO CONCERNED PLEASE CONTACT
# YOUR OWN TELEPHONE PROVIDER AND CONFIGURE THIS YOURSELF.
# AND DO NOT USE THIS FILE.
#
#
# SameTelco as a generic name (???) for BirminghamCable to
#  BirminghamCable call.  i.e. within the "same telephone
#  company".
#
#
# Darryl L Miles
# <dlm@g7led.demon.co.uk>
#
# Under 35 miles (approx)
# British_BirminghamCable_SameTelco
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=British_BirminghamCable_SameTelco


################################################################
# currency settings
################################################################

# 1 pound sterling = 100 pence sterling
# 1 pence sterling is our lowest unit of coinage, but
#  I suspect telephone companys may remember the
#  fractions of pence for billing.
#
# Our monetry format:
#
# e.g. £1.57 = 1 pound and 57 pence.
# e.g. 157p = 1 pund and 57 pence.
#
# defines UKP (PoundSterling) to be used as currency
# symbol (not absolutely needed, default = "$")
currency_symbol=£

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
minimum_costs=0.04

# This is the default rule which is used when no other rule
# applies. The first component "0.033" is the price of one
# "unit", while "60" is the duration in seconds.
# Therefore the following rule means: "Every 60 seconds 0.033
# PoundsSterling are added to the bill"
#
# I've simply taken the worst case price for this, peak time
#  calls.  Just in case I miss a period of time out.
default=(0.033, 60)

################################################################
# charge bands
################################################################

# We are actually charged to the second, but I don't really
#  need the PPP program to be so accurate myself (all that
#  CPU power working out costs, when it can be browsing the
#  Web, ho ho ho :-).  To the nearest 10 (or so) seconds is
#  fine for me.

# Charge:	3.3p/min (peak time)
# Min Charge:	4p/call
# Unit Time:	per second
#
on (monday..friday) between (8:00..17:59) use (0.0055, 10)

# Change:	1.5p/min (off-peak time)
# Min Charge:	4p/call
# Unit Time:	per second
#
on (monday..friday) between (18:00..7:59) use (0.0025, 10)

# Charge:	0.5p/min (weekend rate)
# Min Charge:	4p/call
# Unit Time:	per second
#
on (saturday..sunday) between () use (0.001, 12)
