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
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QLabel *image_label;
    QHBoxLayout *horizontalLayout;
    QLabel *openvc_version_label;
    QComboBox *camera_combbox;
    QComboBox *color_type_combbox;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1500, 537);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        image_label = new QLabel(centralwidget);
        image_label->setObjectName(QString::fromUtf8("image_label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(image_label->sizePolicy().hasHeightForWidth());
        image_label->setSizePolicy(sizePolicy);
        image_label->setBaseSize(QSize(2000, 1000));

        gridLayout->addWidget(image_label, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        openvc_version_label = new QLabel(centralwidget);
        openvc_version_label->setObjectName(QString::fromUtf8("openvc_version_label"));

        horizontalLayout->addWidget(openvc_version_label);

        camera_combbox = new QComboBox(centralwidget);
        camera_combbox->setObjectName(QString::fromUtf8("camera_combbox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(camera_combbox->sizePolicy().hasHeightForWidth());
        camera_combbox->setSizePolicy(sizePolicy1);
        camera_combbox->setEditable(false);

        horizontalLayout->addWidget(camera_combbox);

        color_type_combbox = new QComboBox(centralwidget);
        color_type_combbox->setObjectName(QString::fromUtf8("color_type_combbox"));

        horizontalLayout->addWidget(color_type_combbox);


        gridLayout->addLayout(horizontalLayout, 1, 2, 1, 1);

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
        image_label->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        openvc_version_label->setText(QApplication::translate("MainWindow", "OpenCV not found!", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
