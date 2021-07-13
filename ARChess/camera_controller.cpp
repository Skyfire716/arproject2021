#include "camera_controller.h"
#include <QDebug>
#include <QPixmap>
#include <QList>
#include <QPair>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QTemporaryDir>
#include <math.h>
#include <QtMath>
#include <QtAlgorithms>

void camera_controller::init()
{
    worker->init_b = true;
}

void camera_controller::start_capture()
{
    worker->capture_b = true;
    worker_thread->start();
}

void camera_controller::stop_capture()
{
    worker->capture_b = false;
}


