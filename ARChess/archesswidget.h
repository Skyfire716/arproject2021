#ifndef ARCHESSWIDGET_H
#define ARCHESSWIDGET_H

#include <QWidget>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QImage>
#include <QPixmap>

class archessbackgound : public Qt3DRender::QPaintedTextureImage{
    Q_OBJECT
public:
    archessbackgound(Qt3DCore::QNode *parent = nullptr);
    void paint(QPainter *painter) override;

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

private:
    QWidget *m_3d_window_container;
    Qt3DExtras::Qt3DWindow *m_3d_window;
};

#endif // ARCHESSWIDGET_H
