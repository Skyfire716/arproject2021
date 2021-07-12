#include "chessboard.h"

void chessboard::clear()
{
    min_x = 0;
    min_y = 0;
    max_x = 0;
    max_y = 0;
    origin_index.setX(0);
    origin_index.setY(0);
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            corners[i][j].clear();
        }
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            colors[i][j] = chessboard::UNDEFINED;
            centers[i][j].clear();
        }
    }
}

void chessboard::optimize_chessboard()
{
    QPair<int, int> tlI = get_board_corner_indizes(chessboard::TOP_LEFT_CORNER);
    QPair<int, int> trI = get_board_corner_indizes(chessboard::TOP_RIGHT_CORNER);
    QPair<int, int> brI = get_board_corner_indizes(chessboard::BOTTOM_RIGHT_CORNER);
    QPair<int, int> blI = get_board_corner_indizes(chessboard::BOTTOM_LEFT_CORNER);
    int tlX = tlI.first, tlY = tlI.second;
    int trX = trI.first, trY = trI.second;
    int brX = brI.first, brY = brI.second;
    int blX = blI.first, blY = blI.second;
    QList<QVector2D> topLines;
    QList<QVector2D> leftLines;
    QList<QVector2D> bottomLines;
    QList<QVector2D> rightLines;
    for(int i = 0; i < 9; i++){
        int x = (tlX + i) < 0 ? 8 + (tlX + i) : (((tlX + i) > 8) ? ((tlX + i) % 9) : (tlX + i));
        int y = (brY + i) < 0 ? 8 + (brY + i) : (((brY + i) > 8) ? ((brY + i) % 9) : (brY + i));
        int xx = (blX + i) < 0 ? 8 + (blX + i) : (((blX + i) > 8) ? ((blX + i) % 9) : (blX + i));
        int yy = (blY + i) < 0 ? 8 + (blY + i) : (((blY + i) > 8) ? ((blY + i) % 9) : (blY + i));
        if(i > 1 && i < 8){
            topLines.append(get_mean_line_for_segment(x, tlY, true));
            leftLines.append(get_mean_line_for_segment(blX, yy, false));
            bottomLines.append(get_mean_line_for_segment(xx, blY, true));
            rightLines.append(get_mean_line_for_segment(brX, y, false));
        }
    }
    QVector2D top_heading;
    QVector2D right_heading;
    QVector2D bottom_heading;
    QVector2D left_heading;
    for(QVector2D v : topLines){
        top_heading += v;
    }
    for(QVector2D v : rightLines){
        right_heading += v;
    }
    for(QVector2D v : bottomLines){
        bottom_heading += v;
    }
    for(QVector2D v : leftLines){
        left_heading += v;
    }
    QLineF topLine(corners[tlX + 4][tlY][0].toPointF(), corners[tlX + 4][tlY][0].toPointF() + top_heading.toPointF());
    QLineF rightLine(corners[brX][brY + 4][0].toPointF(), corners[brX][brY + 4][0].toPointF() + right_heading.toPointF());
    QLineF bottomLine(corners[blX + 4][blY][0].toPointF(), corners[blX + 4][blY][0].toPointF() + bottom_heading.toPointF());
    QLineF leftLine(corners[blX][blY + 4][0].toPointF(), corners[blX][blY + 4][0].toPointF() + left_heading.toPointF());
    QPointF realTRCorner;
    QPointF realTLCorner;
    QPointF realBRCorner;
    QPointF realBLCorner;
    QLineF::IntersectType typeTR = topLine.intersect(rightLine, &realTRCorner);
    QLineF::IntersectType typeTL = leftLine.intersect(topLine, &realTLCorner);
    QLineF::IntersectType typeBL = bottomLine.intersect(leftLine, &realBLCorner);
    QLineF::IntersectType typeBR = rightLine.intersect(bottomLine, &realBRCorner);
    if(typeTL != QLineF::NoIntersection && typeTR != QLineF::NoIntersection &&
            typeBL != QLineF::NoIntersection && typeBR != QLineF::NoIntersection){
        corners[tlX][tlY].clear();
        corners[tlX][tlY].append(QVector2D(realTLCorner));
        corners[trX][trY].clear();
        corners[trX][trY].append(QVector2D(realTRCorner));
        corners[blX][blY].clear();
        corners[blX][blY].append(QVector2D(realBLCorner));
        corners[brX][brY].clear();
        corners[brX][brY].append(QVector2D(realBRCorner));
    }
}

void chessboard::validating_colors(cv::Mat image, int(*check_color)(cv::Mat image, cv::Point2f p))
{
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            QLineF diagA(corners[x][y][0].toPointF(), corners[x + 1][y + 1][0].toPointF());
            QLineF diagB(corners[x + 1][y][0].toPointF(), corners[x][y + 1][0].toPointF());
            QPointF center;
            if(QLineF::NoIntersection != diagA.intersect(diagB, &center)){
                int center_color = check_color(image, qpoint2cv_point2f(center.toPoint()));
                if(chessboard::BLACK == center_color){
                    colors[x][y] = chessboard::BLACK;
                }else if(chessboard::WHITE == center_color){
                    colors[x][y] = chessboard::WHITE;
                }
            }
        }
    }
}

