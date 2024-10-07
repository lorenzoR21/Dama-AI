#include "Game.h"
#include <vector>
#include <iostream>
#include <windows.h> 
#include <QFile>
#include <QTextStream>


Game::Game(int diffuculty, bool first_player, bool up, bool turn)
{
	if (!up)
		_myTurn = first_player;
	else
		_myTurn = turn;
	_isThinking = false;
	_difficulty = diffuculty;

	_pieceBoard = new Piece * *[_n];
	for (int i = 0; i < _n; i++)
	{
		_pieceBoard[i] = new Piece * [_n];
		for (int j = 0; j < _n; j++)
			_pieceBoard[i][j] = 0;
	}

	if (up == false)
	{
		for (int i = 0; i < _n; i++)
			for (int j = 0; j < _n; j++)
			{
				if (j % 2 == 0 && (i == 0 || i == 2))
				{
					_pieceBoard[i][j] = new Checker(j, i, false, _pieceBoard);
					black_pieces_left.push_front(i * _n + j);

				}
				else if (j % 2 != 0 && i == 1)
				{
					_pieceBoard[i][j] = new Checker(j, i, false, _pieceBoard);
					black_pieces_left.push_front(i * _n + j);
				}

				if (j % 2 != 0 && (i == 5 || i == 7))
				{
					_pieceBoard[i][j] = new Checker(j, i, true, _pieceBoard);
					white_pieces_left.push_front(i * _n + j);

				}
				else if (j % 2 == 0 && i == 6)
				{
					_pieceBoard[i][j] = new Checker(j, i, true, _pieceBoard);
					white_pieces_left.push_front(i * _n + j);

				}
			}

		if (!first_player && diffuculty != 0)
			start();

	}
	else
	{
		QFile file("Saved board.txt");
		if (!file.open(QFile::ReadOnly))
			std::cerr << "Impossible to open the file";
		QTextStream t(&file);
		QString bp = "";
		int count = 0;
		while (!t.atEnd())
		{
			QString temp = t.readLine(0);
			if (temp == "\n" || temp.isEmpty())
				continue;
			else
			{
				bp += temp;
				count++;
			}
		}
		file.close();
		int i = 0;
		QStringList li = bp.split(";");
		for (auto& s : li)
		{
			QStringList n = s.split(":");


			if (first_player)
			{
				if (n.at(2).toInt() == 2)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new Checker(n.at(1).toInt(), n.at(0).toInt(), false, _pieceBoard);
					black_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}

				else if (n.at(2).toInt() == 4)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new King(n.at(1).toInt(), n.at(0).toInt(), false, _pieceBoard);
					black_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}
				else if (n.at(2).toInt() == 1)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new Checker(n.at(1).toInt(), n.at(0).toInt(), true, _pieceBoard);
					white_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}
				else if (n.at(2).toInt() == 3)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new King(n.at(1).toInt(), n.at(0).toInt(), true, _pieceBoard);
					white_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}

			}
			else
			{
				if (n.at(2).toInt() == 2)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new Checker(n.at(1).toInt(), n.at(0).toInt(), true, _pieceBoard);
					white_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}

				else if (n.at(2).toInt() == 4)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new King(n.at(1).toInt(), n.at(0).toInt(), true, _pieceBoard);
					white_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}
				else if (n.at(2).toInt() == 1)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new Checker(n.at(1).toInt(), n.at(0).toInt(), false, _pieceBoard);
					black_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}
				else if (n.at(2).toInt() == 3)
				{
					_pieceBoard[n.at(0).toInt()][n.at(1).toInt()] = new King(n.at(1).toInt(), n.at(0).toInt(), false, _pieceBoard);
					black_pieces_left.push_front(n.at(0).toInt() * _n + n.at(1).toInt());
				}

			}

			i++;
			if (i == count)
				break;
		}

		if (!_myTurn && diffuculty != 0)
			start();
	}

}

