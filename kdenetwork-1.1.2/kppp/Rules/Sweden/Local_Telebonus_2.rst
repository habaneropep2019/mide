######################################################################
# Swedish rate ruleset for local calls /w Telebonus 2 (15% discount) #
# Created 1998-04-26 by Anders Widell  <d95-awi@nada.kth.se>         #
# May be distributed freely. I take no responsibility for            #
# the correctness of the information in this file.                   #
######################################################################

name=Swedish_Local_Telebonus_2
currency_symbol=SEK
currency_position=right 
currency_digits=2

# Så här beräknar Telia samtalskostnaden (enligt vad jag förstår...)
#
# * en öppningsavgift debiteras för alla samtal så fort man fått svar
# * därefter beräknas kostnaden per sekund
#
# Kostnaden per sekund beror av tiden:
# * hög taxa vardagar kl 08:00-18:00
# * låg taxa övrig tid
#
# Som vardagar räknas måndag till fredag. Dock ej:
# * röda dagar i almanackan
# * Vissa övriga dagar. Vilka dessa är verkar något oklart, och kan kanske
#   variera från år till år efter Telias tycke och smak. Men en tumregel är
#   att "aftnar" typ julafton och nyårsafton inte räknas som vardagar.


# Öppningsavgift: 34 öre
per_connection=0.34
minimum_costs=0.0

# Låg taxa: 9,775 öre/min (räknat per sekund)
default=(0.009775, 6)

# Hög taxa: 19,55 öre/min måndag-fredag 8-18
on (monday..friday) between (8:00..17:59) use (0.009775, 3)

# Röda helgdagar (de som infaller på en lördag/söndag utelämnade)
# nyårsdagen
on (01/01) between () use (0.009775, 6)
# trettondag jul
on (01/06) between () use (0.009775, 6)
# långfredagen
on (easter-2) between () use (0.009775, 6)
# annandag påsk
on (easter+1) between () use (0.009775, 6)
# första maj
on (05/01) between () use (0.009775, 6)
# kristi himmelfärds dag
on (easter+39) between () use (0.009775, 6)
# annandag pingst
on (easter+50) between () use (0.009775, 6)
# juldagen
on (12/25) between () use (0.009775, 6)
# annandag jul
on (12/26) between () use (0.009775, 6)

# "aftnar" (osäkert om följande är korrekt & fullständigt)
# trettondagsafton
on (01/05) between () use (0.009775, 6)
# julafton
on (12/24) between () use (0.009775, 6)
# nyårsafton
on (12/31) between () use (0.009775, 6)