void chessboard::try_letter_detection(cv::Mat image)
{
    /*
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if(api->Init(NULL, "eng")){
        qDebug() << "Couldn't initialize Tesseract";
        return;
    }
    qDebug() << "Tesseract Init";
    char *outputText;
    QPair<int, int> center_pair = get_board_corner_center_indizes(chessboard::TOP_LEFT_CORNER);
    int x = center_pair.first, y = center_pair.second;
    QVector2D top_distance(get_corner_by_indizes(x, y + 1) - get_corner_by_indizes(x + 1, y + 1));
    QVector2D bottom_distance(get_corner_by_indizes(x, y) - get_corner_by_indizes(x + 1, y));
    float width_distance = (get_corner_by_indizes(x, y + 1) - get_corner_by_indizes(x, y)).length();
    cv::Size aOiSize(width_distance, (3/7.0) * width_distance);
    cv::Mat imageMarker(aOiSize, image.type());
    cv::warpPerspective(image, imageMarker, get_rotation_matrix(
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y + 1) + top_distance),
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y) + bottom_distance),
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y + 1)),
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y)), aOiSize), aOiSize);
    cv::flip(imageMarker, imageMarker, 0);
    api->SetImage(imageMarker.data, imageMarker.cols, imageMarker.rows, imageMarker.channels(), imageMarker.step);
    outputText = api->GetUTF8Text();
    QString ocr_output = QString::fromUtf8(outputText);
    qDebug() << "OCR Output " << ocr_output;
    api->End();
    delete api;
    delete [] outputText;
    */
}

QPair<cv::Mat, std::vector<cv::KeyPoint>> chessboard::get_referenceData(cv::Mat reference)
{
    std::vector<cv::KeyPoint> reference_keypoints;
    cv::Mat reference_descriptors;
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
    detector->detect(reference, reference_keypoints );
    return QPair<cv::Mat, std::vector<cv::KeyPoint>>(reference_descriptors, reference_keypoints);
}

void chessboard::setup_reference_data()
{
    terms.clear();
    for(int j = 0; j < 2; j++){
        char start_index = (j < 1) ? 65 : 49;
        char end_index = (j < 1) ? 73 : 57;
        for(char i = start_index; i < end_index; i++){
            QString filename = ":/surfreferences/resources/surfreferences/";
            filename.append(i);
            filename.append("_SurfReference.png");
            QImage q_reference_image = QImage(filename).convertToFormat(QImage::Format_RGB888);
            cv::Mat reference_mat(q_reference_image.height(), q_reference_image.width(), CV_8UC3, (cv::Scalar*)q_reference_image.scanLine(0));
            cv::cvtColor(reference_mat, reference_mat, cv::COLOR_RGB2BGR);
            QPair<cv::Mat, std::vector<cv::KeyPoint>> result = get_referenceData(reference_mat);
            descriptors[j] = result.first;
            keypoints[j] = result.second;
            for(size_t l = 0; l < result.second.size(); l++){
                chessboard::terms.append(chessboard::KeyPoint2QString(result.second.at(l)));
            }
        }
    }
}

QPair<std::vector<cv::KeyPoint>, std::vector<cv::DMatch>> chessboard::get_features(cv::Mat reference_descriptors, std::vector<cv::KeyPoint> reference_keypoints, cv::Mat real_image)
{
    std::vector<cv::KeyPoint> real_keypoints;
    cv::Mat real_descriptors;
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
    cv::Ptr<cv::DescriptorExtractor> descriptor = cv::ORB::create();

    cv::Ptr<cv::DescriptorMatcher> matcher  = cv::DescriptorMatcher::create ( "BruteForce-Hamming" );
    detector->detect(real_image,real_keypoints );
    descriptor->compute(real_image, real_keypoints, real_descriptors);
    std::vector<cv::DMatch> matches;
    matcher->match(reference_descriptors, real_descriptors, matches );
    double min_dist = 10000, max_dist = 0;
    for (int i = 0; i < reference_descriptors.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }
    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < reference_descriptors.rows; i++)
    {
        if (matches[i].distance <= qMax(2*min_dist, 30.0))
        {
            good_matches.push_back(matches[i]);
        }
    }

    /*
    cv::Mat img_match;
    cv::Mat img_goodmatch;
    drawMatches (reference, keypoints_1, real_image, keypoints_2, matches, img_match );
    drawMatches (reference, keypoints_1, real_image, keypoints_2, good_matches, img_goodmatch );
    imshow ( "所有匹配点对", img_match );
    imshow ( "优化后匹配点对", img_goodmatch );
    cv::waitKey(10);
    cv::waitKey(10);
    cv::waitKey(10);
    cv::waitKey(10);
    */
    return QPair<std::vector<cv::KeyPoint>, std::vector<cv::DMatch>>(real_keypoints, good_matches);
}

