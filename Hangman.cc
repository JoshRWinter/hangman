#include <fstream>

#include "Hangman.h"
#include "Dialog.h"

Hangman::Hangman(){
	// run the startup dialog
	Dialog::Startup startup(this);
	if(!startup.exec())
		throw HangmanExit();

	lvls = Hangman::read(startup.get_file());

	setWindowTitle("HangmanQt");
	resize(600, 500);

	label = new QLabel("ayy");
	label->setGeometry(0, 0, width(), height());
	label->setWordWrap(true);

	levelindex = -1;
	next_level();
}

std::vector<HangmanLevel> Hangman::read(const std::string &fname){
	std::vector<HangmanLevel> lvls;

	std::ifstream in(fname);
	if(!in){
		qDebug() << "could not open" << fname.c_str() << "in read mode.";
		return lvls;
	}

	while(in.good()){
		std::string entry;
		std::getline(in, entry);
		if(entry.length() == 0)
			continue;

		std::string challenge, answer;
		auto pos = entry.find("|");
		if(pos == std::string::npos){
			qDebug() << "file" << fname.c_str() << "is corrupt";
			return lvls;
		}

		challenge = entry.substr(0, pos);
		answer = entry.substr(pos + 1);

		lvls.push_back({challenge, answer});
	}

	return lvls;
}

bool Hangman::write(const std::string &fname, const std::vector<HangmanLevel> &lvls){
	std::ofstream out(fname);
	if(!out)
		return false;

	for(const HangmanLevel &level:lvls)
		out << level.challenge << "|" << level.answer << "\n";

	return true;
}

void Hangman::next_level(){
	++levelindex;

	const HangmanLevel &level = lvls.at(levelindex);
	label->setText(level.challenge.c_str());
}
