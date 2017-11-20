#include <QApplication>

#include "Hangman.h"

int main(int argc, char **argv){
	try{
		QApplication app(argc, argv);

		Hangman hangman;

		return app.exec();
	}catch(const HangmanExit&){
		return 1;
	}
}
