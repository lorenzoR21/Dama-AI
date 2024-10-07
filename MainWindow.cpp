#include <QStatusBar>
#include <QPainter>
#include "MainWindow.h"
#include "Settings.h"
#include <QEventLoop>
#include <QToolbar>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QRect>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QFormLayout>
#include <QPalette>
#include <QTextStream>
#include <QTimer>
#include <iostream>


MainWindow* MainWindow::unique_instance = 0;


MainWindow* MainWindow::instance()
{
	if (unique_instance == 0)
		unique_instance = new MainWindow();
	return unique_instance;
}


MainWindow::MainWindow()
{
	setMinimumSize(1300, 800);

	_set = 0;
	_board = 0;
	_background = new QPixmap(":graphics/menu.png");
	_toolbar = 0;
	_t = 0;
	_timer = new QTimer(this);

	connect(_timer, SIGNAL(timeout()), this, SLOT(update_statusbar()));


	//Icona e titolo
	setWindowIcon(QIcon(":/graphics/icon.png"));
	setWindowTitle(QString("URL Dama"));

	//Action and connections
	_new_game = new QAction(QIcon(":graphics/icon.png"), "New Game", this);
	_classification = new QAction(QIcon(":graphics/classification.png"), "Classification", this);
	_about = new QAction(QIcon(":graphics/about.png"), "About", this);
	_undo = new QAction(QIcon(":graphics/undo.png"), "Undo", this);
	_draw = new QAction(QIcon(":graphics/draw.png"), "Draw", this);
	_hint = new QAction(QIcon(":graphics/hint.png"), "Hint", this);
	_clear = new QAction(QIcon(":graphics/clear.png"), "Clear", this);

	//shortcuts 
	_hint->setShortcut(QKeySequence(Qt::Key_H));
	_undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
	_new_game->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
	_classification->setShortcut(QKeySequence(Qt::Key_C));
	_about->setShortcut(QKeySequence(Qt::Key_A));
	_draw->setShortcut(QKeySequence(Qt::Key_D));
	_clear->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));


	connect(_new_game, SIGNAL(triggered()), this, SLOT(new_game()));
	connect(_classification, SIGNAL(triggered()), this, SLOT(show_classification()));
	connect(_about, SIGNAL(triggered()), this, SLOT(about()));
	connect(_undo, SIGNAL(triggered()), this, SLOT(undo()));
	connect(_draw, SIGNAL(triggered()), this, SLOT(draw()));
	connect(_hint, SIGNAL(triggered()), this, SLOT(hint()));
	connect(_clear, SIGNAL(triggered()), this, SLOT(clear()));


	//Set menù
	QWidget* window = new QWidget();
	setCentralWidget(window);

	//Buttons and connections
	_buttonStart = new Button(QRect(centralWidget()->width() / 4, 2.5 * (centralWidget()->height() / 5), 9, 2));
	_buttonClassification = new Button(QRect(centralWidget()->width() / 4, 2.7 * (centralWidget()->height() / 5), 9, 2));
	_buttonAbout = new Button(QRect(centralWidget()->width() / 4, 2.95 * (centralWidget()->height() / 5), 9, 2));

	_buttonStart->setAlignment(Qt::AlignCenter);
	_buttonClassification->setAlignment(Qt::AlignCenter);
	_buttonAbout->setAlignment(Qt::AlignCenter);

	connect(_buttonStart, SIGNAL(clicked()), this, SLOT(start()));
	connect(_buttonClassification, SIGNAL(clicked()), this, SLOT(show_classification()));
	connect(_buttonAbout, SIGNAL(clicked()), this, SLOT(about()));

	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(QColor(139, 69, 19)));
	setPalette(palette);
}


