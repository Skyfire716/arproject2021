#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDebug>
#include <QCamera>
#include <QCameraInfo>
#include <QPixmap>
#include <QSize>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    openvc_version = ui->openvc_version_label;
    camera_box = ui->camera_combbox;
    image_plane = ui->image_label;
    color_box = ui->color_type_combbox;
    color_box->addItems({"Format_Invalid",
                         "Format_Mono",
                         "Format_MonoLSB",
                         "Format_Indexed8",
                         "Format_RGB32",
                         "Format_ARGB32",
                         "Format_ARGB32_Premultiplied",
                         "Format_RGB16",
                         "Format_ARGB8565_Premultiplied",
                         "Format_RGB666",
                         "Format_ARGB6666_Premultiplied",
                         "Format_RGB555",
                         "Format_ARGB8555_Premultiplied",
                         "Format_RGB888",
                         "Format_RGB444",
                         "Format_ARGB4444_Premultiplied",
                         "Format_RGBX8888",
                         "Format_RGBA8888",
                         "Format_RGBA8888_Premultiplied",
                         "Format_BGR30",
                         "Format_A2BGR30_Premultiplied",
                         "Format_RGB30",
                         "Format_A2RGB30_Premultiplied",
                         "Format_Alpha8",
                         "Format_Grayscale8",
                         "Format_Grayscale16",
                         "Format_RGBX64",
                         "Format_RGBA64",
                         "Format_RGBA64_Premultiplied",
                         "Format_BGR888"});
    color_box->setCurrentIndex(13);
    running = ui->running_box;
    QString version_text;
    version_text = "OpenCV: ";
    version_text.append(CV_VERSION);
    openvc_version->setText(version_text);
    cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cameras) {
        camera_box->addItem(cameraInfo.deviceName());
        camera_select_map.insert(camera_box->count(), cameraInfo.deviceName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_camera_combbox_currentIndexChanged(int index)
{
    qDebug() << index << " selected";
    qDebug() << cameras[index].deviceName();
    active_camera = new QCamera(cameras[index]);
    active_camera->load();

    connect(active_camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(camera_state(QCamera::State)));
    connect(active_camera, SIGNAL(statusChanged(QCamera::Status)), this, SLOT(camera_status(QCamera::Status)));

    qDebug() << active_camera->viewfinderSettings().resolution().width() << "x" << active_camera->viewfinderSettings().resolution().height();
    for (QSize size : active_camera->supportedViewfinderResolutions()){
        qDebug() << "Supported Size " << size.width() << " x " << size.height();
    }
    active_camera->setCaptureMode(QCamera::CaptureMode::CaptureViewfinder);


 }

void MainWindow::camera_captured_image(int id, const QImage &preview)
{
    qDebug() << "Got Image " << id; //Should be called after imageCapture->Capture()
    /*
    preview.map(QAbstractVideoBuffer::ReadOnly);
     QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(preview.pixelFormat());
     QImage img( preview.bits(),
                  preview.width(),
                  preview.height(),
                  preview.bytesPerLine(),
                  imageFormat);
    QPixmap pixmap = QPixmap::fromImage(img);
    image_plane->setPixmap(pixmap);
    */
}

void MainWindow::camera_available_image(int id, const QVideoFrame &frame)
{
    qDebug() << "Got Frame " << id << " " << frame.size().width() << "x" << frame.size().height() << " " << frame.pixelFormat();  //Should be called after imageCapture->Capture()
    QImage img;
    QVideoFrame buffer(frame);
    if(buffer.map(QAbstractVideoBuffer::ReadOnly) && running->isChecked()){
        qDebug() << buffer.handleType() << " Handel Type";
        qDebug() << buffer.bytesPerLine() << " Bytes Per Line";
        cv::Mat frameMat = cv::Mat(frame.size().width(), frame.size().height(), CV_8UC2);
        for (int i = 0; i < frame.size().height(); i++){
            memcpy(frameMat.data + (i * buffer.bytesPerLine()), buffer.bits() + (i * buffer.bytesPerLine()), buffer.bytesPerLine());
        }
        cv::Mat bgrframe = cv::Mat(frame.size().width(), frame.size().height(), CV_8UC3);
        cv::Mat bgrframegray = cv::Mat(frame.size().width(), frame.size().height(), CV_8UC3);
        qDebug() << "Cvt Color";
        cv::cvtColor(frameMat, bgrframegray, cv::COLOR_YUV2BGR_UYVY, 3);
        cv::threshold(bgrframegray, bgrframe, 70, 255, 3);
        //cv::adaptiveThreshold(bg)
        cv::Mat gray = cv::Mat(frame.size().width(), frame.size().height(), CV_8UC3);
        qDebug() << "Create QImg";
        img = QImage(bgrframe.cols, bgrframe.rows, format);
        for(int i = 0; i< bgrframe.rows; i++){
            memcpy(img.bits() + (i*bgrframe.step), bgrframe.data + (i*bgrframe.step), bgrframe.step);
        }
        qDebug() << "Done";
        buffer.unmap();
    }
    qDebug() << "Pixmap";
    QPixmap pixmap = QPixmap::fromImage(img);
    // pixmap.scaledToWidth(500, Qt::FastTransformation);
    image_plane->setPixmap(pixmap);
    image_plane->show();
    imagecapture->capture();
}

void MainWindow::camera_image_expsed(int id)
{
    qDebug() << "Image Exposed " << id; //Should be called after imageCapture->Capture()
}

void MainWindow::camera_ready_for_capture(bool ready)
{
    qDebug() << "Camera is ready " << ready;
    if(ready){
        qDebug() << "Capture Picture";
        active_camera->searchAndLock();
        qDebug() << imagecapture->capture(); // Add some path to test
        qDebug() << imagecapture->error();
        active_camera->unlock();
        qDebug() << "Picture took";
    }
}

void MainWindow::camera_state(QCamera::State state)
{
    qDebug() << "Camera State " << state;
}

void MainWindow::camera_status(QCamera::Status status)
{
    qDebug() << "Camera Status " << status;
    if(status == QCamera::LoadedStatus){
        for (QCameraViewfinderSettings settings : active_camera->supportedViewfinderSettings()){
            QSize size = settings.resolution();
            qDebug() << size.width() << "x" << size.height() << " " << settings.pixelFormat() << " " << settings.maximumFrameRate();
        }
        active_camera->setViewfinderSettings(active_camera->supportedViewfinderSettings()[0]);
        active_camera->start();

    }
    if (status == QCamera::ActiveStatus){
            QImageEncoderSettings imageSettings;
            imageSettings.setCodec("image/jpeg");
            imageSettings.setResolution(1600, 1200);
            imagecapture = new QCameraImageCapture(active_camera);
            imagecapture->setEncodingSettings(imageSettings);
            imagecapture->setBufferFormat(QVideoFrame::Format_YUYV);
            qDebug() << "Supports Capture to buffer? "  << imagecapture->isCaptureDestinationSupported(QCameraImageCapture::CaptureDestination::CaptureToBuffer);
            qDebug() << "Is available "  << imagecapture->isAvailable();
            imagecapture->setCaptureDestination(QCameraImageCapture::CaptureDestination::CaptureToBuffer);
            connect(imagecapture, &QCameraImageCapture::imageAvailable, this, &MainWindow::camera_available_image);
            //connect(imagecapture, &QCameraImageCapture::imageCaptured, this, &MainWindow::camera_captured_image);
            //connect(imagecapture, SIGNAL(imageExposed(int)), this, SLOT(camera_image_expsed(int)));
            connect(imagecapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(camera_ready_for_capture(bool)));
            qDebug() << "Available? " << imagecapture->availability();
            qDebug() << "Ready to capture? " << imagecapture->isReadyForCapture();
    }
}



void MainWindow::on_color_type_combbox_currentIndexChanged(int index)
{
    qDebug() << "To index " << index;
    format = (QImage::Format)index;
    qDebug() << "Format " << format;
}

