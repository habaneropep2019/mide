#####################################################################
# German NetCologne rate ruleset
# ------------------------------
# (probably only usefull for anybody living in Cologne)
# This ruleset can be used by NetCologne clients who ordered the package 
# "Time-Line" from NetCologne. For non-NetCologne customers the Telekom
# charge will be higher.
# The PowerLine 10,60,100 packages are a little bit different. You have to
# pay the online charge only if you used the dialup connection longer
# than the package-time you bought. Feel free to change the rules to fit
# your circumstances.
#
# *** for City calls and NetCologne users *** - Use at your own risk.
#     ( monthly package cost : 5,00 DM 			/ 1 month  )
#     ( online charge        : 0,03 DM 			/ 1 minute )
#     ( phone charge         : 0,02 DM 		 	/ 1 minute )
# Since 1.4.1999 there is no more difference between using the
# InterNetCologne between [08:00-18:00] or [18:00-08:00].
# Only the online and phone charges are counted.
# If you are still a Telekom user the phone charge is 0,03 DM / minute.
#                
# edited 20.5.99 <Roland Pabel (pabel@cip.physik.uni-bonn.de)>
#
# Obwohl NetCologne sekundengenau abrechnet, ist es viel einfacher
# in 6 Sekundenschritten abzurechnen. Der errechnete Betrag duerfte
# nur wenig ueber dem Realen liegen. Bisher nicht getestet!
#
# Alle Angaben ohne Gewaehr (besonders nicht von mir :-). 
# Infos zu NetCologne unter http://www.netcologne.de .
#
#####################################################################

name=German_NetCologne_Time-Line
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0

# default : Cost of 6 seconds in the time periods (Monday..Sunday, 0:00..23:59)
default=(0.005, 6)

# That's it, no further rules. :-)
