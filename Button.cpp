#include "Button.h"

Button::Button(QRect pos) : QLabel(0)
{
	setGeometry(pos);
	setStyleSheet("background-color: transparent;");
}

void Button::mousePressEvent(QMouseEvent* e) 
{
	QLabel::mousePressEvent(e);
	emit clicked();
}
