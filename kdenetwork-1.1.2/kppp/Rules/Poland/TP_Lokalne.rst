################################################################
# Regu³ki obliczania kosztów po³±czeñ lokalnych w sieci TP S.A.
# Obowi±zuj± od 1 lipca 1999
# autor: Jacek Stolarczyk <jacek@mer.chemia.polsl.gliwice.pl>
################################################################

name=TP_Lokalne
currency_symbol=PLN
currency_position=right
currency_digits=2
per_connection=0.0
minimum_costs=0.0

# Co 360 sekund dodawane bedzie 29 groszy do rachunku.
# Jest to tak zwana taryfa nocna.
default=(0.29, 360)
# niestety w ci±gu dnia czas miêdzy impulsami jest 2x krótszy
on (monday..sunday) between (8:00..21:59) use (0.29, 180)

