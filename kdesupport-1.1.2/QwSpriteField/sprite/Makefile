POVRAY=x-povray  # Must be Povray 3.0
LDLIBS=-lpng -lz -lm

all: sprite.pov pngtoppmpgm
	$(POVRAY) sprite.ini

clean:
	rm -f *~ pngtoppmpgm

clobber: clean
	rm -f *.ppm *.pbm
