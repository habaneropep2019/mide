################################################################
# Ruleset for Romanian RomTelecom ([22%] TVA included )
# written by Marian Radulescu <marian-r@usa.net>
# 07 Apr 1998
################################################################
name=RomTelecom_Interurban
currency_symbol=Lei
currency_position=right
currency_digits=2
per_connection=0.0
minimum_costs=2342.40
default=(2342.40, 60)
on (monday..friday) between (07:00..09:59) use (1756.80,60)
on (monday..friday) between (10:00..15:59) use (2342.40,60)
on (monday..friday) between (16:00..18:59) use (1756.80,60)
on (monday..friday) between (19:00..20:59) use (2342.40,60)
on (monday..friday) between (21:00..22:59) use (1756.80,60)
on (monday..friday) between (23:00..06:59) use (1171.20,60)
on (saturday..sunday) between (07:00..22:59) use (1756.80,60)
on (saturday..sunday) between (23:00..06:59) use (1171.20,60)
