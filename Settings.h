#pragma once
#include <QString>

enum gameMode { HUMAN_VS_HUMAN, HUMAN_VS_AI };
enum Difficulty { EASY, NORMAL, HARD, NONE };

class Settings
{
	private:
		Difficulty _difficulty;
		gameMode _mode;
		bool _playerIsWhite;
		QString _name;

	public:
		Settings(Difficulty difficulty, gameMode mode, bool playerIsWhite, QString name);

		//getters
		int difficulty();
		Difficulty difficulty_enum() { return _difficulty; };
		gameMode mode() { return _mode; }
		bool playerIsWhite() { return _playerIsWhite; }
		QString name() { return _name; }

		//setters
		void setDifficulty(Difficulty dif) { _difficulty = dif; };
		void setGameMode(gameMode gm) { _mode = gm; };
		void setColour(bool a) { _playerIsWhite = a; ; }
		void setName(QString name) { _name = name; }

};