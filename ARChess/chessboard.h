#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QVector2D>
#include <QList>
#include <QtDebug>
#include <QtMath>

class chessboard : QObject
{
    Q_OBJECT
public:
    chessboard();
    void clear();
    int map(int x, int in_min, int in_max, int out_min, int out_max);
    float map(float x, float in_min, float in_max, float out_min, float out_max);
    bool add_field(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color);
    QVector2D get_center(char letter, char number);

private:
    QList<QVector2D> corners[9][9];
    QList<QVector2D> centers[8][8];
    bool colors[8][8];
    int max_x, min_x, max_y, min_y;

};


#endif // CHESSBOARD_H