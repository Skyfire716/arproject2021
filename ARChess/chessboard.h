#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QVector2D>
#include <QList>
#include <QtDebug>
#include <QtMath>
#include <QLineF>
#include <QImage>
#include <QMatrix4x4>
#include <QPair>
#include <QGenericMatrix>
#include <QQuaternion>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

class chessboard : QObject
{
    Q_OBJECT
    const int MEDIA_SEARCH_LOG_BASE = 5;
public:
    static const int BLACK = 0;
    static const int WHITE = 1;
    static const int UNDEFINED = 2;
    static const int TOP_LEFT_CORNER = 0;
    static const int TOP_RIGHT_CORNER = 1;
    static const int BOTTOM_RIGHT_CORNER = 2;
    static const int BOTTOM_LEFT_CORNER = 3;
    chessboard(QObject *parent = nullptr);
    chessboard(const chessboard &board);
    void clear();
    void optimize_chessboard();
    void validating_colors(cv::Mat image, int(*check_color)(cv::Mat image, cv::Point2f p));
    void try_letter_detection(cv::Mat image);
    void setup_reference_data();
    QVector<int> index_keypoints(QVector<cv::KeyPoint> keypoints, QVector<QString> terms);
    QVector<int> index_keypoints(std::vector<cv::KeyPoint> keypoints, QVector<QString> terms);
    int term_frequency(QString term, QVector<int> document, QVector<QString> terms);
    int term_frequency(int term_index, QVector<int> document);
    int document_frequency(QString term, QVector<QPair<char, QVector<int>>> documents, QVector<QString> terms);
    int document_frequency(int term_index, QVector<QPair<char, QVector<int>>> documents);
    float inverse_document_frequency(QString term, QVector<QPair<char, QVector<int>>> documents, QVector<QString> terms);
    float inverse_document_frequency(int term_index, QVector<QPair<char, QVector<int>>> documents);
    float tf_idf(QString term, int document_index, QVector<QPair<char, QVector<int>>> documents, QVector<QString> terms);
    float tf_idf(int term_index, int document_index, QVector<QPair<char, QVector<int>>> documents);
    float tf_idf(QString term, QVector<int> q, QVector<QPair<char, QVector<int>>> documents, QVector<QString> terms);
    float tf_idf(int term_index, QVector<int> q, QVector<QPair<char, QVector<int>>> documents);
    float simularities(QVector<float> a, QVector<float> b);
    QVector<QPair<char, float>> score(QVector<int> q, QVector<QPair<char, QVector<int>>> documents, QVector<QString> terms);
    QPair<cv::Mat, std::vector<cv::KeyPoint>> get_referenceData(cv::Mat reference);
    QPair<std::vector<cv::KeyPoint>, std::vector<cv::DMatch>> get_features(cv::Mat reference_descriptors, std::vector<cv::KeyPoint>, cv::Mat real_image);
    void detect_simularities(cv::Mat *reference_descriptors, std::vector<cv::KeyPoint> *reference_keypoints, cv::Mat real_image);
    void bla_wrapper(cv::Mat image);
    QString KeyPoint2QString(cv::KeyPoint kp);
    cv::Point2f qvec2d2cv_point2f(QVector2D v);
    cv::Point2f qpoint2cv_point2f(QPoint p);
    cv::Point2f qpoint2f2cv_point2f(QPointF p);
    cv::Point3f qpoint2cv_point3f(QPoint p);
    cv::Point3f qvec2d2cv_point3f(QVector2D v);
    cv::Vec3f qvec2d2cv_vec3f(QVector2D v);
    cv::Vec3f qpoint2cv_vec3f(QPoint p);
    cv::Mat qvec2d2cv_mat(QVector2D v, int cv_mat_type);
    cv::Mat qpoint2cv_mat(QPoint p, int cv_mat_type);
    QVector2D get_mean_line_for_segment(int x, int y, bool x_axis);
    QVector3D cv_mat2qvec3d(cv::Mat m);
    int map(int x, int in_min, int in_max, int out_min, int out_max);
    float map(float x, float in_min, float in_max, float out_min, float out_max);
    bool add_field(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color);
    bool get_color(char letter, char number);
    void drawBoard(cv::Mat image);
    void drawRect(cv::Mat image, int x, int y);
    void drawRect(cv::Mat image, QPointF a, QPointF b, QPointF c, QPointF d);
    QVector2D get_corner_by_indizes(int x, int y);
    QPointF get_board_corner(const int CORNER_CODE);
    QPair<int, int> get_board_corner_indizes(const int CORNER_CODE);
    QPair<int, int> get_board_corner_center_indizes(const int CORNER_CODE);
    QVector2D get_origin();
    QVector3D get_origin_normal();
    QPoint get_origin_index();
    QVector2D get_center(char letter, char number);
    cv::Mat get_homography_matrix();
    cv::Mat get_homography_matrix_boardCorners(cv::Size areaOI);
    QPair<QMatrix3x3, QVector3D> get_rotation_translation();
    QPair<QMatrix3x3, QVector3D> get_rotation_translation(cv::Point2f tl, cv::Point2f tr, cv::Point2f br, cv::Point2f bl, cv::Size areaOI);
    cv::Mat get_rotation_matrix(cv::Point2f tl, cv::Point2f tr, cv::Point2f br, cv::Point2f bl, cv::Size areaOI);
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
    cv::Mat descriptors[16];
    std::vector<cv::KeyPoint> keypoints[16];
    QVector<QPair<char, QVector<int>>> letter_vectors;
    QVector<QPair<char, QVector<int>>> number_vectors;
    QVector<QString> letterterms;
    QVector<QString> numberterms;
};


#endif // CHESSBOARD_H
