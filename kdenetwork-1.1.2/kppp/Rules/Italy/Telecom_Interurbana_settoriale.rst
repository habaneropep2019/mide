################################################################
# kppp ruleset for Italy - local sector call
# 
# Telecom Italia S.p.A.
# Tariffa: Interurbana settoriale 
#
# by Michele Manzato (aragorn@resurrection.com)
################################################################

name=Italy_Telecom_Interurbana_settoriale
currency_symbol=L.
currency_position=left 
currency_digits=0

# Al 13/06/1998 1 scatto = L. 127 + IVA (= L. 152.4)

# La chiamata urbana non comporta scatti all'istante della risposta
per_connection=0.0

# Tariffa				Costo
# ----------------------------------------------------------------
# Ordinaria				1 scatto ogni 3 m 40 s
# Ridotta serale/notturna e festiva 	1 scatto ogni 6 m 40 s

# Tariffa di default: ridotta serale/notturna e festiva.
default=(152.4, 400)

# Da lunedi` a venerdi`, durante il giorno, e la mattina del sabato e` in
# vigore la tariffa ordinaria.
on (monday..friday) between (8:00..18:30) use (152.4, 220)
on (saturday) between (8:00..13:00) use (152.4, 220)

# End of file