void chessboard::detect_simularities(cv::Mat *reference_descriptors, std::vector<cv::KeyPoint> *reference_keypoints, cv::Mat real_image)
{
    QVector<QVector<QString>> current_word_container;
    QVector<QPair<std::vector<cv::KeyPoint>, std::vector<cv::DMatch>>> result_container;
    for(int i =  0; i < 8; i++){
        QPair<std::vector<cv::KeyPoint>, std::vector<cv::DMatch>> result = get_features(reference_descriptors[i], reference_keypoints[i], real_image);
        QVector<QString> word;
        QHash<int, int> map;
        for(size_t j = 0; j < result.second.size(); j++){
            cv::DMatch match = result.second.at(j);
            map.insert(match.queryIdx, match.trainIdx);
        }
        for(size_t j = 0; j < result.first.size(); j++){
            cv::KeyPoint kp = reference_keypoints[j][map.value(j)];
            word.append(KeyPoint2QString(kp));
        }

    }
}

void chessboard::bla_wrapper(cv::Mat image)
{
    qDebug() << "Terms Pointer "  << QString::pointer(&terms);
    qDebug() << "Terms descriptors " << QString::pointer(descriptors);
    qDebug() << "KeyPoints Pointer " << QString::pointer(keypoints);
    QPair<int, int> center_pair = get_board_corner_center_indizes(chessboard::TOP_LEFT_CORNER);
    int x = center_pair.first, y = center_pair.second;
    QVector2D top_distance(get_corner_by_indizes(x, y + 1) - get_corner_by_indizes(x + 1, y + 1));
    QVector2D bottom_distance(get_corner_by_indizes(x, y) - get_corner_by_indizes(x + 1, y));
    float width_distance = (get_corner_by_indizes(x, y + 1) - get_corner_by_indizes(x, y)).length();
    //cv::Size aOiSize(width_distance, (3/7.0) * width_distance);
    cv::Size aOiSize(100, 100);
    cv::Mat H = get_homography_matrix();
    cv::Mat imageMarker(aOiSize, image.type());
    cv::warpPerspective(image, imageMarker, get_rotation_matrix(
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y + 1) + top_distance),
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y) + bottom_distance),
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y + 1)),
                            qvec2d2cv_point2f(get_corner_by_indizes(x, y)), aOiSize), aOiSize);

    bool letter_detect = true;
    if(letter_detect){
        detect_simularities(descriptors, keypoints, imageMarker);
    }else{
        detect_simularities(&descriptors[8], &keypoints[8], imageMarker);
    }
}

QString chessboard::KeyPoint2QString(cv::KeyPoint kp)
{
    QString text;
    text.append(QString::number(kp.angle));
    text.append(",");
    text.append(QString::number(kp.class_id));
    text.append(",");
    text.append(QString::number(kp.octave));
    text.append(",");
    text.append(QString::number(kp.pt.x));
    text.append(",");
    text.append(QString::number(kp.pt.y));
    text.append(",");
    text.append(QString::number(kp.response));
    text.append(",");
    text.append(QString::number(kp.size));
    return text;
}

cv::Point2f chessboard::qvec2d2cv_point2f(QVector2D v)
{
    return cv::Point2f(v.x(), v.y());
}

cv::Point2f chessboard::qpoint2cv_point2f(QPoint p)
{
    return cv::Point2f(p.x(), p.y());
}

cv::Point2f chessboard::qpoint2f2cv_point2f(QPointF p)
{
    return cv::Point2f(p.x(), p.y());
}

cv::Point3f chessboard::qpoint2cv_point3f(QPoint p)
{
    return qvec2d2cv_point3f(QVector2D(p));
}

cv::Point3f chessboard::qvec2d2cv_point3f(QVector2D v)
{
    return cv::Point3f(v.x(), v.y(), 1);
}

cv::Vec3f chessboard::qvec2d2cv_vec3f(QVector2D v)
{
    return cv::Vec3f(v.x(), v.y(), 1);
}

cv::Vec3f chessboard::qpoint2cv_vec3f(QPoint p)
{
    return qvec2d2cv_vec3f(QVector2D(p));
}

cv::Mat chessboard::qvec2d2cv_mat(QVector2D v, int cv_mat_type)
{
    cv::Mat m(3, 1, cv_mat_type);
    *(double*)m.ptr(0, 0) = v.x();
    *(double*)m.ptr(1, 0) = v.y();
    *(double*)m.ptr(2, 0) = 1;
    return m;
}

cv::Mat chessboard::qpoint2cv_mat(QPoint p, int cv_mat_type)
{
    return qvec2d2cv_mat(QVector2D(p), cv_mat_type);
}