bool Game::move(int x0, int y0, int x1, int y1, bool& multiple_eat)
{
	if (!_pieceBoard[y0][x0])
		return false;

	bool white;
	bool ate = false;

	if (_pieceBoard[y0][x0]->white())
		white = true;
	else
		white = false;

	bool rightMove = false;
	int app = 0;
	if (!forced_moves.empty())
	{
		for (auto& vp : forced_moves)
		{
			if (vp.front() % _n == x0 && vp.front() / _n == y0) //controllo se sto muovendo il pezzo giusto
			{
				std::list<int>::iterator it = std::next(vp.begin());
				if (*it % _n == x1 && *it / _n == y1)
				{
					rightMove = true;
					app = vp.size();
				}
			}
		}

		if (!rightMove)
			return false;

		if (app > 2) //mangiata multipla
		{
			multiple_eat = true;
			auto m = forced_moves;
			for (auto& vp : m)
			{
				if (!(vp.front() % _n == x0 && vp.front() / _n == y0)) 
				{
					std::list<int>::iterator it = std::next(vp.begin());
					if (!(*it % _n == x1 && *it / _n == y1))
						forced_moves.remove(vp);
				}
			}
		}
		for (auto& vp : forced_moves)
			vp.pop_front();
	}

	if(!_pieceBoard[y0][x0]->moveTo(x1, y1, ate))
		return false;

	if (ate)
	{
		bool eat_king = false;
		eat(x0, y0, x1, y1, multiple_eat, eat_king);

		if (white)
			black_pieces_left.remove(_moves.back()._eaten->_y * _n + _moves.back()._eaten->_x);
		else
			white_pieces_left.remove(_moves.back()._eaten->_y * _n + _moves.back()._eaten->_x);
	}
	else
	{
		_moves.push_back(Move(_pieceBoard[y1][x1], x0, y0, x1, y1, 0, multiple_eat));
	}

	if (transform(x1, y1))
	{
		bool c = true;
		if (!_pieceBoard[y1][x1]->white())
			c = false;
		_pieceBoard[y1][x1] = 0;
		_pieceBoard[y1][x1] = new King(x1, y1, c, _pieceBoard);
	}

	if (!white)
	{
		black_pieces_left.push_back(y1 * _n + x1);
		black_pieces_left.remove(y0 * _n + x0);

	}
	else
	{
		white_pieces_left.push_back(y1 * _n + x1);
		white_pieces_left.remove(y0 * _n + x0);
	}

	//change turn, solo se non è in corso una mangiata multipla
    if (!multiple_eat)
		_myTurn = !_myTurn;

	return true;
}

bool Game::transform(int x, int y)
{
	if (_pieceBoard[y][x]->white() && y == 0)
		return true;
	if (!_pieceBoard[y][x]->white() && y == 7)
		return true;
	return false;
}

void Game::eat(int x0, int y0, int x1, int y1, bool multiple_eat, bool &eat_king)
{
	//coordinate del pezzo mangiato
	int xy0 = y0 * 8 + x0;
	int xy1 = y1 * 8 + x1;
	int a = abs((xy1)-(xy0)) / 2;
	int m = max(xy0, xy1) - a;
	int xe = m % 8;
	int ye = m / 8;

	_moves.push_back(Move(_pieceBoard[y0][x0], x0, y0, x1, y1, _pieceBoard[ye][xe], multiple_eat));

	_pieceBoard[y1][x1] = _moves.back()._moved;
	_pieceBoard[y1][x1]->_x = _moves.back()._x1;
	_pieceBoard[y1][x1]->_y = _moves.back()._y1;

	_pieceBoard[y0][x0] = 0;

	if (_pieceBoard[ye][xe]->king())
		eat_king = true;
	else
		eat_king = false;

	_pieceBoard[ye][xe] = 0;
}


void Game::undoForEat()
{
	if (_moves.empty())
		return;

	Move last_move = _moves.back();

	_pieceBoard[last_move._y0][last_move._x0] = last_move._moved;
	_pieceBoard[last_move._y0][last_move._x0]->_x = last_move._x0;
	_pieceBoard[last_move._y0][last_move._x0]->_y = last_move._y0;

	if (last_move._eaten)
	{

		int xy0 = last_move._y0 * 8 + last_move._x0;
		int xy1 = last_move._y1 * 8 + last_move._x1;
		int a = abs((xy1)-(xy0)) / 2;
		int m = max(xy0, xy1) - a;
		int xe = m % 8;
		int ye = m / 8;

		_pieceBoard[ye][xe] = last_move._eaten;
		_pieceBoard[ye][xe]->_x = xe;
		_pieceBoard[ye][xe]->_y = ye;
	}
	_pieceBoard[last_move._y1][last_move._x1] = 0;

	_moves.pop_back();
}

