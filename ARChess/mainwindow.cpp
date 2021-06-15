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
    scene_3d = ui->openGLWidget3D;
    QString version_text;
    version_text = "OpenCV: ";
    version_text.append(CV_VERSION);
    openvc_version->setText(version_text);
    camera_box->addItem(EXAMPLE_VIDEO);
    cam_control = new  camera_controller();
    qDebug() << "init";
    connect(cam_control->worker, &camera_worker::camera_detected, this, &MainWindow::add_camerabox_item);
    connect(cam_control->worker, &camera_worker::image_ready, this, &MainWindow::receive_capture);
    qDebug() << "connected";
    cam_control->init();
    cam_control->start_capture();
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
    image_plane->setPixmap(img.scaled(300, 200, Qt::KeepAspectRatio));
    image_plane->update();
}


void MainWindow::on_pushButton_clicked()
{
    cam_control->init();
}

