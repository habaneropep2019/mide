
# $Id: Makefile,v 1.1.1.1 1997/07/04 21:23:49 kulow Exp $

all:	bwlednum.o example

bwlednum.o: bwlednum.h bwlednum.cpp
	moc bwlednum.h -o bwlednum.moc
	gcc  -I$(QTDIR)/include -c bwlednum.cpp

example: bwlednum.o example.cpp 
	moc example.cpp -o example.moc
	gcc  -I$(QTDIR)/include -c example.cpp 
	gcc example.o bwlednum.o -o example -lqt

clean: 
	rm -f *~ *.o *.moc example
