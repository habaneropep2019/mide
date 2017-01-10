################################################################
#
# This is the cost rule for local calls in Argentina, if your
# carrier is Telecom, as of 03/01/98
# 
# Sergio Daniel Freue
# freue@ccurq.uba.ar
################################################################

name=Argentina_Local_Telecom

################################################################
# currency settings
################################################################
currency_symbol=$
currency_position=left
currency_digits=2

################################################################
# connection settings
################################################################
default=(0.054664, 120)

on (monday..saturday) between (00:00..07:59) use (0.054664, 240)
on (monday..friday)   between (10:00..13:29) use (0.054664, 90)
on (monday..friday)   between (22:00..23:59) use (0.054664, 240)
on (saturday)         between (13:00..23:59) use (0.054664, 240)
on (sunday)           between (00:00..23:59) use (0.054664, 240)