bool Game::best_eat(int x_start, int y_start, int& l, int& n_king, std::list<std::list<int>> &paths)
{
	if (!_pieceBoard[y_start][x_start]->can_eat())
		return false;

	int i = 0;
	int n = 0;
	int app = 0;
	int size = 0;
	int king = 0;
	bool eat_king = false;

	std::vector<int> path_king;
	std::list<int> app_path;
	std::vector<std::pair<int, int>> multiple_eat_piece; 

	app_path.push_back(y_start * _n + x_start);
	do
	{
		while (_pieceBoard[y_start][x_start]->can_eat())
		{
			size = _pieceBoard[y_start][x_start]->end_up_point.size();
			if (size > 1)
				if (!multiple_eat_piece.empty()) //serve per la prima volta che incontro un nodo dove posso mangiare più volte (altrimenti da errore alla if successiva, perchè il vettore è ancora nullo),in quel caso vado nel ramo else, dove mi salvo la posizione dove posso mangiare due volte
				{
					if (multiple_eat_piece.back().first == y_start * _n + x_start)
					{
						if (multiple_eat_piece.back().second < size) // se ho mangiato gia una o più volta, vado a considerare solo i lati dove non sono andato
							for (int i = 0; i < (size - multiple_eat_piece.back().second); i++)
								_pieceBoard[y_start][x_start]->end_up_point.pop_back();
					}
					else // altrimenti sono in un'altro nodo dove posso mangiare più volte e lo salvo 
						multiple_eat_piece.push_back(std::pair<int, int>(y_start * _n + x_start, size));
				}
				else 
					multiple_eat_piece.push_back(std::pair<int, int>(y_start * _n + x_start, size));

			eat(x_start, y_start, _pieceBoard[y_start][x_start]->end_up_point.back().first, _pieceBoard[y_start][x_start]->end_up_point.back().second, false, eat_king); //se le mangiate sono due vado a destra
			
			if (eat_king)
				king++;

			x_start = _moves.back()._x1;
			y_start = _moves.back()._y1;

			app_path.push_back(y_start * _n + x_start);

			i++;
		}

		path_king.push_back(king);

		app = i;

		// REWIND
		//ritorno al primo nodo dove posso mangiare più volte
		if (!multiple_eat_piece.empty())
		{
			while (((x_start != multiple_eat_piece.back().first % _n || y_start != multiple_eat_piece.back().first / _n) && app != 0) || ((x_start == multiple_eat_piece.back().first % _n && y_start == multiple_eat_piece.back().first / _n && multiple_eat_piece.back().second >= 1) && app != 0))
			{

				if (x_start == multiple_eat_piece.back().first % _n && y_start == multiple_eat_piece.back().first / _n && multiple_eat_piece.back().second == 1)
					multiple_eat_piece.pop_back();

				if (_moves.back()._eaten->king())
					king--;
				x_start = _moves.back()._x0;
				y_start = _moves.back()._y0;
				undoForEat();
				app--;

				if (!multiple_eat_piece.empty())
				{
					// se sono arrivato al nodo dove posso mangiare più volte, ma ho già controllato tutte mangiate vado indietro ancora
					if (x_start == multiple_eat_piece.back().first % _n && y_start == multiple_eat_piece.back().first / _n && multiple_eat_piece.back().second == 1)
					{
						multiple_eat_piece.pop_back();
						if (app != 0)
						{
							if (_moves.back()._eaten->king())
								king--;
							x_start = _moves.back()._x0;
							y_start = _moves.back()._y0;
							undoForEat();
							app--;
						}
					}
				}


				// se dopo il precedentente controllo non ho piu nodi "mangiata multipla" esco da ciclo, perchè devo entrate nel successivo ciclo che mi riporta allo start
				if (multiple_eat_piece.empty() == true)
					break;

				if (x_start == multiple_eat_piece.back().first % _n && y_start == multiple_eat_piece.back().first / _n && multiple_eat_piece.back().second > 1)
					break;
			}
		}
		//se non ci sono nodi dove posso mangiare più volte oppure già sono stati visitati, ritorno all'inizio
		if (multiple_eat_piece.empty() || (multiple_eat_piece.size() == 1 && multiple_eat_piece.back().second == 1))
			while (app > 0)
			{
				if (_moves.back()._eaten->king())
					king--;
				x_start = _moves.back()._x0;
				y_start = _moves.back()._y0;
				undoForEat();
				app--;
			}

		if (!multiple_eat_piece.empty())
			if (app == 0 && multiple_eat_piece.size() == 1 && multiple_eat_piece.back().second == 1)
				multiple_eat_piece.pop_back();

		// se sono ritornato al nodo dove posso mangiare più volte decremento il numero di mangiate rimanenti
		if (!multiple_eat_piece.empty())
			if (x_start == multiple_eat_piece.back().first % _n && y_start == multiple_eat_piece.back().first / _n && multiple_eat_piece.back().second != 1)
				multiple_eat_piece.back().second--;


		paths.push_back(app_path);

		n++;

		if (i != 0)	// bisogna salvare il percorso precedente, partendo dall'ultimo nodo che mangia più volte
			for (int j = 0; j < (i - app); j++)
				app_path.pop_back();
		else
		{
			app_path.clear();
			app_path.push_back(y_start * _n + x_start);
		}

		i = app; // riparte il conteggio

	} while (!multiple_eat_piece.empty());

	l = 0;
	for (auto q : paths)	//stabilisco la lunghezza della mangiata più lunga
	{
		if (q.size() >= l)
			l = q.size();
	}

	n_king = 0;
	i = 0;
	for (auto q : paths)	//stabilisco il numero di dame mangiate nel percorso migliore
	{
		if (q.size() == l)
			if(path_king[i] > n_king)
				n_king = path_king[i];
		i++;
	}

	i = 0;
	auto copy_paths = paths;
	for (auto q : copy_paths)	//tengo nel vettore app_paths solo le coordinate che mi porteranno al percorso migliore, gestendo il caso in cui ci siano più percorsi
	{
		if (q.size() != l)
			paths.remove(q);
		if (q.size() == l && path_king[i] != n_king)
			paths.remove(q);
		i++;
	}
	return true;
}

