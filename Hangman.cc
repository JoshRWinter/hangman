#include <fstream>
#include <cctype>

#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>

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

	label = new QLabel("", this);
	label->setWordWrap(true);
	label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	levelindex = -1;
	next_level();
}

// override
void Hangman::paintEvent(QPaintEvent*){
	QPainter painter(this);

	const int LINE_WIDTH = 50;
	const int LINE_SPACING = 10;
	const int CHAR_COUNT = lvls[levelindex].answer.length();
	const int xpos = (width() / 2) - ((CHAR_COUNT * (LINE_SPACING + LINE_WIDTH)) / 2);
	const int ypos = 450;

	for(int i = 0; i < CHAR_COUNT; ++i){
		// draw the lines for the letter blanks
		const int x = xpos + (i * (LINE_WIDTH + LINE_SPACING));
		painter.drawLine(QPoint(x, ypos), QPoint(x + LINE_WIDTH, ypos));
	}
}

// override
void Hangman::keyPressEvent(QKeyEvent *event){
	int key = 'A' + event->key() - 0x41;
	if(key < Qt::Key_A || key > Qt::Key_Z)
		return;

	qDebug() << "key event: " << (char)key;

	// see if the user already guessed that one
	if(correct.find(key) != std::string::npos || wrong.find(key) != std::string::npos){
		QMessageBox::warning(this, "Error", "You already guessed that key!");
		return;
	}

	// see if the user got it right
	if(lvls.at(levelindex).answer.find(key) != std::string::npos){
		qDebug() << "you got it right!";
		char str[2] = {(char)key, 0};
		correct.append(str);
		return;
	}
	else{
		qDebug() << "you got it wrong!";
		char str[2] = {(char)key, 0};
		wrong.append(str);
		return;
	}

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
	label->setText(("Level " + std::to_string(levelindex + 1) + " of " + std::to_string(lvls.size()) + ": " + level.challenge).c_str());
	label->setGeometry(0, 0, width(), 500);
}
