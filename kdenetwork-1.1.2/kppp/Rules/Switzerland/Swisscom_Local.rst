# Calls within 10 km in Switzerland
# Created by J. Wezel (jwezel@access.ch)
# Sun Sep 14 01:30:33 1997
# Changed by Daniel Brönnimann (dbroenni@g26.ethz.ch)
# Thu Nov 19 1998
# Valid from 1-Aug-97

name=Swisscom_Local_2

currency_symbol=CHF

currency_position=left
currency_digits=2
per_connection=0.0  
minimum_costs=0.00   
default=(0.10, 90)

# Normal
on (monday..friday) between (8:00..17:00) use (0.10, 90)
on (monday..friday) between (19:00..21:00) use (0.10, 90) 

# Low
on (monday..friday) between (6:00..8:00) use (0.10, 180)
on (monday..friday) between (17:00..19:00) use (0.10, 180)
on (monday..friday) between (21:00..23:00) use (0.10, 180)
on (saturday..sunday) between (6:00..23:00) use (0.10, 180)
on (01/01, 02/01, easter-2, easter+1) between (6:00..23:00) use(0.10, 180) 
on (easter+39, easter+50, 01/08, 12/25,12/26) between (6:00..23:00) use (0.10, 180)

# Night
on (monday..sunday) between (23:00..0:00) use (0.10, 360)
on (monday..sunday) between (0:00..6:00) use (0.10, 360)

