#pragma once
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include "Cell.h"
#include "Game.h"
#include "Settings.h"

class Board : public QGraphicsView
{
	Q_OBJECT
		
		const int _n = 8;
	
	private:

		QGraphicsScene* _scene;
		Cell*** _cells;
		gameMode _gMode;
		int _difficulty;
		bool _playerIsWhite;
		Game* _game;	
		QString _col;

	public:

		Board(gameMode mode, int difficulty, bool playerIsWhite, bool up, bool turn);

		virtual void resizeEvent(QResizeEvent* e);
		void win_lose_mex();
		bool valid_selection(int x0, int y0);
		bool forced_cells();

		// reset game
		void reset(bool up, bool turn);
		
		void save_board();

		//getters
		Game* game() { return _game; }
		gameMode gMode() { return _gMode; }
		QString color() { return _col; };

		friend class Cell;

	public slots:

		// player has moved at the given position (in future AI too)
		void move(int x0, int y0, int x1, int y1);

		// draw board pieces
		void updateBoard(); 

		// undo
		void undo();

		// display hint
		void hint();

		//display forced cells
		void display_cells(int xy_s, int xy_n);
		
		void clear_selected();

		void update_statusbar();
};
