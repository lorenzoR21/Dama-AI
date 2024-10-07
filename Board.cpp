#include "Board.h"
#include <QMessageBox>
#include <QResizeEvent>
#include "MainWindow.h"
#include <QStatusBar>
#include <iostream>
#include <list>


Board::Board(gameMode mode, int difficulty, bool playerIsWhite, bool up, bool turn) : QGraphicsView()
{
	// initialize object members
	_cells = 0;
	_gMode = mode;
	_difficulty = difficulty;
	_playerIsWhite = playerIsWhite;

	if (!up)
	{
		if (_gMode == HUMAN_VS_HUMAN)
			_col = "White turn";
		else if (_gMode == HUMAN_VS_AI && _playerIsWhite)
			_col = "Your turn";
		else if (_gMode == HUMAN_VS_AI && !_playerIsWhite)
			_col = "AI is thinking...";
	}

	else
	{
		if (_gMode == HUMAN_VS_HUMAN)
		{
			if ((turn && _playerIsWhite) || (!turn && !_playerIsWhite))
				_col = "White turn";
			else if ((turn && !_playerIsWhite) || (!turn && _playerIsWhite))
				_col = "Black turn";
		}		
		else if (_gMode == HUMAN_VS_AI)
		{
			if (turn)
				_col = "Your turn";
			else
				_col = "AI is thinking...";
		}
	}


	setBackgroundBrush(QBrush(QColor(139, 69, 19)));

	// setup scene
	_scene = new QGraphicsScene();
	setScene(_scene);
	setInteractive(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	reset(up, turn);

	// window size is automatically adjusted by the overridden resizeEvent method
	resizeEvent(0);
}

void Board::resizeEvent(QResizeEvent* e)
{
	fitInView(_scene->sceneRect(), Qt::KeepAspectRatio);
}

void Board::reset(bool up, bool turn)
{
	if (_cells)
	{
		for (int i = 0; i < _n; i++)
		{
			for (int j = 0; j < _n; j++)
			{
				_scene->removeItem(_cells[i][j]);
				delete _cells[i][j];
			}
			delete[] _cells[i];
		}
		delete[] _cells;
	}


	// clear scene
	_scene->clear();

	// set scene rect
	_scene->setSceneRect(QRectF(0, 0, Cell::cellsize * _n, Cell::cellsize * _n));


	// allocate new grid of cells
	_cells = new Cell * *[_n];
	for (int i = 0; i < _n; i++)
	{
		_cells[i] = new Cell * [_n];
		for (int j = 0; j < _n; j++)
		{
			_cells[i][j] = new Cell(this, j, i);
			_cells[i][j]->setPos(j, i);
			_scene->addItem(_cells[i][j]);
		}
	}

	// allocate new game
	_game = new Game(_difficulty, _playerIsWhite, up, turn);

	// draw pieces
	if (up)
		_game->eat_rules();
		
	updateBoard();

	// connect board to game's AI
	connect(_game, SIGNAL(notifyAIMove(int, int, int, int)), this, SLOT(move(int, int, int, int)), Qt::AutoConnection);
}

void Board::undo()
{
	if (_game->_moves.empty())
		return;

	if (_game->_moves.back()._multiple_eat)
		return;

	if ((_gMode == HUMAN_VS_AI && _game->movesize() <= 1 && !_playerIsWhite))
		return;

	if (!_game->isThinking())
	{
		// reset suggested/moved cells
		for (int i = 0; i < _n; i++)
			for (int j = 0; j < _n; j++)
				_cells[i][j]->setSuggested(false);

		// undo
		_game->undo();
		if (_gMode == HUMAN_VS_AI)
		{
			_game->undo();
			_col = "Your turn";
		}

		if (_gMode == HUMAN_VS_HUMAN)
		{
			if ((_game->myTurn() && _playerIsWhite) || (!_game->myTurn() && !_playerIsWhite))
				_col = "White turn";
			else if ((_game->myTurn() && !_playerIsWhite) || (!_game->myTurn() && _playerIsWhite))
				_col = "Black turn";
		}


		//update board
		updateBoard();
	}
}


void Board::move(int x0, int y0, int x1, int y1)
{
	if (_gMode == HUMAN_VS_HUMAN)
	{
		MainWindow::instance()->update_toolbar(true, true);

		if (((_game->myTurn() && _playerIsWhite) || (!_game->myTurn() && !_playerIsWhite)) && (_cells[y0][x0]->_content == Cell::PIECE_W || _cells[y0][x0]->_content == Cell::P_DAMA_W)) //white turn
		{

			bool multiple_eat = false;
			if (!_game->move(x0, y0, x1, y1, multiple_eat))
				return;

			updateBoard();
			win_lose_mex();


			if (multiple_eat)
			{
				MainWindow::instance()->update_toolbar(false, false);
				return;
			}

			_game->eat_rules();

			//turn change
			_col = "Black turn";

		}
		else if (((!_game->myTurn() && _playerIsWhite) || (_game->myTurn() && !_playerIsWhite)) && (_cells[y0][x0]->_content == Cell::PIECE_B || _cells[y0][x0]->_content == Cell::P_DAMA_B)) //black turn
		{

		bool multiple_eat = false;
		
		if (!_game->move(x0, y0, x1, y1, multiple_eat))
			return;

		
		updateBoard();
		win_lose_mex();

		if (multiple_eat)
		{
			MainWindow::instance()->update_toolbar(false, false);
			return;
		}
			
		_game->eat_rules();

		//turn change
		_col = "White turn";

		}
		else
			return;
	}
	else if (_gMode == HUMAN_VS_AI)
	{
		if (_game->myTurn() && !valid_selection(x0, y0))
			return;

		// do nothing if AI is thinking
		if (_game->isThinking())
			return;

		MainWindow::instance()->update_toolbar(true, true);


		bool multiple_eat = false;
		
		if (!_game->move(x0, y0, x1, y1, multiple_eat))
			return;

		updateBoard();
		win_lose_mex();

		if (multiple_eat)
		{
			MainWindow::instance()->update_toolbar(false, false);
			return;
		}

		_game->eat_rules();

		_col = "Your turn";

		// if it is AI's turn, run AI
		if (_gMode == HUMAN_VS_AI && _game->myTurn() == false)
		{
			MainWindow::instance()->update_toolbar(false, false);
			_col = "AI is thinking...";
			_game->start();
		}
	}

}


void Board::updateBoard()
{
	Cell::cellContent player;
	Cell::cellContent AI;
	Cell::cellContent playerK;
	Cell::cellContent AIk;

	if (_playerIsWhite)
	{
		player = Cell::PIECE_W;
		playerK = Cell::P_DAMA_W;
		AI = Cell::PIECE_B;
		AIk = Cell::P_DAMA_B;
	}
	else
	{
		player = Cell::PIECE_B;
		playerK = Cell::P_DAMA_B;
		AI = Cell::PIECE_W;
		AIk = Cell::P_DAMA_W;
	}
	for (int i = 0; i < _n; i++)
		for (int j = 0; j < _n; j++)
		{
			if (_game->pieceBoard()[i][j] != 0 && _game->pieceBoard()[i][j]->white() && !_game->pieceBoard()[i][j]->king())
				_cells[i][j]->setContent(player);
			else if (_game->pieceBoard()[i][j] != 0 && !_game->pieceBoard()[i][j]->white() && !_game->pieceBoard()[i][j]->king())
				_cells[i][j]->setContent(AI);
			else if (_game->pieceBoard()[i][j] != 0 && _game->pieceBoard()[i][j]->white() && _game->pieceBoard()[i][j]->king())
				_cells[i][j]->setContent(playerK);
			else if (_game->pieceBoard()[i][j] != 0 && !_game->pieceBoard()[i][j]->white() && _game->pieceBoard()[i][j]->king())
				_cells[i][j]->setContent(AIk);
			else
				_cells[i][j]->setContent(Cell::EMPTY);
		}

}


void Board::win_lose_mex()
{
	if (!_game->ended())
		return;
	

	if (_gMode == HUMAN_VS_HUMAN)
	{
		if ((_playerIsWhite && _game->wins() == 1) || (!_playerIsWhite && _game->wins() == 2))
		{
			QMessageBox::StandardButton choice =
				QMessageBox::information(this, "Match ended", QString("WHITE WIN!\nDo you want to start a new game? "), QMessageBox::Yes | QMessageBox::Close);
			if (choice == QMessageBox::Yes)
				MainWindow::instance()->start();
			else
				MainWindow::instance()->close();
		}
		else if ((_playerIsWhite && _game->wins() == 2) || (!_playerIsWhite && _game->wins() == 1))
		{
			QMessageBox::StandardButton choice =
				QMessageBox::information(this, "Match ended", QString("BLACK WIN!\nDo you want to start a new game? "), QMessageBox::Yes | QMessageBox::Close);
			if (choice == QMessageBox::Yes)
				MainWindow::instance()->start();
			else
				MainWindow::instance()->close();
		}
	}
	else if (_gMode == HUMAN_VS_AI)
	{
		if (_game->wins() == 1)
		{
			MainWindow::instance()->save_class();
			QMessageBox::StandardButton choice =
				QMessageBox::information(this, "Match ended", "YOU WIN\nDou you want to start a new game?", QMessageBox::Yes | QMessageBox::Close);
			if (choice == QMessageBox::Yes)
				MainWindow::instance()->start();
			else
				MainWindow::instance()->close();
		}
		else if (_game->wins() == 2)
		{
			QMessageBox::StandardButton choice =
				QMessageBox::information(this, "Match ended", "AI WIN\nDou you want to start a new game?", QMessageBox::Yes | QMessageBox::Close);
			if (choice == QMessageBox::Yes)
				MainWindow::instance()->start();
			else
				MainWindow::instance()->close();
		}
	}
}

void Board::hint()
{
	if (!_game->isThinking())
	{
		auto _moves = _game->Hint();
		for (auto& _move : _moves)
		{
			_cells[_move / _n][_move % _n]->setSuggested(true);
			_cells[_move / _n][_move % _n]->setSuggested(true);
		}
	}
}

void Board::clear_selected()
{
	_cells[0][0]->clear_selected_cells();
}

void Board::display_cells(int xy_s, int xy_n)
{
	if (!_game->isThinking())
	{
		_cells[xy_s / _n][xy_s % _n]->setSuggested(true);
		_cells[xy_n / _n][xy_n % _n]->setSuggested(true);
	}
}

//selects forced cells with equal validity 
bool Board::forced_cells()
{
	if (_game->forced_moves.empty())
		return false;
	else
	{
		if (!_game->_moves.empty())
		{
			if (_game->_moves.back()._multiple_eat)
			{
				auto app = _game->forced_moves;
				for (auto& p : app)
				{
					if (p.front() != _game->_moves.back()._y1 * _n + _game->_moves.back()._x1)
						_game->forced_moves.remove(p);
				}
			}
		}
		for (auto& p : _game->forced_moves)
		{
			display_cells(p.front(), *std::next(p.begin()));
		}
	}
	return true;
}



bool Board::valid_selection(int x0, int y0)
{
	if (_playerIsWhite && (_cells[y0][x0]->_content == Cell::PIECE_B || _cells[y0][x0]->_content == Cell::P_DAMA_B))
		return false;
	if (!_playerIsWhite && (_cells[y0][x0]->_content == Cell::PIECE_W || _cells[y0][x0]->_content == Cell::P_DAMA_W))
		return false;

	return true;
}

void Board::update_statusbar()
{
	MainWindow::instance()->statusBar()->showMessage(_col + "          " + MainWindow::instance()->date(MainWindow::instance()->timer()->elapsed_time()));
}


void Board::save_board()
{
	if (_game->ended())
		return;

	if (!_game->_moves.empty())
	{
		if (_game->_moves.back()._multiple_eat == true)
			while (_game->_moves.back()._multiple_eat)
			{
				_game->undo();
				if (_game->_moves.empty())
					break;
			}

		updateBoard();
	}

	QFile file("Saved board.txt");
	if (!file.open(QFile::WriteOnly | QIODevice::Text))
		std::cerr << "Impossible to open the file";
	QTextStream t(&file);
	file.resize(0);

	for (int i = 0; i < _n; i++)
		for (int j = 0; j < _n; j++)
			if (_cells[i][j]->content() != Cell::EMPTY)
				t << i << ":" << j << ":" << _cells[i][j]->content() << ";\n";
	file.close();
}