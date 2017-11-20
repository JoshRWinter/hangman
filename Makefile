.PHONY := clean

all: Makefile.qmake
	make -f Makefile.qmake
	./Hangman

Makefile.qmake: Hangman.pro
	qmake -o Makefile.qmake

clean:
	make -f Makefile.qmake clean distclean
