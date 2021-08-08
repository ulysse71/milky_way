CCOPTS	= -std=c++11 -Wall -g
XLIBS	= -lglut -lGLU -lGL -lm

all	: mw mw3d

mw	: mw.cc
	g++ $(CCOPTS) mw.cc -o mw

mw3d	: mw3d.cc
	g++ $(CCOPTS) mw3d.cc $(XLIBS) -o mw3d
	

clean	:
	rm -f *.o

distclean dclean	: clean
	rm -f mw mw3d
