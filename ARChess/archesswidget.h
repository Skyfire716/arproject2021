#ifndef ARCHESSWIDGET_H
#define ARCHESSWIDGET_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QImage>
#include <QPixmap>
#include <qboxlayout.h>

class archessbackgound : public Qt3DRender::QPaintedTextureImage{
    Q_OBJECT
public:
    archessbackgound(Qt3DCore::QNode *parent = nullptr);
    void paint(QPainter *painter) override;
    QWidget *parent_widget;
    QSize paint_area_size;
    QRect window_area;
    
public slots:
    void receive_image(QPixmap img);
    
private:
    QPixmap background;
    
};

class archesswidget : public QWidget
{
    Q_OBJECT
public:
    explicit archesswidget(QWidget *parent = nullptr);
    archessbackgound *planeTextureImage;

    
private:
    QBoxLayout *layout;
    QWidget *m_3d_window_container;
    Qt3DExtras::Qt3DWindow *m_3d_window;
    Qt3DRender::QCamera *objectsCamera;
    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector;
    float camera_angle_deg = 45;
    
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // ARCHESSWIDGET_H
