################################################################
#
# kppp ruleset for Italy
# by Michele Manzato (aragorn@resurrection.com)
#
# Telecom_TUT.rst
# Comunicazioni urbane e settoriali / Urban and sectorial calls
# con / with Telecom Italia S.p.A.
# 
# Le comunicazioni telefoniche urbane sono quelle che si svolgono 
# tra abbonati della stessa rete urbana. Per conoscere la rete 
# urbana di appartenenza basta consultare la prima pagina 
# dell'elenco telefonico del proprio comune.
#
# 'Urban' call rates apply when both the caller and the receiver 
# belong to the same urban network. To know which is your urban 
# network, consult the phone directory at your town's first 
# page.
#
################################################################


# Nome del ruleset // Ruleset name
name=Telecom_TUT

# Impostazioni della valuta // Currency settings
currency_symbol=L.
currency_position=left 
currency_digits=0

# Settimane normali // Normal weeks 
per_connection=0.0
default=(152.4, 400)
on (monday..friday) between (8:00..18:30) use (152.4, 220)
on (saturday) between (8:00..13:00) use (152.4, 220)

# Nelle festivita' (Natale, S.Stefano, Capodanno, Pasquetta, ecc) si applica
# la tariffa minima // On bank holidays (Christmas Day, Boxing Day, New Year's 
# Day, etc.) the minimum charge rate applies 
on (01/01) between () use (152.4, 400)
on (01/06) between () use (152.4, 400)
on (04/25) between () use (152.4, 400)
on (05/01) between () use (152.4, 400)
on (08/15) between () use (152.4, 400)
on (11/01) between () use (152.4, 400)
on (12/08) between () use (152.4, 400)
on (12/25) between () use (152.4, 400)
on (12/25) between () use (152.4, 400)
on (easter + 1) between () use (152.4, 400)

# Nota: IVA inclusa // Note: VAT is included 

# Fine // End of file