void MainWindow::start()
{
	if (!upload_game())
	{
		bool i = gameSettings();

		if (i)
		{
			if (_toolbar)
			{
				removeToolBar(_toolbar);
				delete _toolbar;
			}
			create_toolbar_statusBar();
			setCentralWidget(_board);
			_timer->start(200);

			setMinimumSize(800, 800);
			QPalette palette;
			palette.setBrush(QPalette::Background, QBrush(QColor(255, 255, 255)));
			setPalette(palette);
		}
		else if (_board)
			if (!i && _board->game()->ended())
				close();
			else
				return;
	}
	else
	{
		setCentralWidget(_board);
		create_toolbar_statusBar();
		_timer->start(200);

		setMinimumSize(800, 800);
		QPalette palette;
		palette.setBrush(QPalette::Background, QBrush(QColor(255, 255, 255)));
		setPalette(palette);
	}

}


void MainWindow::new_game()
{
	QMessageBox::StandardButton choice =
		QMessageBox::question(this, "Confirm", QString("Do you want to start a new game? "), QMessageBox::Yes | QMessageBox::No);
	if (choice == QMessageBox::Yes)
		start();
	else
		return;
}


void MainWindow::show_classification()
{
	QStringList dif;
	dif << "Easy";
	dif << "Normal";
	dif << "Hard";
	bool ok1;
	QString choice = QInputDialog::getItem(this, "Chose classification", "Difficulty", dif, 0, false, &ok1);
	if (!ok1 || choice.isEmpty())
		return;

	QStringList act;
	act << "Show";
	act << "Clear";
	bool ok;
	QString choice1 = QInputDialog::getItem(this, "Chose Action", "Action", act, 0, false, &ok);
	if (!ok || choice1.isEmpty())
		return;

	if (choice1 == "Show")
		show_class(choice);
	else if (choice1 == "Clear")
		clear_class(choice);
}


void MainWindow::about()
{
	QString text("<html><h1>URL Dama");
	text = text + "</h1>"
		"<big>A Qt-based italian Dama game with AI.</big><br><br>"
		"Developed by three students of Computer & Telecomunications Engeneering"
		" for the exam 'Tecniche di Programmazione'.</li><br><br>"
		"<br><small>(c) 2022 University of Cassino and L.M.</small></html>";

	QMessageBox::about(this, "About URL Dama", text);
}


void MainWindow::undo()
{
	_board->undo();
}

void MainWindow::draw()
{
	if (!_board->game()->draw())
		QMessageBox::information(this, "Draw", "Missing condition for draw!");
	else
	{
		QMessageBox::StandardButton choice =
			QMessageBox::question(this, "Draw", QString("Game ended in a draw, do you want to start a new game? "), QMessageBox::Yes | QMessageBox::Close);
		if (choice == QMessageBox::Yes)
			start();
		else
			close();
	}
}

void MainWindow::hint()
{
	_board->hint();
}

void MainWindow::clear()
{
	_board->clear_selected();
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
	if (_board)
		_board->resizeEvent(e);
	else
	{
		QWidget* window = new QWidget();
		if (centralWidget()->height() - centralWidget()->geometry().y() < 800)
		{
			window->setGeometry(centralWidget()->geometry());
			QLabel* label = new QLabel(window);
			label->setFixedSize(QSize(centralWidget()->width(), centralWidget()->height()));
			label->setGeometry(0, centralWidget()->height() / 10, centralWidget()->width(), 9 * (centralWidget()->height() / 10));
			label->setPixmap(*(_background));
			label->setAlignment(Qt::AlignCenter);
			label->setScaledContents(true);

		}
		else
		{
			window->setGeometry(centralWidget()->geometry());
			QLabel* label = new QLabel(window);
			label->setFixedSize(QSize(centralWidget()->width(), 800));
			label->setGeometry(0, centralWidget()->height() / 10, centralWidget()->width(), 700);
			label->setPixmap(*(_background));
			label->setAlignment(Qt::AlignCenter);
			label->setScaledContents(true);
		}

		setCentralWidget(window);

		_buttonStart->setGeometry(centralWidget()->width() / 4, 2.5 * (centralWidget()->height() / 5), 9, 2);
		_buttonClassification->setGeometry(centralWidget()->width() / 4, 2.7 * (centralWidget()->height() / 5), 9, 2);
		_buttonAbout->setGeometry(centralWidget()->width() / 4, 2.95 * (centralWidget()->height() / 5), 9, 2);

		QHBoxLayout* layout = new QHBoxLayout(window);
		layout->addWidget(_buttonStart);
		layout->addWidget(_buttonClassification);
		layout->addWidget(_buttonAbout);
	}
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	if (_board)
	{
		if (_board->gMode() == HUMAN_VS_AI && _board->game()->myTurn() == false && !_board->game()->ended())
			e->ignore();
		else if (_board->game()->ended())
		{
			QFile file("Saved game.txt");
			file.resize(0);
			close();
			return;
		}
		else if (_board->game()->draw())
		{
			close();
			return;
		}
	}

	QMessageBox::StandardButton choice =
		QMessageBox::question(this, "Confirm", QString("Do you want to quit? "), QMessageBox::Yes | QMessageBox::No);
	if (choice == QMessageBox::Yes)
	{
		if (_board)
			save_game();
		close();
	}
	else
		e->ignore();
}


