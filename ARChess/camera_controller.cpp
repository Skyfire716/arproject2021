#include "camera_controller.h"
#include <QDebug>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QTemporaryDir>
#include <math.h>
#include <QtMath>

double camera_worker::angle(cv::Point a, cv::Point b){
    return (acos((a.x*b.x+a.y*b.y * 1.0)/(sqrt(pow(a.x, 2)+pow(a.y, 2))*sqrt(pow(b.x, 2)+pow(b.y, 2) * 1.0))) * 180.0 / M_PI);
}

double camera_worker::angle(cv::Point2f a, cv::Point2f b)
{
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

void camera_worker::diagonal_probeing(cv::Point2f start_corner, float diagonalLength, cv::Point2f diagonalNormalized, cv::Point2f diagonalNormalVec, cv::Point2f *result_array, bool is_black)
{
    int sign = 1;
    if(diagonalLength < 0){
        sign = -1;
        diagonalLength = abs(diagonalLength);
    }
    //qDebug() << "SIGN";
    cv::Point2f corner;
    cv::Point2f last_corner;
    last_corner.x = start_corner.x;
    last_corner.y = start_corner.y;
    result_array[0].x = start_corner.x;
    result_array[0].y = start_corner.y;
    corner.x = start_corner.x;
    corner.y = start_corner.y;
    //qDebug() << "Array";
    int point_to_point = 0;
    int black_counter = 0;
    int white_counter = 0;
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
        white_counter += (value >= 500);
        if(i == (int) diagonalLength && black_counter < 40 && white_counter <= 40){
            qDebug() << "Mixed Filed detected. Aborting";
            white_counter = 0;
            black_counter = 0;
            i = 8 * diagonalLength;
        }
        /*
        if(i == (int) diagonalLength && white_counter > 40){
            qDebug() << "Alot WHITE DETECTED";
            white_counter = 0;
        }
        if(i == (int)diagonalLength && black_counter < 40){
            qDebug() << "TO MUCH WHITE DETECTED";
            black_counter = 0;
            i = 8 * diagonalLength;
        }
        */
        //qDebug() << "Value Done " << QString::number(value);
        point_to_point += value;
        if(is_black){
            //qDebug() << "WHITE CROSSING " << (value > WHITE_FIELD_CROSSING);
        }else{
            //qDebug() << "BLACK CROSSING " << (value < BLACK_FIELD_CROSSING);
        }
        //qDebug() << "DISTANCE " << (50 < point_distance(last_corner, cv::Point2f(corner.x + sign * i * diagonalNormalized.x, corner.y + sign * i * diagonalNormalized.y)));
        if(((!is_black && value < BLACK_FIELD_CROSSING) || (is_black && value > WHITE_FIELD_CROSSING)) && 50 < point_distance(last_corner, cv::Point2f(x, y))) {
            bool added_point = false;
            float meandistance = 0;
            //qDebug() << "PTP " << QString::number(point_to_point);
            cv::Point2f last_to_now = line_P2P(last_corner, cv::Point2f(x, y));
            int color_between_result = check_color(threshold_image, last_corner.x + last_to_now.x / 2, last_corner.y + last_to_now.y / 2);
            if(color_between_result == 2){
                qDebug() << "Undefined Color Abort";
                break;
            }
            //qDebug() << "Color " << color_between_result << " isBlack " << is_black;
            for(int o = 0; o < 8; o++){
                if(is_zero(result_array[o]) && ((is_black && point_to_point < BLACK_FIELD_SUM) || (!is_black && point_to_point > WHITE_FIELD_SUM)) &&
                        (o == 0 || is_zero(last_corner) || ((is_black && color_between_result == 0) || (!is_black && color_between_result == 1)))){
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

cv::Point2f camera_worker::intersection_P2PLine_P2PLine(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f p4)
{
    //x = ((x2 - x1)(x3y4 - y3x4)-(x4-x3)(x1y2 - y1x2)) / ((x2-x1)(y4-y3)-(y2-y1)(x4-x3))
    //y = ((y2-y1)(x3y4-y3x4)-(y4-y3)(x1y2-y1x2)) / ((x2-x1)(y4-y3)-(y2-y1)(x4-x3))
    float denominator = (p2.x - p1.x) * (p4.y - p3.y) - (p2.y - p1.y) * (p4.x - p3.x);
    cv::Point2f intersection_point(((p2.x - p1.x) * (p3.x * p4.y - p3.y * p4.x) - (p4.x - p3.x) * (p1.x * p2.y - p1.y * p2.x)) / denominator,
                             ((p2.y - p1.y) * (p3.x * p4.y - p3.y * p4.x) - (p4.y - p3.y) * (p1.x * p2.y - p1.y * p2.x)) / denominator);
    return intersection_point;
}

cv::Point2f camera_worker::line_P2P(cv::Point2f p1, cv::Point2f p2)
{
    return cv::Point2f(p2.x - p1.x, p2.y - p1.y);
}

cv::Point2f camera_worker::angled_vector_from_normal(cv::Point2f normal, float angle_deg)
{
    float rad = qDegreesToRadians(angle_deg);
    return cv::Point2f(qCos(rad * normal.x) - qSin(rad * normal.y), qSin(rad * normal.x) + qCos(rad * normal.y));
}

float camera_worker::intersection_NormalLine_NormalLine(cv::Point2f line_p1, cv::Point2f line_n1, cv::Point2f line_p2, cv::Point2f line_n2)
{
    //qDebug() << "P1 " << QString::number(line_p1.x) << " " << QString::number(line_p1.y);
    //qDebug() << "N1 " << QString::number(line_n1.x) << " " << QString::number(line_n1.y);
    //qDebug() << "P2 " << QString::number(line_p2.x) << " " << QString::number(line_p2.y);
    //qDebug() << "N2 " << QString::number(line_n2.x) << " " << QString::number(line_n2.y);
    float r = ((line_p1.y - line_p2.y) - ((line_p1.x - line_p2.x) * line_n2.y) / line_n2.x) / (-line_n1.y + (line_n1.x * line_n2.y / line_n2.x));
    float s = (line_p1.x + r * line_n1.x - line_p2.x) / line_n2.x;
    //qDebug() << "r " << r << " s " << s << " s2 " << s2;
    if(line_p1.x + r * line_n1.x != line_p2.x + s * line_n2.x || line_p1.y + r * line_n1.y != line_p2.y + s * line_n2.y){
        //qDebug() << "x " << QString::number(line_p1.x + r * line_n1.x) << " =? " << QString::number(line_p2.x + s * line_n2.x);
        //qDebug() << "y " << QString::number(line_p1.y + r * line_n1.y) << " =? " << QString::number(line_p2.y + s * line_n2.y);
        qDebug() << "Error";
    }
    return r;
}

int camera_worker::check_color(cv::Mat image, int x, int y)
{
    int sum = 0;
    for(int i = -COLOR_CHECK_AREA; i <= COLOR_CHECK_AREA; i++){
        for(int j = -COLOR_CHECK_AREA; j <= COLOR_CHECK_AREA; j++){
            if(i >= 0 && i < image.cols && j >= 0 && j < image.rows){
                sum += image.at<uchar>(y, x);
            }
        }
    }
    if(sum < 0.05 * pow(COLOR_CHECK_AREA, 2) * 1){
        return 0;
    }else if(sum > 0.95 * pow(COLOR_CHECK_AREA, 2) * 255){
        return 1;
    }else{
        return 2;
    }
}

bool camera_worker::is_zero(cv::Point2f p)
{
    return (p.x == 0 && p.y == 0);
}

void camera_worker::check_texture(cv::Point2f start, cv::Point2f *resultA, cv::Point2f *resultB, cv::Point2f guideA, cv::Point2f guideB, cv::Point2f normalDiagonalA, cv::Point2f normalDiagonalB, cv::Point2f guideAB)
{
    cv::Point2f probe_point = start;
    int last_color = 2;
    for(int i = 0; i < 8; i++){
        if(!is_zero(resultA[i]) && !is_zero(resultB[i])){
            cv::Point2f p2p_line = line_P2P(resultA[i], resultB[i]);
            normalizeVec(&p2p_line);
            cv::Point2f normal_p2p_line(-p2p_line.y, p2p_line.x);
            if(angle(guideA, normal_p2p_line) > 7 || angle(guideB, normal_p2p_line) > 7){
                qDebug() << "Leaving";
                goto SINGLE_SIDE_CHECK;
            }
            cv::circle(camera_image, resultA[i], 5, cv::Scalar(0, 255, 0), 4);
            cv::circle(camera_image, resultB[i], 5, cv::Scalar(0, 255, 0), 4);
            cv::line(camera_image, resultA[i], resultB[i], cv::Scalar(0, 0, 255), 3);
            cv::Point2f betterNormal(guideA.x + guideB.x + normal_p2p_line.x, guideA.y + guideB.y + normal_p2p_line.y);
            normalizeVec(&betterNormal);
            qDebug() << "LOL";
            float r = intersection_NormalLine_NormalLine(probe_point, betterNormal, resultA[i], p2p_line);
            float r2 = intersection_NormalLine_NormalLine(probe_point, betterNormal, resultB[i], p2p_line);
            qDebug() << "LOl Ende";
            cv::circle(camera_image, r * betterNormal + probe_point, 5, cv::Scalar(0, 0, 255), 4, cv::LINE_8);
            cv::circle(camera_image, 2 * r * betterNormal + probe_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
            cv::circle(camera_image, 2 * r2 * betterNormal + probe_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
            probe_point = 2 * r * betterNormal + probe_point;
            continue;
        }
        SINGLE_SIDE_CHECK:
        if(!is_zero(resultA[i])){

        }
        if(!is_zero(resultB[i])){
            cv::circle(camera_image, resultB[i], 5, cv::Scalar(0, 255, 0), 4);
            cv::Point2f normal_guideB(-guideB.y, guideB.x);
            //cv::Point2f p2p_line = angled_vector_from_normal(normalDiagonalB, 170);
            cv::Point2f p2p_line = guideAB;
            cv::line(camera_image, resultB[i], resultB[i] + 70 * p2p_line, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
            normalizeVec(&p2p_line);
            cv::Point2f normal_p2p_line(-p2p_line.y, p2p_line.x);
            cv::line(camera_image, resultB[i], resultB[i] + 70 * normal_p2p_line, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
            cv::Point2f betterNormal(guideA.x + guideB.x + normal_p2p_line.x, guideA.y + guideB.y + normal_p2p_line.y);
            normalizeVec(&betterNormal);
            qDebug() << "LOLLL";
            print_vec(probe_point);
            print_vec(betterNormal);
            print_vec(resultB[i]);
            print_vec(p2p_line);
            float r = intersection_NormalLine_NormalLine(probe_point, betterNormal, resultB[i], p2p_line);
            qDebug() << "LOLLLLL";
            cv::circle(camera_image, 2 * r * betterNormal + probe_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
            probe_point = 2 * r * betterNormal + probe_point;
            if(i > 0){
                qDebug() << "Shift Probe";
                float r2 = intersection_NormalLine_NormalLine(resultB[i - 1], normal_p2p_line, resultB[i], p2p_line);
                float r3 = intersection_NormalLine_NormalLine(probe_point, normal_p2p_line, resultB[i], p2p_line);
                cv::Point2f distance_probe(r3 * normal_p2p_line.x, r3 * normal_p2p_line.y);
                cv::Point2f distance_vec(r2 * normal_p2p_line.x, r2 * normal_p2p_line.y);
                float p2p_distance = normalizeVec(&distance_vec);
                float probe_distance = normalizeVec(&distance_probe);
                qDebug() << "P2P D " << QString::number(p2p_distance);
                qDebug() << "Probe D " << QString::number(probe_distance);
                cv::circle(camera_image, probe_point + betterNormal * (probe_distance - p2p_distance / 2), 5, cv::Scalar(0, 0, 255), 4, cv::LINE_8);
                probe_point = probe_point + betterNormal * (probe_distance - p2p_distance / 2);
            }
            int color_result = check_color(threshold_image, probe_point.x, probe_point.y);
            if(color_result == last_color && last_color != 2){
                qDebug() << "Two times Same Color -> Error Maybe Edge detected";
                i = 8;
            }
            if(color_result == 1){
                qDebug() << "White";
            }else if(color_result == 0){
                qDebug() << "Black";
            }else{
                qDebug() << "Undefined";
            }
            last_color = color_result;

        }else {
            i = 8;
        }
    }
}

void camera_worker::print_vec(cv::Point2f p)
{
    qDebug() << QString::number(p.x) << " " << QString::number(p.y);
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
                        cv::Point2f center_point = intersection_P2PLine_P2PLine(*a, *d, *b, *c);
                        //Diagonale durch laufen
                        cv::Point2f lineAC = line_P2P(*a, *c);
                        cv::Point2f lineAB = line_P2P(*a, *b);
                        cv::Point2f lineCD = line_P2P(*c, *d);
                        cv::Point2f lineBD = line_P2P(*b, *d);
                        float lengthAC = normalizeVec(&lineAC);
                        float lengthAB = normalizeVec(&lineAB);
                        float lengthCD = normalizeVec(&lineCD);
                        float lengthBD = normalizeVec(&lineBD);
                        cv::Point2f normalLineAC(-lineAC.y, lineAC.x);
                        cv::Point2f normalLineAB(-lineAB.y, lineAB.x);
                        cv::Point2f normalLineCD(-lineCD.y, lineCD.x);
                        cv::Point2f normalLineBD(-lineBD.y, lineBD.x);
                        float topScalar;
                        float rightScalar;
                        float bottomScalar;
                        float leftScalar;
                        cv::Point2f diagonalA = line_P2P(*a, *d);
                        cv::Point2f diagonalB = line_P2P(*b, *c);
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
                            cv::line(camera_image, *a, *c, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                            cv::line(camera_image, *a, *b, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                            cv::line(camera_image, *c, *d, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                            cv::line(camera_image, *b, *d, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                            cv::circle(camera_image, center_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
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
                            bool is_black = threshold_image.at<uchar>(center_point.y, center_point.x) < threshold_value;
                            //qDebug() << "Is Black? " << is_black << " area " << check_color(threshold_image, center_point.x, center_point.y);
                            diagonal_probeing(*a, -lengthA, diagonalA, normalDiagonalA, tldiagonal, is_black);
                            diagonal_probeing(*b, -lengthB, diagonalB, normalDiagonalB, bldiagonal, is_black);
                            diagonal_probeing(*c, lengthB, diagonalB, normalDiagonalB, trdiagonal, is_black);
                            diagonal_probeing(*d, lengthA, diagonalA, normalDiagonalA, brdiagonal, is_black);
                            for(int i = 0; i < 8; i++){
                                for(int j = 0; j < 4; j++){
                                    if(diagonalArray[j][i].x != 0 && diagonalArray[j][i].y != 0){
                                        counterArray[j]++;
                                    }
                                }
                            }

                            check_texture(center_point, tldiagonal, trdiagonal, lineAB, lineCD, normalDiagonalA, normalDiagonalB, lineAC);
                            check_texture(center_point - lineAC, tldiagonal, trdiagonal, lineAB, lineCD, normalDiagonalA, normalDiagonalB, lineAC);
                            //check_texture(center_point, trdiagonal, brdiagonal, lineAC, lineBD, normalDiagonalA, normalDiagonalB, lineCD);


/*
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

                                    }
                                }else if(matches == 3){
                                    qDebug() << "Tribble Match Index " << i;
                                }else if(matches == 2){
                                    qDebug() << "Dubble Match Index " << i;
                                }
                            }
*/
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