bool Game::eat_rules()
{
	forced_moves.clear();

	std::list<int> ctr = white_pieces_left;
	if (!_myTurn)
		ctr = black_pieces_left;

	std::list<std::list<int>> app_best;
	int lung_eat = 0;
	int app_lung_eat = 0;
	int n_king = 0;
	int app_n_king = 0;

	for (auto& p : ctr)
	{
		app_best.clear();
		if (best_eat(p % _n, p / _n, lung_eat, n_king, app_best))
		{
			if (lung_eat > app_lung_eat)	//obbligatorio mangiare dove ci sono più pezzi
			{
				app_lung_eat = lung_eat;
				app_n_king = n_king;
				forced_moves = app_best;
			}
			else if (lung_eat == app_lung_eat)
			{
				if (_pieceBoard[p / _n][p % _n]->king() == false && _pieceBoard[forced_moves.front().front() / _n][forced_moves.front().front() % _n]->king() == false)
					for (auto& q : app_best)
						forced_moves.push_back(q);
				else if (_pieceBoard[p / _n][p % _n]->king() == true && _pieceBoard[forced_moves.front().front() / _n][forced_moves.front().front() % _n]->king() == false)	//a parità di pezzi da prendere, obbligatorio mangiare con la dama
					forced_moves = app_best;
				else if (_pieceBoard[p / _n][p % _n]->king() == true && _pieceBoard[forced_moves.front().front() / _n][forced_moves.front().front() % _n]->king() == true && n_king >= app_n_king)	//a parità di pezzi da prendere, la dama sceglie la presa dove si mangiano più dame
				{
					if (app_n_king == n_king)
						for (auto& q : app_best)
							forced_moves.push_back(q);
					else
					{
						app_n_king = n_king;
						forced_moves = app_best;
					}
				}
			}
		}
	}
	
	if (forced_moves.empty())
		return false;

	return true;
}

