################################################################
# This is the cost rule for local calls in Malaysia, if you
# using Telekom, TMnet or Jaring  as of 03/01/98.
# This cost rule include only Dial-Up charge from Telekom and 
# not Access charges from both Jaring or TMnet
#	Acess 	: RM0.01 per minute (from TMnet and Jaring)
#	Dial-up	: RM0.015 per minute (from Telekom)
#	Total	: RM0.025 per minute
# Or in other words the cost rule only calculate phone bill !
#
# Choong Hong Cheng
# chc@tm.net.my OR chc@rocketmail.com
################################################################


################################################################
#
# NAME OF THE RULESET. This is NEEDED for accounting purposes.
#
################################################################
name=Malaysia_Telekom

################################################################
# currency settings
################################################################

currency_symbol=RM
currency_position=left 
currency_digits=2

################################################################
# connection settings
################################################################

per_connection=0.015
minimum_costs=0.015
flat_init_costs=(0.015,60)
default=(0.015, 60)
