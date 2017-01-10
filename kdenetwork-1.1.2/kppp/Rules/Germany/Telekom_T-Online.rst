########################German-T-Online###################################
#                                                                        #
# German  T-Online Online Rules    at  01. April 1999                    #
#                                                                        #
# created  99/04/01 by Alexander Schmidt<AlexanderSchmidt@T-Online.de>   #
#                                                                        #
# corrected 99/04/05 by Wolf-RuedigerJuergens <WJuergens@t-online.de>    #
#                                                                        #
##########################################################################

name=German_T-Online 
currency_symbol=DM 
currency_position=right 
currency_digits=2 
per_connection=0.06 
minimum_costs=0.06 
default=(0.06, 60) 

# Montag bis Sonntag 
on (monday..sunday) between (0:00..23:59)   use (0.06, 60) 
