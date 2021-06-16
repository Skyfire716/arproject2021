#include "chessglwidget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

chessglwidget::chessglwidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

void chessglwidget::receive_capture(unsigned char *image, int width, int height)
{
    //qDebug() << "Received Image";
    if(image){
        image_data = image;
        image_width = width;
        image_height = height;
        this->update();
    }else{
        qDebug() << "Faild to load texture";
    }
}

void chessglwidget::initializeGL()
{
    image_data = NULL;
    camera_pos.setX(0);
    camera_pos.setY(0);
    camera_pos.setZ(0);
    snowman_pos.setX(0);
    snowman_pos.setY(0);
    snowman_pos.setZ(-20);
    initializeOpenGLFunctions();
    glClearColor(0.5294117647058824, 0.8078431372549020, 0.9215686274509804, 1.0f);
    glClearDepth(1);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_FRONT);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);
    glEnable(GL_LIGHT0);
}

void chessglwidget::resizeGL(int w, int h)
{
    camera.setToIdentity();
    camera.translate(camera_pos);
    camera.perspective(VERTICAL_ANGLE, w / float(h), NEAR_PLANE, FAR_PLANE);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(camera.data());
}

void chessglwidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPixelStorei(GL_PACK_ALIGNMENT, 3);
    //glPixelZoom(1.0, 1.0);
    //glMatrixMode(GL_PROJECTION);
    //glPushMatrix();
    //glDisable(GL_DEPTH);
    //glRasterPos2i(0, 0);
    if(image_data){
        glDrawPixels(image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        //image_data = NULL;
    }
    //glPopMatrix();
    //glEnable(GL_DEPTH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    draw_snowman();
    glPopMatrix();

}

void chessglwidget::mousePressEvent(QMouseEvent *event)
{

}

void chessglwidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void chessglwidget::mouseDoubleClickEvent(QMouseEvent *event)
{

}

void chessglwidget::mouseMoveEvent(QMouseEvent *event)
{

}

void chessglwidget::wheelEvent(QWheelEvent *event)
{

}

void chessglwidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Got Key Event " << event->key();
    if(event->key()  == Qt::Key_Escape){
        qDebug() << "Closing Application";
        QApplication::quit();
    }
}

void chessglwidget::keyReleaseEvent(QKeyEvent *event)
{

}

void chessglwidget::draw_snowman()
{
    glLoadIdentity();
    glTranslatef(snowman_pos.x(), snowman_pos.y(), snowman_pos.z());
    //Bottom
    glPushMatrix();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    drawSphere(1.5, 10, 10);
    glPopMatrix();
    //Middle
    glPushMatrix();
    glTranslatef(0.0, 2.2, 0.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    drawSphere(1.0, 10, 10);
    glPopMatrix();
    //Head
    glPushMatrix();
    glTranslatef(0.0, 3.7, 0.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    drawSphere(0.75, 10, 10);
    glTranslatef(0.0, 0.0, 0.73);
    drawCone(0.2, 0.7, 6, 3);
    glPopMatrix();
}

void chessglwidget::drawSphere(double r, int lats, int longs)
{
    int i, j;
    for(i = 0; i <= lats; i++){
        double lat0 = M_PI * (-0.5 + (double) (i-1) /lats);
        double z0 = r * sin(lat0);
        double zr0 = r * cos(lat0);

        double lat1 = M_PI * (-0.5 * (double) i/lats);
        double z1 = r * sin(lat1);
        double zr1 = r * cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++){
            double lng = 2 * M_PI * (double) (j-1) /longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

void chessglwidget::drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, height);
    for(int angle = 0;  angle < 360; angle++){
        glVertex3f(sin((double)angle) * base, cos((double) angle) * base, 0.f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for(int angle = 0; angle < 360; angle++){
        glNormal3f(0, -1, 0);
        glVertex3f(sin((double) angle) * base, cos((double) angle)*base, 0.f);
    }
    glEnd();
}
