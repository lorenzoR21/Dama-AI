#include "Cell.h"
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include "Board.h"

std::vector<QPixmap> Cell::pieces = std::vector<QPixmap>();

Cell::Cell(Board* board, int bx, int by) : QGraphicsRectItem()
{
	_x = bx;
	_y = by;
	_content = EMPTY;
	_board = board;
	_selected = false;
	_mouseover = false;
	_suggested = false;
	_draggable = false;
	this->setZValue(0);


	if (_y % 2 == 0)
		_color = _x % 2 == 0 ? color_dark : color_bright;
	else
		_color = _x % 2 == 0 ? color_bright : color_dark;

	if (Cell::pieces.empty())
	{
		pieces.push_back(QPixmap(":/graphics/pawnW.png").scaled(1.2 * cellsize, 1.2 * cellsize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		pieces.push_back(QPixmap(":/graphics/pawnB.png").scaled(1.2 * cellsize, 1.2 * cellsize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		pieces.push_back(QPixmap(":/graphics/damaW.png").scaled(1.2 * cellsize, 1.2* cellsize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		pieces.push_back(QPixmap(":/graphics/damaB.png").scaled(1.2* cellsize, 1.2 * cellsize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));	
	}


	setFlags(QGraphicsItem::ItemIsSelectable |
		QGraphicsItem::ItemIsMovable);

	setAcceptHoverEvents(true);

	// set rectangle position and dimension
	setRect(cellsize * _x, cellsize * _y, cellsize, cellsize);

}
void Cell::clear_selected_cells()
{
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			_board->_cells[i][j]->setSelected(false);
			_board->_cells[i][j]->setSuggested(false);
		}
}

void Cell::mouseMoveEvent(QGraphicsSceneMouseEvent* event) 
{
	if (_board->_game->isThinking())
		return;

	if (_board->_cells[_y][_x]->_content == EMPTY)
		return;

	if (_board->forced_cells())
	{
		bool continuare = false;

		for (auto c : _board->_game->forced_moves)
		{
			if (_x == c.front() % 8 && _y == c.front() / 8)
				continuare = true;
		}
		if (!continuare)
			return;
	}

	_selected = true;

	// possible move
	if (_board->_game->pieceBoard()[_y][_x])
	{
		auto pMove = _board->_game->pieceBoard()[_y][_x]->_possibleMove();
		for (auto& move : pMove)
			_board->_cells[move.second][move.first]->setSelected(true);
	}
	_draggable = true;

	// update
	update();

	QGraphicsRectItem::mouseMoveEvent(event);
}

void Cell::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) 
{
	if (!_selected)
		return;

	if (_board->_cells[_y][_x]->_content == EMPTY)
		return;

	int prev_x = _x;
	int prev_y = _y;
	bool doMove = false;
	if (_draggable) 
	{
		auto pMove = _board->_game->pieceBoard()[_y][_x]->_possibleMove();

		QList<QGraphicsItem*> colItems = collidingItems();
		if (colItems.isEmpty())
		{
			_x = QPointF(this->scenePos()).x();
			_y = QPointF(this->scenePos()).y();
		}
		else 
		{
			QGraphicsItem* closestItem = 0;
			int shortestDist = 100;
			for(auto item : colItems) 
			{
				QLineF line(item->sceneBoundingRect().center(), this->sceneBoundingRect().center());
				for (auto p : pMove)
					if (item->pos().x() == p.first && item->pos().y() == p.second)
						if (line.length() < shortestDist)
						{
							shortestDist = line.length();
							closestItem = item;
						}
			}
			if (closestItem != 0)
			{
				_x = QPointF(closestItem->scenePos()).x();
				_y = QPointF(closestItem->scenePos()).y();
				doMove = true;
			}
		}
		_draggable = false;
	}

	// clear selections
	clear_selected_cells();

	// do the move
	if (doMove)
		_board->move(prev_x, prev_y, _x, _y);

	_board->_cells[prev_y][prev_x]->setPos(QPointF(prev_x, prev_y));
	_board->_cells[prev_y][prev_x]->_x = prev_x;
	_board->_cells[prev_y][prev_x]->_y = prev_y;

	_selected = false;

	update();

	QGraphicsRectItem::mouseReleaseEvent(event);
}

void Cell::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
	_mouseover = true;
	update();
}
void Cell::hoverLeaveEvent(QGraphicsSceneHoverEvent* e)
{
	_mouseover = false;
	update();
}

void Cell::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setRenderHint(QPainter::TextAntialiasing);

	if (_suggested && !_draggable)
	{
		painter->setRenderHint(QPainter::HighQualityAntialiasing);
		painter->setPen(QPen(QBrush(_suggested ? Qt::yellow : Qt::red), 4, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
	}
	else
		painter->setPen(Qt::NoPen);

	// draw cell background
	if (_draggable)
	{
		setZValue(2);
		painter->setBrush(Qt::transparent);
	}
	else
	{
		painter->setBrush(QBrush(_color));
		if (!_suggested)
			painter->setPen(QPen(QColor(0, 0, 0), 2, Qt::SolidLine));
	}

	painter->drawRect(rect());

	// draw cell selection
	if ((_mouseover || _selected) && !_draggable)
	{
		painter->setOpacity(0.3);
		if (_selected)
			painter->setBrush(QBrush(color_selected));
		else
			painter->setBrush(QBrush(color_over));
		painter->drawRect(rect());
		painter->setOpacity(1.0);
	}

	// draw piece
	if (_content)
		painter->drawPixmap(rect().topLeft() - QPointF(0.1 * cellsize, 0.1 * cellsize), pieces[_content - 1]);
}
