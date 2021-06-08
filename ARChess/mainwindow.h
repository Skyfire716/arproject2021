#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QMap>
#include <QString>
#include <QList>
#include <QCameraInfo>
#include <QCamera>
#include <QCameraImageCapture>
#include <QVideoFrame>
#include <QVideoWidget>
#include <QCheckBox>
#include <opencv2/opencv.hpp>
#include "camera_controller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format);

private slots:
    void on_camera_combbox_currentIndexChanged(int index);
    void receive_capture(QPixmap img);

    void on_pushButton_clicked();

public slots:
    void add_camerabox_item(QString item_name);

private:
    Ui::MainWindow *ui;
    QLabel *openvc_version;
    QLabel *image_plane;
    QComboBox *camera_box;
    QComboBox *color_box;
    QMap<int, QString> camera_select_map;
    QList<QCameraInfo> cameras;
    QCamera *active_camera;
    QCameraImageCapture *imagecapture;
    QImage::Format format;
    camera_controller *cam_control;
};
#endif // MAINWINDOW_H
