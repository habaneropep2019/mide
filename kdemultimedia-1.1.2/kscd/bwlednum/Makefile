
# $Id$

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
