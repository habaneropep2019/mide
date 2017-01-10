################################################################
#                                                              #
# Phone cost rules for Landsimi Islands  (Iceland Telecom)     #
#                                                              #
# Authors: Thorarinn R. Einarsson (thori@mindspring.com)       #
#          Bjarni R. Einarsson (bre@netverjar.is)              #
#                                                              #
# Created on February 2, 1999                                  #
# Last updated on 5. September, 1999                           #
#                                                              #
# Ath. Höfundar bera EKKI ábyrgð á röngum útreikningum !       #
#                                                              #
# Vitað er að útreikningar eru ónákvæmir, því Landssíminn      #
# rukkar fyrir skrefið, ekki mínútuna.  Þessi gjaldskrá miðast #
# hinsvegar við mínútur, og byggir á upplýsingum úr síma-      #
# skránni frá 1999.                                            #
#                                                              #
################################################################

###  Name of ruleset ###
name=Island_almennur_taxti

###  Currency symbol  ### (ekki það alþjóðlega heldur það sem birtist)
currency_symbol=kr.

# Define the position of the currency symbol.
currency_position=right 

# Define the number of significant digits.
currency_digits=2

################################################################
# connection settings
################################################################

# NOTE: rules are applied from top to bottom - the
#       LAST matching rule is the one used for the
#       cost computations.

#
# Kostnaður við tengingu  (1 skref)
#
 
per_connection=3.32

#
# Lágmarkskostnaður (þarf ekki að nota því 3.32 kemur á hverja tengingu)
#

minimum_costs=0.0

#
# Sléttur kostnaður er ekki notaður á Íslandi
#

flat_init_costs=(0.0,0.0)

#
# Sjálfgefinn kostnaður - Alla virka daga á daginn.
# Virka daga bætast 1.56 kr. við á hverjum 60 sekúndum
# (eða 128 sek. í skrefi)
#
default=(1.56, 60)


###################
# Aukareglurnar:  #################################
###################

#
# Kvöld og næturtaxti alla virka daga
#
# 0.78 kr. á mín. (eða 256 sek. í skrefi)
#

on (monday..friday) between (19:00..07:59) use (0.78, 60)

#
# Helgartaxti
#
# Sama og K&N taxti eða 0.78 kr. á mín.
#

on (saturday..sunday) between () use (0.78, 60)


#
# Hátíðisdagar:  Nota kvöld- og næturtaxta
#
# Jóladagur
#

on (12/25) between () use (0.78, 60)

#
# Hátíðisdagar nota kvöld- og næturtaxta
#
# Hér eru nokkrir...

on (12/1, 12/24..12/26, 12/31, 01/01, 05/01, 06/17) between () use (0.78, 60)

#
# og aðrir tengdir páskum
#

# Skírdagur
on (easter-3) between() use (0.78, 60)

# Föstudagurinn langi
on (easter-2) between() use (0.78, 60)

# Annar í Páskum
on (easter+1) between () use (0.78, 60)

# Hvítasunnudagur
on (easter+50) between () use (0.78, 60)
