#ifndef CHESSGLWIDGET_H
#define CHESSGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QMatrix4x4>
#include <QVector3D>

class chessglwidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    const float VERTICAL_ANGLE = 45.0;
    const float NEAR_PLANE = 0.01f;
    const float FAR_PLANE = 1000.0f;
public:
    explicit chessglwidget(QWidget *parent = nullptr);
    float light_pos[3] = {0.8, 0.8, 0.0};
    float ambient_color[4] = {0.3, 0.3, 0.3, 1.0};
    float diffuse_color[4] = {0.8, 0.8, 0.8, 1.0};
    QVector3D snowman_pos;
    QVector3D camera_pos;
    QOpenGLContext *shared_gl_context;


public slots:
    void receive_capture(unsigned char* image, int width, int height);

signals:


    // QOpenGLWidget interface
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void draw_snowman();
    void drawSphere(double r, int lats, int longs);
    void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
    unsigned char* image_data;
    int image_width;
    int image_height;
    QMatrix4x4 camera;
};

#endif // CHESSGLWIDGET_H