void Game::undo()
{
	if (_moves.empty())
		return;

	forced_moves.clear();

	bool repeat = false;

	do
	{
		Move last_move = _moves.back();

		bool white = last_move._moved->white();

		_pieceBoard[last_move._y0][last_move._x0] = last_move._moved;
		_pieceBoard[last_move._y0][last_move._x0]->_x = last_move._x0;
		_pieceBoard[last_move._y0][last_move._x0]->_y = last_move._y0;

		if (!white)
		{
			black_pieces_left.push_back(last_move._y0 * _n + last_move._x0);
			black_pieces_left.remove(last_move._y1 * _n + last_move._x1);
		}
		else
		{
			white_pieces_left.push_back(last_move._y0 * _n + last_move._x0);
			white_pieces_left.remove(last_move._y1 * _n + last_move._x1);
		}

		if (last_move._eaten)
		{

			int xy0 = last_move._y0 * 8 + last_move._x0;
			int xy1 = last_move._y1 * 8 + last_move._x1;
			int a = abs((xy1)-(xy0)) / 2;
			int m = max(xy0, xy1) - a;
			int xe = m % 8;
			int ye = m / 8;

			_pieceBoard[ye][xe] = last_move._eaten;
			_pieceBoard[ye][xe]->_x = xe;
			_pieceBoard[ye][xe]->_y = ye;

			if (white)
				black_pieces_left.push_back(ye * _n + xe);
			else
				white_pieces_left.push_back(ye * _n + xe);

		}
		_pieceBoard[last_move._y1][last_move._x1] = 0;
		_moves.pop_back();

		if (!_moves.empty())
		{

			if (_moves.back()._multiple_eat == true && _moves.back()._x1 == last_move._x0 && _moves.back()._y1 == last_move._y0)
				repeat = true;
			else
				repeat = false;
		}
		else
			repeat = false;
		
		if (!repeat)
			_myTurn = !_myTurn;

	} while (repeat);

	eat_rules();
 }

bool Game::draw()
{
	bool c = 0;
	if (black_pieces_left.size() < 6 && white_pieces_left.size() < 6)
	{
		for (int i = _moves.size() - 1; i > _moves.size() - 11; i--)
		{
			if (_moves[i]._x0 == _moves[i - 4]._x0)
				if (_moves[i]._y0 == _moves[i - 4]._y0)
					c = true;
				else
					c = false;
		}
	}
	return c;
}

int Game::wins()
{
	if (black_pieces_left.empty())
		return 1;
	if (white_pieces_left.empty())
		return 2;
	int no_moves_b = 1;
	int no_moves_w = 2;

	for (auto& b : black_pieces_left)
		if (!_pieceBoard[b / _n][b % _n]->_possibleMove().empty())
			no_moves_b = 0;

	for (auto& b : white_pieces_left)
		if (!_pieceBoard[b / _n][b % _n]->_possibleMove().empty())
			no_moves_w = 0;

	if (no_moves_b == 1)
		return 1;
	if (no_moves_w == 2)
		return 2;
	return 0;
}

bool Game::ended()
{
	if (wins() != 0)
		return true;
	return false;
}

float Game::utility()
{
	if (wins() == 1)
		return -5000;
	else if (wins() == 2)
		return 5000;
	else
		return 0;
}

// make best move for the given player
float Game::negamax(int d, bool max_player, float a, float b, std::pair<int, int>* best_move)
{
	bool me = false;
	// return utility if game is ended (more reliable than eval())
	if (ended())
		return (max_player ? 1 : -1) * (utility() + d);

	// return evaluation if maximum game depth has been reached
	if (d == 0)
		return (max_player ? 1 : -1) * evaluation(max_player);

	// initialize maximum evaluation for this node 'u'
	float eu = -std::numeric_limits<float>::infinity();

	// loop over children 'v' of current game node 'u'
	if (!eat_rules())
	{
		auto _piecesCopy = max_player ? black_pieces_left : white_pieces_left;	// <-- make a copy since this is a shared object

		for (auto& p : _piecesCopy)
		{
			auto moves = _pieceBoard[p / _n][p % _n]->_possibleMove();
			for (auto& m : moves)
			{
				// make the move corresponding to 'v'
				move(p % _n, p / _n, m.first, m.second, me);

				// maximise utility and store best move so far
				float ev = -negamax(d - 1, !max_player, -b, -a);

				// restore previous game state / undo the move
				undo();

				// store best move
				if (ev > eu)
				{
					eu = ev;
					if (best_move)
					{
						best_move->first = p;
						best_move->second = m.second * _n + m.first;
					}
				}

				// update best move
				if (eu > a)
					a = eu;

				// alpha-beta pruning
				if (a >= b)
					return a;

			}
		}
	}
	else
	{
		int cont = 0;
		auto f_m = forced_moves;
		for (std::list<std::list<int>>::iterator it = f_m.begin(); it != f_m.end(); it++)
		{
			auto mov = *it;
			bool m_eat;
			for (std::list<int> ::iterator m_it = mov.begin(); m_it != mov.end(); m_it++)
			{
				m_eat = false;
				if (std::next(m_it) == mov.end())
					break;

				move(*m_it % _n, *m_it / _n, *std::next(m_it) % _n, *std::next(m_it) / _n, m_eat);
				if (!m_eat)
					break;
			}

			// maximise utility and store best move so far
			float ev = -negamax(d - 1, !max_player, -b, -a);

			// restore previous game state / undo the move
			undo();

			// store best move
			if (ev > eu)
			{
				eu = ev;
				if (best_move)
				{
					best_move->first = mov.front();
					best_move->second = cont;
				}
			}

			cont++;

			// update best move
			if (eu > a)
				a = eu;

			// alpha-beta pruning
			if (a >= b)
				return a;
		}
	}
	return eu;
}


