#ifndef CHESSBOARD_CONTROLLER_H
#define CHESSBOARD_CONTROLLER_H

#include <QObject>
#include <QList>
#include <QVector2D>
#include <QPair>
#include "chessboard.h"

class chessboard_controller : QObject
{
    Q_OBJECT
public:
    chessboard_controller();
    bool add_rect(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color);
    void switch_board();

private:
    chessboard boards[2];
    bool active;


};

#endif // CHESSBOARD_CONTROLLER_H