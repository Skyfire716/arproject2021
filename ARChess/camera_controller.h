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

class camera_worker : public QObject
{
    Q_OBJECT
    QThread worker_thread;
    typedef std::vector<cv::Point> contour_t;
    typedef std::vector<contour_t> contour_vector_t;

public:
    volatile bool init_b;
    volatile bool capture_b;
    volatile bool switch_camera_b;
    volatile bool threshold_change_b;
    volatile bool threshold_method_b;
    volatile int new_cv_index;
    volatile int threshold_value_shared;
    volatile int thresold_method_shared;

    bool running;

public slots:
    void capture_video();
    void change_camera(int cv_index);
    void change_threshold(int thresold);
    void change_threshold_method(int threshold_method);
    void run();

signals:
    void image_ready(QPixmap img);
    void image_ready(QImage img);
    void image_ready(unsigned char* image, int width, int height);
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