bool MainWindow::gameSettings()
{
	bool i = true;
	// chose name
	QString player_name = QInputDialog::getText(this, "Chose player name", "Player name:");
	if (player_name.isEmpty() || player_name[0] == " ")
		return false;

	// chose mode and difficulty
	QStringList game_mode;
	game_mode << "Human vs AI";
	game_mode << "Human vs Human";
	bool ok1;
	QString choice = QInputDialog::getItem(this, "Chose game mode", "Game mode:", game_mode, 0, false, &ok1);
	if (!ok1 || choice.isEmpty())
		return false;
	gameMode g;
	Difficulty d;
	if (choice == "Human vs AI")
	{
		g = HUMAN_VS_AI;
		QStringList game_difficulty;
		game_difficulty << "Easy";
		game_difficulty << "Normal";
		game_difficulty << "Hard";
		bool ok;
		QString choice = QInputDialog::getItem(this, "Chose AI difficulty", "AI difficulty:", game_difficulty, 0, false, &ok);
		if (!ok || choice.isEmpty())
			return false;

		if (choice == "Easy")
			d = EASY;
		else if (choice == "Normal")
			d = NORMAL;
		else
			d = HARD;
	}
	else
		g = HUMAN_VS_HUMAN;

	// chose colour
	QStringList colour;
	colour << "White";
	colour << "Black";
	bool yes;
	bool col;
	QString choice2 = QInputDialog::getItem(this, "Chose Player Colour", "Player Colour:", colour, 0, false, &yes);
	if (!yes || choice2.isEmpty())
		return false;
	if (choice2 == "White")
		col = true;
	else
		col = false;

	//Set Board
	if (i)
	{
		if (g == HUMAN_VS_AI)
			_set = new Settings(d, g, col, player_name);
		else
			_set = new Settings(NONE, g, col, player_name);
		_board = new Board(_set->mode(), _set->difficulty(), _set->playerIsWhite(), false, false);
		_t = new Timer(0);
	}
	return i;
}


void MainWindow::save_game()
{
	QFile saveFile("Saved game.txt");
	if (!saveFile.open(QIODevice::WriteOnly))
		std::cerr << "Cannot save project file";

	QJsonObject json_obj;
	json_obj["_name"] = _set->name();
	json_obj["_game_mode"] = _set->mode();
	if (_set->mode() == HUMAN_VS_AI)
		json_obj["_difficulty"] = _set->difficulty_enum();
	json_obj["_colour"] = _set->playerIsWhite();
	json_obj["_time"] = _t->elapsed_time();
	json_obj["_myTurn"] = _board->game()->myTurn();

	saveFile.write(QJsonDocument(json_obj).toJson());
	saveFile.close();

	_board->save_board();
}

