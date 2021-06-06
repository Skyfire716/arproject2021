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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_camera_combbox_currentIndexChanged(int index);
    void camera_captured_image(int id, const QImage &preview);
    void camera_available_image(int id, const QVideoFrame  &frame);
    void camera_image_expsed(int id);
    void camera_ready_for_capture(bool ready);
    void camera_state(QCamera::State state);
    void camera_status(QCamera::Status status);

    void on_color_type_combbox_currentIndexChanged(int index);

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
    QCheckBox *running;
};
#endif // MAINWINDOW_H
