#ifndef DIALOG_H
#define DIALOG_H

#include <tuple>

#include <QDialog>
#include <QWidget>
#include <QLineEdit>

#include "Hangman.h"

#define CHALLENGE_STR_LEN 200
#define CHALLENGE_STR_MIN 5
#define ANSWER_STR_MIN 3
#define ANSWER_STR_LEN 8

namespace Dialog{

class Startup:public QDialog{
public:
	Startup(QWidget*);
	std::string get_file()const;

private:
	std::string filename;
};

class Create:public QDialog{
public:
	Create(QWidget*);

private:
	std::vector<HangmanLevel> levels;
};

class Entry:public QDialog{
public:
	Entry(QWidget*);
	std::tuple<std::string, std::string> get()const;

private:
	QLineEdit *challenge, *answer;
};

}

#endif // DIALOG_H
