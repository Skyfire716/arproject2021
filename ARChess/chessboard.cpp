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

cv::Point2f chessboard::qvec2d2cv_point2f(QVector2D v)
{
    return cv::Point2f(v.x(), v.y());
}

cv::Point2f chessboard::qpoint2cv_point2f(QPoint p)
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
    if(max_x - min_x > 8 || max_y - min_y > 8){
        qDebug() << "Error in Indexes";
        return false;
    }
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
    qDebug() << "Adding " << local_offset << " with center " << center << " at " << x_index << " " << y_index;
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
    cv::Mat homographyMatrix(cv::Size(3, 3), CV_32FC1);
    cv::Point2f targetCorners[4];
    targetCorners[0] = qvec2d2cv_point2f(corners[0][1][0]);
    targetCorners[1] = qvec2d2cv_point2f(corners[1][1][0]);
    targetCorners[2] = qvec2d2cv_point2f(corners[1][0][0]);
    targetCorners[3] = qvec2d2cv_point2f(corners[0][0][0]);
    homographyMatrix = cv::getPerspectiveTransform(targetCorners, dstPoints);
    return homographyMatrix;
}

QPair<cv::Mat, cv::Mat> chessboard::get_rotation_translation()
{
    cv::Mat K(cv::Size(3, 3), CV_64FC1);    //Intrinsic Camera Parameters
    cv::Mat rotation(cv::Size(3, 3), CV_64FC1);
    cv::Mat translation(cv::Size(3, 1), CV_64FC1);
    cv::Mat H = get_homography_matrix();
    cv::Mat Kinv = K.inv();
    double norm = sqrt(H.at<double>(0,0)*H.at<double>(0,0) +
                       H.at<double>(1,0)*H.at<double>(1,0) +
                       H.at<double>(2,0)*H.at<double>(2,0));
    H /= norm;
    cv::Mat c1  = H.col(0);
    cv::Mat c2  = H.col(1);
    cv::Mat c3 = c1.cross(c2);
    translation = H.col(2);
    for (int i = 0; i < 3; i++)
    {
        rotation.at<double>(i,0) = c1.at<double>(i,0);
        rotation.at<double>(i,1) = c2.at<double>(i,0);
        rotation.at<double>(i,2) = c3.at<double>(i,0);
    }
    /*
    cv::Mat W, U, Vt;
    cv::SVDecomp(rotation, W, U, Vt);
    rotation = U*Vt;
    */
    QPair<cv::Mat, cv::Mat> pair(rotation, translation);
    return pair;
}

cv::Mat chessboard::get_rotation_matrix()
{
    cv::Point2f dstPoints[4];
    dstPoints[0].x = -0.5; dstPoints[0].y = 0.5;
    dstPoints[1].x = 0.5; dstPoints[1].y = 0.5;
    dstPoints[2].x = 0.5; dstPoints[2].y = -0.5;
    dstPoints[3].x = -0.5; dstPoints[3].y = -0.5;
    cv::Mat homographyMatrix(cv::Size(3, 3), CV_32FC1);
    cv::Point2f targetCorners[4];
    targetCorners[0] = qvec2d2cv_point2f(corners[0][1][0]);
    targetCorners[1] = qvec2d2cv_point2f(corners[1][1][0]);
    targetCorners[2] = qvec2d2cv_point2f(corners[1][0][0]);
    targetCorners[3] = qvec2d2cv_point2f(corners[0][0][0]);
    homographyMatrix = cv::getPerspectiveTransform(targetCorners, dstPoints);
    qDebug() << "Homography Mat";
    return homographyMatrix;
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
}
