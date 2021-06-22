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

#define A1 0
#define A3 1
#define A5 2
#define A7 3
#define B2 4
#define B4 5
#define B6 6
#define B8 7
#define C1 8
#define C3 9
#define C5 10
#define C7 11
#define D2 12
#define D4 13
#define D6 14
#define D8 15
#define E1 16
#define E3 17
#define E5 18
#define E7 19
#define F2 20
#define F4 21
#define F6 22
#define F8 23
#define G1 24
#define G3 25
#define G5 26
#define G7 27
#define H2 28
#define H4 29
#define H6 30
#define H8 31

class camera_worker : public QObject
{
    Q_OBJECT
    QThread worker_thread;
    typedef std::vector<cv::Point> contour_t;
    typedef std::vector<contour_t> contour_vector_t;
    const int BLACK_FIELD_SUM = 58000; //Experience based
    const int WHITE_FIELD_CROSSING = 3000;
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
    volatile int result_image_index;
    volatile int new_cv_index;
    volatile int threshold_value_shared;
    volatile int thresold_method_shared;
    float normalizeVec(cv::Point2f *p);
    double calculateAngles(std::vector<cv::Point> points);
    double angle(cv::Point a, cv::Point b);
    float point_distance(cv::Point a, cv::Point b);
    float point_distance(cv::Point2f a, cv::Point2f b);
    int get_ordered_points(cv::Rect rect, std::vector<cv::Point> points);
    void diagonal_probeing(cv::Point2f start_corner, float diagonalLength, cv::Point2f diagonalNormalized, cv::Point2f diagonalNormalVec, cv::Point2f *result_array);
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
