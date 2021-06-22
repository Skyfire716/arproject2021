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

float camera_worker::normalizeVec(cv::Point2f *p)
{
    float length = sqrt(pow(p->x, 2) + pow(p->y, 2));
    p->x = p->x / length;
    p->y = p->y / length;
    return length;
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

float camera_worker::point_distance(cv::Point a, cv::Point b){
    return sqrt((pow(b.x - a.x, 2) + pow(b.y - a.y, 2)) * 1.0);
}

float camera_worker::point_distance(cv::Point2f a, cv::Point2f b){
    return sqrt((pow(b.x - a.x, 2) + pow(b.y - a.y, 2)) * 1.0);
}

int camera_worker::get_ordered_points(cv::Rect rect, std::vector<cv::Point> points)
{
    cv::Point rectTL = cv::Point2f(rect.x, rect.y);
    cv::Point rectTR = cv::Point2f(rect.x + rect.width, rect.y);
    cv::Point rectBL = cv::Point2f(rect.x, rect.y + rect.height);
    cv::Point rectBR = cv::Point2f(rect.x + rect.width, rect.y + rect.height);
    cv::Point *a = NULL, *b = NULL, *c = NULL, *d = NULL;
    int index_a = 0, index_b = 0, index_c = 0, index_d = 0;
    for(int i = 0; i < 4; i++){
        if(a){
            if(point_distance(rectTL, points[i]) <= point_distance(rectTL, *a)){
               a = &points[i];
               index_a = i;
            }
        } else{
            a = &points[i];
            index_a = i;
        }
        if(b){
            if(point_distance(rectBL, points[i]) < point_distance(rectBL, *b)){
                b = &points[i];
                index_b = i;
            }
        }else{
            b = &points[i];
            index_b = i;
        }
        if(c){
            if(point_distance(rectTR, points[i]) < point_distance(rectTR, *c)){
                c = &points[i];
                index_c = i;
            }
        }else{
            c = &points[i];
            index_c = i;
        }
        if(d){
            if(point_distance(rectBR, points[i]) <= point_distance(rectBR, *d)){
                d = &points[i];
                index_d = i;
            }
        }else{
            d = &points[i];
            index_d = i;
        }
    }
    return (index_a * 1000 + index_b * 100 + index_c * 10 + index_d);
}

void camera_worker::diagonal_probeing(cv::Point2f start_corner, float diagonalLength, cv::Point2f diagonalNormalized, cv::Point2f diagonalNormalVec, cv::Point2f *result_array)
{
    int sign = 1;
    if(diagonalLength < 0){
        sign = -1;
        diagonalLength = abs(diagonalLength);
    }
    //qDebug() << "SIGN";
    cv::Point2f corner;
    cv::Point2f last_corner;
    result_array[0].x = start_corner.x;
    result_array[0].y = start_corner.y;
    corner.x = start_corner.x;
    corner.y = start_corner.y;
    //qDebug() << "Array";
    int point_to_point = 0;
    int black_counter = 0;
    for(int i = 0; i < (int)(8 * diagonalLength); i++){
        int value = 0;
        //qDebug() << "Value";
        float x = corner.x + sign * i * diagonalNormalized.x;
        float y = corner.y + sign * i * diagonalNormalized.y;
        if(x < 0 || x > threshold_image.cols){
            qDebug() << "X Dimension Missmatch " << x << " " << threshold_image.cols;
            break;
        }
        if(y < 0 || y > threshold_image.rows){
            qDebug() << "Y Dimension Missmatch";
            break;
        }
        for(int j = -7; j <= 7; j++){
            //Add checking for Image Dimensions
            float xx = x + j * diagonalNormalVec.x;
            float yy = y + j * diagonalNormalVec.y;
            if(xx < 0 || xx > threshold_image.cols){
                qDebug() << "XX Dimension Missmatch";
                break;
            }
            if(yy < 0 || yy > threshold_image.rows){
                qDebug() << "YY Dimension Missmatch";
                break;
            }
            value += threshold_image.at<uchar>(yy, xx);
        }
        black_counter += (value < 500);
        if(i == (int)diagonalLength && black_counter < 40){
            qDebug() << "TO MUCH WHITE DETECTED";
            black_counter = 0;
            i = 8 * diagonalLength;
        }
        //qDebug() << "Value Done " << QString::number(value);
        point_to_point += value;
        //qDebug() << "WHITE CROSSING " << (value > WHITE_FIELD_CROSSING);
        //qDebug() << "DISTANCE " << (50 < point_distance(last_corner, cv::Point2f(corner.x + sign * i * diagonalNormalized.x, corner.y + sign * i * diagonalNormalized.y)));
        if(value > WHITE_FIELD_CROSSING && 50 < point_distance(last_corner, cv::Point2f(x, y))) {
            bool added_point = false;
            float meandistance = 0;
            for(int o = 0; o < 8; o++){
                if(result_array[o].x == 0 && result_array[o].y == 0 && point_to_point < BLACK_FIELD_SUM){
                    if(o == 2){
                        //i = 8 * diagonalLength;
                    }
                    if(o != 0){
                        meandistance /= o;
                    }
                    //qDebug() << "Mean " << QString::number(meandistance);
                    //qDebug() << "PTP " << QString::number(point_to_point);
                    point_to_point = 0;
                    cv::circle(camera_image, cv::Point(x, y), 10, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                    //qDebug() << "To Big TR " << QString::number(value) << " i " << QString::number(i%((int)(8*diagonalLength))) << " d to last " << QString::number(point_distance(last_corner, cv::Point2f(x, y)));
                    bool contains = false;
                    for(int l = 0; l < o; l++){
                        contains |= (result_array[l].x == x && result_array[l].y == y);
                    }
                    if(!contains){
                        result_array[o].x = x;
                        result_array[o].y = y;
                    }
                    /*
                    if(o != 0 && meandistance != 0 && abs(point_distance(result_array[o], result_array[o-1])) > meandistance * 1.3){
                        qDebug() << "Distance is longer than mean " << QString::number(abs(point_distance(result_array[o], result_array[o-1]))) << " mean " << QString::number(meandistance);
                    }
                    */
                    last_corner.x = x;
                    last_corner.y = y;
                    added_point = true;
                    o = 8;
                }else{
                    if(o > 0){
                        meandistance += abs(point_distance(result_array[o], result_array[o-1]));
                    }
                }
            }
            if(!added_point){
                i = (int)(8 * diagonalLength);
            }
        }
    }
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
            bool is_first = true;
            cv::Point *masterA;
            cv::Point *masterB;
            cv::Point *masterC;
            cv::Point *masterD;
            for(size_t k = 0; k < contours.size(); k++){
                contour_t approx_contour;
                cv::approxPolyDP(contours[k], approx_contour, cv::arcLength(contours[k], true) * 0.02, true);
                boundingRects[k] = cv::boundingRect(approx_contour);
                contour_vector_t cov, approx;
                cov.emplace_back(contours[k]);
                approx.emplace_back(approx_contour);
                cv::drawContours(camera_image, cov, -1, cv::Scalar(255, 0, 0), 4, 1);
                cv::Scalar color(0, 0, 255);
                if(approx_contour.size() == 4 && boundingRects[k].area() > 200){
                    //cv::drawContours(camera_image, approx, -1, cv::Scalar(0, 255, 0), 4, 1);
                    //cv::line(camera_image, approx_contour[0], approx_contour[1], color, 4, cv::LINE_8);
                    //cv::line(camera_image, approx_contour[1], approx_contour[2], color, 4, cv::LINE_8);
                    //cv::line(camera_image, approx_contour[2], approx_contour[3], color, 4, cv::LINE_8);
                    cv::Point *a = NULL;
                    cv::Point *b = NULL;
                    cv::Point *c = NULL;
                    cv::Point *d = NULL;
                    int ordering = get_ordered_points(boundingRects[k], approx_contour);
                    a = &approx_contour[(ordering / 1000) % 10];
                    b = &approx_contour[(ordering/100) % 10];
                    c = &approx_contour[(ordering / 10) % 10];
                    d = &approx_contour[ordering % 10];
                    unsigned int counter = 0;
                    if(a && b && c && d && !(a == b || b == c || c == d || d == a || b == d || a == c)){
                        //Diagonale durch laufen
                        cv::Point2f lineAC(c->x - a->x, c->y - a->y);
                        cv::Point2f lineAB(b->x - a->x, b->y - a->y);
                        cv::Point2f lineCD(d->x - c->x, d->y - c->y);
                        cv::Point2f lineBD(d->x - b->x, d->y - b->y);
                        float lengthAC = normalizeVec(&lineAC);
                        float lengthAB = normalizeVec(&lineAB);
                        float lengthCD = normalizeVec(&lineCD);
                        float lengthBD = normalizeVec(&lineBD);
                        cv::Point2f diagonalA(d->x - a->x, d->y - a->y);
                        cv::Point2f diagonalB(b->x - c->x, b->y - c->y);
                        float lengthA = normalizeVec(&diagonalA);
                        float lengthB = normalizeVec(&diagonalB);
                        if(abs(point_distance(*a, *d)) < abs(point_distance(*b, *c))){
                            for(int i = 0; i < (int) abs(point_distance(*a, *d)); i++){
                                counter += (threshold_image.at<uchar>(a->y + i * diagonalA.y, a->x + i * diagonalA.x) > threshold_value);
                            }
                            counter /= (unsigned int) abs(point_distance(*a, *d));
                        }else{
                            for(int i = 0; i < (int) abs(point_distance(*c, *b)); i++){
                                counter += (threshold_image.at<uchar>(c->y + i * diagonalB.y, c->x - i * diagonalB.x) > threshold_value);
                            }
                            counter /= (unsigned int) abs(point_distance(*c, *b));
                        }
                        if(is_first){
                            char fields_found = 0;
                            unsigned char counterArray[4];
                            counterArray[COUNTER_BL] = counterArray[COUNTER_BR] = counterArray[COUNTER_TR] = counterArray[COUNTER_TL] = 0;
                            cv::Point2f normalDiagonalA(-diagonalA.y, diagonalA.x);
                            cv::Point2f normalDiagonalB(-diagonalB.y, diagonalB.x);
                            cv::Point2f corner;
                            cv::Point2f last_corner;
                            cv::Point2f tldiagonal[8];
                            cv::Point2f trdiagonal[8];
                            cv::Point2f bldiagonal[8];
                            cv::Point2f brdiagonal[8];
                            cv::Point2f *diagonalArray[4] = {tldiagonal, bldiagonal, trdiagonal, brdiagonal};
                            for(int i = 0; i < 8; i++){
                                for(int j = 0; j < 4; j++){
                                    diagonalArray[j][i].x = 0;
                                    diagonalArray[j][i].y = 0;
                                }
                            }
                            diagonal_probeing(*a, -lengthA, diagonalA, normalDiagonalA, tldiagonal);
                            diagonal_probeing(*b, lengthB, diagonalB, normalDiagonalB, bldiagonal);
                            diagonal_probeing(*c, -lengthB, diagonalB, normalDiagonalB, trdiagonal);
                            diagonal_probeing(*d, lengthA, diagonalA, normalDiagonalA, brdiagonal);
                            for(int i = 0; i < 8; i++){
                                for(int j = 0; j < 4; j++){
                                    if(diagonalArray[j][i].x != 0 && diagonalArray[j][i].y != 0){
                                        counterArray[j]++;
                                    }
                                }
                            }
                            qDebug() << counterArray[COUNTER_TL] << " corners TL";
                            qDebug() << counterArray[COUNTER_TR] << " corners TR";
                            qDebug() << counterArray[COUNTER_BL] << " corners BL";
                            qDebug() << counterArray[COUNTER_BR] << " corners BR";
                            qDebug() << "";
                            int smallest = 0;
                            int biggest = 0;
                            int smaller = -1;
                            int bigger = -1;
                            int number = 0;
                            for(int i = 0; i < 4; i++){
                                if(counterArray[i] > counterArray[biggest]){
                                    biggest = i;
                                }
                                if(counterArray[i] < counterArray[smallest]){
                                    smallest = i;
                                }
                            }
                            for(int i = 0; i < 4; i++){
                                if(i != smallest && i != biggest){
                                    if(smaller == -1){
                                        smaller = i;
                                    }else {
                                        if(counterArray[smaller] > counterArray[i]){
                                            bigger = smaller;
                                            smaller = i;
                                        }else{
                                            bigger = i;
                                        }
                                    }
                                }
                            }
                            number += counterArray[biggest] * 1000 + counterArray[bigger] * 100 + counterArray[smaller] * 10 + counterArray[smallest];
                            qDebug() << "Checking Number " << QString::number(number);
                            for(int i = 0; i < 32; i++){
                                int marker = CHESS_BOARD_MAP[i];
                                int matches = 0;
                                matches += (marker % 10 == counterArray[smallest]);
                                matches += ((marker / 10) % 10 == counterArray[smaller]) * (matches == 1);
                                matches += ((marker / 100) % 10 == counterArray[bigger]) * (matches == 2);
                                matches += ((marker / 1000) % 100 == counterArray[biggest]) * (matches == 3);
                                if(matches == 4){
                                    qDebug() << "Index " << i << " possible marker";
                                    int *ranges = CHESS_BOARD_RANGER[i];
                                    for(int j = 0; j < 4; j++){
                                        int walker = 0;
                                        while(walker < ranges[(0 + j) % 4] || walker < ranges[(1 + j) % 4] || walker < ranges[(2 + j) % 4] || walker < ranges[(3 + j) % 4]){


                                        }
                                    }
                                }else if(matches == 3){
                                    qDebug() << "Tribble Match Index " << i;
                                }else if(matches == 2){
                                    qDebug() << "Dubble Match Index " << i;
                                }
                            }

                            /*
                            for(int i = 0; i < qMin(counterArray[COUNTER_TL], counterArray[COUNTER_TR]); i++){
                                cv::Point2f tl = tldiagonal[i];
                                cv::Point2f tr = trdiagonal[i];
                                cv::Point2f lineTLTR(tr.x - tl.x, tr.y - tl.y);
                                float length = normalizeVec(&lineTLTR);
                                if(length/lengthAC == 1 || angle(lineAC, lineTLTR) > 3){
                                    continue;
                                }
                                qDebug() << "Angle " << angle(lineAC, lineTLTR);
                                qDebug() << i << " Lengts " << length << " " << lengthAC << " " << (length / lengthAC);
                            }
                            */
                            fields_found += counterArray[COUNTER_TR] + counterArray[COUNTER_TL] + counterArray[COUNTER_BL] + counterArray[COUNTER_BR];
                            if(fields_found != 0){
                                is_first = false;
                            }
                        }
                    }
                    if(counter <= (unsigned int) threshold_value){
                        //cv::rectangle(camera_image, boundingRects[k], cv::Scalar(0, 0, 0), 4);//Black
                    }else{
                        //cv::rectangle(camera_image, boundingRects[k], cv::Scalar(255, 255, 255), 4);//White
                    }
                    //cv::polylines(camera_image, approx_contour, true, color, 4);
                }
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


