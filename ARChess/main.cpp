#include "mainwindow.h"

#include <QApplication>
#if Q_OS_ANDROID
#include <QtAndroid>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

#ifdef Q_OS_ANDROID
    auto  result = QtAndroid::checkPermission(QString("android.permission.CAMERA"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.CAMERA"}));
        if(resultHash["android.permission.CAMERA"] == QtAndroid::PermissionResult::Denied)
        {

        }
    }
#endif
    w.show();
    return a.exec();
}
