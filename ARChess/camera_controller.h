#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include<opencv2/videoio.hpp>
#include<QString>
#include<QList>
#include<QThread>
#include<QObject>
#include<QImage>
#include <QPixmap>
#include <QDebug>
#include <QVector>
#include <cstdio>
#include "chessboard_controller.h"
#include "chessboard.h"
#include "camera_worker.h"

class camera_controller : public QObject
{
    Q_OBJECT
    QThread *worker_thread;
public:
    camera_controller(){
        worker_thread = new QThread();
        worker = new camera_worker();
        worker->moveToThread(worker_thread);
        worker->running = true;
        connect(worker_thread, &QThread::started, worker, &camera_worker::run);
        connect(worker, &camera_worker::finished, worker_thread, &QThread::quit);
        connect(worker, &camera_worker::finished, worker, &camera_worker::deleteLater);
        connect(worker_thread, &QThread::finished, worker_thread, &QThread::deleteLater);
    }
    ~camera_controller(){
        worker_thread->quit();
        worker_thread->wait();
    }
    camera_worker *worker;
    
public slots:
    void init();
    void start_capture();
    void stop_capture();
private:
    
};

#endif // CAMERA_CONTROLLER_H
