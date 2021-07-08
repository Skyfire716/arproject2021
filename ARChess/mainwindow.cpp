#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QPixmap>
#include <QSize>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "camera_controller.h"
#include "camera_worker.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    openvc_version = ui->openvc_version_label;
    camera_box = ui->camera_combbox;
    result_image_picker = ui->result_image_box;
    threshold_method_box = ui->threshold_method_ui;
    threshold_slider = ui->threshold_slider_id;
    threshold_value_label = ui->threshold_value_label_ui;
    arwidget = ui->widget3d;
    QString version_text;
    version_text = "OpenCV: ";
    version_text.append(CV_VERSION);
    openvc_version->setText(version_text);
    cam_control = new  camera_controller();
    qDebug() << "init";
    connect(cam_control->worker, &camera_worker::camera_detected, this, &MainWindow::add_camerabox_item);
    connect(cam_control->worker, &camera_worker::image_ready, arwidget->planeTextureImage, &archessbackgound::receive_image);
    connect(cam_control->worker, &camera_worker::chessboard_updated, this, &MainWindow::chessboard_updated);
    connect(cam_control->worker, &camera_worker::new_ar_rotation, arwidget, &archesswidget::ar_rotation);
    connect(cam_control->worker, &camera_worker::new_ar_transform, arwidget, &archesswidget::transform_update);
    qDebug() << "connected";
    cam_control->init();
    cam_control->start_capture();
    camera_box->addItem(EXAMPLE_VIDEO_STABEL);
    camera_box->addItem(EXAMPLE_VIDEO_MOVING);
    camera_box->addItem(EXAMPLE_VIDEO);
    threshold_method_box->insertItem(0, "Thresh_Binary");
    threshold_method_box->insertItem(1, "Thresh_Binary_Inv");
    threshold_method_box->insertItem(2, "Thresh_Trunc");
    threshold_method_box->insertItem(3, "Thresh_ToZero");
    threshold_method_box->insertItem(4, "Thresh_ToZero_Inv");
    threshold_method_box->insertItem(5, "Adaptive");
    threshold_method_box->setCurrentIndex(0);
    result_image_picker->insertItem(0, "Camera Image");
    result_image_picker->insertItem(1, "Gray Image");
    result_image_picker->insertItem(2, "Threshold Image");
    result_image_picker->setCurrentIndex(0);
    cam_control->worker->change_threshold_method(0);
    cam_control->worker->change_threshold(threshold_slider->value());
    //Individual Values for testing Today
    threshold_slider->setValue(140);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::mat_to_qimage_ref(cv::Mat &mat, QImage::Format format) {
    return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

QString MainWindow::r(QRect r)
{
    QString s;
    s.append(QString::number(r.topLeft().x()));
    s.append(", ");
    s.append(QString::number(r.topLeft().y()));
    s.append(", ");
    s.append(MainWindow::r(r.size()));
    return s;
}

QString MainWindow::r(QSize s)
{
    QString str;
    str.append("w ");
    str.append(QString::number(s.width()));
    str.append(" h ");
    str.append(QString::number(s.height()));
    return  str;;
}

void MainWindow::add_camerabox_item(QString item_name){
    camera_box->addItem(item_name);
}

void MainWindow::on_camera_combbox_currentIndexChanged(int index)
{
    QString cam_name = camera_box->itemText(index);
    qDebug() << "Change Camera to " << index << " " << cam_name;
    int cv_index = 0;
    if(cam_name.contains(EXAMPLE_VIDEO)){
        cv_index = -1;
    }else if(cam_name.contains(EXAMPLE_VIDEO_STABEL)){
        cv_index = -2;
    }else if(cam_name.contains(EXAMPLE_VIDEO_MOVING)){
        cv_index = -3;
    }else{
        cv_index = cam_name.remove("Camera ", Qt::CaseInsensitive).toInt();
    }
    cam_control->worker->change_camera(cv_index);
}


void MainWindow::on_pushButton_clicked()
{
    cam_control->init();
}


void MainWindow::on_threshold_slider_id_valueChanged(int value)
{
    qDebug() << "Threshold Value changed to " << value;
    cam_control->worker->change_threshold(value);
    if(threshold_value_label){
        threshold_value_label->setText(QString::number(value));
    }
}

void MainWindow::on_threshold_method_ui_currentIndexChanged(int index)
{
    qDebug() << "Threshold Method changed " << index;
    cam_control->worker->change_threshold_method(index);
}

void MainWindow::chessboard_updated(QPixmap pix)
{
    openvc_version->setPixmap(pix);
}

void MainWindow::on_result_image_box_currentIndexChanged(int index)
{
    qDebug() << "Changing Result Image to " << index;
    cam_control->worker->change_result_image(index);
}

