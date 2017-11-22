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

#define STARTING_XOFF 88
#define STARTING_X_EXCLAMATION 1265

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
	void animate();
	static bool write(const std::string&,const std::vector<HangmanLevel>&);
	static std::vector<HangmanLevel> read(const std::string&);
	static int speed(int);
	static const char *get_exclamation(int);

private:
	void next_level();

	std::string wrong;
	std::string correct;
	std::vector<HangmanLevel> lvls;
	int levelindex;
	bool winner;
	QLabel *label;
	const char *exclamation;

	int t_time;
	int xoff;
};

#endif // HANGMAN_H
