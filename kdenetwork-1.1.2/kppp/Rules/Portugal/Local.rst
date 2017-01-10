################################################################
#
#	Portugal Telecom Ruleset for local calls without
#	activation charge
#	Valid from: 1 Fev 1998
#
#	Filipe Jorge Marques de Almeida
#	fjcfma@rnl.ist.utl.pt
#
################################################################

name=Telecom Local without Activation

currency_symbol=Esc
currency_position=right 
currency_digits=2

# No Activation
per_connection=0.0

# Economical ( yeah, right... ;)
default=(10.0, 360)

# Daylight rates
on (monday..friday) between (9:00..21:00) use (10.0, 180)

# Holydays
on (1/1, 2/24, 4/25, 5/1, 5/10, 6/11, 8/15, 10/5, 11/1, 12/1, 12/8, 12/25) between () use (10.0, 360)

