#include <QBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>
#include <QFileDialog>

#include "Dialog.h"

// select action dialog
Dialog::Startup::Startup(QWidget *parent):QDialog(parent){
	setWindowTitle("HangmanQt");

	auto vbox = new QVBoxLayout;
	auto play = new QPushButton("Play");
	auto create = new QPushButton("Create");

	QObject::connect(play, &QPushButton::clicked, [this]{
		QString file = QFileDialog::getOpenFileName(this, "Choose a level pack", "levels", "Level files (*.hlevel)");
		if(!file.isNull()){
			filename = file.toStdString();
			QDialog::accept();
		}
	});
	QObject::connect(create, &QPushButton::clicked, [this]{
		Dialog::Create create(this);
		create.exec();
	});

	setLayout(vbox);
	vbox->addWidget(play);
	vbox->addWidget(create);
}

std::string Dialog::Startup::get_file()const{
	return filename;
}

// create level dialog
Dialog::Create::Create(QWidget *parent):QDialog(parent){
	setWindowTitle("Create Hangman Level");

	auto vbox = new QVBoxLayout;
	auto hbox = new QHBoxLayout;
	setLayout(vbox);

	auto list = new QListWidget;
	auto add = new QPushButton("Add");
	auto remove = new QPushButton("Remove");
	auto save = new QPushButton("Export");

	QObject::connect(add, &QPushButton::clicked, [this, list]{
		Dialog::Entry entry(this);
		if(entry.exec()){
			const auto [challenge, answer] = entry.get();

			// check to make sure this question doesn't already exist
			bool exists = false;
			for(const HangmanLevel &level:levels){
				if(level.challenge == challenge){
					exists = true;
					break;
				}
			}
			if(exists){
				QMessageBox::critical(this, "Error", "That challenge already exists in this level");
				return;
			}

			list->addItem(challenge.c_str());
			levels.push_back({challenge, answer});
		}
	});
	QObject::connect(remove, &QPushButton::clicked, [this, list]{
		if(list->count() == 0 || list->currentItem() == NULL)
			return;

		// get the selected item
		const int row = list->currentRow();

		// delete it first from <levels> then from the QListWidget
		for(auto it = levels.begin(); it != levels.end();){
			const HangmanLevel &level = *it;
			if(level.challenge == list->currentItem()->text().toStdString()){
				it = levels.erase(it);
				continue;
			}

			++it;
		}
		delete list->takeItem(row);
	});
	QObject::connect(save, &QPushButton::clicked, [this]{
		QString saved = QFileDialog::getSaveFileName(this, "Save this hangman level");
		if(!saved.isNull()){
			if(!Hangman::write(saved.toStdString(), levels))
				QMessageBox::critical(this, "Error", ("Unable to open \"" + saved.toStdString() + "\" in write mode.").c_str());
			else
				accept();
		}
	});

	hbox->addWidget(add);
	hbox->addWidget(remove);
	hbox->addWidget(save);
	vbox->addWidget(list);
	vbox->addLayout(hbox);
}

// level entry dialog
Dialog::Entry::Entry(QWidget *parent):QDialog(parent){
	setWindowTitle("Entry for Hangman Level");

	auto vbox = new QVBoxLayout;
	auto hbox = new QHBoxLayout;
	auto formlayout = new QFormLayout;
	setLayout(vbox);

	challenge = new QLineEdit;
	answer = new QLineEdit;
	auto ok = new QPushButton("OK");
	auto cancel = new QPushButton("Cancel");

	QObject::connect(ok, &QPushButton::clicked, this, &QDialog::accept);
	QObject::connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

	formlayout->addRow("Challenge", challenge);
	formlayout->addRow("Answer", answer);
	hbox->addWidget(ok);
	hbox->addWidget(cancel);
	vbox->addLayout(formlayout);
	vbox->addLayout(hbox);
}

std::tuple<std::string, std::string> Dialog::Entry::get()const{
	return {challenge->text().toStdString(), answer->text().toStdString()};
}
