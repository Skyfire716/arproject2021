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
#include <QOpenGLWidget>
#include <QSlider>
#include "camera_controller.h"
#include "chessglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

const QString EXAMPLE_VIDEO = "Video";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format);

private slots:
    void on_camera_combbox_currentIndexChanged(int index);
    void receive_capture(QPixmap img);
    void on_pushButton_clicked();
    void on_threshold_slider_id_valueChanged(int value);
    void on_threshold_method_ui_currentIndexChanged(int index);

public slots:
    void add_camerabox_item(QString item_name);

private:
    Ui::MainWindow *ui;
    QLabel *openvc_version;
    QLabel *image_plane;
    QComboBox *camera_box;
    QComboBox *threshold_method_box;
    QSlider *threshold_slider;
    QMap<int, QString> camera_select_map;
    QList<QCameraInfo> cameras;
    QCamera *active_camera;
    QCameraImageCapture *imagecapture;
    QImage::Format format;
    camera_controller *cam_control;
    chessglwidget *opengl_scene;
};
#endif // MAINWINDOW_H
