#include "flippedtextureimage.h"

#include <QPainter>
#include <QImage>
#include <QPaintDevice>

FlippedTextureImage::FlippedTextureImage(Qt3DCore::QNode *parent) : Qt3DRender::QPaintedTextureImage(parent) {

}

void FlippedTextureImage::paint(QPainter *painter) {
    painter->drawImage(0, 0, backgroundImage);
}

void FlippedTextureImage::receive_capture(QImage img)
{
    backgroundImage = img;
    this->update();
}
