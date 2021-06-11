#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QApplication>

class commonWidget: public QWidget
{
    Q_OBJECT
public:
    commonWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::Dialog | Qt::WindowType::WindowMinMaxButtonsHint | Qt::WindowType::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
    }
    virtual ~commonWidget() {};

protected:
    void mousePressEvent(QMouseEvent* event) {
        mouse_is_press = true;
        mouse_move_len = event->globalPos() - this->pos();
    }
    void mouseMoveEvent(QMouseEvent* event) {
        if (mouse_is_press && (event->buttons() && Qt::LeftButton)
            && (event->globalPos() - mouse_move_len).manhattanLength() > QApplication::startDragDistance())
        {
            move(event->globalPos() - mouse_move_len);
            mouse_move_len = event->globalPos() - pos();
        }
    }
    void mouseReleaseEvent(QMouseEvent* event) {
        mouse_is_press = false;
    }

private:
    bool mouse_is_press;
    QPoint  mouse_move_len;
};