QVector2D chessboard::get_mean_line_for_segment(int x, int y, bool x_axis)
{
    QVector<QLineF> topLines;
    if(x > 0 && x_axis){
        for(QVector2D v : corners[x - 1][y]){
            for(QVector2D v1 : corners[x][y]){
                topLines.append(QLineF(v.toPointF(), v1.toPointF()));
            }
        }
    }else if(y > 0){
        for(QVector2D v : corners[x][y - 1]){
            for(QVector2D v1 : corners[x][y]){
                topLines.append(QLineF(v.toPointF(), v1.toPointF()));
            }
        }
    }else{
        return QVector2D();
    }
    QVector2D mean_heading;
    for(QLineF line : topLines){
        mean_heading += QVector2D(line.dx(), line.dy());
    }
    mean_heading /= topLines.length();
    return mean_heading;
}

QVector3D chessboard::cv_mat2qvec3d(cv::Mat m)
{
    if(m.rows == 3 && m.cols == 1 && m.type() == CV_64FC1){
        QVector3D v;
        v.setX(m.at<double>(0, 0));
        v.setY(m.at<double>(1, 0));
        v.setZ(m.at<double>(2, 0));
        return v;
    }else{
        return QVector3D();
    }
}

int chessboard::map(int x, int in_min, int in_max, int out_min, int out_max)
{
    if(in_max - in_min == 0){
        return x;
    }
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float chessboard::map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool chessboard::add_field(QVector2D local_offset, QPointF tl_corner, QPointF tr_corner, QPointF bl_corner, QPointF br_corner, QPointF center, bool color)
{
    if(local_offset.x() > 7 || local_offset.x() < -7 || local_offset.y() > 7  || local_offset.y() < -7){
        qDebug() << "Offset out of Range";
        return false;
    }
    max_x = (max_x < local_offset.x()) ? local_offset.x() : max_x;
    min_x = (min_x > local_offset.x()) ? local_offset.x() : min_x;
    max_y = (max_y < local_offset.y()) ? local_offset.y() : max_y;
    min_y = (min_y > local_offset.y()) ? local_offset.y() : min_y;
    if((max_x - min_x) > 8 || (max_y - min_y) > 8){
        qDebug() << "Error in Indexes";
        return false;
    }
    /*
    qDebug() << "MinX " << min_x;
    qDebug() << "MinY " << min_y;
    qDebug() << "MaxX " << max_x;
    qDebug() << "MaxY " << max_y;
    */
    int x_index = local_offset.x();
    int y_index = local_offset.y();
    if(x_index < 0){
        x_index = 8 + x_index;
    }
    if(y_index < 0){
        y_index = 8 + y_index;
    }
    if(colors[x_index][y_index] != chessboard::UNDEFINED && colors[x_index][y_index] != color){
        qDebug() << "Suspicious coloring";
        return false;
    }
    corners[x_index][y_index + 1].push_back(QVector2D(tl_corner));
    corners[x_index + 1][y_index + 1].push_back(QVector2D(tr_corner));
    corners[x_index][y_index].push_back(QVector2D(bl_corner));
    corners[x_index + 1][y_index].push_back(QVector2D(br_corner));
    
    colors[x_index][y_index] = color;
    
    centers[x_index][y_index].push_back(QVector2D(center));
    return true;
}

bool chessboard::get_color(char letter, char number)
{
    QPoint p = map_koords_to_index(letter, number);
    if(p.x() == -1 || p.y() == -1){
        return chessboard::UNDEFINED;
    }
    return colors[p.x()][p.y()];
}

void chessboard::drawBoard(cv::Mat image)
{
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(!corners[i][j].empty()){
                for(QVector2D v : corners[i][j]){
                    cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                }
            }
            if(!corners[i + 1][j].empty()){
                for(QVector2D v : corners[i + 1][j]){
                    cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                }
            }
            if(!corners[i][j + 1].empty()){
                for(QVector2D v : corners[i][j + 1]){
                    cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                }
            }
            if(!corners[i + 1][j + 1].empty()){
                for(QVector2D v : corners[i + 1][j + 1]){
                    cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                }
            }
            if(!centers[i][j].empty()){
                QString name;
                name.append(QString::number(i));
                name.append(", ");
                name.append(QString::number(j));
                for(QVector2D v : centers[i][j]){
                    //cv::putText(image, name.toStdString(), qvec2d2cv_point2f(v) - cv::Point2f(20, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                }
                for(QVector2D v : centers[i][j]){
                    if(colors[i][j] == chessboard::BLACK){
                        cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(255, 255, 255), 3, cv::LINE_8);
                    }else if(colors[i][j] == chessboard::WHITE){
                        cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(0, 0, 0), 3, cv::LINE_8);
                    }else{
                        cv::circle(image, qvec2d2cv_point2f(v), 5, cv::Scalar(0, 0, 255), 3, cv::LINE_8);
                    }
                }
            }
        }
    }
    QPointF a = get_board_corner(chessboard::TOP_LEFT_CORNER);
    QPointF b = get_board_corner(chessboard::BOTTOM_LEFT_CORNER);
    QPointF c = get_board_corner(chessboard::TOP_RIGHT_CORNER);
    QPointF d = get_board_corner(chessboard::BOTTOM_RIGHT_CORNER);
    drawRect(image, a, b, c, d);
}

