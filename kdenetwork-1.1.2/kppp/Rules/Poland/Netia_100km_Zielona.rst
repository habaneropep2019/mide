################################################################
# Regu³ki obliczania kosztów po³±czeñ miêdzymiastowych w sieci Netia,
# na odleg³o¶æ do 100 km miêdzy centralami wojewódzkimi.
# Taryfa zielona
# autor: Jacek Stolarczyk <jacek@mer.chemia.polsl.gliwice.pl>
################################################################

name=Netia_100km_Zielona
currency_symbol=PLN
currency_position=right
currency_digits=2
per_connection=0.0
minimum_costs=0.0
default=(0.66, 60)

# taryfa do 100km
on (monday..friday)  between (8:00..18:00)  use (0.66, 60)
on (monday..friday)  between (18:00..22:00)  use (0.49, 60)
on (monday..friday)  between (22:00..8:00)  use (0.33, 60)
on (saturday..sunday)  between (8:00..22:00)  use (0.49, 60)
on (saturday..sunday)  between (22:00..8:00)  use (0.33, 60)

# ¦wiêta (taryfa sobotnio-niedzielna)
on (01/01, easter, easter+1, 05/01, 05/03, 08/15, 11/01, 11/11, 12/25, 12/26) between (8:00..22:00) use (0.49, 60)
on (01/01, easter, easter+1, 05/01, 05/03, 08/15, 11/01, 11/11, 12/25, 12/26) between (22:00..8:00) use (0.33, 60)

#obja¶nienia dni ¶wi±tecznych
# 01/01		Nowy Rok
# easter		Niedziela Wielkanocna
# easter+1	Poniedzia³ek Wielkanocny
# 05/01		¦wiêto Pracy
# 05/03		Dzieñ Konstytucji 3 Maja
# 08/15		Wniebowziêcie
# 11/01		Wszystkich ¦wiêtych
# 11/11		¦wiêto Niepodleg³o¶ci
# 12/25		Bo¿e Narodzenie
# 12/26		Bo¿e Narodzenie
# niestety na li¶cie brakuje Bo¿ego Cia³a - nie wiem jaka regu³a rz±dzi jego
# wyborem tego czwartku
