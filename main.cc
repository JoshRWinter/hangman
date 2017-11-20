#include <QApplication>

#include "Hangman.h"

int main(int argc, char **argv){
	QApplication app(argc, argv);

	Hangman hangman;

	return app.exec();
}
