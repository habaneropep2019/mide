################################################################
#
# kppp ruleset for Italy
# by Michele Manzato (aragorn@resurrection.com)
#
# Telecom_0-15km.rst
# Comunicazioni interurane entro i 15km // Inter-urban calls 
# within a distance of 15km 
# con // with Telecom Italia S.p.A.
# 
# Le comunicazioni telefoniche interurbane sono quelle che si 
# svolgono tra abbonati di reti urbane diverse. Questo ruleset
# si applica per chiamate interurbane entro i 15km.
#
# 'Inter-urban' calls occur when the caller and the receiver 
# belong to different urban networks. This ruleset applies to 
# inter-urban calls within a distance of 15km.
#
################################################################

# Nome del ruleset // Ruleset name
name=Telecom_0-15km

# Impostazioni della valuta // Currency settings
currency_symbol=L.
currency_position=left 
currency_digits=0

# Settimane normali // Normal weeks 
per_connection=0.0
default=(152.4, 150)
on (monday..friday) between (8:00..18:30) use (152.4, 75)
on (saturday) between (8:00..13:00) use (152.4, 75)

# Nelle festivita' (Natale, S.Stefano, Capodanno, Pasquetta, ecc) si applica
# la tariffa minima // On bank holidays (Christmas Day, Boxing Day, New Year's 
# Day, etc.) the minimum charge rate applies 
on (01/01) between () use (152.4, 150)
on (01/06) between () use (152.4, 150)
on (04/25) between () use (152.4, 150)
on (05/01) between () use (152.4, 150)
on (08/15) between () use (152.4, 150)
on (11/01) between () use (152.4, 150)
on (12/08) between () use (152.4, 150)
on (12/25) between () use (152.4, 150)
on (12/26) between () use (152.4, 150)
on (easter + 1) between () use (152.4, 150)

# Nota: IVA inclusa // Note: VAT is included

# Fine // End of file
