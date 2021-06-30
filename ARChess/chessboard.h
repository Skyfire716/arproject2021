#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QVector2D>
#include <QList>
#include <QtDebug>
#include <QtMath>
#include <opencv2/opencv.hpp>

class chessboard : QObject
{
    Q_OBJECT
    
public:
    static const int BLACK = 0;
    static const int WHITE = 1;
    static const int UNDEFINED = 2;
    chessboard(QObject *parent = nullptr);
    chessboard(const chessboard &board);
    void clear();
    cv::Point2f qvec2d2cv_point2f(QVector2D v);
    int map(int x, int in_min, int in_max, int out_min, int out_max);
    float map(float x, float in_min, float in_max, float out_min, float out_max);
    bool add_field(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color);
    bool get_color(char letter, char number);
    void drawBoard(cv::Mat image);
    QVector2D get_origin();
    QPoint get_origin_index();
    QVector2D get_center(char letter, char number);
    
private:
    QPoint map_koords_to_index(char letter, char number);
    QPoint map_index_to_koords(int x, int y);
    QList<QVector2D> corners[9][9];
    QList<QVector2D> centers[8][8];
    char colors[8][8];
    QPoint origin_index;
    int max_x, min_x, max_y, min_y;
    
};


#endif // CHESSBOARD_H
