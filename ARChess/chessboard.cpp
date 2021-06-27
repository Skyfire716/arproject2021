#include "chessboard.h"


chessboard::chessboard()
{

}

void chessboard::clear()
{
    min_x = 0;
    min_y = 0;
    max_x = 0;
    max_y = 0;
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            corners[i][j].clear();
        }
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            colors[i][j] = false;
            centers[i][j].clear();
        }
    }
}

int chessboard::map(int x, int in_min, int in_max, int out_min, int out_max)
{
    if(in_max - in_min == 0){
        return x;
    }
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float chessboard::map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool chessboard::add_field(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color)
{
    if(centers[(int) local_offset.x()][(int) local_offset.y()].empty()){
        max_x = (max_x < local_offset.x()) ? local_offset.x() : max_x;
        min_x = (min_x > local_offset.x()) ? local_offset.x() : min_x;
        max_y = (max_y < local_offset.y()) ? local_offset.y() : max_y;
        min_y = (min_y > local_offset.y()) ? local_offset.y() : min_y;
        qDebug() << "MIN MAX X " << QString::number(min_x) << QString::number(max_x);
        qDebug() << "MIN MAX Y " << QString::number(min_y) << QString::number(max_y);
        if(max_x - min_x > 8 || max_y - min_y > 8){
            qDebug() << "Error in Indexes";
            return false;
        }
        corners[map((int)local_offset.x(), min_x, max_x, 0, 7)][map((int)local_offset.y() + 1, min_y, max_y, 0, 7)].push_back(QVector2D(tl_corner));
        corners[map((int)local_offset.x() + 1, min_x, max_x, 0, 7)][map((int)local_offset.y() + 1, min_y, max_y, 0, 7)].push_back(QVector2D(tr_corner));
        corners[map((int)local_offset.x(), min_x, max_x, 0, 7)][map((int)local_offset.y(), min_y, max_y, 0, 7)].push_back(QVector2D(bl_corner));
        corners[map((int)local_offset.x() + 1, min_x, max_x, 0, 7)][map((int)local_offset.y(), min_y, max_y, 0, 7)].push_back(QVector2D(br_corner));
        colors[map((int)local_offset.x(), min_x, max_x, 0, 7)][map((int)local_offset.y(), min_y, max_y, 0, 7)] = color;
        centers[map((int)local_offset.x(), min_x, max_x, 0, 7)][map((int)local_offset.y(), min_y, max_y, 0, 7)].push_back(QVector2D(center));
        return true;
    }
    return false;
}

QVector2D chessboard::get_center(char letter, char number)
{
    int y_index = 0;
    int x_index = 0;
    if(letter >= 65 && letter < 73){
        x_index = letter % 65;
        if(number >= 0 && number < 8){
            y_index = number;
        }else if(number >= 48 && number < 56){
            y_index = number % 48;
        }else{
            goto GET_CENTER_END;
        }
        x_index = map(map(x_index, 0, 7, min_x, max_x), min_x, max_x, 0, 7);
        y_index = map(map(y_index, 0, 7, min_y, max_y), min_y, max_y, 0, 7);
        return centers[x_index][y_index].at(0);
    }
    GET_CENTER_END:
    return QVector2D(-1, -1);
}
