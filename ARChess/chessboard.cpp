#include "chessboard.h"

void chessboard::clear()
{
    min_x = 0;
    min_y = 0;
    max_x = 0;
    max_y = 0;
    origin_index.setX(0);
    origin_index.setY(0);
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            corners[i][j].clear();
        }
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            colors[i][j] = chessboard::UNDEFINED;
            centers[i][j].clear();
        }
    }
}

cv::Point2f chessboard::qvec2d2cv_point2f(QVector2D v)
{
    return cv::Point2f(v.x(), v.y());
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
    if(local_offset.x() > 7 || local_offset.x() < -7 || local_offset.y() > 7  || local_offset.y() < -7){
        qDebug() << "Offset out of Range";
        return false;
    }
    max_x = (max_x < local_offset.x()) ? local_offset.x() : max_x;
    min_x = (min_x > local_offset.x()) ? local_offset.x() : min_x;
    max_y = (max_y < local_offset.y()) ? local_offset.y() : max_y;
    min_y = (min_y > local_offset.y()) ? local_offset.y() : min_y;
    if(max_x - min_x > 8 || max_y - min_y > 8){
        qDebug() << "Error in Indexes";
        return false;
    }
    int x_index = local_offset.x();
    int y_index = local_offset.y();
    if(x_index < 0){
        x_index = 8 + x_index;
    }
    if(y_index < 0){
        y_index = 8 + y_index;
    }
    if(colors[x_index][y_index] != chessboard::UNDEFINED && colors[x_index][y_index] != color){
        qDebug() << "Suspicious coloring";
        return false;
    }
    
    corners[x_index][y_index + 1].push_back(QVector2D(tl_corner));
    corners[x_index + 1][y_index + 1].push_back(QVector2D(tr_corner));
    corners[x_index][y_index].push_back(QVector2D(bl_corner));
    corners[x_index + 1][y_index].push_back(QVector2D(br_corner));
    
    colors[x_index][y_index] = color;
    
    centers[x_index][y_index].push_back(QVector2D(center));
    return true;
}

bool chessboard::get_color(char letter, char number)
{
    QPoint p = map_koords_to_index(letter, number);
    if(p.x() == -1 || p.y() == -1){
        return chessboard::UNDEFINED;
    }
    return colors[p.x()][p.y()];
}

void chessboard::drawBoard(cv::Mat image)
{
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(!corners[i][j].empty()){
                cv::circle(image, qvec2d2cv_point2f(corners[i][j][0]), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            }
            if(!corners[i + 1][j].empty()){
                cv::circle(image, qvec2d2cv_point2f(corners[i + 1][j][0]), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            }
            if(!corners[i][j + 1].empty()){
                cv::circle(image, qvec2d2cv_point2f(corners[i][j + 1][0]), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            }
            if(!corners[i + 1][j + 1].empty()){
                cv::circle(image, qvec2d2cv_point2f(corners[i + 1][j + 1][0]), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            }
            if(!centers[i][j].empty()){
                if(colors[i][j] == chessboard::BLACK){
                    cv::circle(image, qvec2d2cv_point2f(centers[i][j][0]), 5, cv::Scalar(255, 255, 255), 3, cv::LINE_8);
                }else if(colors[i][j] == chessboard::WHITE){
                    cv::circle(image, qvec2d2cv_point2f(centers[i][j][0]), 5, cv::Scalar(0, 0, 0), 3, cv::LINE_8);
                }else{
                    cv::circle(image, qvec2d2cv_point2f(centers[i][j][0]), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                }
            }
        }
    }
}

QVector2D chessboard::get_origin()
{
    return QVector2D(0, 0);
}

QPoint chessboard::get_origin_index()
{
    return map_index_to_koords(0, 0);
}

QVector2D chessboard::get_center(char letter, char number)
{
    QPoint p = map_koords_to_index(letter, number);
    if(p.x() == -1 || p.y() == -1){
        return QVector2D(-1, -1);
    }
    return centers[p.x()][p.y()].at(0);
}

QPoint chessboard::map_koords_to_index(char letter, char number)
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
        x_index = ((min_x < 0 ? 8 - min_x : min_x) + x_index) % 8;
        y_index = ((min_y < 0 ? 8 - min_y : min_y) + y_index) % 8;
        return QPoint(x_index, y_index);
    }
    GET_CENTER_END:
    return QPoint(-1, -1);
}

QPoint chessboard::map_index_to_koords(int x, int y)
{
    if(x < 0 || x > 7 || y < 0 || y > 7){
        return QPoint(-1, -1);
    }
    char x_index = min_x + x;
    char y_index = min_y + y;
    if(x_index < 0){
        x_index = 8 - x_index;
    }
    if(y_index < 0){
        y_index = 8 - y_index;
    }
    return QPoint((x_index % 8) + 65, y_index % 8);
}

chessboard::chessboard(QObject *parent) : QObject(parent)
{}

chessboard::chessboard(const chessboard &board)
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if(i < 8 && j < 8){
                this->centers[i][j].append(board.centers[i][j]);
                this->colors[i][j] = board.colors[i][j];
            }
            this->corners[i][j].append(board.corners[i][j]);
        }
    }
}
