#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QVector2D>
#include <QList>
#include <QtDebug>
#include <QtMath>
#include <QLineF>
#include <QMatrix4x4>
#include <QGenericMatrix>
#include <QQuaternion>
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
    cv::Point2f qpoint2cv_point2f(QPoint p);
    cv::Point3f qpoint2cv_point3f(QPoint p);
    cv::Point3f qvec2d2cv_point3f(QVector2D v);
    cv::Vec3f qvec2d2cv_vec3f(QVector2D v);
    cv::Vec3f qpoint2cv_vec3f(QPoint p);
    cv::Mat qvec2d2cv_mat(QVector2D v, int cv_mat_type);
    cv::Mat qpoint2cv_mat(QPoint p, int cv_mat_type);
    QVector3D cv_mat2qvec3d(cv::Mat m);
    int map(int x, int in_min, int in_max, int out_min, int out_max);
    float map(float x, float in_min, float in_max, float out_min, float out_max);
    bool add_field(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color);
    bool get_color(char letter, char number);
    void drawBoard(cv::Mat image);
    QVector2D get_origin();
    QVector3D get_origin_normal();
    QPoint get_origin_index();
    QVector2D get_center(char letter, char number);
    cv::Mat get_homography_matrix();
    QPair<QMatrix3x3, QVector3D> get_rotation_translation();
    cv::Mat get_rotation_matrix();
    QQuaternion get_rotation_matrix(bool placeholder);
    QQuaternion cv_mat2qquaternion(cv::Mat rot_mat);
    
private:
    QPoint map_koords_to_index(char letter, char number);
    QPoint map_index_to_koords(int x, int y);
    QVector2D mean_vec(QList<QVector2D> list);
    QList<QVector2D> corners[9][9];
    QList<QVector2D> centers[8][8];
    char colors[8][8];
    QPoint origin_index;
    int max_x, min_x, max_y, min_y;
    
};


#endif // CHESSBOARD_H
