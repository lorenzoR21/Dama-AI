#pragma once

#include <QElapsedTimer>

class Timer: public QElapsedTimer
{
private:

	int t_save;

public:

	Timer(int t_s) { t_save = t_s; start(); }

	void start() { QElapsedTimer::start(); };
	void restart() { start(); }
	int elapsed_time() {return elapsed() + t_save;};

};