// since this class inherits from thread, we can override run()
// so that it is automatically executed on a separate thread
// when the 'start()' is called
// AI is implemented here
void Game::run()
{
	// nothing to do if game is ended
	if (ended())
		return;

	eat_rules();
	if (forced_moves.size() == 1 && forced_moves.front().size() == 2)
	{
		emit notifyAIMove(forced_moves.front().front() % _n, forced_moves.front().front() / _n, forced_moves.front().back() % _n, forced_moves.front().back() / _n);
		return;
	}
	else if (forced_moves.size() == 1 && forced_moves.front().size() > 2)
	{
		std::list<int> app = forced_moves.front();
		for (std::list<int> ::iterator it = app.begin(); it != app.end(); it++)
		{

			if (std::next(it) == app.end())
				break;
			emit notifyAIMove(*it % _n, *it / _n, *std::next(it) % _n, *std::next(it) / _n);

		}
		return;
	}
	// begin thinking
	_isThinking = true;

	// AI
	std::pair<int, int> best_move;
	float mM = negamax(_difficulty, !_myTurn, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), &best_move);

	// end thinking
	_isThinking = false;

	// notify move
	if (!eat_rules())
	{
		emit notifyAIMove(best_move.first % _n, best_move.first / _n, best_move.second % _n, best_move.second / _n);
	}
	else
	{
		int i = 0;
		std::list<int> app;
		for (auto& m : forced_moves)
		{
			if (i == best_move.second)
			{
				app = m;
				break;
			}
			i++;
		}
		for (std::list<int> ::iterator it = app.begin(); it != app.end(); it++) 
		{
			if (std::next(it) == app.end())
				break;
			emit notifyAIMove(*it % _n, *it / _n, *std::next(it) % _n, *std::next(it) / _n);
		}
	}

}


// give quick hint 
std::list<int> Game::Hint()
{
	std::list<int> hint;
	std::pair<int, int> temp;

	eat_rules();
	if (forced_moves.size() == 1)
	{
		hint = forced_moves.back();
		return hint;
	}
	negamax(5, !_myTurn, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), &temp);
	if (!eat_rules())
	{
		hint.push_back(temp.first);
		hint.push_back(temp.second);
		return hint;
	}
	else
	{
		int i = 0;
		for (auto& m : forced_moves)
		{
			if (i == temp.second)
			{
				hint = m;
				break;
			}
			i++;
		}
	}

	return hint;
}

