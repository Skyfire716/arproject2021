#include "camera_controller.h"
#include <QDebug>
#include <QPixmap>
#include <QList>
#include <QPair>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QTemporaryDir>
#include <math.h>
#include <QtMath>
#include <QtAlgorithms>

double camera_worker::angle(cv::Point a, cv::Point b){
    return angle(cv::Point2f(a.x, a.y), cv::Point2f(b.x, b.y));
}

double camera_worker::angle(cv::Point2f a, cv::Point b)
{
    return angle(a, cv::Point2f(b.x, b.y));
}

double camera_worker::angle(cv::Point a, cv::Point2f b)
{
    return  angle(cv::Point2f(a.x, a.y), b);
}

double camera_worker::angle(cv::Point2f a, cv::Point2f b)
{
    QVector2D av(a.x, a.y);
    QVector2D bv(b.x, b.y);
    return qRadiansToDegrees(qAcos(QVector2D::dotProduct(av, bv) / (av.length() * bv.length())));
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
    return point_distance(cv::Point2f(a.x, a.y), cv::Point2f(b.x, b.y));
}

float camera_worker::point_distance(cv::Point2f a, cv::Point b)
{
    return point_distance(a, cv::Point2f(b.x, b.y));
}

float camera_worker::point_distance(cv::Point a, cv::Point2f b)
{
    return point_distance(cv::Point2f(a.x, a.y), b);
}

