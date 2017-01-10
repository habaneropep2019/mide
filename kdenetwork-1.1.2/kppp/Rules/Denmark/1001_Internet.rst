##################################################################
# Danish rate ruleset for 1001 Internet.                         #
# Created 98.12.13 by Jens Kristian Søgaard <jks@distributed.dk> #
# May be distributed freely. I take no responsibility for        #
# the correctness of the information in this file.               #
##################################################################

name=1001_Internet
currency_symbol=DKR
currency_position=right
currency_digits=2

# Initial expense: 0.25 DKR
# Expensive rate 0.27 DKR
# Mid       rate 0.20 DKR
# Cheap     rate 0.10 DKR

per_connection=0.25
minimum_costs=0.0

# Expensive rate (Calculated per second)
default=(0.0045, 1)

# Mid rate (Calculated per second)
on (monday..friday) between (16:00..18:59) use (0.00333333, 1)
on (saturday) between (08:00..15:59) use (0.00333333, 1)

# Cheap rate (Calculated per second)
on (monday..friday) between (19:00..07:59) use (0.0016666667, 1)
on (saturday) between (16:00..07:59) use (0.0016666667, 1)
on (sunday) between () use (0.001666667, 1)

# National Holidays:

# Newyear (0.1 DKR/Min)
on (01/01) between () use (0.00166667, 1)
# "Skærtorsdag" (0.1 DKR/Min)
on (easter-3) between () use (0.00166667, 1)
# "Langfredag" (0.1 DKR/Min)
on (easter-2) between () use (0.00166667, 1)
# "2. påskedag" (0.1 DKR/Min)
on (easter+1) between () use (0.00166667, 1)
# "St. bededag" (0.1 DKR/Min)
on (easter+26) between () use (0.0016667, 1)
# "Kristi himmelfart" (0.145 DKR/Min)
on (easter+39) between () use (0.0016667, 1)
# "2. pinsedag" (0.1 DKR/Min)
on (easter+50) between () use (0.0016667, 1)
# Christmas day (0.1 DKR/Min)
on (12/25) between () use (0.00166667, 1)
# 2.nd christmas day (0.1 DKR/Min)
on (12/26) between () use (0.00166667, 1)
