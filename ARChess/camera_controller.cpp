#include "camera_controller.h"
#include <QDebug>
#include <QPixmap>
#include <opencv2/opencv.hpp>

void camera_worker::capture_video()
{

}

void camera_worker::initialize_camera()
{
    int camera_index = 0;
    bool found_cam = true;
    do{
        cv_camera.open(camera_index);
        found_cam = cv_camera.isOpened();
        if(found_cam){
            QString item_str;
            item_str = "Camera ";
            item_str.append(QString::number(camera_index));
            cv_cameras.append(camera_index);
            emit camera_detected(item_str);
            qDebug() << "Found Camera " << camera_index;
        }
        camera_index++;
    }while(found_cam || camera_index < 10);
    qDebug() << "Found "  << cv_cameras.length() << " Cameras";
}

void camera_worker::change_camera(int cv_index)
{
    new_cv_index = cv_index;
    switch_camera_b = true;
    qDebug() << "Switching Cam in CV";
}

void camera_worker::run()
{
    qDebug() << "Running";
    while(running){
        if(init_b){
            init_b = false;
            initialize_camera();
        }
        if(switch_camera_b){
            switch_camera_b = false;
            cv_camera.open(new_cv_index);
        }
        if(capture_b && cv_camera.isOpened()){
            cv_camera >> camera_image;
            if(camera_image.empty()){
                qDebug() << "Image empty";
                break;
            }
            cv::cvtColor(camera_image, camera_image, CV_BGR2RGB);
            QImage img((uchar*)camera_image.data, camera_image.cols, camera_image.rows, camera_image.step, QImage::Format_RGB888);
            if (!img.isNull()){
                emit image_ready(QPixmap::fromImage(img));
            }
        }
    }
}

void camera_controller::init()
{
    worker->init_b = true;
}

void camera_controller::start_capture()
{
    worker->capture_b = true;
    worker_thread->start();
}

void camera_controller::stop_capture()
{
    worker->capture_b = false;
}