float camera_worker::point_distance(cv::Point2f a, cv::Point2f b){
    QVector2D av(a.x, a.y);
    QVector2D bv(b.x, b.y);
    return av.distanceToPoint(bv);
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
    cv::circle(camera_image, start_corner, 10, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
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
            //qDebug() << "X Dimension Missmatch " << x << " " << threshold_image.cols;
            break;
        }
        if(y < 0 || y > threshold_image.rows){
            //qDebug() << "Y Dimension Missmatch";
            break;
        }
        for(int j = -7; j <= 7; j++){
            //Add checking for Image Dimensions
            float xx = x + j * diagonalNormalVec.x;
            float yy = y + j * diagonalNormalVec.y;
            if(xx < 0 || xx > threshold_image.cols){
                //qDebug() << "XX Dimension Missmatch";
                break;
            }
            if(yy < 0 || yy > threshold_image.rows){
                //qDebug() << "YY Dimension Missmatch";
                break;
            }
            value += threshold_image.at<uchar>(yy, xx);
        }
        black_counter += (value < 500);
        white_counter += (value >= 500);
        if(i == (int) diagonalLength && black_counter < 40 && white_counter <= 40){
            //qDebug() << "Mixed Filed detected. Aborting";
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
        if(((!is_black && value < BLACK_FIELD_CROSSING) || (is_black && value > WHITE_FIELD_CROSSING)) && (diagonalLength * 0.85) < point_distance(last_corner, cv::Point2f(x, y))) {
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
                    cv::Point2f more_accurate_pix = getsubPixel(cv::Point2f(x, y));
                    x = more_accurate_pix.x;
                    y = more_accurate_pix.y;
                    int harris_check_dimensions = 25;
                    bool field_checkb = true;
                    cv::Rect rect(x - harris_check_dimensions, y - harris_check_dimensions, 2 * harris_check_dimensions, 2 * harris_check_dimensions);
                    field_checkb = field_check(rect, cv::Point2f(x, y));
                    if(!field_checkb){
                        o = 8;
                        continue;
                    }
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

cv::Point2f camera_worker::getsubPixel(cv::Point2f point)
{
    float x = point.x;
    float y = point.y;
    int subpixelSearchLength = 150;
    if(x - subpixelSearchLength >= 0 && x+subpixelSearchLength < threshold_image.cols && y-subpixelSearchLength >= 0 && y +subpixelSearchLength < threshold_image.rows){
        cv::Rect crop(x-subpixelSearchLength, y-subpixelSearchLength, 2 * subpixelSearchLength, 2 * subpixelSearchLength);
        cv::Mat harris = threshold_image(crop);
        cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 100, 0.001);
        std::vector<cv::Point2f> corners;
        corners.push_back(cv::Point2f(subpixelSearchLength, subpixelSearchLength));
        cv::cornerSubPix(harris, corners, cv::Size(5, 5), cv::Size(-1, -1), criteria);
        if(corners.size() > 0){
            x += corners[0].x - subpixelSearchLength;
            y += corners[0].y - subpixelSearchLength;
        }
    }
    return cv::Point2f(x, y);
}

cv::Point2f camera_worker::mean_point(QList<cv::Point> points)
{
    cv::Point2f mean(0, 0);
    for (cv::Point p : points) {
        mean.x += p.x;
        mean.y += p.y;
    }
    return mean / (points.length() * 1.0);
}

void camera_worker::harris_values(cv::Rect rect, QList<cv::Point> *edges)
{
    if(!rect_in_mat(threshold_image, rect)){
        qDebug() << "Leaving Harris";
        return;
    }
    cv::Mat harris_mat = threshold_image(rect);
    //Val falsch
    cv::Mat harris(harris_mat.rows,harris_mat.cols, CV_MAKETYPE(CV_32FC1, 6));
    cv::cornerEigenValsAndVecs(harris_mat, harris, 3, 3);
    int dropout = 10000;
    for(int x = 0; x < harris.cols; x++){
        for(int y = 0; y < harris.rows; y++){
            cv::Vec6f vals = harris.at<cv::Vec6f>(y, x);
            float l1 = vals[0];
            float l2 = vals[1];
            double harris_val = l1 * l2 - 0.04 * pow(l1 + l2, 2);
            if(harris_val / dropout < 0){
                cv::Point2f center_point(rect.tl().x + rect.width / 2.0, rect.tl().y + rect.height / 2.0);
                //cv::circle(camera_image, cv::Point2f(rect.tl().x + x, rect.tl().y + y), 2, cv::Scalar(0, 0, 255), 1, cv::LINE_8);
                float distance = point_distance(cv::Point2f(rect.tl().x + x, rect.tl().y + y), center_point);
                if(distance <= 18 && distance >= 15){
                    if(edges){
                        edges->push_back(cv::Point(x, y));
                    }
                }
            }
        }
    }
}

bool camera_worker::field_check(cv::Rect rect, cv::Point2f p)
{
    cv::Rect rects[4];
    QList<cv::Point> clusters[4];
    QList<cv::Point> edges;
    harris_values(rect, &edges);
    if(edges.isEmpty()){
        return false;
    }
    for(cv::Point point : edges){
        int x = point.x;
        int y = point.y;
        for(int i = 0; i < 4; i++){
            if(clusters[i].isEmpty()){
                clusters[i].push_back(cv::Point(x, y));
                i = 4;
                break;
            }
            for(cv::Point p : clusters[i]){
                if(10 > point_distance(p, cv::Point(x, y))){
                    clusters[i].push_back(cv::Point(x, y));
                    i = 4;
                    break;
                }
            }
        }
    }
    for(int i = 0; i < 4; i ++){
        if(!clusters[i].isEmpty()){
            //qDebug() << "Rect " << i;
            rects[i] = cv::boundingRect(clusters[i].toVector().toStdVector()) + rect.tl();
            //cv::rectangle(camera_image, rects[i], cv::Scalar(0, 0, 255), 10, cv::LINE_8);
        }
    }
    int index_map[6][2] ={{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};
    QList<QPair<int, cv::Point2f>> probemap;
    //float scalar = 18;
    float scalar = point_distance(p, cv::Point2f(p.x + edges.first().x, p.y + edges.first().y));
    for(int i = 0; i < 6; i++){
        int index_a = index_map[i][0];
        int index_b = index_map[i][1];
        if(!((rects[index_a] & rects[index_b]).area() > 0) && !rects[index_a].empty() && !rects[index_b].empty()){
            cv::Point2f u = line_P2P(p, rects[index_a].tl());
            cv::Point2f v = line_P2P(p, rects[index_b].tl());
            float angleuv = angle(u, v);
            if(20 < angleuv && angleuv < 160){
                cv::Point2f probing = u + v;
                normalizeVec(&probing);
                //float scalar = 25;
                QPair<int, cv::Point2f> pair(check_color(threshold_image, p.x + scalar *probing.x, p.y + scalar * probing.y), scalar * probing);
                probemap.push_back(pair);
                //qDebug() << "Angle " << QString::number(angleuv);
                //qDebug() << "Probing Color " << check_color(threshold_image, p.x + scalar * probing.x, p.y + scalar * probing.y);
                //cv::circle(camera_image, p + scalar * probing, 5, cv::Scalar(255, 255, 255), 3, cv::LINE_8);
            }
        }
    }
    bool missmatch = false;
    int count_matches = 0;
    for(int i = 0; i < probemap.length(); i++){
        for(int j = i; j < probemap.length(); j++){
            if(probemap[i].first == probemap[j].first && i != j){
                cv::Point2f u = probemap[i].second;
                cv::Point2f v = probemap[j].second;
                float angleuv = angle(u, v);
                float angleuvup = angle(p + line_P2P(u, v), p + line_P2P(u, p));
                float angleuvvp = angle(p + line_P2P(u, v), p + line_P2P(v, p));
                if(angleuvup < 5 && angleuvvp < 5){
                //if(point_distance(p + u, p + v) > point_distance(p + u, p) + point_distance(p + v, p)){
                    //qDebug() << "Same Color on Diagonal";
                    count_matches++;
                }else{
                    //qDebug() << "Same Color not on Diagonal!!!";
                    //qDebug() << "U " << probemap[i].first;
                    //qDebug() << "V " << probemap[j].first;
                    print_vec(u);
                    print_vec(v);
                    print_vec(p);
                    //qDebug() << "Angle " << QString::number(angleuv);
                    //qDebug() << "Angle UVUP " << QString::number(angleuvup);
                    //qDebug() << "Angle UVVP " << QString::number(angleuvvp);
                    //qDebug() << "U to V " << QString::number(point_distance(p + u, p + v));
                    //qDebug() << "U to P " << QString::number(point_distance(p + u, p));
                    //qDebug() << "V to P " << QString::number(point_distance(p + v, p));
                    missmatch = true;
                    return false;
                }
            }
        }
    }
    if(missmatch){
        //qDebug() << "Wong field setup!!!";
    }
    //qDebug() << "Counter " << count_matches;
    if(count_matches <= 1){
        return false;
    }
    return true;
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

float camera_worker::distance_point_to_line(cv::Point2f lineA, cv::Point2f lineB, cv::Point2f p)
{
    float nomitor = abs((lineB.x - lineA.x) * (lineA.y - p.y) - (lineA.x - p.x) * (lineB.y - lineA.y));
    float denomitor = qSqrt(pow(lineB.x - lineA.x, 2) + pow(lineB.y - lineA.y, 2));
    return nomitor / denomitor;
}

int camera_worker::check_color(cv::Mat image, int x, int y)
{
    return check_color(image, cv::Point(x, y), COLOR_CHECK_AREA);
}

int camera_worker::check_color(cv::Mat image, cv::Point p, float area)
{
    int sum = 0;
    for(int i = -area; i <= area; i++){
        for(int j = -area; j <= area; j++){
            if(i >= 0 && i < image.cols && j >= 0 && j < image.rows){
                sum += image.at<uchar>(p.y, p.x);
            }
        }
    }
    if(sum < 0.05 * pow(area, 2) * 1){
        return 0;
    }else if(sum > 0.95 * pow(area, 2) * 255){
        return 1;
    }else{
        return 2;
    }
}

int camera_worker::check_color(cv::Mat image, cv::Point p)
{
    return check_color(image, p.x, p.y);
}

bool camera_worker::is_zero(cv::Point2f p)
{
    return (p.x == 0 && p.y == 0);
}

bool camera_worker::is_nan(cv::Point2f p)
{
    return isnan(p.x) || isnan(p.y);
}

bool camera_worker::point_in_mat(cv::Mat image, cv::Point2f p)
{
    return (p.x >= 0 && p.y >= 0 && p.x < image.cols && p.y < image.rows);
}

bool camera_worker::rect_in_mat(cv::Mat image, cv::Rect rect)
{
    return (point_in_mat(image, rect.tl()) && point_in_mat(image, rect.br()));
}

void camera_worker::find_center_points(cv::Point2f center_point, cv::Point2f direction, float length, contour_vector_t contours)
{
    cv::Point2f center_right_line = line_P2P(center_point, center_point + direction);
    float length2 = normalizeVec(&center_right_line);
    //cv::line(camera_image, center_point, center_point + center_right_line * r, cv::Scalar(0, 0, 255), 4, cv::LINE_8);
    QList<cv::Point> onLine;
    for(size_t x = 0; x < contours.size(); x++){
        for(size_t y = 0; y < contours[x].size(); y++){
            if(5 > distance_point_to_line(center_point, center_point + center_right_line * 10 * length2, contours[x][y])){
                onLine.push_back(contours[x][y]);
            }
        }
    }
    qDebug() << "On Line "<< onLine.length();
    QList<cv::Point> merged_points;
    while(!onLine.empty()){
        cv::Point p = onLine.first();
        if(onLine.empty()){
            continue;
        }
        QList<cv::Point> cluster;
        for(cv::Point point : onLine){
            if(15 > point_distance(p, point)){
                cluster.push_back(point);
            }
        }
        for(cv::Point point : cluster){
            onLine.removeAll(point);
        }
        merged_points.push_back(mean_point(cluster));
    }
    for(cv::Point p : merged_points){
        //cv::circle(camera_image, p, 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
    }
    for(cv::Point p : merged_points){
        cv::Point minD1;
        cv::Point minD2;
        for(cv::Point p1 : merged_points){
            if(p != p1){
                float distance = point_distance(p, p1);
                if((length * 1.1) > distance || distance > (length * 0.9)){
                    minD1 = p1;
                }else{
                    if(((length * 1.1) > distance || distance > (length * 0.9)) && p1 != minD1){
                        minD2 = p1;
                    }
                }
            }
        }
        cv::Point center1 = p + ((p - minD1) / 2.0);
        cv::Point center2 = (p - minD2);
        int c1Color = check_color(threshold_image, center1, 25);
        int c2Color = check_color(threshold_image, center2, 25);
        if(c1Color != c2Color && c1Color != 2 && c2Color != 2){
            cv::circle(camera_image, center1, 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            //cv::circle(camera_image, center2, 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            //cv::line(camera_image, minD1, p, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
            //cv::line(camera_image, minD2, p, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
        }
    }
}

void camera_worker::check_texture(cv::Point2f start, cv::Point2f *resultA, cv::Point2f *resultB, cv::Point2f guideA, cv::Point2f guideB, cv::Point2f normalDiagonalA, cv::Point2f normalDiagonalB, cv::Point2f guideAB)
{
    cv::Point2f probe_point = start;
    cv::Point2f old_probe_point;
    cv::Point2f p2p_line;
    for(int i = 0; i < 8; i++){
        if(!is_zero(resultA[i]) && !is_zero(resultB[i])){
            p2p_line = line_P2P(resultA[i], resultB[i]);
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
            //cv::circle(camera_image, r * betterNormal + probe_point, 5, cv::Scalar(0, 0, 255), 4, cv::LINE_8);
            //cv::circle(camera_image, 2 * r * betterNormal + probe_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
            //cv::circle(camera_image, 2 * r2 * betterNormal + probe_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
            old_probe_point = probe_point;
            probe_point = 2 * r * betterNormal + probe_point;
            goto CENTER_PROBE_POINT;
        }
        SINGLE_SIDE_CHECK:
        if(!is_zero(resultA[i])){

        }
        OTHER_SIDE_CHECK:
        if(!is_zero(resultB[i])){
            cv::circle(camera_image, resultB[i], 5, cv::Scalar(0, 255, 0), 4);
            cv::Point2f normal_guideB(-guideB.y, guideB.x);
            cv::Point2f p2p_line = angled_vector_from_normal(normalDiagonalB, 170);
            p2p_line = guideAB;
            cv::line(camera_image, resultB[i], resultB[i] + 70 * p2p_line, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
            normalizeVec(&p2p_line);
            cv::Point2f normal_p2p_line(-p2p_line.y, p2p_line.x);
            cv::line(camera_image, resultB[i], resultB[i] + 70 * normal_p2p_line, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
            cv::Point2f betterNormal(guideA.x + guideB.x + normal_p2p_line.x, guideA.y + guideB.y + normal_p2p_line.y);
            normalizeVec(&betterNormal);
            if(is_nan(probe_point) || is_nan(betterNormal) || is_nan(resultB[i]) || is_nan(p2p_line)){
                continue;
            }
            print_vec(probe_point);
            print_vec(betterNormal);
            print_vec(resultB[i]);
            print_vec(p2p_line);
            float r = intersection_NormalLine_NormalLine(probe_point, betterNormal, resultB[i], p2p_line);
            cv::circle(camera_image, 2 * r * betterNormal + probe_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
            old_probe_point = probe_point;
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
            goto CENTER_PROBE_POINT;
        }else {
            i = 8;
        }
        CENTER_PROBE_POINT:
        QList<cv::Point> topedges;
        QList<cv::Point> bottomedges;
        QList<cv::Point> leftedges;
        QList<cv::Point> rightedges;
        cv::Point2f normal = line_P2P(old_probe_point, probe_point);
        float d = normalizeVec(&normal);
        /*
        cv::Point2f normal_normal(-normal.y, normal.x);
        cv::Point2f top_point = probe_point + normal * (d / 2.0);
        cv::Point2f bottom_point = probe_point - normal * (d / 2.0);
        cv::Point2f left_point = probe_point - normal_normal * (d / 2.0);
        cv::Point2f right_point = probe_point + normal_normal * (d / 2.0);
        float scalar = d * 0.8;
        while (topedges.empty()) {
            cv::Rect rect(probe_point + normal * scalar, cv::Size(50, 50));
            print_vec(rect.tl());
            if(rect.tl().y < 0){
                return;
            }
            qDebug() << "Top "<< topedges.length();
            harris_values(rect, &topedges);
            qDebug() << "Top "<< topedges.length();
            scalar += 0.02;
        }
        float side_length = 10;
        cv::Rect rectt(top_point.x - side_length, top_point.y - side_length, 2 * side_length, 2 * side_length);
        cv::Rect rectb(bottom_point.x - side_length, bottom_point.y - side_length, 2 * side_length, 2 * side_length);
        cv::Rect rectl(left_point.x - side_length, left_point.y - side_length, 2 * side_length, 2 * side_length);
        cv::Rect rectr(right_point.x - side_length, right_point.y - side_length, 2 * side_length, 2 * side_length);
        cv::rectangle(camera_image, rectt, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
        cv::rectangle(camera_image, rectb, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
        cv::rectangle(camera_image, rectl, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
        cv::rectangle(camera_image, rectr, cv::Scalar(0, 0, 255), 5, cv::LINE_8);
        harris_values(rectt, &topedges);
        harris_values(rectb, &bottomedges);
        harris_values(rectl, &leftedges);
        harris_values(rectr, &rightedges);
        if(topedges.empty()){
            qDebug() << "Top Empty";
        }
        if(bottomedges.empty()){
            qDebug() << "Bottom Empty";
        }
        if(leftedges.empty()){
            qDebug() << "Left Empty";
        }
        if(rightedges.empty()){
            qDebug() << "Right Empty";
        }
        cv::Point2f meanTop = mean_point(topedges);
        cv::Point2f meanBottom = mean_point(bottomedges);
        cv::Point2f meanLeft = mean_point(leftedges);
        cv::Point2f meanRight = mean_point(rightedges);
        print_vec(meanBottom);
        print_vec(meanLeft);
        print_vec(meanRight);
        print_vec(meanTop);
        cv::circle(camera_image, meanTop, 7, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
        cv::circle(camera_image, meanBottom, 7, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
        cv::circle(camera_image, meanLeft, 7, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
        cv::circle(camera_image, meanRight, 7, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
        cv::Point2f real_center = intersection_P2PLine_P2PLine(meanTop, meanBottom, meanLeft, meanRight);
        cv::circle(camera_image, real_center, 7, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
        */
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
    result_image_index_b = true;
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
                cv_camera.release();
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
        if(capture_b && cv_camera.isOpened() && cv_camera.read(camera_image)){
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
                        *a = getsubPixel(*a);
                        *b = getsubPixel(*b);
                        *c = getsubPixel(*c);
                        *d = getsubPixel(*d);
                        cv::Point2f center_point = intersection_P2PLine_P2PLine(*a, *d, *b, *c);
                        if(isnan(center_point.x) || isnan(center_point.y) || center_point.x < 0 || center_point.x > threshold_image.cols || center_point.y < 0 || center_point.y > threshold_image.rows){
                            continue;
                        }
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
                            int harris_check_dimensions = 25;
                            int x = d->x;
                            int y = d->y;
                            qDebug() << "Field Check";
                            if(x - harris_check_dimensions >= 0 && y - harris_check_dimensions >= 0 && x + harris_check_dimensions < threshold_image.cols && y + harris_check_dimensions < threshold_image.rows){
                                cv::Rect rect(x - harris_check_dimensions, y - harris_check_dimensions, 2 * harris_check_dimensions, 2 * harris_check_dimensions);
                                field_check(rect, *d);
                            }
                            x = a->x;
                            y = a->y;
                            if(x - harris_check_dimensions >= 0 && y - harris_check_dimensions >= 0 && x + harris_check_dimensions < threshold_image.cols && y + harris_check_dimensions < threshold_image.rows){
                                cv::Rect rect(x - harris_check_dimensions, y - harris_check_dimensions, 2 * harris_check_dimensions, 2 * harris_check_dimensions);
                                field_check(rect, *a);
                            }
                            x = b->x;
                            y = b->y;
                            if(x - harris_check_dimensions >= 0 && y - harris_check_dimensions >= 0 && x + harris_check_dimensions < threshold_image.cols && y + harris_check_dimensions < threshold_image.rows){
                                cv::Rect rect(x - harris_check_dimensions, y - harris_check_dimensions, 2 * harris_check_dimensions, 2 * harris_check_dimensions);
                                field_check(rect, *b);
                            }
                            x = c->x;
                            y = c->y;
                            if(x - harris_check_dimensions >= 0 && y - harris_check_dimensions >= 0 && x + harris_check_dimensions < threshold_image.cols && y + harris_check_dimensions < threshold_image.rows){
                                cv::Rect rect(x - harris_check_dimensions, y - harris_check_dimensions, 2 * harris_check_dimensions, 2 * harris_check_dimensions);
                                field_check(rect, *c);
                            }
                            bool is_black = threshold_image.at<uchar>(center_point.y, center_point.x) < threshold_value;
                            //qDebug() << "Is Black? " << is_black << " area " << check_color(threshold_image, center_point.x, center_point.y);
                            diagonal_probeing(*a, -lengthA, diagonalA, normalDiagonalA, tldiagonal, is_black);
                            diagonal_probeing(*b, -lengthB, diagonalB, normalDiagonalB, bldiagonal, is_black);
                            diagonal_probeing(*c, lengthB, diagonalB, normalDiagonalB, trdiagonal, is_black);
                            diagonal_probeing(*d, lengthA, diagonalA, normalDiagonalA, brdiagonal, is_black);
                            cv::Point2f maxTR;
                            for(int i = 0; i < 8; i++){
                                for(int j = 0; j < 4; j++){
                                    if(!is_zero(diagonalArray[j][i])){
                                        if(j == 2){
                                            maxTR = diagonalArray[j][i];
                                        }
                                        counterArray[j]++;
                                    }
                                }
                            }

                            find_center_points(center_point, lineAC, lengthAC, contours);
                            /*
                            cv::Point2f center_right_line = line_P2P(center_point, center_point + lineAC);
                            normalizeVec(&center_right_line);
                            cv::Point2f top_down_line = line_P2P(maxTR, maxTR + lineCD);
                            normalizeVec(&top_down_line);
                            float r = intersection_NormalLine_NormalLine(center_point, center_right_line, maxTR, top_down_line);
                            //cv::line(camera_image, center_point, center_point + center_right_line * r, cv::Scalar(0, 0, 255), 4, cv::LINE_8);
                            QList<cv::Point> onLine;
                            for(size_t x = 0; x < contours.size(); x++){
                                for(size_t y = 0; y < contours[x].size(); y++){
                                    if(5 > distance_point_to_line(center_point, center_point + center_right_line * r, contours[x][y])){
                                        onLine.push_back(contours[x][y]);
                                    }
                                }
                            }
                            qDebug() << "On Line "<< onLine.length();
                            QList<cv::Point> merged_points;
                            while(!onLine.empty()){
                                cv::Point p = onLine.first();
                                if(onLine.empty()){
                                    continue;
                                }
                                QList<cv::Point> cluster;
                                for(cv::Point point : onLine){
                                    if(15 > point_distance(p, point)){
                                        cluster.push_back(point);
                                    }
                                }
                                for(cv::Point point : cluster){
                                    onLine.removeAll(point);
                                }
                                merged_points.push_back(mean_point(cluster));
                            }
                            for(cv::Point p : merged_points){
                                cv::circle(camera_image, p, 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                            }

                            */

                            qDebug() << counterArray[COUNTER_TL] << " corners TL";
                            qDebug() << counterArray[COUNTER_TR] << " corners TR";
                            qDebug() << counterArray[COUNTER_BL] << " corners BL";
                            qDebug() << counterArray[COUNTER_BR] << " corners BR";
                            qDebug() << "";
                            //check_texture(center_point, tldiagonal, trdiagonal, lineAB, lineCD, normalDiagonalA, normalDiagonalB, lineAC);
                            //check_texture(center_point - lineAC, tldiagonal, trdiagonal, lineAB, lineCD, normalDiagonalA, normalDiagonalB, lineAC);
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
            if(result_image_index_b){
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
                result_image_index_b = false;
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


