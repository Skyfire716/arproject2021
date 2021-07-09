#include "camera_worker.h"
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QPixmap>

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
    QVector2D av = cv_point2f2qvec2d(a);
    QVector2D bv = cv_point2f2qvec2d(b);
    return qRadiansToDegrees(qAcos(QVector2D::dotProduct(av, bv) / (av.length() * bv.length())));
}

float camera_worker::normalizeVec(cv::Point2f *p)
{
    float length = qSqrt(pow(p->x, 2) + pow(p->y, 2));
    p->x = p->x / length;
    p->y = p->y / length;
    return length;
}

double camera_worker::calculateAngles(std::vector<cv::Point> points) {
    double angled = 0;
    for(size_t i = 0; i < points.size(); i++) {
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
    QVector2D av = cv_point2f2qvec2d(a);
    QVector2D bv = cv_point2f2qvec2d(b);
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

int camera_worker::neighbour_rect_probing(cv::Point2f tl, cv::Point2f bl, cv::Point2f tr, cv::Point2f br)
{
    cv::Point2f center_point = intersection_P2PLine_P2PLine(tl, br, bl, tr);
    return check_color(threshold_image, center_point);
}

int camera_worker::neighbour_validation_probing(cv::Point2f tl, cv::Point2f tr, cv::Point2f bl, cv::Point2f br)
{
    cv::Point2f center_point = intersection_P2PLine_P2PLine(tl, br, bl, tr);
    cv::Point2f lineAC = line_P2P(tl, tr);
    cv::Point2f lineCD = line_P2P(tr, br);
    float lengthAC = normalizeVec(&lineAC);
    float lengthCD = normalizeVec(&lineCD);
    int current_color = check_color(threshold_image, center_point);
    int top_color = check_color(threshold_image, center_point - lengthCD * lineCD);
    int bottom_color = check_color(threshold_image, center_point + lengthCD * lineCD);
    int left_color = check_color(threshold_image, center_point - lengthAC * lineAC);
    int right_color = check_color(threshold_image, center_point + lengthAC * lineAC);
    int tl_corner = 0;
    int tr_corner = 0;
    int bl_corner = 0;
    int br_corner = 0;
    int b_edge = 0;
    int l_edge = 0;
    int r_edge = 0;
    int t_edge = 0;
    if(top_color != current_color){
        bl_corner++;
        br_corner++;
        b_edge++;
        l_edge++;
        r_edge++;
    }
    if(bottom_color != current_color){
        tl_corner++;
        tr_corner++;
        l_edge++;
        r_edge++;
        t_edge++;
    }
    if(left_color != current_color){
        tr_corner++;
        br_corner++;
        b_edge++;
        t_edge++;
        r_edge++;
    }
    if(right_color != current_color){
        tl_corner++;
        bl_corner++;
        b_edge++;
        t_edge++;
        l_edge++;
    }
    if((tl_corner == 2 || br_corner == 2 || bl_corner == 2 || tr_corner == 2) || (b_edge == 3 || l_edge == 3 || r_edge == 3 || t_edge == 3)){
        return current_color;
    }
    return chessboard::UNDEFINED;
}

QPointF camera_worker::cv_point2f2qpoint(cv::Point2f p)
{
    return QPointF(p.x, p.y);
}

QVector2D camera_worker::cv_point2f2qvec2d(cv::Point2f p)
{
    return QVector2D(p.x, p.y);
}

cv::Point2f camera_worker::line_probeing(cv::Point2f start_corner, float line_length, cv::Point2f line_normalized_vec)
{
    int sign = 1;
    if(line_length < 0){
        sign = -1;
        line_length = abs(line_length);
    }
    return getsubPixel(start_corner + sign * line_length * line_normalized_vec);
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
    //int subpixelSearchLength = 150;
    int subpixelSearchLength = 85;
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

void camera_worker::harris_values(cv::Rect rect, QList<QPair<cv::Point, double> > *harris_features)
{
    if(!rect_in_mat(threshold_image, rect)){
        qDebug() << "Leaving Harris";
        return;
    }
    cv::Mat harris_mat = threshold_image(rect);
    //Val falsch
    cv::Mat harris(harris_mat.rows,harris_mat.cols, CV_MAKETYPE(CV_32FC1, 6));
    cv::cornerEigenValsAndVecs(harris_mat, harris, 3, 3);
    for(int x = 0; x < harris.cols; x++){
        for(int y = 0; y < harris.rows; y++){
            cv::Vec6f vals = harris.at<cv::Vec6f>(y, x);
            float l1 = vals[0];
            float l2 = vals[1];
            double harris_val = l1 * l2 - 0.04 * pow(l1 + l2, 2);
            if(harris_val / HARRIS_DROPOUT < 0 || harris_val / HARRIS_DROPOUT > 0){
                QPair<cv::Point, double> p(cv::Point(x, y), harris_val);
                harris_features->push_back(p);
            }
        }
    }
}

void camera_worker::harris_edges(cv::Rect rect, QList<cv::Point> *edges)
{
    QList<QPair<cv::Point, double>> harris_features;
    harris_values(rect, &harris_features);
    for(QPair<cv::Point, double> pair : harris_features){
        if(pair.second / HARRIS_DROPOUT < 0){
            int x = pair.first.x;
            int y = pair.first.y;
            cv::Point2f center_point(rect.tl().x + rect.width / 2.0, rect.tl().y + rect.height / 2.0);
            float distance = point_distance(cv::Point2f(rect.tl().x + x, rect.tl().y + y), center_point);
            if(distance <= 18 && distance >= 15){
                if(edges){
                    edges->push_back(pair.first);
                }
            }
        }
    }
}

void camera_worker::harris_corner(cv::Rect rect, QList<cv::Point> *corners)
{
    QList<QPair<cv::Point, double>> harris_features;
    harris_corner(rect, &harris_features);
    for(QPair<cv::Point, double> pair : harris_features){
        corners->append(pair.first);
    }
}

void camera_worker::harris_corner(cv::Rect rect, QList<QPair<cv::Point, double> > *corners)
{
    QList<QPair<cv::Point, double>> harris_features;
    harris_values(rect, &harris_features);
    for(QPair<cv::Point, double> pair : harris_features){
        if(pair.second / HARRIS_DROPOUT > 0){
            int x = pair.first.x;
            int y = pair.first.y;
            cv::Point2f center_point(rect.tl().x + rect.width / 2.0, rect.tl().y + rect.height / 2.0);
            float distance = point_distance(cv::Point2f(rect.tl().x + x, rect.tl().y + y), center_point);
            if(distance <= 18){
                if(corners){
                    corners->push_back(pair);
                }
            }
        }
    }
}

QPair<int, int> camera_worker::vec2pair(QVector2D v)
{
    return QPair<int, int>(v.x(), v.y());
}

bool camera_worker::valid_neighbour(cv::Point2f p, cv::Point2f axisA, cv::Point2f axisB)
{
    int current_color = check_color(threshold_image, p);
    int top_color = check_color(threshold_image, p + axisA);
    int bottom_color = check_color(threshold_image, p - axisA);
    int left_color = check_color(threshold_image, p + axisB);
    int right_color = check_color(threshold_image, p - axisB);
    int tl_corner = 0;
    int tr_corner = 0;
    int bl_corner = 0;
    int br_corner = 0;
    int b_edge = 0;
    int l_edge = 0;
    int r_edge = 0;
    int t_edge = 0;
    if(top_color != current_color){
        bl_corner++;
        br_corner++;
        b_edge++;
        l_edge++;
        r_edge++;
    }
    if(bottom_color != current_color){
        tl_corner++;
        tr_corner++;
        l_edge++;
        r_edge++;
        t_edge++;
    }
    if(left_color != current_color){
        tr_corner++;
        br_corner++;
        b_edge++;
        t_edge++;
        r_edge++;
    }
    if(right_color != current_color){
        tl_corner++;
        bl_corner++;
        b_edge++;
        t_edge++;
        l_edge++;
    }
    if(tl_corner == 2 || br_corner == 2 || bl_corner == 2 || tr_corner == 2){
        return true;
    }
    if(b_edge == 3 || l_edge == 3 || r_edge == 3 || t_edge == 3){
        return true;
    }
    return false;
}

bool camera_worker::probe_neighbours(cv::Point2f tl, cv::Point2f tr, cv::Point2f bl, cv::Point2f br, QVector2D current_pos, chessboard_controller &chesscontroller, QSet<QPair<int, int>> &no_field, QSet<QPair<int, int>> &valid){
    cv::Point2f center_point = intersection_P2PLine_P2PLine(tl, br, tr, bl);
    int center_color = check_color(threshold_image, center_point, 50);
    if(center_color == 2){
        //cv::rectangle(camera_image, cv::Rect(center_point, cv::Size(20, 20)), cv::Scalar(0, 0, 255), 5, cv::LINE_8);
        //cv::putText(camera_image, "C", center_point, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 4, cv::LINE_8);
        return false;
    }
    if(!chesscontroller.add_rect(current_pos, cv_point2f2qpoint(tl), cv_point2f2qpoint(tr), cv_point2f2qpoint(bl), cv_point2f2qpoint(br), cv_point2f2qpoint(center_point), center_color)){
        return false;
    }
    cv::Point2f lineAC = line_P2P(tl, tr);
    cv::Point2f lineAB = line_P2P(tl, bl);
    cv::Point2f lineCD = line_P2P(tr, br);
    cv::Point2f lineBD = line_P2P(bl, br);
    float lengthAC = normalizeVec(&lineAC);
    float lengthAB = normalizeVec(&lineAB);
    float lengthCD = normalizeVec(&lineCD);
    float lengthBD = normalizeVec(&lineBD);
    cv::Point2f normalLineAC(-lineAC.y, lineAC.x);
    cv::Point2f normalLineAB(-lineAB.y, lineAB.x);
    cv::Point2f normalLineCD(-lineCD.y, lineCD.x);
    cv::Point2f normalLineBD(-lineBD.y, lineBD.x);
    cv::Point2f tltn = line_probeing(tl, -lengthAB, lineAB);
    cv::Point2f tlln = line_probeing(tl, -lengthAC, lineAC);
    cv::Point2f blln = line_probeing(bl, -lengthBD, lineBD);
    cv::Point2f blbn = line_probeing(bl, lengthAB, lineAB);
    cv::Point2f trtn = line_probeing(tr, -lengthCD, lineCD);
    cv::Point2f trrn = line_probeing(tr, lengthAC, lineAC);
    cv::Point2f brrn = line_probeing(br, lengthBD, lineBD);
    cv::Point2f brbn = line_probeing(br, lengthCD, lineCD);
    //       |         TLTN         TRTN           |
    //       |           |            |            |
    //       |           |            |            |
    //       |           |    top     |            |
    //       |           |            |            |
    //       |           |            |            |
    //-----TLLN----------TL----------TR----------TRRN
    //       |           |            |            |
    //       |           |            |            |
    //       |   left    |  current   |  right     |
    //       |           |            |            |
    //       |           |            |            |
    //-----BLLN----------BL----------BR----------BRRN
    //       |           |            |            |
    //       |           |            |            |
    //       |           |   bottom   |            |
    //       |           |            |            |
    //       |         BLBN         BRBN           |
    QPointF top_center = cv_point2f2qpoint(intersection_P2PLine_P2PLine(tltn, tr, trtn, tl));
    QPointF bottom_center = cv_point2f2qpoint(intersection_P2PLine_P2PLine(bl, brbn, br, blbn));
    QPointF left_center = cv_point2f2qpoint(intersection_P2PLine_P2PLine(tlln, bl, tl, blln));
    QPointF right_center = cv_point2f2qpoint(intersection_P2PLine_P2PLine(tr, brrn, trrn, br));
    QVector2D top = QVector2D(current_pos.x(), current_pos.y() + 1);
    QVector2D right = QVector2D(current_pos.x() + 1, current_pos.y());
    QVector2D bottom = QVector2D(current_pos.x(), current_pos.y() - 1);
    QVector2D left = QVector2D(current_pos.x() - 1, current_pos.y());
    int counter = 0;
    int left_neighbour_color = neighbour_validation_probing(tlln, tl, blln, bl);
    int right_neighbour_color = neighbour_validation_probing(tr, trrn, br, brrn);
    int top_neighbour_color = neighbour_validation_probing(tltn, trtn, tl, tr);
    int bottom_neighbour_color = neighbour_validation_probing(bl, br, blbn, brbn);
    if(!valid.contains(vec2pair(left)) && !no_field.contains(vec2pair(left)) && (center_color != left_neighbour_color)){
        if(left_neighbour_color == 2){
            no_field.insert(vec2pair(left));
        }else{
            if(chesscontroller.add_rect(left, cv_point2f2qpoint(tlln), cv_point2f2qpoint(tl), cv_point2f2qpoint(blln), cv_point2f2qpoint(bl), left_center, left_neighbour_color)){
                counter += 1;
                valid.insert(vec2pair(left));
                probe_neighbours(tlln, tl, blln, bl, left, chesscontroller, no_field, valid);
            }
        }
    }
    if(!valid.contains(vec2pair(right)) && !no_field.contains(vec2pair(right)) && (center_color != right_neighbour_color)){
        if(right_neighbour_color == 2){
            no_field.insert(vec2pair(right));
        }else{
            if(chesscontroller.add_rect(right, QPointF(tr.x, tr.y), QPointF(trrn.x, trrn.y), QPointF(br.x, br.y), QPointF(brrn.x, brrn.y), right_center, right_neighbour_color)){
                valid.insert(vec2pair(right));
                counter += 1;
                probe_neighbours(tr, trrn, br, brrn, right, chesscontroller, no_field, valid);
            }
        }
    }
    if(!valid.contains(vec2pair(top)) && !no_field.contains(vec2pair(top)) && (center_color != top_neighbour_color)){
        if(top_neighbour_color == 2){
            no_field.insert(vec2pair(top));
        }else{
            if(chesscontroller.add_rect(top, QPointF(tltn.x, tltn.y), QPointF(trtn.x, trtn.y), QPointF(tl.x, tl.y), QPointF(tr.x, tr.y), top_center, top_neighbour_color)){
                valid.insert(vec2pair(top));
                counter += 1;
                probe_neighbours(tltn, trtn, tl, tr, top, chesscontroller, no_field, valid);
            }
        }
    }
    if(!valid.contains(vec2pair(bottom)) && !no_field.contains(vec2pair(bottom)) && (center_color != bottom_neighbour_color)){
        if(bottom_neighbour_color == 2){
            no_field.insert(vec2pair(bottom));
        }else{
            if(chesscontroller.add_rect(bottom, QPointF(bl.x, bl.y), QPointF(br.x, br.y), QPointF(blbn.x, blbn.y), QPointF(brbn.x, brbn.y), bottom_center, bottom_neighbour_color == 0)){
                counter += 1;
                valid.insert(vec2pair(bottom));
                probe_neighbours(bl, br, blbn, brbn, bottom, chesscontroller, no_field, valid);
            }
        }
    }
    return (counter >= 2);
}

float camera_worker::intersection_NormalLine_NormalLine(cv::Point2f line_p1, cv::Point2f line_n1, cv::Point2f line_p2, cv::Point2f line_n2)
{
    float r = ((line_p1.y - line_p2.y) - ((line_p1.x - line_p2.x) * line_n2.y) / line_n2.x) / (-line_n1.y + (line_n1.x * line_n2.y / line_n2.x));
    float s = (line_p1.x + r * line_n1.x - line_p2.x) / line_n2.x;
    if(line_p1.x + r * line_n1.x != line_p2.x + s * line_n2.x || line_p1.y + r * line_n1.y != line_p2.y + s * line_n2.y){
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
    cv::Rect rect(p.x - area, p.y - area, 2*area, 2*area);
    if(!rect_in_mat(image, rect)){
        return chessboard::UNDEFINED;
    }
    int sum = 0;
    for(int i = -area; i <= area; i++){
        for(int j = -area; j <= area; j++){
            if(i >= 0 && i < image.cols && j >= 0 && j < image.rows){
                sum += image.at<uchar>(p.y, p.x);
            }
        }
    }
    if(sum < 0.05 * pow(area, 2) * 1){
        return chessboard::BLACK;
    }else if(sum > 0.95 * pow(area, 2) * 255){
        return chessboard::WHITE;
    }else{
        return chessboard::UNDEFINED;
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
    return qIsNaN(p.x) || qIsNaN(p.y);
}

bool camera_worker::point_in_mat(cv::Mat image, cv::Point2f p)
{
    return (p.x >= 0 && p.y >= 0 && p.x < image.cols && p.y < image.rows);
}

bool camera_worker::rect_in_mat(cv::Mat image, cv::Rect rect)
{
    return (point_in_mat(image, rect.tl()) && point_in_mat(image, rect.br()));
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
            if(new_cv_index < 0){
                qDebug() << "Load from File";
                //TODO Check here if you get an error on non linux systems
                QTemporaryDir tempDir;
                if(tempDir.isValid()){
                    QString filename = "";
                    QString loading_file = "";
                    switch (new_cv_index) {
                        case -1:
                            filename.append("/archess_hard.mp4");
                            loading_file.append(":/videos/resources/videos/VID_20210603_121318.mp4");
                            threshold_value = 68;
                            break;
                        case -2:
                            filename.append("/archess_stabel.mp4");
                            loading_file.append(":/videos/resources/videos/stabel_chessboard.mp4");
                            threshold_value = 140;
                            break;
                        case -3:
                            filename.append("/archess_moving.mp4");
                            loading_file.append(":/videos/resources/videos/moving_chessboard.mp4");
                            threshold_value = 64;
                            break;
                    }
                    const QString tempFile = tempDir.path() +  filename;
                    if(QFile::copy(loading_file, tempFile)){
                        qDebug() << "Mp4 " << tempFile;
                        qDebug() << "Open? " << cv_camera.open(tempFile.toStdString());
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
            if(threshold_method == 5){
                cv::adaptiveThreshold(gray_image, threshold_image, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 33, 5);
            }else{
                cv::threshold(gray_image, threshold_image, threshold_value, 255, threshold_method);
            }
            contour_vector_t contours;
            cv::findContours(threshold_image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
            std::vector<cv::Rect> boundingRects(contours.size());
            bool is_first = true;
            for(size_t k = 0; k < contours.size(); k++){
                contour_t approx_contour;
                cv::approxPolyDP(contours[k], approx_contour, cv::arcLength(contours[k], true) * 0.02, true);
                boundingRects[k] = cv::boundingRect(approx_contour);
                contour_vector_t cov, approx;
                cov.emplace_back(contours[k]);
                approx.emplace_back(approx_contour);
                //cv::drawContours(camera_image, cov, -1, cv::Scalar(255, 0, 0), 4, 1);
                cv::Scalar color(0, 0, 255);
                if(approx_contour.size() == 4 && boundingRects[k].area() > 200){
                    cv::Point *a = NULL;
                    cv::Point *b = NULL;
                    cv::Point *c = NULL;
                    cv::Point *d = NULL;
                    int ordering = get_ordered_points(boundingRects[k], approx_contour);
                    a = &approx_contour[(ordering / 1000) % 10];
                    b = &approx_contour[(ordering/100) % 10];
                    c = &approx_contour[(ordering / 10) % 10];
                    d = &approx_contour[ordering % 10];
                    if(a && b && c && d && !(a == b || b == c || c == d || d == a || b == d || a == c)){
                        *a = getsubPixel(*a);
                        *b = getsubPixel(*b);
                        *c = getsubPixel(*c);
                        *d = getsubPixel(*d);
                        cv::Point2f center_point = intersection_P2PLine_P2PLine(*a, *d, *b, *c);
                        if(qIsNaN(center_point.x) || qIsNaN(center_point.y) || center_point.x < 0 || center_point.x > threshold_image.cols || center_point.y < 0 || center_point.y > threshold_image.rows){
                            continue;
                        }
                        //Diagonale durch laufen
                        cv::Point2f lineAC = line_P2P(*a, *c);
                        cv::Point2f lineAB = line_P2P(*a, *b);
                        cv::Point2f lineCD = line_P2P(*c, *d);
                        cv::Point2f lineBD = line_P2P(*b, *d);
                        normalizeVec(&lineAC);
                        normalizeVec(&lineAB);
                        normalizeVec(&lineCD);
                        normalizeVec(&lineBD);
                        cv::Point2f normalLineAC(-lineAC.y, lineAC.x);
                        cv::Point2f normalLineAB(-lineAB.y, lineAB.x);
                        cv::Point2f normalLineCD(-lineCD.y, lineCD.x);
                        cv::Point2f normalLineBD(-lineBD.y, lineBD.x);
                        cv::Point2f diagonalA = line_P2P(*a, *d);
                        cv::Point2f diagonalB = line_P2P(*b, *c);
                        normalizeVec(&diagonalA);
                        normalizeVec(&diagonalB);
                        if(is_first){
                            QSet<QPair<int, int>> valid;
                            QSet<QPair<int, int>> no_field;
                            probe_neighbours(*a, *c, *b, *d, QVector2D(0, 0), my_chessboard_controller, no_field, valid);
                            if(valid.size() >= 64){
                                cv::line(camera_image, *a, *c, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                                cv::line(camera_image, *a, *b, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                                cv::line(camera_image, *c, *d, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                                cv::line(camera_image, *b, *d, cv::Scalar(0, 255, 0), 5, cv::LINE_8);
                                cv::circle(camera_image, center_point, 5, cv::Scalar(0, 255, 0), 4, cv::LINE_8);
                                emit chessboard_updated(QPixmap::fromImage(my_chessboard_controller.get_image()));
                                my_chessboard_controller.get_current_board().drawBoard(camera_image);
                                QPair<QQuaternion, QVector3D> trans = my_chessboard_controller.get_transform();
                                qDebug() << "Got Chessboard";
                                QQuaternion q = trans.first;
                                QVector3D transV = trans.second;
                                qDebug() << "Created Subtypes";
                                emit new_ar_transform_singels(q.scalar(), q.x(), q.y(), q.z(), transV.x(), transV.y(), transV.z());
                                qDebug() << "Sendt To Arwidget";
                                //cv::Mat imageMarker(cv::Size(200, 200), camera_image.type());
                                //cv::warpPerspective(camera_image, imageMarker, my_chessboard_controller.get_current_board().get_rotation_matrix(), cv::Size(200, 200));
                                //QImage img((uchar*)imageMarker.data, imageMarker.cols, imageMarker.rows, imageMarker.step, QImage::Format_RGB888);
                                //emit chessboard_updated(QPixmap::fromImage(img));
                                my_chessboard_controller.switch_board();
                                is_first = false;
                                this->thread()->msleep(750);
                            }else{
                                my_chessboard_controller.clear_current();
                            }

                            //this->thread()->msleep(750);
                        }
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