float Game::evaluation(bool maxP)
{
	int eval = 0;
	int v1 = 0, v2 = 0;
	int n_blackCheckers = 0, n_blackKings = 0, n_whiteCheckers = 0, n_whiteKings = 0;
	int n_blackCheckers_center = 0, n_blackKings_center = 0, n_whiteCheckers_center = 0, n_whiteKings_center = 0;
	int n_blackCheckers_edge = 0, n_blackKings_edge = 0, n_whiteCheckers_edge = 0, n_whiteKings_edge = 0;
	int code = 0;

	static int edge[14] = { 0, 2, 4, 6, 15, 16, 31, 32, 47, 48, 57, 59, 61, 63 }; //OK

	static int center[8] = { 18, 20, 27, 29, 34, 36, 43, 45 }; // OK

	static int safeedge[4] = { 6, 15, 48, 57 };  //OK

	int tempo = 0;
	int n_checkers = 0, n_kings = 0;

	const int turn = 2;						//color to move gets +turn
	const int brv = 3;						//multiplier for back rank
	const int kcv = 5;						//multiplier for kings in center
	const int mcv = 1;						//multiplier for men in center
	const int mev = 1;						//multiplier for men on edge
	const int kev = 5;						//multiplier for kings on edge
	const int cramp = 5;					//multiplier for cramp
	const int opening = -2;					// multipliers for tempo
	const int midgame = -1;
	const int endgame = 2;
	const int intactdoublecorner = 3;

	int backrank = 0;

	for (auto& w : white_pieces_left)
		if (_pieceBoard[w / _n][w % _n]->king())
			n_whiteKings++;
	int wp = white_pieces_left.size();
	n_whiteCheckers = std::abs(wp - n_whiteKings);

	for (auto& b : black_pieces_left)
		if (_pieceBoard[b / _n][b % _n]->king())
			n_blackKings++;
	int bp = black_pieces_left.size();
	n_blackCheckers = std::abs(bp - n_blackKings);

	v1 = 100 * n_blackCheckers + 130 * n_blackKings;
	v2 = 100 * n_whiteCheckers + 130 * n_whiteKings;

	eval = v1 - v2;							
	eval += (250 * (v1 - v2)) / (v1 + v2);	

	n_checkers = n_blackCheckers + n_whiteCheckers;
	n_kings = n_blackKings + n_whiteKings;

	//
	if (_myTurn)
		eval += turn;
	else
		eval -= turn;
	//

	/* cramp */
	if (_pieceBoard[4][0] != 0 && _pieceBoard[5][1] != 0)
		if ((!_pieceBoard[4][0]->king() && !_pieceBoard[4][0]->white()) && ((!_pieceBoard[5][1]->king() && !_pieceBoard[5][1]->white())))
			eval += cramp;
	if (_pieceBoard[3][7] != 0 && _pieceBoard[2][6] != 0)
		if ((!_pieceBoard[3][7]->king() && _pieceBoard[3][7]->white()) && ((!_pieceBoard[2][6]->king() && _pieceBoard[2][6]->white())))
			eval -= cramp;

	/* back rank guard */
	code = 0;
	if (_pieceBoard[0][0] != 0 && !_pieceBoard[0][0]->king())
		code++;
	if (_pieceBoard[0][2] != 0 && !_pieceBoard[0][2]->king())
		code += 2;
	if (_pieceBoard[0][4] != 0 && !_pieceBoard[0][4]->king())
		code += 4;
	if (_pieceBoard[0][6] != 0 && !_pieceBoard[0][6]->king())
		code += 8; 
	switch (code) {
	case 0:
		code = 0;
		break;

	case 1:
		code = -1;
		break;

	case 2:
		code = 1;
		break;

	case 3:
		code = 0;
		break;

	case 4:
		code = 1;
		break;

	case 5:
		code = 1;
		break;

	case 6:
		code = 2;
		break;

	case 7:
		code = 1;
		break;

	case 8:
		code = 1;
		break;

	case 9:
		code = 0;
		break;

	case 10:
		code = 7;
		break;

	case 11:
		code = 4;
		break;

	case 12:
		code = 2;
		break;

	case 13:
		code = 2;
		break;

	case 14:
		code = 9;
		break;

	case 15:
		code = 8;
		break;
	}

	backrank = code;

	code = 0;
	if (_pieceBoard[7][1] != 0 && !_pieceBoard[7][1]->king())
		code += 8;
	if (_pieceBoard[7][3] != 0 && !_pieceBoard[7][3]->king())
		code += 4;
	if (_pieceBoard[7][5] != 0 && !_pieceBoard[7][5]->king())
		code += 2;
	if (_pieceBoard[7][7] != 0 && !_pieceBoard[7][7]->king())
		code++; 
	switch (code) {
	case 0:
		code = 0;
		break;

	case 1:
		code = -1;
		break;

	case 2:
		code = 1;
		break;

	case 3:
		code = 0;
		break;

	case 4:
		code = 1;
		break;

	case 5:
		code = 1;
		break;

	case 6:
		code = 2;
		break;

	case 7:
		code = 1;
		break;

	case 8:
		code = 1;
		break;

	case 9:
		code = 0;
		break;

	case 10:
		code = 7;
		break;

	case 11:
		code = 4;
		break;

	case 12:
		code = 2;
		break;

	case 13:
		code = 2;
		break;

	case 14:
		code = 9;
		break;

	case 15:
		code = 8;
		break;
	}

	backrank -= code;
	eval += brv * backrank;

	if (_pieceBoard[0][6] != 0 && !_pieceBoard[0][6]->king() && !_pieceBoard[0][6]->white())
		if ((_pieceBoard[1][5] != 0 && !_pieceBoard[1][5]->king() && !_pieceBoard[1][5]->white()) || (_pieceBoard[1][7] != 0 && !_pieceBoard[1][7]->king() && !_pieceBoard[1][7]->white()))
			eval += intactdoublecorner;

	if (_pieceBoard[7][1] != 0 && !_pieceBoard[7][1]->king() && _pieceBoard[7][1]->white())
		if ((_pieceBoard[6][0] != 0 && !_pieceBoard[6][0]->king() && _pieceBoard[6][0]->white()) || (_pieceBoard[6][2] != 0 && !_pieceBoard[6][2]->king() && _pieceBoard[6][2]->white()))
			eval -= intactdoublecorner;


	/* center control */
	for (int i = 0; i < 8; i++)
	{
		if (_pieceBoard[center[i] / _n][center[i] % _n] != 0)
		{
			if (!_pieceBoard[center[i] / _n][center[i] % _n]->white() && !_pieceBoard[center[i] / _n][center[i] % _n]->king())
				n_blackCheckers_center++;
			if (!_pieceBoard[center[i] / _n][center[i] % _n]->white() && _pieceBoard[center[i] / _n][center[i] % _n]->king())
				n_blackKings_center++;
			if (_pieceBoard[center[i] / _n][center[i] % _n]->white() && !_pieceBoard[center[i] / _n][center[i] % _n]->king())
				n_whiteCheckers_center++;
			if (_pieceBoard[center[i] / _n][center[i] % _n]->white() && _pieceBoard[center[i] / _n][center[i] % _n]->king())
				n_whiteKings_center++;
		}
	}

	eval += (n_blackCheckers_center - n_whiteCheckers_center) * mcv;
	eval += (n_blackKings_center - n_whiteKings_center) * kcv;

	/*edge*/
	for (int i = 0; i < 14; i++)
	{
		if (_pieceBoard[edge[i] / _n][edge[i] % _n] != 0)
		{
			if (!_pieceBoard[edge[i] / _n][edge[i] % _n]->white() && !_pieceBoard[edge[i] / _n][edge[i] % _n]->king())
				n_blackCheckers_edge++;
			if (!_pieceBoard[edge[i] / _n][edge[i] % _n]->white() && _pieceBoard[edge[i] / _n][edge[i] % _n]->king())
				n_blackKings_edge++;
			if (_pieceBoard[edge[i] / _n][edge[i] % _n]->white() && !_pieceBoard[edge[i] / _n][edge[i] % _n]->king())
				n_whiteCheckers_edge++;
			if (_pieceBoard[edge[i] / _n][edge[i] % _n]->white() && _pieceBoard[edge[i] / _n][edge[i] % _n]->king())
				n_whiteKings_edge++;
		}
	}

	eval -= (n_blackCheckers_edge - n_whiteCheckers_edge) * mev;
	eval -= (n_blackKings_edge - n_whiteKings_edge) * kev;

	/* tempo */
	for (auto& w : white_pieces_left)
		if (!_pieceBoard[w / _n][w % _n]->king())
			tempo -= 7 - (w / _n);
	for (auto& b : black_pieces_left)
		if (!_pieceBoard[b / _n][b % _n]->king())
			tempo += b / _n;

	if (n_checkers >= 16)
		eval += opening * tempo;
	if ((n_checkers <= 15) && (n_checkers >= 12))
		eval += midgame * tempo;
	if (n_checkers < 9)
		eval += endgame * tempo;

	for (int i = 0; i < 4; i++) 
	{
		if (n_blackKings + n_blackCheckers > n_whiteKings + n_whiteCheckers && n_whiteKings < 3)
		{
			if (_pieceBoard[safeedge[i] / _n][safeedge[i] % _n] != 0 && _pieceBoard[safeedge[i] / _n][safeedge[i] % _n]->white() && _pieceBoard[safeedge[i] / _n][safeedge[i] % _n]->king())
				eval -= 15;
		}

		if (n_whiteKings + n_whiteCheckers > n_blackKings + n_blackCheckers && n_blackKings < 3) 
		{
			if (_pieceBoard[safeedge[i] / _n][safeedge[i] % _n] != 0 && !_pieceBoard[safeedge[i] / _n][safeedge[i] % _n]->white() && _pieceBoard[safeedge[i] / _n][safeedge[i] % _n]->king())
				eval += 15;
		}
	}

	return(eval);
}