bool MainWindow::upload_game()
{
	if (!_board)
	{
		QFile file("Saved game.txt");
		if (!file.open(QIODevice::ReadOnly) || file.size() == 0)
			return false;
		QMessageBox::StandardButton choice =
			QMessageBox::question(this, "Upload game", QString("Game found, do you whant to play it? "), QMessageBox::Yes | QMessageBox::No);
		if (choice == QMessageBox::Yes)
		{
			QByteArray file_content = file.readAll();
			QJsonDocument json_doc(QJsonDocument::fromJson(file_content));
			QJsonObject json_obj = json_doc.object();

			if (json_obj["_game_mode"].toInt() == 1)
			{
				if (json_obj.contains("_name") && json_obj.contains("_game_mode") && json_obj.contains("_colour") && json_obj.contains("_difficulty"))
				{
					_set = new Settings(static_cast<Difficulty>(json_obj["_difficulty"].toInt()), static_cast<gameMode>(json_obj["_game_mode"].toInt()), json_obj["_colour"].toBool(), json_obj["_name"].toString());
					_board = new Board(_set->mode(), _set->difficulty(), _set->playerIsWhite(), true, json_obj["_myTurn"].toBool());
				}
			}
			else
			{
				if (json_obj.contains("_name") && json_obj.contains("_game_mode") && json_obj.contains("_colour"))
				{
					_set = new Settings(NONE, static_cast<gameMode>(json_obj["_game_mode"].toInt()), json_obj["_colour"].toBool(), json_obj["_name"].toString());
					_board = new Board(_set->mode(), _set->difficulty(), _set->playerIsWhite(), true, json_obj["_myTurn"].toBool());
				}
			}

			if (json_obj.contains("_time"))
				_t = new Timer(json_obj["_time"].toInt());

			return true;
		}
		else
			return false;
	}
	else
		return false;

}


void MainWindow::create_toolbar_statusBar()
{
	//Toolbar
	_toolbar = new QToolBar(this);
	_toolbar->setMovable(false);
	_toolbar->setIconSize(QSize(30, 30));
	_toolbar->addAction(_new_game);
	_toolbar->addAction(_undo);
	_toolbar->addAction(_hint);
	_toolbar->addAction(_clear);
	_toolbar->addSeparator();
	_toolbar->addSeparator();
	_toolbar->addAction(_draw);
	_toolbar->addSeparator();
	_toolbar->addSeparator();
	_toolbar->addAction(_classification);
	_toolbar->addAction(_about);
	addToolBar(_toolbar);

	_toolbar->setMovable(false);

	setContentsMargins(0, 0, 0, 0);

	statusBar()->showMessage(_board->color() + "          " + date(_t->elapsed_time()));

}

void MainWindow::update_toolbar(bool u, bool h)
{
	_undo->setEnabled(u);
	_hint->setEnabled(h);
}

void MainWindow::update_statusbar()
{
	if (_board)
	{
		_board->update_statusbar();
		_timer->start(200);
	}
}


QString MainWindow::date(int ms)
{
	QString d = "";
	QString h; QString m; QString s;

	if (ms < 3600000)
	{
		h = "00";
		if (ms < 60000)
		{
			m = "00";
			if (ms < 1000)
				s = "00";
			else
				s = QString::number(ms / 1000);
		}
		else
		{
			if (((ms / 1000) / 60) % 60 == 0)
				m = "00";
			else
				m = QString::number(((ms / 1000) / 60) % 60);

			if ((ms / 1000) % 60 == 0)
				s = "00";
			else
				s = QString::number((ms / 1000) % 60);
		}
	}
	else
	{
		h = QString::number((((ms / 1000) / 60) / 60) % 24);

		m = QString::number(((ms / 1000) / 60) % 60);
		s = QString::number((ms / 1000) % 60);
	}

	if (s.toInt() < 10 && s.toInt() > 0)
		s = QString::number(0) + s;
	if (m.toInt() < 10 && m.toInt() > 0)
		m = QString::number(0) + m;
	if (h.toInt() < 10 && h.toInt() > 0)
		h = QString::number(0) + h;

	d = h + ":" + m + ":" + s;
	return d;
}


