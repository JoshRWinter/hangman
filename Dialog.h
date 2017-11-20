#ifndef DIALOG_H
#define DIALOG_H

#include <tuple>

#include <QDialog>
#include <QWidget>
#include <QLineEdit>

#include "Hangman.h"

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
	Create(QWidget*, const std::vector<HangmanLevel>&);

private:
	std::vector<HangmanLevel> levels;
};

class Entry:public QDialog{
public:
	Entry(QWidget*, const HangmanLevel&);
	std::tuple<std::string, std::string> get()const;

private:
	QLineEdit *challenge, *answer;
};

}

#endif // DIALOG_H