void chessboard::drawRect(cv::Mat image, int x, int y)
{
    drawRect(image, corners[x][y + 1][0].toPointF(), corners[x][y][0].toPointF(), corners[x + 1][y + 1][0].toPointF(), corners[x + 1][y][0].toPointF());
}

void chessboard::drawRect(cv::Mat image, QPointF a, QPointF b, QPointF c, QPointF d)
{
    cv::line(image, qpoint2f2cv_point2f(b), qpoint2f2cv_point2f(d), cv::Scalar(0, 0, 255), 3, cv::LINE_8);
    cv::line(image, qpoint2f2cv_point2f(b), qpoint2f2cv_point2f(a), cv::Scalar(0, 0, 255), 3, cv::LINE_8);
    cv::line(image, qpoint2f2cv_point2f(d), qpoint2f2cv_point2f(c), cv::Scalar(0, 0, 255), 3, cv::LINE_8);
    cv::line(image, qpoint2f2cv_point2f(a), qpoint2f2cv_point2f(c), cv::Scalar(0, 0, 255), 3, cv::LINE_8);
}

QVector2D chessboard::get_corner_by_indizes(int x, int y)
{
    return corners[x][y][0];
}

QPointF chessboard::get_board_corner(const int CORNER_CODE)
{
    QPair<int, int> indizes = get_board_corner_indizes(CORNER_CODE);
    if(indizes.first < 0 || indizes.second < 0){
        return QPointF();
    }else{
        return corners[indizes.first][indizes.second][0].toPointF();
    }
}

QPair<int, int> chessboard::get_board_corner_indizes(const int CORNER_CODE)
{
    QPair<int, int> pair = get_board_corner_center_indizes(CORNER_CODE);
    int x = pair.first, y = pair.second;
    switch(CORNER_CODE){
        case chessboard::TOP_LEFT_CORNER:
            return QPair<int, int>(x, y + 1);
        case chessboard::TOP_RIGHT_CORNER:
            return QPair<int, int>(x + 1, y + 1);
        case chessboard::BOTTOM_RIGHT_CORNER:
            return QPair<int, int>(x + 1, y);
        case chessboard::BOTTOM_LEFT_CORNER:
            return QPair<int, int>(x, y);
        default:
            return QPair<int, int>(-1, -1);
    }
}

QPair<int, int> chessboard::get_board_corner_center_indizes(const int CORNER_CODE)
{
    int tlX = min_x, tlY = max_y;
    int trX = max_x, trY = max_y;
    int brX = max_x, brY = min_y;
    int blX = min_x, blY = min_y;
    switch(CORNER_CODE){
        case chessboard::TOP_LEFT_CORNER:
            tlX = (tlX < 0) ? (8 + tlX) : tlX;
            tlY = (tlY > 8) ? (tlY % 8) : tlY;
            return QPair<int, int>(tlX, tlY);
        case chessboard::TOP_RIGHT_CORNER:
            trX = (trX > 8) ? (trX % 8) : trX;
            trY = (trY > 8) ? (trY % 8) : trY;
            return QPair<int, int>(trX, trY);
        case chessboard::BOTTOM_RIGHT_CORNER:
            brX = (brX > 8) ? (brX % 8) : brX;
            brY = (brY < 0) ? (8 + brY) : brY;
            return QPair<int, int>(brX, brY);
        case chessboard::BOTTOM_LEFT_CORNER:
            blX = (blX < 0) ? (8 + blX) : blX;
            blY = (blY < 0) ? (8 + blY) : blY;
            return QPair<int, int>(blX, blY);
        default:
            return QPair<int, int>(-1, -1);
    }
}

QVector2D chessboard::get_origin()
{
    return QVector2D(0, 0);
}

QVector3D chessboard::get_origin_normal()
{
    cv::Mat H = get_rotation_matrix();
    QVector3D a = cv_mat2qvec3d(H * qvec2d2cv_mat(corners[0][1][0], CV_64FC1));
    QVector3D b = cv_mat2qvec3d(H * qvec2d2cv_mat(corners[0][0][0], CV_64FC1));
    QVector3D d = cv_mat2qvec3d(H * qvec2d2cv_mat(corners[1][0][0], CV_64FC1));
    return QVector3D::normal(a - b, d - b);
}

QPoint chessboard::get_origin_index()
{
    return map_index_to_koords(0, 0);
}

QVector2D chessboard::get_center(char letter, char number)
{
    QPoint p = map_koords_to_index(letter, number);
    if(p.x() == -1 || p.y() == -1){
        return QVector2D(-1, -1);
    }
    return centers[p.x()][p.y()].at(0);
}

