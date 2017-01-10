# I don't know if it's the same rule for the rest of Brazil!
# Cobranca telefonica da Telebahia, Brasil. Nao sei se a cobranca e a mesma
# no resto do Brasil. Nao estranho o custe de 0 aos sabados e domingos.
# Nestes horarios, cobra-se apenas um pulso (8 centavos) por ligacao. Como
# este valor eh o custo minimo, ele nao cobra nada pelo resto da ligacao.
#
#   Marcus Brito
#   Mailto: sadpazu@base.com.br

name=Telebahia
currency_position=right 
currency_digits=3
per_connection=0.0
minimum_costs=0.08
default=(0.08, 240)

on (monday..friday) between (0:00..05:59) use (0.0, 100)
on (saturday) between (14:00..23:59) use (0.0, 100)
on (sunday) between (00:00..23:59) use (0.0, 100)


