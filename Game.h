#pragma once

#include <QThread>
#include "Piece.h"
#include <vector>


struct Move
{
	int _x0, _y0, _x1, _y1;	
	Piece* _moved;			
	Piece* _eaten;			
	bool _multiple_eat;

	Move(Piece* moved, int x0, int y0, int x1, int y1, Piece* eaten, bool multiple_eat)
		: _moved(moved), _x0(x0), _y0(y0), _x1(x1), _y1(y1), _eaten(eaten), _multiple_eat(multiple_eat) {}
};


class Game : public QThread
{
	Q_OBJECT

    protected:

		const int _n = 8;
		Piece*** _pieceBoard;
		bool _myTurn;
		bool _isThinking;	
		int _difficulty;
		std::list<int> white_pieces_left;
		std::list<int> black_pieces_left;


		// eat methods
		void undoForEat();
		void eat(int x0, int y0, int x1, int y1, bool multiple_eat, bool& eat_king);
		bool best_eat(int x_start, int y_start, int& l, int& n_king, std::list<std::list<int>> &paths);

		// AI methods
		float utility();
		float evaluation(bool maxP);

		float negamax(
			int d,
			bool max_player,
			float a = -std::numeric_limits<float>::infinity(),
			float b = +std::numeric_limits<float>::infinity(),
			std::pair<int, int>* best_move = 0);

		void run();

	public:

		std::vector<Move> _moves;

		Game(int diffuculty, bool first_player, bool up, bool turn);
		std::list<std::list<int>> forced_moves; // mosse obbligatorie in quel turno

		std::list<int> bp() const { return black_pieces_left; };
		std::list<int> wp() const { return white_pieces_left; };

		bool transform(int x, int y);

		//getters
		bool myTurn() { return _myTurn; }
		bool isThinking() { return _isThinking; }
		int movesize() { return _moves.size(); }

		//setter
		void setMyTurn(bool mt) { _myTurn = mt;}

		//
		bool move(int x0, int y0, int x1, int y1, bool& multiple_eat);
		void undo();
		bool eat_rules();


		bool draw();
		int wins();
		bool ended();
		std::list<int> Hint();


		Piece*** pieceBoard() { return _pieceBoard; }

	signals:

		// AI found the best move
		void notifyAIMove(int x0, int y0, int x1, int y1);

};