################################################################
# kppp ruleset for Italy
# 
# Telecom Italia S.p.A.
# Tariffa: extraurbana (teleselezione) oltre i 60 km
#
# by Michele Manzato (aragorn@resurrection.com)
################################################################

name=Italy_Telecom_60+_km
currency_symbol=L.
currency_position=left 
currency_digits=0

# Al 13/06/1998 1 scatto = L. 127 + IVA (= L. 152.4)

# La chiamata extraurbana comporta due scatti per ogni chiamata, addebitati
# all'istante della risposta
per_connection=304.8 

# Tariffa				Costo
# ----------------------------------------------------------------
# Ordinaria				1 scatto ogni 22.5 secondi
# Ridotta serale/notturna e festiva 	1 scatto ogni 50 secondi

# Tariffa di default: ridotta serale/notturna e festiva.
default=(152.4, 50)
# Da lunedi` a venerdi`, durante il giorno, e la mattina del sabato e` in
# vigore la tariffa ordinaria.
on (monday..friday) between (8:00..18:30) use (304.8, 45)
on (saturday) between (8:00..13:00) use (304.8, 45)

# End of file
