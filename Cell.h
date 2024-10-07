#pragma once

#include <QGraphicsRectItem>
#include <QColor>

class Board;

class Cell : public QGraphicsRectItem
{
	enum cellContent{EMPTY, PIECE_W, PIECE_B, P_DAMA_W, P_DAMA_B};

	// options
	static const int cellsize = 100;
	QColor color_dark = QColor(139, 69, 19);
	QColor color_bright = QColor(205, 133, 63);
	QColor color_over = QColor(255, 228, 181);
	QColor color_selected = QColor(255, 255, 0);

	// graphics
	static std::vector<QPixmap> pieces;


	protected:

		Board* _board;
		int _x, _y;					// board coordinates
		cellContent _content;		// cell content
		QColor _color;				// cell color
		bool _mouseover;			// whether the mouse is over the cell
		bool _selected;				// whether the cell is selected 
		bool _suggested;			// whether the cell is suggested for a move
		bool _draggable = false;

		virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

	    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* e);
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* e);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	public:

		Cell(Board* board, int bx, int by);

		// getters
		bool empty() { return _content == EMPTY; }
		cellContent content() { return _content; }

		// setters
		void setContent(cellContent newContent) { _content = newContent; update(); }
		void setSelected(bool selected) { _selected = selected; update(); }
		void setSuggested(bool suggested) { _suggested = suggested; setZValue(suggested); update(); }
			
		friend class Board;

	public slots:
		void clear_selected_cells();
};
