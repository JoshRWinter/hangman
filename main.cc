#include <QApplication>

#include "Hangman.h"

int main(int argc, char **argv){
	QApplication app(argc, argv);

	try{
		Hangman hangman;
		hangman.show();
	}catch(const HangmanExit&){
		return 0;
	}

	return app.exec();
}
