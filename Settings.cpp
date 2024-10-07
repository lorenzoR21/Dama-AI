#include "Settings.h"

Settings::Settings(Difficulty difficulty, gameMode mode, bool playerIsWhite, QString name)
{
	_difficulty = difficulty;
	_mode = mode;
	_playerIsWhite = playerIsWhite;
	_name = name;

}
int Settings::difficulty()
{
	if (_difficulty == EASY)
		return 3;
	else if (_difficulty == NORMAL)
		return 7;
	else if (_difficulty == HARD)
		return 9;
	else if (_difficulty == NONE)
		return 0;
}