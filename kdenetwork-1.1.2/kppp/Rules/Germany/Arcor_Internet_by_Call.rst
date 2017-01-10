#################################################################################
# German Mannesmann Arcor - Internet by Call accounting rules
#
# Mannesmann Arcor - Internet by Call
#
# based on German Telekom City Call
#   created  97/07/18 by Stefan Markgraf <S_Markgraf@hotmail.com>
#   modified 98/05/01 by Stefan Troeger  <stefan.troeger@wirtschaft.tu-chemnitz.de>
#
# created 98/12/19 by Gregor Eikmeier <gregor.eikmeier@arcormail.de>
#   based on reduction of costs since 98/12/14 
#   look at http://www.arcor.de/tarife/internet.html)
# modified 99/05/22 by Olaf Doering <olaf.doering@muenster.de>
#
#################################################################################

name=German_Arcor_Internet_by_Call
currency_symbol=DM
currency_position=right 
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(0.06, 60)

# Einheitlich 6 Pfg. pro Minute
on (monday..sunday) between (0:00..23:59)  use (0.06, 60)

