#ifndef TESTWIDGET3D_H
#define TESTWIDGET3D_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <QTimer>
#include "flippedtextureimage.h";

class testwidget3d : public QWidget
{
    Q_OBJECT
public:
    explicit testwidget3d(QWidget *parent = nullptr);
    FlippedTextureImage *planeTextureImage;

private:
    QWidget *m_3d_window_container;
    Qt3DExtras::Qt3DWindow *m_3d_window;
    QTimer timer;

};

#endif // TESTWIDGET3D_H