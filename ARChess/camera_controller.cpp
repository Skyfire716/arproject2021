#include "camera_controller.h"
#include <QDebug>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QTemporaryDir>
#include <math.h>

double camera_worker::angle(cv::Point a, cv::Point b){
    return (acos((a.x*b.x+a.y*b.y * 1.0)/(sqrt(pow(a.x, 2)+pow(a.y, 2))*sqrt(pow(b.x, 2)+pow(b.y, 2) * 1.0))) * 180.0 / M_PI);
}

double camera_worker::calculateAngles(std::vector<cv::Point> points) {
    double angled = 0;
    for(int i = 0; i < points.size(); i++) {
        cv::Point v = points[i] - points[(i+1) % 4];
        cv::Point u = points[(i+2)%4] - points[(i+1)%4];
        double subangle = angle(u, v);
        if (subangle <= 80 || subangle >= 100){
            subangle /= 2;
        }
        angled += subangle;
    }
    return angled;
}

void camera_worker::capture_video()
{

}

void camera_worker::change_result_image(int mat_index)
{
    result_image_index = mat_index;
}

void camera_worker::initialize_camera()
{
    int camera_index = 0;
    bool found_cam = true;
    cv::VideoCapture temp_cam;
#ifdef Q_OS_ANDROID
    camera_image = CV_CAP_ANDROID;
    qDebug() << "Android CAP "  << QString::number(CV_CAP_ANDROID) << "\t" << QString::number(CV_CAP_ANDROID_BACK) << "\t" << QString::number(CV_CAP_ANDROID_FRONT);
    temp_cam.open(CV_CAP_ANDROID);
    qDebug() << "Is open " << temp_cam.isOpened();
    temp_cam.open(CV_CAP_ANDROID_BACK);
    qDebug() << "Is open? " << temp_cam.isOpened();
    temp_cam.open(CV_CAP_ANDROID_FRONT);
    qDebug() << "Is open= " << temp_cam.isOpened();
#endif
    do{
        temp_cam.open(camera_index);
        found_cam = temp_cam.isOpened();
        if(found_cam){
            qDebug() << "Backend " << QString::fromStdString(temp_cam.getBackendName());
            QString item_str;
            item_str = "Camera ";
            item_str.append(QString::number(camera_index));
            cv_cameras.append(camera_index);
            emit camera_detected(item_str);
            qDebug() << "Found Camera " << camera_index;
        }
        camera_index++;
        //qDebug() << "Searching camera " << camera_index;
    }while(found_cam || camera_index < 10);
    qDebug() << "Found "  << cv_cameras.length() << " Cameras";
}

void camera_worker::change_camera(int cv_index)
{
    new_cv_index = cv_index;
    switch_camera_b = true;
    qDebug() << "Switching Cam in CV";
}

void camera_worker::change_threshold(int thresold)
{
    threshold_value_shared = thresold;
    threshold_change_b = true;
}

void camera_worker::change_threshold_method(int threshold_method)
{
    thresold_method_shared = threshold_method;
    threshold_method_b = true;
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
            if(new_cv_index == -1){
                qDebug() << "Load from File";
                //TODO Check here if you get an error on non linux systems
                QTemporaryDir tempDir;
                if(tempDir.isValid()){
                    const QString tempFile = tempDir.path() +  "/archess.mp4";
                    if(QFile::copy(":/videos/resources/videos/VID_20210603_121318.mp4", tempFile)){
                        qDebug() << "Mp4 " << tempFile;
                        cv_camera.open(tempFile.toStdString());
                    }
                }
            }else{
                cv_camera.open(new_cv_index);
            }
        }
        if(threshold_change_b){
            threshold_change_b = false;
            threshold_value = threshold_value_shared;
        }
        if(threshold_method_b){
            threshold_method_b = false;
            threshold_method = thresold_method_shared;
        }
        if(capture_b && cv_camera.isOpened()){
            cv_camera >> camera_image;
            if(camera_image.empty()){
                qDebug() << "Image empty";
                break;
            }
            /*
             * DO OpenCV Operations here
             **/
            cv::cvtColor(camera_image, gray_image, CV_BGR2GRAY);
            cv::threshold(gray_image, threshold_image, threshold_value, 255, threshold_method);
            contour_vector_t contours;
            cv::findContours(threshold_image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
            std::vector<cv::Rect> boundingRects(contours.size());
            for(size_t k = 0; k < contours.size(); k++){
                contour_t approx_contour;
                cv::approxPolyDP(contours[k], approx_contour, cv::arcLength(contours[k], true) * 0.02, true);
                boundingRects[k] = cv::boundingRect(approx_contour);
                contour_vector_t cov, approx;
                cov.emplace_back(contours[k]);
                approx.emplace_back(approx_contour);
                cv::drawContours(camera_image, cov, -1, cv::Scalar(255, 0, 0), 4, 1);
                cv::Scalar color(0, 0, 255);
                if(approx_contour.size() == 4){
                    cv::drawContours(camera_image, approx, -1, cv::Scalar(0, 255, 0), 4, 1);
                    cv::line(camera_image, approx_contour[0], approx_contour[1], color, 4, cv::LINE_8);
                    cv::line(camera_image, approx_contour[1], approx_contour[2], color, 4, cv::LINE_8);
                    cv::line(camera_image, approx_contour[2], approx_contour[3], color, 4, cv::LINE_8);
                    /*
                    qDebug() << "Test";
                    cv::RotatedRect rRect = cv::RotatedRect(approx_contour[0], approx_contour[1], approx_contour[2]);
                    qDebug() << "Test2";
                    if(rRect.center.x > camera_image.cols/3 && rRect.center.x < 2 * camera_image.cols/3
                            && rRect.center.y > camera_image.rows/3 && rRect.center.y < 2 * camera_image.rows/3){
                        cv::Point2f vertices[4];
                        qDebug() << "Test3";
                        rRect.points(vertices);
                        for (int i = 0; i < 4; i++){
                            qDebug() << "Test3";
                            cv::line(camera_image, vertices[i], vertices[(i+1)%4], color, 2);
                        }
                    }
                    */

                }

                unsigned int counter = 0;
                for(int i = 0; i < boundingRects[k].width; i++){
                    for(int j = 0; j < boundingRects[k].height; j++){
                        counter += threshold_image.at<uchar>(boundingRects[k].y + j, boundingRects[k].x + i);
                        //Divide Counter every row
                    }
                }
                counter /= boundingRects[k].area();
                if(counter <= (unsigned int) threshold_value){
                    cv::rectangle(camera_image, boundingRects[k], cv::Scalar(0, 0, 0), 4);//Black
                }else{
                    cv::rectangle(camera_image, boundingRects[k], cv::Scalar(255, 255, 255), 4);//White
                }
                //cv::polylines(camera_image, approx_contour, true, color, 4);
            }
            //Change the Pointer to result Image if you want to see another output mat
            switch (result_image_index) {
                case 0: result_image = &camera_image;
                    break;
                case 1: result_image = &gray_image;
                    break;
                case 2: result_image = &threshold_image;
                    break;
                default:
                    result_image = &camera_image;
            }
            if(result_image->channels() == 1){
                cv::cvtColor(*result_image, *result_image, CV_GRAY2RGB);
            }else if(result_image->channels() == 3){
                cv::cvtColor(*result_image, *result_image, CV_BGR2RGB);
            }
            QImage img((uchar*)result_image->data, result_image->cols, result_image->rows, result_image->step, QImage::Format_RGB888);
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


