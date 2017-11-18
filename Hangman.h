#ifndef HANGMAN_H
#define HANGMAN_H

#include <exception>

#include <QWidget>
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
	static bool write(const std::string&,const std::vector<HangmanLevel>&);
	static std::vector<HangmanLevel> read(const std::string&);
};

#endif // HANGMAN_H
