################################################################
# Danish rate ruleset for regional calls (Fjernpris)           #
# Created 98.12.05 by Anders S. Jensen <asj@maersk-medical.com>#
# May be distributed freely. I take no responsibility for      #
# the correctness of the information in this file.             #
################################################################

name=Danish_Regional
currency_symbol=DKR
currency_position=right 
currency_digits=2

# (This is a modified copy of Teledanmark_Local.rst)
# This is how TeleDanmark calculates their horrible fees
# (As far as I can figure)
# 
# The calculations is based on an analog line connected through
# their digital services. The old analog switchboards prices
# is calculated slighly different, but they're almost obsolete now.
# (There is no initial expense, but local taxes are claimed as soon as
# you go off hook. Digital services claims the initial expense when
# you dial the first digit whether the call is actually made or not. 
# The minute taxes is accounted for when the call successfully goes through.)
# So here it is:
# Initial expense: 0.125 DKR
# Minute taxes in off-peak hours: 0.23 DKR/Min
# Minute taxes in peak hours: 0.46 DKR/Min
# Peak hours is 8.00 - 19.30 monday to saturday
# Off-peak hours is everything else, and on national holidays


# Initial expense is 0.125 DKR
per_connection=0.125
minimum_costs=0.0

# Off-peak: 0.23 DKR/Min (Calculated per second)
default=(0.003833, 1)

# Peak hours: 0.46 DKR/Min
on (monday..saturday) between (8:00..19:29) use (0.007666, 1)

# National Holidays:

# Newyear (0.23 DKR/Min)
on (01/01) between () use (0.003833, 1)
# "Skærtorsdag" (0.23 DKR/Min)
on (easter-3) between () use (0.003833, 1)
# "Langfredag" (0.23 DKR/Min)
on (easter-2) between () use (0.003833, 1)
# "2. påskedag" (0.23 DKR/Min)
on (easter+1) between () use (0.003833, 1)
# "St. bededag" (0.23 DKR/Min)
on (easter+26) between () use (0.003833, 1)
# "Kristi himmelfart" (0.23 DKR/Min)
on (easter+39) between () use (0.003833, 1)
# "2. pinsedag" (0.23 DKR/Min)
on (easter+50) between () use (0.003833, 1)
# Christmas day (0.23 DKR/Min)
on (12/25) between () use (0.003833, 1)
# 2.nd christmas day (0.23 DKR/Min)
on (12/26) between () use (0.003833, 1)
