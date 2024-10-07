#pragma once

#include <list>

class Game;

class Piece
{
	protected:

		const int _n = 8;
		int _x;
		int _y;
		bool _white;
		bool _king = false;
		Piece*** _pieceBoard;
		
	public:

		Piece(int x, int y, bool white, Piece*** board);

		//getter
		bool white() { return _white; }
		int x() { return _x; }
		int y() { return _y; }
		bool king() { return _king; }

		//setter
		void setWhite(bool white) { _white = white; }
		void setX(int x) { _x = x; }
		void setY(int y) { _y = y; }
		void setKing(bool king) { _king = king; }


		std::list<std::pair<int, int>> end_up_point;

		virtual std::list<std::pair<int, int>> _possibleMove() = 0;
		virtual bool validMove(int x, int y, bool eat);
		bool moveTo(int x, int y, bool& ate);
		virtual bool can_eat() = 0;

		friend Game;
};

class Checker: public Piece
{	
	public:
	
		Checker(int x, int y, bool white, Piece*** board) : Piece(x, y, white, board) {}
		virtual std::list<std::pair<int, int>> _possibleMove();
		virtual bool validMove(int x, int y, bool eat);
		virtual bool can_eat();

};

class King: public Piece
{		
	public:
		
		King(int x, int y, bool white, Piece*** board) : Piece(x, y, white, board) { _king = true; };
		virtual std::list<std::pair<int, int>> _possibleMove();
		virtual bool validMove(int x, int y, bool eat);
		virtual bool can_eat();
};