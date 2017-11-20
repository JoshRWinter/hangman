#include <fstream>
#include <cctype>

#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QFontDatabase>
#include <QTimer>

#include "Hangman.h"
#include "Dialog.h"

Hangman::Hangman(){

	setWindowTitle("HangmanQt");
	resize(600, 500);

	label = new QLabel("", this);
	label->setWordWrap(true);
	label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	reset();
}

void Hangman::reset(){
	hide();

	levelindex = -1;
	winner = false;

	// run the startup dialog
	Dialog::Startup startup(this);
	if(!startup.exec())
		throw HangmanExit();

	lvls = Hangman::read(startup.get_file());

	next_level();

	show();
}

// override
void Hangman::paintEvent(QPaintEvent*){
	if(levelindex < 0)
		return;

	QPainter painter(this);
	QFont font("NotoSansMono", 40);
	QFont font_small("NotoSansMono", 20);
	painter.setFont(font);

	const int LINE_WIDTH = 50;
	const int LINE_SPACING = 10;
	const int CHAR_COUNT = lvls[levelindex].answer.length();
	const int xpos = (width() / 2) - ((CHAR_COUNT * (LINE_SPACING + LINE_WIDTH)) / 2);
	const int ypos = 450;

	// draw the letter blanks
	for(int i = 0; i < CHAR_COUNT; ++i){
		// draw the lines for the letter blanks
		const int x = xpos + (i * (LINE_WIDTH + LINE_SPACING));
		painter.drawLine(QPoint(x, ypos), QPoint(x + LINE_WIDTH, ypos));
	}

	// draw the correct letters
	const std::string &answer = lvls.at(levelindex).answer;
	const QFontMetrics metrics(font);
	for(unsigned i = 0; i < answer.size(); ++i){
		const char c = answer.at(i);

		// try to find <c> in the <correct> set
		for(const char x:correct){
			if(c == x){
				const char str[2] = {c, 0};
				painter.drawText(xpos + (i * (LINE_WIDTH + LINE_SPACING)) + (LINE_WIDTH/2) - (metrics.width(str) / 2), ypos - 5, str);

				break;
			}
		}
	}

	// draw the error letters
	const QFontMetrics smallmetrics(font_small);
	painter.setFont(font_small);
	int errory = 95; // error y
	for(const char c:wrong){
		const int x = 380;

		char str[] = {c, 0};
		painter.drawText(x - (smallmetrics.width(str) / 2), errory, str);
		errory += smallmetrics.height() + 5;
	}

	// draw the hangy thing
	painter.setPen(QPen(QBrush(Qt::SolidPattern), 3));
	const int xoff = 88, yoff = 0; // conveniently change these to easily move hangman dude and apparatus
	// pole
	painter.drawLine(QPoint(xoff + 130, yoff + 50), QPoint(xoff + 130, yoff + 320));
	// overhang
	painter.drawLine(QPoint(xoff + 130, yoff + 50), QPoint(xoff + 205, yoff + 50));
	// hangy thing
	painter.drawLine(QPoint(xoff + 205, yoff + 50), QPoint(xoff + 205, yoff + 80));
	// base
	painter.drawLine(QPoint(xoff + 110, yoff + 320), QPoint(xoff + 150, yoff + 320));
	painter.drawLine(QPoint(xoff + 110, yoff + 320), QPoint(xoff + 110, yoff + 330));
	painter.drawLine(QPoint(xoff + 150, yoff + 320), QPoint(xoff + 150, yoff + 330));

	// draw the hangman dude
	painter.setPen(QPen(QBrush(Qt::SolidPattern), 5));
	const int error = wrong.size();
	if(error == 0)
		return;
	// head
	painter.drawEllipse(xoff + 170, yoff + 80, yoff + 70, 70);
	if(error == 1) return;
	// body
	painter.drawLine(QPoint(xoff + 205, yoff + 150), QPoint(xoff + 205, yoff + 240));
	if(error == 2) return;
	// left arm
	painter.drawLine(QPoint(xoff + 205, yoff + 150), QPoint(xoff + 175, yoff + 200));
	if(error == 3) return;
	// right arm
	painter.drawLine(QPoint(xoff + 205, yoff + 150), QPoint(xoff + 235, yoff + 200));
	if(error == 4) return;
	// left leg
	painter.drawLine(QPoint(xoff + 205, yoff + 240), QPoint(xoff + 175, yoff + 300));
	if(error == 5) return;
	// right leg
	painter.drawLine(QPoint(xoff + 205, yoff + 240), QPoint(xoff + 235, yoff + 300));
	// dead eyes
	painter.drawLine(QPoint(xoff + 185, yoff + 100), QPoint(xoff + 195, yoff + 110));
	painter.drawLine(QPoint(xoff + 185, yoff + 110), QPoint(xoff + 195, yoff + 100));
	painter.drawLine(QPoint(xoff + 215, yoff + 100), QPoint(xoff + 225, yoff + 110));
	painter.drawLine(QPoint(xoff + 215, yoff + 110), QPoint(xoff + 225, yoff + 100));
}

// override
void Hangman::keyPressEvent(QKeyEvent *event){
	int key = 'A' + event->key() - 0x41;
	if(key < Qt::Key_A || key > Qt::Key_Z || wrong.size() >= GUESSES)
		return;

	// see if the user already guessed that one
	if(correct.find(key) != std::string::npos || wrong.find(key) != std::string::npos){
		QMessageBox::warning(this, "Error", "You already guessed that key!");
		return;
	}

	// see if the user got it right
	if(lvls.at(levelindex).answer.find(key) != std::string::npos){
		// right
		char str[2] = {(char)key, 0};
		correct.append(str);

		// win check
		bool win = true;
		for(const char a:lvls[levelindex].answer){
			bool found = false;
			for(const char b:correct){
				if(a == b){
					found = true;
					break;
				}
			}
			if(!found){
				win = false;
				break;
			}
		}
		if(win){
			winner = true;
			QTimer::singleShot(2000, [this]{
				QMessageBox::information(this, "You Win!", "noice");
				next_level();
			});
		}
	}
	else{
		// wrong
		char str[2] = {(char)key, 0};
		wrong.append(str);

		// lose check
		if(wrong.size() == GUESSES){
			QTimer::singleShot(2000, [this]{
				QMessageBox::information(this, "You Lose", "Loooosseerrrr");
				reset();
			});
		}
	}

	repaint();
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
	// clear previous level
	wrong.clear();
	correct.clear();

	++levelindex;
	winner = false;

	if(levelindex >= (int)lvls.size()){
		QMessageBox::information(this, "coolio", "You win everything nice job");
		reset();
		return;
	}

	const HangmanLevel &level = lvls.at(levelindex);
	label->setText(("Level " + std::to_string(levelindex + 1) + " of " + std::to_string(lvls.size()) + ": " + level.challenge).c_str());
	label->setGeometry(0, 0, width(), 500);
}
