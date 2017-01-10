################################################################
#
# Règles pour Mauritius Telecoms
# Appels locaux
#
# Fichier original par
# $Id: Mauritius_Telecoms_Internet.rst,v 1.1.2.1 1999/07/03 19:18:46 porten Exp $
# (C) 1997 Czo <sirol@asim.lip6.fr>
#
# Modifié par Fabrice Eudes <eudes@gat.univ-lille1.fr>
# le 21 Decembre 1997.
#
#
# Modifié par Alain Bastien <alain_bastien@hotmail.com>
################################################################

# Les nouveaux tarifs de Mauritius Telecoms sont en vigueur
# depuis le 1er Juillet 1999.
#

name=Mauritius_Telecoms_Internet
currency_symbol=Rs
currency_position=left
currency_digits=2


################################################################
# Paramètres de connection
################################################################

# NOTE: les règles sont parcourues du dèbut vers la fin du fichier
#       donc seulement la dernière règle appropriée est prise en
#       considération. Placez donc les règles les plus générales
#       avant les plus spécifiques.

# Ceci est un coût supplémentaire éventuel par appel. Si vous
# n'êtes pas concerné, posez le égal à zéro ou commentez-le.
# per_connection=0.0

# Frais minimum par appel. Si le coût d'un appel est inférieur
# à cette valeur, alors cette dernière est le coût retenu.
minimum_costs=0.0

# C'est ce que vous payez pour les trois premières minutes de
# connexion, peu importe si l'appel dure 1 ou 179 secondes.
# flat_init_costs=(0.74,180)

# Ceci est la règle utilisée par défaut lorsqu'aucune autre ne
# s'applique. Le premier nombre est le prix correspondant à la
# durée en secondes qui est le second nombre.
default=(1.50, 60)
# Ceci n'engage que moi, mais si toutes les heures de la semaine
# sont incluses dans les règles ci-dessous; quelle est l'utilité
# de ce paramètre ?!?!

on (monday..sunday) between (07:00..18:59) use (1.50, 60)
on (monday..sunday)   between (19:00..23:59) use (1.00, 60)
on (monday..sunday)   between (00:00..06:59) use (0.50, 60)

# Jours fériés :-))



