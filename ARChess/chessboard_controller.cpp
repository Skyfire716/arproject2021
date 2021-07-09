#include "chessboard_controller.h"

chessboard_controller::chessboard_controller()
{
    
}

bool chessboard_controller::add_rect(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color)
{
    return boards[!active].add_field(local_offset, tl_corner, tr_corner, bl_corner, br_corner, center, color);
}

void chessboard_controller::switch_board()
{
    active = !active;
    boards[!active].clear();
}

void chessboard_controller::clear_current()
{
    boards[!active].clear();
}

void chessboard_controller::optimize_current_board()
{
    boards[!active].optimize_chessboard();
}

void chessboard_controller::validate_current_board(cv::Mat image, int (*check_color)(cv::Mat, cv::Point2f))
{
    boards[!active].validating_colors(image, check_color);
}

void chessboard_controller::try_letter_detection(cv::Mat image)
{
    boards[!active].try_letter_detection(image);
}

chessboard chessboard_controller::get_current_board()
{
    return boards[!active];
}

bool chessboard_controller::get_field(char letter, char number)
{
    return boards[!active].get_color(letter, number);
}

QQuaternion chessboard_controller::get_ar_rotation()
{
    QPair<QMatrix3x3, QVector3D> pair = boards[!active].get_rotation_translation();
    return QQuaternion::fromRotationMatrix(pair.first);
}

QPair<QQuaternion, QVector3D> chessboard_controller::get_transform()
{
    QPair<QMatrix3x3, QVector3D> pair = boards[!active].get_rotation_translation();
    return QPair<QQuaternion, QVector3D>(QQuaternion::fromRotationMatrix(pair.first).normalized(), pair.second);
}

bool chessboard_controller::get_origin_color()
{
    QVector2D origin = boards[!active].get_origin();
    qDebug() << "Origin by " << origin;
    return get_field(origin.x(), origin.y());
}

QImage chessboard_controller::get_image()
{
    qDebug() << "normal " << boards[!active].get_origin_normal();
    QImage img(80, 80, QImage::Format_RGB666);
    QPoint center = boards[!active].get_origin_index();
    qDebug() << "Center Coords " << center;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            QColor color;
            int board_color = boards[!active].get_color(i + 65, j);
            if(board_color == chessboard::BLACK){
                color.setRgb(0, 0, 0);
            }else if(board_color == chessboard::WHITE){
                color.setRgb(255, 255, 255);
            }else if(board_color == chessboard::UNDEFINED){
                color.setRgb(255, 0, 0);
            }else{
                color.setRgb(0, 0, 255);
            }
            if(i == (center.x() % 65) && j == center.y()){
                color.setRgb(0, 255, 0);
            }
            for(int s = 0; s < 10; s++){
                for(int k = 0; k < 10; k++){
                    img.setPixelColor(10 * i + s, 10 * j + k, color);
                }
            }
        }
    }
    return img;
}
