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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    openvc_version = ui->openvc_version_label;
    camera_box = ui->camera_combbox;
    image_plane = ui->image_label;
    threshold_method_box = ui->threshold_method_ui;
    scene_3d = ui->openGLWidget3D;
    threshold_slider = ui->threshold_slider_id;
    QString version_text;
    version_text = "OpenCV: ";
    version_text.append(CV_VERSION);
    openvc_version->setText(version_text);
    cam_control = new  camera_controller();
    qDebug() << "init";
    connect(cam_control->worker, &camera_worker::camera_detected, this, &MainWindow::add_camerabox_item);
    connect(cam_control->worker, &camera_worker::image_ready, this, &MainWindow::receive_capture);
    qDebug() << "connected";
    cam_control->init();
    cam_control->start_capture();
    camera_box->addItem(EXAMPLE_VIDEO);
    threshold_method_box->insertItem(0, "Thresh_Binary");
    threshold_method_box->insertItem(1, "Thresh_Binary_Inv");
    threshold_method_box->insertItem(2, "Thresh_Trunc");
    threshold_method_box->insertItem(3, "Thresh_ToZero");
    threshold_method_box->insertItem(4, "Thresh_ToZero_Inv");
    threshold_method_box->setCurrentIndex(0);
    cam_control->worker->change_threshold_method(0);
    cam_control->worker->change_threshold(threshold_slider->value());
    //scene_3d->
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::mat_to_qimage_ref(cv::Mat &mat, QImage::Format format) {
    return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

void MainWindow::add_camerabox_item(QString item_name){
    camera_box->addItem(item_name);
}

void MainWindow::on_camera_combbox_currentIndexChanged(int index)
{
    image_plane->setText("Loading other Camera...");
    QString cam_name = camera_box->itemText(index);
    qDebug() << "Change Camera to " << index << " " << cam_name;
    int cv_index = 0;
    if(cam_name.contains(EXAMPLE_VIDEO)){
        cv_index = -1;
    }else{
        cv_index = cam_name.remove("Camera ", Qt::CaseInsensitive).toInt();
    }
    cam_control->worker->change_camera(cv_index);
}

void MainWindow::receive_capture(QPixmap img)
{
    image_plane->setPixmap(img.scaled(1024, 740, Qt::KeepAspectRatio));
    image_plane->update();
}


void MainWindow::on_pushButton_clicked()
{
    cam_control->init();
}


void MainWindow::on_threshold_slider_id_valueChanged(int value)
{
    qDebug() << "Threshold Value changed to " << value;
    cam_control->worker->change_threshold(value);

}

void MainWindow::on_threshold_method_ui_currentIndexChanged(int index)
{
    qDebug() << "Threshold Method changed " << index;
    cam_control->worker->change_threshold_method(index);
}

