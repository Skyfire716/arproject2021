#ifndef CAMERA_WORKER_H
#define CAMERA_WORKER_H

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include "chessboard_controller.h"
//#include "chessboard.h"

class camera_worker : public QObject
{
    Q_OBJECT
    QThread worker_thread;
    typedef std::vector<cv::Point> contour_t;
    typedef std::vector<contour_t> contour_vector_t;
    const int BLACK_FIELD_SUM = 58000; //Experience based
    const int WHITE_FIELD_SUM = 500;
    const int WHITE_FIELD_CROSSING = 3000;
    const int BLACK_FIELD_CROSSING = 500;
    static const int COLOR_CHECK_AREA = 5;
    const int HARRIS_DROPOUT = 10000;
    const float LENGTH_VARYING_PERCENTAGE = 0.15;
    const int COUNTER_TL = 0;
    const int COUNTER_BL = 1;
    const int COUNTER_TR = 2;
    const int COUNTER_BR = 3;

public:
    volatile bool init_b;
    volatile bool capture_b;
    volatile bool switch_camera_b;
    volatile bool threshold_change_b;
    volatile bool threshold_method_b;
    volatile bool result_image_index_b;
    volatile int result_image_index;
    volatile int new_cv_index;
    volatile int threshold_value_shared;
    volatile int thresold_method_shared;
    float normalizeVec(cv::Point2f *p);
    double calculateAngles(std::vector<cv::Point> points);
    double angle(cv::Point a, cv::Point b);
    double angle(cv::Point2f a, cv::Point b);
    double angle(cv::Point a, cv::Point2f b);
    double angle(cv::Point2f a, cv::Point2f b);
    float point_distance(cv::Point a, cv::Point b);
    float point_distance(cv::Point2f a, cv::Point b);
    float point_distance(cv::Point a, cv::Point2f b);
    float point_distance(cv::Point2f a, cv::Point2f b);
    int get_ordered_points(cv::Rect rect, std::vector<cv::Point> points);
    int neighbour_rect_probing(cv::Point2f tl, cv::Point2f bl, cv::Point2f tr, cv::Point2f br);
    int neighbour_validation_probing(cv::Point2f tl, cv::Point2f tr, cv::Point2f bl, cv::Point2f br);
    QPointF cv_point2f2qpoint(cv::Point2f p);
    QVector2D cv_point2f2qvec2d(cv::Point2f p);
    cv::Point2f line_probeing(cv::Point2f start_corner, float line_length, cv::Point2f line_normalized_vec);
    cv::Point2f intersection_P2PLine_P2PLine(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f p4);
    cv::Point2f line_P2P(cv::Point2f p1, cv::Point2f p2);
    cv::Point2f angled_vector_from_normal(cv::Point2f normal, float angle_deg);
    cv::Point2f getsubPixel(cv::Point2f point);
    cv::Point2f mean_point(QList<cv::Point> points);
    void harris_values(cv::Rect rect, QList<QPair<cv::Point, double>> *harris_features);
    void harris_edges(cv::Rect rect, QList<cv::Point> *edges);
    void harris_corner(cv::Rect rect, QList<cv::Point> *corners);
    void harris_corner(cv::Rect rect, QList<QPair<cv::Point, double>> *corners);
    QPair<int, int> vec2pair(QVector2D v);
    bool probe_field();
    bool valid_neighbour(cv::Point2f p, cv::Point2f axisA, cv::Point2f axisB);
    bool probe_neighbours(cv::Point2f tl, cv::Point2f tr, cv::Point2f bl, cv::Point2f br, QVector2D current_pos, chessboard_controller &chesscontroller, QSet<QPair<int, int>> &no_field, QSet<QPair<int, int>> &valid);
    bool field_check(cv::Rect rect, cv::Point2f p);
    float intersection_NormalLine_NormalLine(cv::Point2f line_p1, cv::Point2f line_n1, cv::Point2f line_p2, cv::Point2f line_n2);
    float distance_point_to_line(cv::Point2f lineA, cv::Point2f lineB, cv::Point2f p);
    static int check_color(cv::Mat image, int x, int y);
    static int check_color(cv::Mat image, cv::Point p, float area);
    static int check_color(cv::Mat image, cv::Point p);
    static int check_color_wrapper(cv::Mat image, cv::Point2f p);
    bool is_zero(cv::Point2f p);
    bool is_nan(cv::Point2f p);
    static bool point_in_mat(cv::Mat image, cv::Point2f p);
    static bool rect_in_mat(cv::Mat image, cv::Rect rect);
    void print_vec(cv::Point2f p);
    bool running;

public slots:
    void capture_video();
    void change_result_image(int mat_index);
    void change_camera(int cv_index);
    void change_threshold(int thresold);
    void change_threshold_method(int threshold_method);
    void run();

signals:
    void image_ready(QPixmap img);
    void finished();
    void new_ar_transform_singels(float scalar, float xpos, float ypos, float zpos, float x, float y, float z);
    void camera_detected(QString cam_name);
    void chessboard_updated(QPixmap pix);

private:
    int threshold_value;
    int threshold_method;
    QImage *camera_image_ref;
    cv::Mat camera_image;
    cv::Mat gray_image;
    cv::Mat threshold_image;
    cv::Mat *result_image;
    void initialize_camera();
    QList<int> cv_cameras;
    cv::VideoCapture cv_camera;
    chessboard_controller my_chessboard_controller;
};

#endif // CAMERA_WORKER_H