cv::Mat chessboard::get_homography_matrix()
{
    cv::Point2f dstPoints[4];
    dstPoints[0].x = -0.5; dstPoints[0].y = 0.5;
    dstPoints[1].x = 0.5; dstPoints[1].y = 0.5;
    dstPoints[2].x = 0.5; dstPoints[2].y = -0.5;
    dstPoints[3].x = -0.5; dstPoints[3].y = -0.5;
    cv::Mat homographyMatrix(cv::Size(3, 3), CV_64FC1);
    cv::Point2f targetCorners[4];
    targetCorners[0] = qvec2d2cv_point2f(corners[0][1][0]);
    targetCorners[1] = qvec2d2cv_point2f(corners[1][1][0]);
    targetCorners[2] = qvec2d2cv_point2f(corners[1][0][0]);
    targetCorners[3] = qvec2d2cv_point2f(corners[0][0][0]);
    homographyMatrix = cv::getPerspectiveTransform(targetCorners, dstPoints);
    return homographyMatrix;
}

cv::Mat chessboard::get_homography_matrix_boardCorners(cv::Size areaOI)
{
    return get_rotation_matrix(qpoint2f2cv_point2f(get_board_corner(TOP_LEFT_CORNER)), qpoint2f2cv_point2f(get_board_corner(TOP_RIGHT_CORNER)),
                               qpoint2f2cv_point2f(get_board_corner(BOTTOM_LEFT_CORNER)), qpoint2f2cv_point2f(get_board_corner(BOTTOM_RIGHT_CORNER)), areaOI);
}

QPair<QMatrix3x3, QVector3D> chessboard::get_rotation_translation()
{
    qDebug() << "Starting Translation Row";
    for(int j = 0; j < 8; j++){
        for(int i = 0; i < 8; i++){
            get_rotation_translation(qpoint2f2cv_point2f(corners[i][j + 1][0].toPointF()),
                                     qpoint2f2cv_point2f(corners[i + 1][j + 1][0].toPointF()),
                                     qpoint2f2cv_point2f(corners[i + 1][j][0].toPointF()),
                                     qpoint2f2cv_point2f(corners[i][j][0].toPointF()),
                                     cv::Size(200, 200));
        }
    }
    qDebug() << "Ending Translation Row";
    return get_rotation_translation(qpoint2f2cv_point2f(get_board_corner(TOP_LEFT_CORNER)), qpoint2f2cv_point2f(get_board_corner(TOP_RIGHT_CORNER)),
                                    qpoint2f2cv_point2f(get_board_corner(BOTTOM_LEFT_CORNER)), qpoint2f2cv_point2f(get_board_corner(BOTTOM_RIGHT_CORNER)), cv::Size(200, 200));
}

