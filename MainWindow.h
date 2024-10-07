#pragma once

#include <QMainWindow>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QWidget>
#include "Board.h"
#include "Settings.h"
#include "Button.h"
#include "Timer.h"



class MainWindow : public QMainWindow
{

	Q_OBJECT

private:

	Board* _board;
	Settings* _set;
	QPixmap* _background;
	QToolBar* _toolbar;

	Button* _buttonStart;
	Button* _buttonClassification;
	Button* _buttonAbout;

	// singleton
	static MainWindow* unique_instance;
	MainWindow();

	QAction* _about;
	QAction* _new_game;
	QAction* _classification;
	QAction* _undo;
	QAction* _draw;
	QAction* _hint;
	QAction* _clear;

	QTimer* _timer;
	Timer* _t;


public:

	// singleton
	static MainWindow* instance();
	Settings* set() { return _set; };
	Timer* timer() { return _t; };

	//Events
	void resizeEvent(QResizeEvent* e);
	void closeEvent(QCloseEvent* e);

	//Game settings and match
	bool gameSettings();
	void save_game();
	bool upload_game();

	void update_toolbar(bool u, bool h);

	void create_toolbar_statusBar();

	QString date(int ms);

	//classification
	void insert_game(QString s);
	void clear_class(QString s);
	void show_class(QString s);
	void save_class();
	bool max_search(std::vector<std::pair<QString, int>> v, int value);
	void message_class(QString s, QString dif);

public slots:

	void start();
	void new_game();
	void show_classification();
	void about();
	void undo();
	void draw();
	void hint();
	void clear();
	void update_statusbar();
};

struct sort_p
{
	bool operator()(const std::pair<QString, int>& left, const std::pair<QString, int>& right)
	{
		return left.second < right.second;
	}
};