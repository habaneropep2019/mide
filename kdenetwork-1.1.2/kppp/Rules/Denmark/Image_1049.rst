##################################################################
# Danish rate ruleset for Image 1049.                            #
# Created 98.12.13 by Jens Kristian Søgaard <jks@distributed.dk> #
# May be distributed freely. I take no responsibility for        #
# the correctness of the information in this file.               #
##################################################################

name=Image_1049
currency_symbol=DKR
currency_position=right
currency_digits=2

# Initial expense: 0.1 DKR
# Expensive rate 0.22 DKR
# Cheap     rate 0.11 DKR
# Expensive rate weekdays + Saturday 06:00 - 20:00
# Cheap rate weekdays + Saturday 20:00 - 06:00 and the whole of Sunday

per_connection=0.1
minimum_costs=0.0

# Expensive rate (Calculated per second)
default=(0.0036666667, 1)

# Cheap rate (Calculated per second)
on (monday..saturday) between (20:00..05:59) use (0.0018333333, 1)
on (sunday) between () use (0.0018333333, 1)


