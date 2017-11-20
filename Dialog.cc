#include <cctype>

#include <QLabel>
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
	auto hbox = new QHBoxLayout;
	auto play = new QPushButton("Play");
	auto create = new QPushButton("Create");
	auto edit = new QPushButton("Edit");
	auto label = new QLabel("Welcome to Hangman. You can play the game, or create your own level files.");
	label->setWordWrap(true);

	play->setToolTip("Play the classic game of Hangman!");
	create->setToolTip("Create a Hangman level file");
	edit->setToolTip("Edit an existing level file");

	QObject::connect(play, &QPushButton::clicked, [this]{
		QString file = QFileDialog::getOpenFileName(this, "Choose a level pack", "levels", "Level files (*.hlevel)");
		if(!file.isNull()){
			filename = file.toStdString();
			QDialog::accept();
		}
	});
	QObject::connect(create, &QPushButton::clicked, [this]{
		Dialog::Create create(this, {});
		create.exec();
	});
	QObject::connect(edit, &QPushButton::clicked, [this]{
		QString file = QFileDialog::getOpenFileName(this, "Choose a level pack", "levels", "Level files (*.hlevel)");
		if(!file.isNull()){
			Dialog::Create create(this, Hangman::read(file.toStdString()));
			create.exec();
		}
	});

	setLayout(vbox);
	vbox->addWidget(label);
	vbox->addLayout(hbox);
	hbox->addWidget(play);
	hbox->addWidget(create);
	hbox->addWidget(edit);
}

std::string Dialog::Startup::get_file()const{
	return filename;
}

// create level dialog
Dialog::Create::Create(QWidget *parent, const std::vector<HangmanLevel> &lvls):QDialog(parent){
	levels = lvls;
	setWindowTitle("Create/Edit Hangman Level");

	auto vbox = new QVBoxLayout;
	auto hbox = new QHBoxLayout;
	setLayout(vbox);

	auto list = new QListWidget;
	auto add = new QPushButton("Add");
	auto edit = new QPushButton("Edit");
	auto remove = new QPushButton("Remove");
	auto save = new QPushButton("Export");
	auto cancel = new QPushButton("Cancel");

	add->setToolTip("Add a level");
	edit->setToolTip("Edit the selected level");
	remove->setToolTip("Remove the selected level");
	save->setToolTip("Export this level to a file");

	// add levels to the list
	for(const HangmanLevel &lvl:levels){
		auto item = new QListWidgetItem(lvl.challenge.c_str());
		item->setToolTip(lvl.answer.c_str());
		list->addItem(item);
	}

	QObject::connect(add, &QPushButton::clicked, [this, list]{
		Dialog::Entry entry(this, HangmanLevel("", ""));
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

			auto item = new QListWidgetItem(challenge.c_str());
			item->setToolTip(answer.c_str());
			list->addItem(item);
			levels.push_back({challenge, answer});
		}
	});
	QObject::connect(edit, &QPushButton::clicked, [this, list]{
		if(list->count() == 0 || list->currentItem() == NULL){
			QMessageBox::critical(this, "Error", "No selected item!");
			return;
		}

		// find the level
		QListWidgetItem *item = list->currentItem();
		HangmanLevel *lvl = NULL;
		for(HangmanLevel &level:levels){
			if(level.challenge == item->text().toStdString())
				lvl = &level;
		}
		Q_ASSERT(lvl);

		Dialog::Entry entry(this, *lvl);
		if(entry.exec()){
			const auto [challenge, answer] = entry.get();
			lvl->challenge = challenge;
			lvl->answer = answer;
			item->setText(challenge.c_str());
			item->setToolTip(answer.c_str());
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
	QObject::connect(save, &QPushButton::clicked, [this, list]{
		if(list->count() == 0){
			QMessageBox::critical(this, "Error", "There are no challenges in this level!");
			return;
		}

		QFileDialog chooser(this);
		chooser.setAcceptMode(QFileDialog::AcceptSave);
		chooser.setDefaultSuffix("hlevel");
		chooser.setWindowTitle("Save this hangman level");
		chooser.setNameFilter("Hangman levels (*.hlevel)");
		if(chooser.exec()){
			std::string saved = chooser.selectedFiles().at(0).toStdString();
			if(!Hangman::write(saved, levels))
				QMessageBox::critical(this, "Error", ("Unable to open \"" + saved + "\" in write mode.").c_str());
			else
				accept();
		}
	});
	QObject::connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

	hbox->addWidget(add);
	hbox->addWidget(edit);
	hbox->addWidget(remove);
	hbox->addWidget(save);
	hbox->addWidget(cancel);
	vbox->addWidget(list);
	vbox->addLayout(hbox);
}

// level entry dialog
Dialog::Entry::Entry(QWidget *parent, const HangmanLevel &level):QDialog(parent){
	setWindowTitle("Entry for Hangman Level");
	resize(300, 0);

	auto vbox = new QVBoxLayout;
	auto hbox = new QHBoxLayout;
	auto formlayout = new QFormLayout;
	setLayout(vbox);

	challenge = new QLineEdit(level.challenge.c_str());
	answer = new QLineEdit(level.answer.c_str());;
	auto ok = new QPushButton("OK");
	auto cancel = new QPushButton("Cancel");

	QObject::connect(ok, &QPushButton::clicked, [this]{
		// disallow pipe delimeter
		if(challenge->text().toStdString().find("|") != std::string::npos ||
			answer->text().toStdString().find("|") != std::string::npos){
			QMessageBox::warning(this, "Error", "The pipe character ('|') is not allowed in challenges or answers!");
			return;
		}

		// answer field whitespace check
		std::string ans = answer->text().toStdString();
		for(const auto &c:ans){
			if(!isalpha(c)){
				QMessageBox::critical(this, "Error", "Only alphabetic characters are allowed in the answer field");
				return;
			}
		}

		// challenge field max check
		if(challenge->text().length() > CHALLENGE_STR_LEN){
			QMessageBox::warning(this, "Error", ("The challenge field is too long (maximum " + std::to_string(CHALLENGE_STR_LEN) + " characters)").c_str());
			return;
		}

		// challenge field min check
		if(challenge->text().length() < CHALLENGE_STR_MIN){
			QMessageBox::warning(this, "Error", ("The challenge field is too short (minimum " + std::to_string(CHALLENGE_STR_MIN) + " characters)").c_str());
			return;
		}

		// answer field max check
		if(answer->text().length() > ANSWER_STR_LEN){
			QMessageBox::warning(this, "Error", ("The answer field is too long (maximum " + std::to_string(ANSWER_STR_LEN) + " characters)").c_str());
			return;
		}

		// answer field max check
		if(answer->text().length() < ANSWER_STR_MIN){
			QMessageBox::warning(this, "Error", ("The answer field is too short (minimum " + std::to_string(ANSWER_STR_MIN) + " characters)").c_str());
			return;
		}

		// convert answer to uppercase
		for(auto &c:ans)
			c = toupper(c);
		answer->setText(ans.c_str());

		// goood
		accept();
	});
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
