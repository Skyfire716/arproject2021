#ifndef ARCHESSWIDGET_H
#define ARCHESSWIDGET_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPair>
#include <QVector3D>
#include <qboxlayout.h>
#include <QQuaternion>

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

signals:
    void value_click_changed(int x_value, int y_value);

public slots:
    void new_ar_transform_singels(float scalar, float xpos, float ypos, float zpos, float x, float y, float z);
    
private:
    Qt3DRender::QLayer *objectsLayer;
    Qt3DCore::QEntity *rootEntity;
    Qt3DCore::QTransform *white_figures[16];
    Qt3DCore::QTransform *black_figures[16];
    Qt3DCore::QTransform *bauerTransform;
    QBoxLayout *layout;
    QWidget *m_3d_window_container;
    Qt3DExtras::Qt3DWindow *m_3d_window;
    Qt3DRender::QCamera *objectsCamera;
    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector;
    float camera_angle_deg = 45;
    
protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
    void instantiate_figures();
};

#endif // ARCHESSWIDGET_H
