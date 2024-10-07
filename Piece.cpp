#include "piece.h"

Piece::Piece(int x, int y, bool white, Piece*** board)
{
	_x = x;
	_y = y;
	_white = white;
	_pieceBoard = board;
}

bool Piece::validMove(int x, int y, bool eat)
{	
	//Power for the distance 
	int x2 = (_x - x) * (_x - x);
	int y2 = (_y - y) * (_y - y);

	//Point in board
	if (x >= _n || y >= _n)
		return false;
	
	if (x < 0 || y < 0)
		return false;

	//Point is free
	if (_pieceBoard[y][x] != 0)
			return false;

	//Point is at distance , Point is diagonal
	if (eat)
	{
		if ((x2 + y2) != 8)
			return false;
	}
	else
	{
		if ((x2 + y2) != 2)
			return false;
	}
	
	return true;
}

bool Piece::moveTo(int x, int y, bool& ate)
{
	ate = can_eat();

	if (!validMove(x, y, ate))
		return false;

	if (!ate)
	{
		_pieceBoard[y][x] = this;
		_pieceBoard[_y][_x] = 0;
		_x = x;
		_y = y;
	}
	return true;
}


std::list<std::pair<int, int>> Checker::_possibleMove()
{
	std::list<std::pair<int, int>> possibleMove;

	if (_pieceBoard[_y][_x])
	{
		if (can_eat())
		{
			possibleMove.push_back(end_up_point.front());
			possibleMove.push_back(end_up_point.back());
		}
		else
		{
			if (validMove(_x - 1, _y - 1, false))
				possibleMove.push_back(std::pair<int, int>(_x - 1, _y - 1));
			if (validMove(_x + 1, _y - 1, false))
				possibleMove.push_back(std::pair<int, int>(_x + 1, _y - 1));
			if (validMove(_x - 1, _y + 1, false))
				possibleMove.push_back(std::pair<int, int>(_x - 1, _y + 1));
			if (validMove(_x + 1, _y + 1, false))
				possibleMove.push_back(std::pair<int, int>(_x + 1, _y + 1));
		}
	}
	return possibleMove;
}


bool Checker::validMove(int x, int y, bool eat)
{	
	if (!Piece::validMove(x, y, eat))
		return false;

	if (_white == false) // black up
		if (_y > y)
			return false;

	if (_white == true) //white down
		if (_y < y)
			return false;
	return true;
}

bool Checker::can_eat()
{
	end_up_point.clear();

	bool canEat = false;

	if (validMove(_x + 2, _y + 2, true))
		if (_pieceBoard[_y + 1][_x + 1] != 0 && _pieceBoard[_y + 1][_x + 1]->white() != _white && !_pieceBoard[_y + 1][_x + 1]->king())
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x + 2, _y + 2));
		}
	if (validMove(_x - 2, _y + 2, true))
		if (_pieceBoard[_y + 1][_x - 1] != 0 && _pieceBoard[_y + 1][_x - 1]->white() != _white && !_pieceBoard[_y + 1][_x - 1]->king())
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x - 2, _y + 2));
		}
	if (validMove(_x - 2, _y - 2, true))
		if (_pieceBoard[_y - 1][_x - 1] != 0 && _pieceBoard[_y - 1][_x - 1]->white() != _white && !_pieceBoard[_y - 1][_x - 1]->king())
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x - 2, _y - 2));
		}
	if (validMove(_x + 2, _y - 2, true))
		if (_pieceBoard[_y - 1][_x + 1] != 0 && _pieceBoard[_y - 1][_x + 1]->white() != _white && !_pieceBoard[_y - 1][_x + 1]->king())
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x + 2, _y - 2));
		}

	return canEat;

}

bool King::validMove(int x, int y, bool eat)
{
	if (!Piece::validMove(x, y, eat))
		return false;
	return true;
}

std::list<std::pair<int, int>> King::_possibleMove()
{
	std::list<std::pair<int, int>> possibleMove;

	if (_pieceBoard[_y][_x])
	{
		if (can_eat())
		{
			for (std::list<std::pair<int, int>>::iterator it = end_up_point.begin(); it != end_up_point.end(); it++)
				possibleMove.push_back(*it);
		}
		else
		{
			if (validMove(_x - 1, _y - 1, false))
				possibleMove.push_back(std::pair<int, int>(_x - 1, _y - 1));
			if (validMove(_x + 1, _y - 1, false))
				possibleMove.push_back(std::pair<int, int>(_x + 1, _y - 1));
			if (validMove(_x - 1, _y + 1, false))
				possibleMove.push_back(std::pair<int, int>(_x - 1, _y + 1));
			if (validMove(_x + 1, _y + 1, false))
				possibleMove.push_back(std::pair<int, int>(_x + 1, _y + 1));
		}
	}
	return possibleMove;
}

bool King::can_eat()
{
	end_up_point.clear();

	bool canEat = false;

	if (validMove( _x + 2, _y + 2, true))
		if (_pieceBoard[_y + 1][_x + 1] != 0 && _pieceBoard[_y + 1][_x + 1]->white() != _white)
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x + 2, _y + 2));
		}
	if (validMove(_x - 2, _y + 2, true))
		if (_pieceBoard[_y + 1][_x - 1] != 0 && _pieceBoard[_y + 1][_x - 1]->white() != _white)
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x - 2, _y + 2));
		} 
	if (validMove(_x - 2, _y - 2, true))
		if (_pieceBoard[_y - 1][_x - 1] != 0 && _pieceBoard[_y - 1][_x - 1]->white() != _white)
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x - 2, _y - 2));
		}
	if (validMove(_x + 2, _y - 2, true))
		if (_pieceBoard[_y - 1][_x + 1] != 0 && _pieceBoard[_y - 1][_x + 1]->white() != _white)
		{
			canEat = true;
			end_up_point.push_front(std::pair<int, int>(_x + 2, _y - 2));
		}

	return canEat;
}