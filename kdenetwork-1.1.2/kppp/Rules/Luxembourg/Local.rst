################################################################
#
# Regles pour P & T Luxembourg
# Appels locaux
#
# Fichier original par
# $Id: Local.rst,v 1.1 1998/12/08 10:53:42 mario Exp $
# (C) 1998 Patrick Vande Walle <patrick.vande-walle@cec.be>
#
################################################################

# Les nouveaux tarifs de P & T Luxembourg sont en vigueur
# depuis  ???.
#
# D'apres P & T Luxembourg, une unite de communication 
# devrait couter:
#
# 5 LUF TVA Comprise.
#
# Il y a trois periodes tarifaitaires: 4, 8 ou 16 minutes
#
# Il n'y a pas de zones telephoniques au Grand-Duche

name=Luxembourg_Area
currency_symbol=LUF
currency_position=right
currency_digits=2

################################################################
# Parametres de connection
################################################################

# Ceci est un cout supplementaire eventuel par appel. Si vous
# n'etes pas concerne, posez le egal à zero ou commentez-le.

per_connection=0.0

# Frais minimum par appel. Si le cout d'un appel est inferieur
# à cette valeur, alors cette derniere est le cout retenu.

minimum_costs=0.0

# C'est ce que vous payez pour les quatre premieres minutes de
# connexion, peu importe si l'appel dure 1 ou 480 secondes.

flat_init_costs=(5,240)

# Ceci est la regle utilisee par defaut lorsqu'aucune autre ne
# s'applique. Le premier nombre est le prix correspondant à la
# duree en secondes qui est le second nombre.

default=(5, 240)

# Regles tarifaires

on (monday..sunday)   between (23:00..06:00) use (5, 960)
on (monday..friday)   between (08:00..19:00) use (5, 240)
on (monday..friday)   between (19:00..23:00) use (5, 480)
on (monday..friday)   between (06:00..08:00) use (5, 480)
on (saturday)         between (6:00..23:00) use (5, 480)
on (sunday)           between (00:00..23:59) use (5, 960)

# Jours feries

on (01/01, easter+1, 05/01, 05/08, easter+38, easter+50, 06/23, 08/15, 11/01, 12/25) between (00:00..23:59) use (5, 960)