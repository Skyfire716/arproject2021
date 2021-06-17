#ifndef FLIPPEDTEXTUREIMAGE_H
#define FLIPPEDTEXTUREIMAGE_H

#include <Qt3DRender/QPaintedTextureImage>
#include <QUrl>

class FlippedTextureImage : public Qt3DRender::QPaintedTextureImage
{
public:
    FlippedTextureImage(Qt3DCore::QNode *parent = Q_NULLPTR);
    void paint(QPainter *painter) override;

public slots:
    void receive_capture(QImage img);

private:
    QImage backgroundImage = QImage(":/images/resources/images/IMG_20210603_121201_1.jpg");
};

#endif // FLIPPEDTEXTUREIMAGE_H
