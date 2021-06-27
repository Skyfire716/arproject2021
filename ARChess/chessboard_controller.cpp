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
