################################################################
# kppp ruleset for Italy - 0 to 15km extra-urban call
# 
# Telecom Italia S.p.A.
# Tariffa: extraurbana (teleselezione) tra i 0 ed i 15 km
#
# by Michele Manzato (aragorn@resurrection.com)
################################################################

name=Italy_Telecom_0-15_km
currency_symbol=L.
currency_position=left 
currency_digits=0

# Al 13/06/1998 1 scatto = L. 127 + IVA (= L. 152.4)

# La chiamata extraurbana comporta due scatti per ogni chiamata, addebitati
# all'istante della risposta
per_connection=304.8 

# Tariffa				Costo
# ----------------------------------------------------------------
# Ordinaria				1 scatto ogni 75 secondi
# Ridotta serale/notturna e festiva 	1 scatto ogni 150 secondi

# Tariffa di default: ridotta serale/notturna e festiva.
default=(152.4, 150)

# Da lunedi` a venerdi`, durante il giorno, e la mattina del sabato e` in
# vigore la tariffa ordinaria.
on (monday..friday) between (8:00..18:30) use (152.4, 75)
on (saturday) between (8:00..13:00) use (152.4, 75)

# End of file