QPair<QMatrix3x3, QVector3D> chessboard::get_rotation_translation(cv::Point2f tl, cv::Point2f tr, cv::Point2f br, cv::Point2f bl, cv::Size areaOI)
{
    cv::Mat K(cv::Size(3, 3), CV_64FC1);    //Intrinsic Camera Parameters
    cv::Mat H = get_rotation_matrix(tl, tr, br, bl, areaOI);
    QVector3D colX(H.at<double>(0, 0), H.at<double>(1, 0), H.at<double>(2, 0));
    QVector3D colY(H.at<double>(0, 1), H.at<double>(1, 1), H.at<double>(2, 1));
    QVector3D colZ(H.at<double>(0, 2), H.at<double>(1, 2), H.at<double>(2, 2));
    QMatrix4x4 qH;
    qH.setColumn(0, colX);
    qH.setColumn(1, colY);
    qH.setColumn(2, colZ);
    float lense_focal_length = 634.0;
    float fMarkerSize = 0.032;
    const double fScaleLeft[3] = { 1.0f / lense_focal_length, 1.0f / lense_focal_length, -1.0f };
    const double fScaleRight[3] = { 1.0f / fMarkerSize, 1.0f / fMarkerSize, 1.0f };
    // R = C^-1 H S^-1
    QMatrix4x4 qK;
    qK.setColumn(0, QVector4D(1.0f / lense_focal_length, 0, 0, 0));
    qK.setColumn(1, QVector4D(0, 1.0f / lense_focal_length, 0, 0));
    qK.setColumn(2, QVector4D(0, 0, 1, 0));
    qK.setColumn(3, QVector4D(0, 0, 0, 1));
    double lense_times_marker = (1.0f / lense_focal_length) * (1.0f / fMarkerSize);
    QVector3D col0(lense_times_marker, lense_times_marker, -1.0f / fMarkerSize);
    QVector3D col1(1.0f / lense_focal_length, 1.0f / lense_focal_length, -1.0f);
    colX *= col0;
    colY *= col0;
    colZ *= col1;
    if(colZ.z() > 0.0f){
        colX *= -1;
        colY *= -1;
        colZ *= -1;
    }
    double fXLenVec = colX.length();
    double fYLenVec = colY.length();
    double fTransScaleVec = 2.0f / (fXLenVec + fYLenVec);
    QVector3D translationVec = colZ * fTransScaleVec;
    colX *= (1.0f / fXLenVec);
    colY *= (1.0f / fYLenVec);
    colZ = QVector3D::crossProduct(colX, colY);
    double fZLenVec = colZ.length();
    colZ *= (1.0f / fZLenVec);
    colY = QVector3D::crossProduct(colX, colZ);
    colY *= -1.0;
    QMatrix4x4 bigRotMat;
    bigRotMat.setColumn(0, colX.toVector4D());
    bigRotMat.setColumn(1, colY.toVector4D());
    bigRotMat.setColumn(2, colZ.toVector4D());
    QMatrix3x3 rotMat = bigRotMat.toGenericMatrix<3, 3>();
    QPointF a = get_board_corner(TOP_LEFT_CORNER);
    QPointF b = get_board_corner(BOTTOM_LEFT_CORNER);
    QPointF d = get_board_corner(BOTTOM_RIGHT_CORNER);
    QPointF c = get_board_corner(TOP_RIGHT_CORNER);
    QVector3D qa(a);
    QVector3D qb(b);
    QVector3D qc(c);
    QVector3D qd(d);
    qa.setZ(1);
    qb.setZ(1);
    qc.setZ(1);
    qd.setZ(1);
    //qDebug() << "2d QA " << qa;
    //qDebug() << "2d QB " << qb;
    //qDebug() << "2d QC " << qc;
    //qDebug() << "2d QD " << qd;
    //qDebug() << "H Invertable " << (qH.determinant() != 0);
    if(qH.determinant() != 0){
        //qDebug() << "3d QA " << (qa * qK.inverted()) * qH.inverted();
        //qDebug() << "3d QB " << (qb * qK.inverted()) * qH.inverted();
        //qDebug() << "3d QC " << (qc * qK.inverted()) * qH.inverted();
        //qDebug() << "3d QD " << (qd * qK.inverted()) * qH.inverted();
        QVector3D qa3d = qa * qK.inverted() * qH.inverted();
        QVector3D qb3d = qb * qK.inverted() * qH.inverted();
        QVector3D qc3d = qc * qK.inverted() * qH.inverted();
        QVector3D qd3d = qd * qK.inverted() * qH.inverted();
        QVector3D center = qa3d + ((qd3d - qa3d) / 2.0);
        //qDebug() << "Center 3D Coord " << center;
        //qDebug() << "Center 2D Coord " << qK * qH * center;
        qDebug() << "Trans " << translationVec;
        //translationVec += center;
    }

    /*
    QLineF diagA(get_board_corner(TOP_LEFT_CORNER), get_board_corner(BOTTOM_RIGHT_CORNER));
    QLineF diagB(get_board_corner(TOP_RIGHT_CORNER), get_board_corner(BOTTOM_LEFT_CORNER));
    QPointF center;
    if(QLineF::NoIntersection != diagA.intersect(diagB, &center)){
        QQuaternion q = QQuaternion::fromRotationMatrix(rotMat);
        translationVec = q.rotatedVector(QVector3D(center.x(), center.y(), 1));
    }
    */
    return QPair<QMatrix3x3, QVector3D>(rotMat, translationVec);
}

cv::Mat chessboard::get_rotation_matrix(cv::Point2f tl, cv::Point2f tr, cv::Point2f br, cv::Point2f bl, cv::Size areaOI)
{
    cv::Point2f dstPoints[4];
    /*
    dstPoints[0].x = 0; dstPoints[0].y = 0;
    dstPoints[1].x = 600; dstPoints[1].y = 0;
    dstPoints[2].x = 0; dstPoints[2].y = 600;
    dstPoints[3].x = 600; dstPoints[3].y = 600;
    */

    dstPoints[0].x = 0; dstPoints[0].y = 0;
    dstPoints[1].x = areaOI.width; dstPoints[1].y = 0;
    dstPoints[2].x = 0; dstPoints[2].y = areaOI.height;
    dstPoints[3].x = areaOI.width; dstPoints[3].y = areaOI.height;
    /*
    double w = areaOI.width / 2.0;
    double h = areaOI.height / 2.0;
    dstPoints[0].x = -w; dstPoints[0].y = -h;
    dstPoints[1].x = w; dstPoints[1].y = -h;
    dstPoints[2].x = -w; dstPoints[2].y = h;
    dstPoints[3].x = w; dstPoints[3].y = h;
    */
    cv::Mat homographyMatrix(cv::Size(3, 3), CV_32FC1);
    cv::Point2f srcCorners[4];
    srcCorners[0] = tl;
    srcCorners[1] = tr;
    srcCorners[2] = br;
    srcCorners[3] = bl;
    homographyMatrix = cv::getPerspectiveTransform(srcCorners, dstPoints);
    qDebug() << "Homography Mat";
    return homographyMatrix;
}

