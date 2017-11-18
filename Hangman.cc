#include <fstream>

#include <QApplication>

#include "Hangman.h"
#include "Dialog.h"

Hangman::Hangman(){
	// run the startup dialog
	Dialog::Startup startup(this);
	if(!startup.exec())
		throw HangmanExit();

	setWindowTitle("HangmanQt");
}

bool Hangman::write(const std::string &fname, const std::vector<HangmanLevel> &lvls){
	std::ofstream out(fname);
	if(!out)
		return false;

	for(const HangmanLevel &level:lvls)
		out << level.challenge << "|" << level.answer << "\n";

	return true;
}
