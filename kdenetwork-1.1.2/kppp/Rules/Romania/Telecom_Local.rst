################################################################
# Ruleset for Romanian RomTelecom ( [22%] TVA included )
# written by Marian Radulescu <marian-r@usa.net>
# 29 Ian 1998
################################################################
name=RomTelecom_Local
currency_symbol=Lei
currency_position=right
currency_digits=2
per_connection=0.0
minimum_costs=256.20
default=(256.20, 60)
on (monday..friday) between (07:00..09:59) use (128.10,60)
on (monday..friday) between (10:00..15:59) use (256.20,60)
on (monday..friday) between (16:00..18:59) use (128.10,60)
on (monday..friday) between (19:00..20:59) use (256.20,60)
on (monday..friday) between (21:00..22:59) use (128.10,60)
on (monday..friday) between (23:00..06:59) use ( 64.05,60)
on (saturday..sunday) between (07:00..22:59) use (128.10,60)
on (saturday..sunday) between (23:00..06:59) use ( 64.05,60)
