#################################################################
# Danish rate ruleset for Favorite Internet at Tele Danmark     #
# Created 99.08.08 by Peter Thorsager <thorsager@post.tele.dk>  #
# Since the ISP market changes fast these rates may be outdated #
# May be distributed freely. I take no responsibility for       #
# the correctness of the information in this file.              #
#################################################################

name=DK_TDK_FI
currency_symbol=kr
currency_position=right 
currency_digits=2

# Tele Danmark Favorit Internet Prices
# Initial expense: 0.25 DKR
# Minute taxes in off-peak hours: 0.1 kr/min - reduced to 0.08 kr/min after 1 hour
# Minute taxes in reduced peak hours: 0.20 kr/min
# Minute taxes in peak hours: 0.29 kr/min
# Reduced peak hours is 16:00 - 19:30 monday to friday &  8:00 - 16:00 on saturdays
# Peak hours is 8:00 - 19:30 monday to friday
# Off-peak hours is everything else, and on national holidays


# Initial expense is 0.25 DKR
per_connection=0.25
minimum_costs=0.0

# Off-peak: 0.1 kr/min (per second) and 0.08kr/min +1 hour connect (per second)
default=(0.001667, 1)
default=(0.001333, 1, 3600)

# Peak hours: 0.29 kr/min (per second)
on (monday..friday) between (8:00..15:59) use (0.004833, 1)

# Reduced peak hours: 0.20 kr/min (per second)
on (monday..friday) between (16:00..19:29) use (0.003333, 1)
on (saturday..saturday) between (8:00..15:59) use (0.003333, 1)


# National Holidays ( like off-peak hours )

# Newyear
on (01/01) between () use (0.001667, 1)
on (01/01) between () use (0.001333, 1, 3600)

# "Skærtorsdag"
on (easter-3) between () use (0.001667, 1)
on (easter-3) between () use (0.001667, 1, 3600)

# "Langfredag"
on (easter-2) between () use (0.001667, 1)
on (easter-2) between () use (0.001667, 1, 3600)

# "2. påskedag"
on (easter+1) between () use (0.001667, 1)
on (easter+1) between () use (0.001667, 1, 3600)

# "St. bededag"
on (easter+26) between () use (0.001667, 1)
on (easter+26) between () use (0.001667, 1, 3600)

# "Kristi himmelfart"
on (easter+39) between () use (0.001667, 1)
on (easter+39) between () use (0.001667, 1, 3600)

# "2. pinsedag"
on (easter+50) between () use (0.001667, 1)
on (easter+50) between () use (0.001667, 1, 3600)

# Christmas day
on (12/25) between () use (0.001667, 1)
on (12/25) between () use (0.001667, 1, 3600)

# 2.nd christmas day
on (12/26) between () use (0.001667, 1)
on (12/26) between () use (0.001667, 1, 3600)
