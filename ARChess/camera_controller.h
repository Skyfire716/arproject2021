#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include<opencv2/videoio.hpp>
#include<QString>
#include<QList>
#include<QThread>
#include<QObject>
#include<QImage>
#include <QPixmap>
#include <QDebug>
#include <QVector>
#include <cstdio>

#define CHESSBOARD_A1 0
#define CHESSBOARD_A3 1
#define CHESSBOARD_A5 2
#define CHESSBOARD_A7 3
#define CHESSBOARD_B2 4
#define CHESSBOARD_B4 5
#define CHESSBOARD_B6 6
#define CHESSBOARD_B8 7
#define CHESSBOARD_C1 8
#define CHESSBOARD_C3 9
#define CHESSBOARD_C5 10
#define CHESSBOARD_C7 11
#define CHESSBOARD_D2 12
#define CHESSBOARD_D4 13
#define CHESSBOARD_D6 14
#define CHESSBOARD_D8 15
#define CHESSBOARD_E1 16
#define CHESSBOARD_E3 17
#define CHESSBOARD_E5 18
#define CHESSBOARD_E7 19
#define CHESSBOARD_F2 20
#define CHESSBOARD_F4 21
#define CHESSBOARD_F6 22
#define CHESSBOARD_F8 23
#define CHESSBOARD_G1 24
#define CHESSBOARD_G3 25
#define CHESSBOARD_G5 26
#define CHESSBOARD_G7 27
#define CHESSBOARD_H2 28
#define CHESSBOARD_H4 29
#define CHESSBOARD_H6 30
#define CHESSBOARD_H8 31

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
    const int COLOR_CHECK_AREA = 5;
    const int HARRIS_DROPOUT = 10000;
    const float LENGTH_VARYING_PERCENTAGE = 0.15;
    const int COUNTER_TL = 0;
    const int COUNTER_BL = 1;
    const int COUNTER_TR = 2;
    const int COUNTER_BR = 3;
    const int A1_RANGES[4] = {7, 7, 0, 0};
    const int A3_RANGES[4] = {5, 7, 2, 0};
    const int A5_RANGES[4] = {3, 7, 4, 0};
    const int A7_RANGES[4] = {1, 7, 6, 0};
    const int B2_RANGES[4] = {6, 6, 1, 1};
    const int B4_RANGES[4] = {4, 6, 3, 1};
    const int B6_RANGES[4] = {2, 6, 5, 1};
    const int B8_RANGES[4] = {0, 6, 7, 1};
    const int C1_RANGES[4] = {7, 5, 0, 2};
    const int C3_RANGES[4] = {5, 5, 2, 2};
    const int C5_RANGES[4] = {3, 5, 4, 2};
    const int C7_RANGES[4] = {1, 5, 6, 2};
    const int D2_RANGES[4] = {6, 4, 1, 3};
    const int D4_RANGES[4] = {4, 4, 3, 3};
    const int D6_RANGES[4] = {2, 4, 5, 3};
    const int D8_RANGES[4] = {0, 4, 7, 3};
    const int E1_RANGES[4] = {7, 3, 0, 4};
    const int E3_RANGES[4] = {5, 3, 2, 4};
    const int E5_RANGES[4] = {3, 3, 4, 4};
    const int E7_RANGES[4] = {1, 3, 6, 4};
    const int F2_RANGES[4] = {6, 2, 1, 5};
    const int F4_RANGES[4] = {4, 2, 3, 5};
    const int F6_RANGES[4] = {2, 2, 5, 5};
    const int F8_RANGES[4] = {0, 2, 7, 5};
    const int G1_RANGES[4] = {7, 1, 0, 6};
    const int G3_RANGES[4] = {5, 1, 2, 6};
    const int G5_RANGES[4] = {3, 1, 4, 6};
    const int G7_RANGES[4] = {1, 1, 6, 6};
    const int H2_RANGES[4] = {6, 0, 1, 7};
    const int H4_RANGES[4] = {4, 0, 3, 7};
    const int H6_RANGES[4] = {2, 0, 5, 7};
    const int H8_RANGES[4] = {0, 0, 7, 7};
    int CHESS_BOARD_RANGER[32][4] = {{7, 7, 0, 0}, {5, 7, 2, 0}, {3, 7, 4, 0}, {1, 7, 6, 0},
                                           {6, 6, 1, 1}, {4, 6, 3, 1}, {2, 6, 5, 1}, {0, 6, 7, 1},
                                           {7, 5, 0, 2}, {5, 5, 2, 2}, {3, 5, 4, 2}, {1, 5, 6, 2},
                                           {6, 4, 1, 3}, {4, 4, 3, 3}, {2, 4, 5, 3}, {0, 4, 7, 3},
                                           {7, 3, 0, 4}, {5, 3, 2, 4}, {3, 3, 4, 4}, {1, 3, 6, 4},
                                           {6, 2, 1, 5}, {4, 2, 3, 5}, {2, 2, 5, 5}, {0, 2, 7, 5},
                                           {7, 1, 0, 6}, {5, 1, 2, 6}, {3, 1, 4, 6}, {1, 1, 6, 6},
                                           {6, 0, 1, 7}, {4, 0, 3, 7}, {2, 0, 5, 7}, {0, 0, 7, 7}
                                          };
    const int CHESS_BOARD_MAP[32] = {
                                        8111,   //A1
                                        6311,   //A3
                                        5411,   //A5
                                        7211,   //A7
                                        7222,   //B2
                                        5422,   //B4
                                        6322,   //B6
                                        7211,   //B8
                                        6311,   //C1
                                        6333,   //C3
                                        5433,   //C5
                                        6322,   //C7
                                        5422,   //D2
                                        5444,   //D4
                                        5433,   //D6
                                        5411,   //D8
                                        5411,   //E1
                                        5433,   //E3
                                        5444,   //E5
                                        5422,   //E7
                                        6322,   //F2
                                        5433,   //F4
                                        6333,   //F6
                                        6311,   //F8
                                        7211,   //G1
                                        6322,   //G3
                                        5422,   //G5
                                        7222,   //G7
                                        7211,   //H2
                                        5411,   //H4
                                        6311,   //H6
                                        8111,   //H8
                                        };

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
    bool rect_probing(cv::Point2f tl, cv::Point2f bl, cv::Point2f tr, cv::Point2f br, QVector2D current_pos);
    void line_probeing(cv::Point2f start_corner, float line_length, cv::Point2f line_normalized_vec, cv::Point2f lineNormalized, cv::Point2f *result_array);
    void center_probing(cv::Point2f *guideA, cv::Point2f *guideB, cv::Point2f center, int *max_valid_index);
    void diagonal_center_probing(cv::Point2f *guideA, int maxA, cv::Point2f *guideB, int maxB, cv::Point2f *diagonal);
    void diagonal_probeing(cv::Point2f start_corner, float diagonalLength, cv::Point2f diagonalNormalized, cv::Point2f diagonalNormalVec, cv::Point2f *result_array, bool is_black);
    cv::Point2f intersection_P2PLine_P2PLine(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f p4);
    cv::Point2f line_P2P(cv::Point2f p1, cv::Point2f p2);
    cv::Point2f angled_vector_from_normal(cv::Point2f normal, float angle_deg);
    cv::Point2f getsubPixel(cv::Point2f point);
    cv::Point2f mean_point(QList<cv::Point> points);
    void harris_values(cv::Rect rect, QList<QPair<cv::Point, double>> *harris_features);
    void harris_edges(cv::Rect rect, QList<cv::Point> *edges);
    void harris_corner(cv::Rect rect, QList<cv::Point> *corners);
    void harris_corner(cv::Rect rect, QList<QPair<cv::Point, double>> *corners);
    bool field_check(cv::Rect rect, cv::Point2f p);
    float intersection_NormalLine_NormalLine(cv::Point2f line_p1, cv::Point2f line_n1, cv::Point2f line_p2, cv::Point2f line_n2);
    float distance_point_to_line(cv::Point2f lineA, cv::Point2f lineB, cv::Point2f p);
    int check_color(cv::Mat image, int x, int y);
    int check_color(cv::Mat image, cv::Point p, float area);
    int check_color(cv::Mat image, cv::Point p);
    bool is_zero(cv::Point2f p);
    bool is_nan(cv::Point2f p);
    bool point_in_mat(cv::Mat image, cv::Point2f p);
    bool rect_in_mat(cv::Mat image, cv::Rect rect);
    void find_center_points(cv::Point2f center_point, cv::Point2f direction, float length, contour_vector_t contours);
    void check_texture(cv::Point2f start, cv::Point2f *resultA, cv::Point2f *resultB, cv::Point2f guideA, cv::Point2f guideB, cv::Point2f normalDiagonalA, cv::Point2f normalDiagonalB, cv::Point2f guideAB);
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
    void camera_detected(QString cam_name);

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
    QList<QPair<QVector2D, bool>> chessboard;
};

class camera_controller : public QObject
{
    Q_OBJECT
    QThread *worker_thread;
public:
    camera_controller(){
        worker_thread = new QThread();
        worker = new camera_worker();
        worker->moveToThread(worker_thread);
        worker->running = true;
        connect(worker_thread, &QThread::started, worker, &camera_worker::run);
        connect(worker, &camera_worker::finished, worker_thread, &QThread::quit);
        connect(worker, &camera_worker::finished, worker, &camera_worker::deleteLater);
        connect(worker_thread, &QThread::finished, worker_thread, &QThread::deleteLater);
    }
    ~camera_controller(){
        worker_thread->quit();
        worker_thread->wait();
    }
    camera_worker *worker;

public slots:
    void init();
    void start_capture();
    void stop_capture();
private:

};

#endif // CAMERA_CONTROLLER_H
