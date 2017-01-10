################################################################
#
# This is a accounting set for Telkom South Africa
# Please note that the costs used are those corresponding to
# the '97/'98 directory and are based on local phone calls
# being made. Strictly speaking, the call charges are not 31c
# but 30,9c per unit. The odd cost is due to the inclusion
# of VAT. I've rounded it off to keep things simple.
#
# The postfix 'A'..'D' is related to the distance maps
# and will affect the duration of 1 call unit
# 
# Distance Table
# Type	 Distance     Std Time   CallMore Time
# ----   --------     --------   -------------
#  A      0-50 km     180 sec    480 sec
#  B     >50-100 km     37,6     75,2
#  C     >100-200 km    18,8     45,2
#  D     >200 km        13,6     28,8
#
# South African residents, please note the following:
#  - Consult your local directory to determine which distance type
#    to use
#  - Most ISP providers in S.A. have pop's (point of pressence) in
#    the major cities, so it's very likely type A will be applicable
#  - If unsure, contact a local Telkom office in your area  
#
# Any problems, please email me
# Regards
#
# Jacques Eloff, 
# email: repstosd@global.co.za
#
################################################################

################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=South_African_Distance_A

################################################################
# currency settings
################################################################

# Currency symbol R ('Rand') eg, R 1.20 would imply 1 Rand and 20 cents
currency_symbol=R
currency_position=left
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

flat_init_costs=(0.31,180)
default=(0.31,180)

#
# more complicated rules:
#

# This is used for the so called 'CallMore Time' schedule that was
# introduced by Telkom.
on (saturday..sunday) between (0:00..23:59) use (0.31,480)
on (monday..friday) between (0:00..06:59) use (0.31,480)
on (monday..friday) between (19:00..23:59) use (0.31,480)

# This is used for the so called 'Normal Time' schedule
on (monday..friday) between (07:00..18:59) use (0.31,180)
################################################################
#
# This is a accounting set for Telkom South Africa
# Please note that the costs used are those corresponding to
# the '97/'98 directory and are based on local phone calls
# being made. Strictly speaking, the call charges are not 31c
# but 30,9c per unit. The odd cost is due to the inclusion
# of VAT. I've rounded it off to keep things simple.
#
# The postfix 'A'..'D' is related to the distance maps
# and will affect the duration of 1 call unit
# 
# Distance Table
# Type	 Distance     Std Time   CallMore Time
# ----   --------     --------   -------------
#  A      0-50 km     180 sec    480 sec
#  B     >50-100 km     37,6     75,2
#  C     >100-200 km    18,8     45,2
#  D     >200 km        13,6     28,8
#
# South African residents, please note the following:
#  - Consult your local directory to determine which distance type
#    to use
#  - Most ISP providers in S.A. have pop's (point of pressence) in
#    the major cities, so it's very likely type A will be applicable
#  - If unsure, contact a local Telkom office in your area  
#
# Any problems, please email me
# Regards
#
# Jacques Eloff, 
# email: repstosd@global.co.za
#
################################################################

################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=South_African_Distance_B

################################################################
# currency settings
################################################################

# Currency symbol R ('Rand') eg, R 1.20 would imply 1 Rand and 20 cents
currency_symbol=R
currency_position=left
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

flat_init_costs=(0.31,37.6)
default=(0.31,37.6)

#
# more complicated rules:
#

# This is used for the so called 'CallMore Time' schedule that was
# introduced by Telkom.
on (saturday..sunday) between (0:00..23:59) use (0.31,75.2)
on (monday..friday) between (0:00..06:59) use (0.31,75.2)
on (monday..friday) between (19:00..23:59) use (0.31,75.2)

# This is used for the so called 'Normal Time' schedule
on (monday..friday) between (07:00..18:59) use (0.31,37.6)
################################################################
#
# This is a accounting set for Telkom South Africa
# Please note that the costs used are those corresponding to
# the '97/'98 directory and are based on local phone calls
# being made. Strictly speaking, the call charges are not 31c
# but 30,9c per unit. The odd cost is due to the inclusion
# of VAT. I've rounded it off to keep things simple.
#
# The postfix 'A'..'D' is related to the distance maps
# and will affect the duration of 1 call unit
# 
# Distance Table
# Type	 Distance     Std Time   CallMore Time
# ----   --------     --------   -------------
#  A      0-50 km     180 sec    480 sec
#  B     >50-100 km     37,6     75,2
#  C     >100-200 km    18,8     45,2
#  D     >200 km        13,6     28,8
#
# South African residents, please note the following:
#  - Consult your local directory to determine which distance type
#    to use
#  - Most ISP providers in S.A. have pop's (point of pressence) in
#    the major cities, so it's very likely type A will be applicable
#  - If unsure, contact a local Telkom office in your area  
#
# Any problems, please email me
# Regards
#
# Jacques Eloff, 
# email: repstosd@global.co.za
#
################################################################

################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=South_African_Distance_C

################################################################
# currency settings
################################################################

# Currency symbol R ('Rand') eg, R 1.20 would imply 1 Rand and 20 cents
currency_symbol=R
currency_position=left
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

flat_init_costs=(0.31,18.8)
default=(0.31,18.8)

#
# more complicated rules:
#

# This is used for the so called 'CallMore Time' schedule that was
# introduced by Telkom.
on (saturday..sunday) between (0:00..23:59) use (0.31,45.2)
on (monday..friday) between (0:00..06:59) use (0.31,45.2)
on (monday..friday) between (19:00..23:59) use (0.31,45.2)

# This is used for the so called 'Normal Time' schedule
on (monday..friday) between (07:00..18:59) use (0.31,18.8)
################################################################
#
# This is a accounting set for Telkom South Africa
# Please note that the costs used are those corresponding to
# the '97/'98 directory and are based on local phone calls
# being made. Strictly speaking, the call charges are not 31c
# but 30,9c per unit. The odd cost is due to the inclusion
# of VAT. I've rounded it off to keep things simple.
#
# The postfix 'A'..'D' is related to the distance maps
# and will affect the duration of 1 call unit
# 
# Distance Table
# Type	 Distance     Std Time   CallMore Time
# ----   --------     --------   -------------
#  A      0-50 km     180 sec    480 sec
#  B     >50-100 km     37,6     75,2
#  C     >100-200 km    18,8     45,2
#  D     >200 km        13,6     28,8
#
# South African residents, please note the following:
#  - Consult your local directory to determine which distance type
#    to use
#  - Most ISP providers in S.A. have pop's (point of pressence) in
#    the major cities, so it's very likely type A will be applicable
#  - If unsure, contact a local Telkom office in your area  
#
# Any problems, please email me
# Regards
#
# Jacques Eloff, 
# email: repstosd@global.co.za
#
################################################################

################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=South_African_Distance_D

################################################################
# currency settings
################################################################

# Currency symbol R ('Rand') eg, R 1.20 would imply 1 Rand and 20 cents
currency_symbol=R
currency_position=left
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

flat_init_costs=(0.31,13.6)
default=(0.31,13.6)

#
# more complicated rules:
#

# This is used for the so called 'CallMore Time' schedule that was
# introduced by Telkom.
on (saturday..sunday) between (0:00..23:59) use (0.31,28.8)
on (monday..friday) between (0:00..06:59) use (0.31,28.8)
on (monday..friday) between (19:00..23:59) use (0.31,28.8)

# This is used for the so called 'Normal Time' schedule
on (monday..friday) between (07:00..18:59) use (0.31,13.6)
