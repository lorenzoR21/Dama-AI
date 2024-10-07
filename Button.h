#include <QLabel>
#include <QMouseEvent>

#pragma once

class Button : public QLabel
{
    Q_OBJECT

 
    public:

        Button(QRect pos);

        // override
        virtual void mousePressEvent(QMouseEvent* e) override;

    signals:
        void clicked();
};

