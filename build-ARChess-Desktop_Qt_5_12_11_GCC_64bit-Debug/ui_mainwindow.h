/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *openvc_version_label;
    QComboBox *camera_combbox;
    QComboBox *color_type_combbox;
    QLabel *image_label;
    QPushButton *pushButton;
    QOpenGLWidget *openGLWidget3D;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1044, 623);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        openvc_version_label = new QLabel(centralwidget);
        openvc_version_label->setObjectName(QString::fromUtf8("openvc_version_label"));

        horizontalLayout->addWidget(openvc_version_label);

        camera_combbox = new QComboBox(centralwidget);
        camera_combbox->setObjectName(QString::fromUtf8("camera_combbox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(camera_combbox->sizePolicy().hasHeightForWidth());
        camera_combbox->setSizePolicy(sizePolicy);
        camera_combbox->setEditable(false);

        horizontalLayout->addWidget(camera_combbox);

        color_type_combbox = new QComboBox(centralwidget);
        color_type_combbox->setObjectName(QString::fromUtf8("color_type_combbox"));

        horizontalLayout->addWidget(color_type_combbox);


        gridLayout->addLayout(horizontalLayout, 1, 2, 1, 1);

        image_label = new QLabel(centralwidget);
        image_label->setObjectName(QString::fromUtf8("image_label"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(image_label->sizePolicy().hasHeightForWidth());
        image_label->setSizePolicy(sizePolicy1);
        image_label->setBaseSize(QSize(2000, 1000));

        gridLayout->addWidget(image_label, 2, 0, 1, 1);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 1, 0, 1, 1);

        openGLWidget3D = new QOpenGLWidget(centralwidget);
        openGLWidget3D->setObjectName(QString::fromUtf8("openGLWidget3D"));

        gridLayout->addWidget(openGLWidget3D, 2, 2, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        openvc_version_label->setText(QApplication::translate("MainWindow", "OpenCV not found!", nullptr));
        image_label->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Redetect Cameras", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
