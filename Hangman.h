#ifndef HANGMAN_H
#define HANGMAN_H

#include <QWidget>
#include <QLabel>
#include <QDebug>

#define CHALLENGE_STR_LEN 50
#define CHALLENGE_STR_MIN 5
#define ANSWER_STR_MIN 3
#define ANSWER_STR_LEN 12
#define GUESSES 7

struct HangmanLevel{
	HangmanLevel(const std::string &c, const std::string &a)
	:challenge(c)
	,answer(a)
	{}

	std::string challenge;
	std::string answer;
};

class Hangman:public QWidget{
public:
	Hangman();
	void reset();
	void paintEvent(QPaintEvent*);
	void keyPressEvent(QKeyEvent*);
	static bool write(const std::string&,const std::vector<HangmanLevel>&);
	static std::vector<HangmanLevel> read(const std::string&);
	static std::string truncate(const std::string&);

private:
	void next_level();

	std::string wrong;
	std::string correct;
	std::vector<HangmanLevel> lvls;
	int levelindex;
	bool winner;
	QLabel *label;
};

#endif // HANGMAN_H