void MainWindow::clear_class(QString s)
{
	if (s == "Easy")
	{
		QFile file("Classification_easy.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		file.resize(0);
		file.close();
	}
	else if (s == "Normal")
	{
		QFile file("Classification_normal.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		file.resize(0);
		file.close();

	}
	else if (s == "Hard")
	{
		QFile file("Classification_hard.txt");
		file.open(QIODevice::WriteOnly | QIODevice::Text);
		file.resize(0);
		file.close();
	}
}


void MainWindow::show_class(QString s)
{
	if (s == "Easy")
		message_class("Classification_easy.txt", s);
	else if (s == "Normal")
		message_class("Classification_normal.txt", s);
	else if (s == "Hard")
		message_class("Classification_hard.txt", s);
}


void MainWindow::save_class()
{
	if (_set->difficulty_enum() == EASY)
		insert_game("Classification_easy.txt");
	else if (_set->difficulty_enum() == NORMAL)
		insert_game("Classification_normal.txt");
	else if (_set->difficulty_enum() == HARD)
		insert_game("Classification_hard.txt");

}


void MainWindow::insert_game(QString s)
{
	QFile file(s);
	if (!file.open(QFile::ReadOnly | QIODevice::Text))
		std::cerr << "Impossible to open the file";
	QTextStream t(&file);
	std::vector<std::pair<QString, int>> v;
	QString in = "";
	int i = 0;
	while (!t.atEnd())
	{
		QString temp = t.readLine(0);
		if (temp == "\n" || temp.isEmpty())
			continue;
		else
		{
			in += temp;
			i++;
		}
	}
	file.close();
	if (in.isEmpty() || i < 10)
	{
		if (!file.open(QFile::WriteOnly | QIODevice::Append))
			std::cerr << "Impossible to open the file";
		t << _set->name() + ":" + QString::number(_t->elapsed_time()) + ";\n";
		file.close();
	}

	else if (!in.isEmpty() && i >= 10)
	{
		QStringList li = in.split(";");
		for (auto& s : li)
		{
			QStringList n = s.split(":");
			if (n.at(0).isEmpty() || n.at(0) == "\n")
				continue;
			else
				v.push_back(std::pair<QString, int>{ n.at(0), n.at(1).toInt()});
		}

		if (max_search(v, _t->elapsed_time()))
		{

			v.push_back(std::pair<QString, int>{ _set->name(), _t->elapsed_time()});
			std::sort(v.begin(), v.end(), sort_p());
			v.pop_back();
			if (!file.open(QFile::WriteOnly | QIODevice::Text))
				std::cerr << "Impossible to open the file";
			file.resize(0);
			int j = 0;
			for (auto& p : v)
			{
				t << p.first + ":" + QString::number(p.second) + ";\n";
				j++;
				if (j == 10)
					break;
			}

			file.close();
		}
		else
			return;
	}
}



bool MainWindow::max_search(std::vector<std::pair<QString, int>> v, int value)
{
	int max = v.front().second;
	int count = 0;
	for (auto& p : v)
	{
		if (p.second > max)
			max = p.second;
		count++;
		if (count == 10)
			break;
	}
	if (value < max)
		return true;
	else
		return false;
}


void MainWindow::message_class(QString s, QString dif)
{
	QFile file(s);
	if (!file.open(QFile::ReadOnly | QIODevice::Text))
		std::cerr << "Impossible to open the file";
	QTextStream t(&file);
	QString in = "";
	while (!t.atEnd())
	{
		QString temp = t.readLine(0);
		if (temp == "\n" || temp.isEmpty())
			continue;
		else
			in += temp;
	}
	file.close();
	QString text("<html><h1>Classification");
	text += "</h1>";
	text += "<big>Difficulty chosen: " + dif + "</big><br><br>";
	std::vector<std::pair<QString, int>> v;
	QStringList li = in.split(";");
	for (auto& s : li)
	{
		QStringList n = s.split(":");
		if (n.at(0).isEmpty() || n.at(0) == "\n")
			continue;
		else
			v.push_back(std::pair<QString, int>{ n.at(0), n.at(1).toInt()});
	}

	std::sort(v.begin(), v.end(), sort_p());
	int i = 0;
	for (auto& s : v)
		text = text + "<br>" + QString::number(++i) + "- " + s.first + "=  " + date(s.second);
	text += "</html>";

	QMessageBox::information(this, "Classification", text);
}