cv::Mat chessboard::get_rotation_matrix()
{
    return get_rotation_matrix(qvec2d2cv_point2f(corners[0][1][0]), qvec2d2cv_point2f(corners[1][1][0]),
            qvec2d2cv_point2f(corners[1][0][0]), qvec2d2cv_point2f(corners[0][0][0]), cv::Size(200, 200));
}

QQuaternion chessboard::get_rotation_matrix(bool placeholder)
{
    cv::Mat homographyMatrix = get_rotation_matrix();
    QMatrix4x4 rot_mat;
    QVector4D row1(homographyMatrix.at<float>(0, 0), homographyMatrix.at<float>(0, 1), homographyMatrix.at<float>(0, 2), 0);
    QVector4D row2(homographyMatrix.at<float>(1, 0), homographyMatrix.at<float>(1, 1), homographyMatrix.at<float>(1, 2), 0);
    QVector4D row3(homographyMatrix.at<float>(2, 0), homographyMatrix.at<float>(2, 1), homographyMatrix.at<float>(2, 2), 0);
    QVector4D row4(0, 0, 0, 1);
    rot_mat.setRow(0, row1);
    rot_mat.setRow(1, row2);
    rot_mat.setRow(2, row3);
    rot_mat.setRow(3, row4);
    QGenericMatrix<3, 3, float> mat3 = rot_mat.toGenericMatrix<3, 3>();
    qDebug() << "Homography Mat " << rot_mat;
    QQuaternion rot = QQuaternion::fromRotationMatrix(mat3);

    rot.normalize();

    return rot;
}

QQuaternion chessboard::cv_mat2qquaternion(cv::Mat rot_mat)
{
    if(rot_mat.rows != 3 || rot_mat.cols != 3){
        qDebug() << "Rows colums not matching";
        return QQuaternion();
    }
    QMatrix4x4 qrot_mat;
    QVector4D row1(rot_mat.at<float>(0, 0), rot_mat.at<float>(0, 1), rot_mat.at<float>(0, 2), 0);
    QVector4D row2(rot_mat.at<float>(1, 0), rot_mat.at<float>(1, 1), rot_mat.at<float>(1, 2), 0);
    QVector4D row3(rot_mat.at<float>(2, 0), rot_mat.at<float>(2, 1), rot_mat.at<float>(2, 2), 0);
    QVector4D row4(0, 0, 0, 1);
    qrot_mat.setRow(0, row1);
    qrot_mat.setRow(1, row2);
    qrot_mat.setRow(2, row3);
    qrot_mat.setRow(3, row4);
    QGenericMatrix<3, 3, float> mat3 = qrot_mat.toGenericMatrix<3, 3>();
    qDebug() << "Homography Mat " << qrot_mat;
    QQuaternion rot = QQuaternion::fromRotationMatrix(mat3);
    rot.normalize();
    qDebug() << "Quaterion " << rot;
    return rot;
}

QPoint chessboard::map_koords_to_index(char letter, char number)
{
    int y_index = 0;
    int x_index = 0;
    if(letter >= 65 && letter < 73){
        x_index = letter % 65;
        if(number >= 0 && number < 8){
            y_index = number;
        }else if(number >= 48 && number < 56){
            y_index = number % 48;
        }else{
            goto GET_CENTER_END;
        }
        x_index = ((min_x < 0 ? 8 - min_x : min_x) + x_index) % 8;
        y_index = ((min_y < 0 ? 8 - min_y : min_y) + y_index) % 8;
        return QPoint(x_index, y_index);
    }
    GET_CENTER_END:
    return QPoint(-1, -1);
}

QPoint chessboard::map_index_to_koords(int x, int y)
{
    if(x < 0 || x > 7 || y < 0 || y > 7){
        return QPoint(-1, -1);
    }
    char x_index = min_x + x;
    char y_index = min_y + y;
    if(x_index < 0){
        x_index = 8 - x_index;
    }
    if(y_index < 0){
        y_index = 8 - y_index;
    }
    return QPoint((x_index % 8) + 65, y_index % 8);
}

QVector2D chessboard::mean_vec(QList<QVector2D> list)
{
    QVector2D v(0, 0);
    for(QVector2D p : list){
        v += p;
    }
    return (v / list.length());
}

chessboard::chessboard(QObject *parent) : QObject(parent)
{}

chessboard::chessboard(const chessboard &board)
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if(i < 8 && j < 8){
                this->centers[i][j].append(board.centers[i][j]);
                this->colors[i][j] = board.colors[i][j];
            }
            this->corners[i][j].append(board.corners[i][j]);
        }
    }
    this->min_x = board.min_x;
    this->max_x = board.max_x;
    this->min_y = board.min_y;
    this->max_y = board.max_y;
    this->origin_index = board.origin_index;
}
