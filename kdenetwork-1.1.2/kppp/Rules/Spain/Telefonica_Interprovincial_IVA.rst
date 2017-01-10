################################################################
#
# Telefonica at Spain
#
################################################################
name=Spain Telefonica Interprovincial IVA

################################################################
# currency settings
################################################################
currency_symbol=PTS
currency_position=right 
currency_digits=2


################################################################
# connection settings
################################################################

per_connection=17.4
minimum_costs=17.4

# Por Defecto reducida
default=(0.2018, 1)

# Tarifa Normal
on (monday..friday) between (17:00..21:59) use (0.4836, 1)

# Tarifa Punta
on (monday..friday) between (8:00..16:59) use (0.7627, 1)
on (saturday) between (8:00..13:59) use (0.7627, 1)

# Festivos: Reducida
on (1/1) between () use (0.2018, 1)
on (1/6) between () use (0.2018, 1)
on (5/1) between () use (0.2018, 1)
on (10/12) between () use (0.2018, 1)
on (11/1) between () use (0.2018, 1)
on (12/6) between () use (0.2018, 1)
on (12/8) between () use (0.2018, 1)
on (12/25) between () use (0.2018, 1)
on (easter) between () use (0.2018, 1)
on (easter+50) between () use (0.2018, 1)
