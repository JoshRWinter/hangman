#ifndef HANGMAN_H
#define HANGMAN_H

#include <exception>

#include <QWidget>
#include <QLabel>
#include <QDebug>

class HangmanExit:public std::exception{
public:
	virtual const char *what()const noexcept{
		return "need to exit";
	}
};

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
	void paintEvent(QPaintEvent*);
	static bool write(const std::string&,const std::vector<HangmanLevel>&);
	static std::vector<HangmanLevel> read(const std::string&);

private:
	void next_level();

	std::vector<HangmanLevel> lvls;
	int levelindex;
	QLabel *label;
};

#endif // HANGMAN_H
