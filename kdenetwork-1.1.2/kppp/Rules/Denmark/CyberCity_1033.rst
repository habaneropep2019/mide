##################################################################
# Danish rate ruleset for CyberCity 1033.                        #
# Created 98.12.13 by Jens Kristian Søgaard <jks@distributed.dk> #
# May be distributed freely. I take no responsibility for        #
# the correctness of the information in this file.               #
##################################################################

name=CyberCity_1033
currency_symbol=DKR
currency_position=right
currency_digits=2

# Initial expense: 0.2 DKR
# Expensive rate 0.28 DKR
# Cheap     rate 0.14 DKR
# Expensive rate weekdays 08:00 - 16:00
# Cheap rate weekdays 16:00 - 08:00 and the whole of Saturday and Sunday

per_connection=0.2
minimum_costs=0.0

# Expensive rate (Calculated per second)
default=(0.004666667, 1)

# Cheap rate (Calculated per second)
on (monday..friday) between (16:00..07:59) use (0.00233333333, 1)
on (saturday..sunday) between () use (0.00233333333, 1)
