#ifndef CHESSGLWIDGET_H
#define CHESSGLWIDGET_H

#include <QOpenGLWidget>

class chessglwidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit chessglwidget(QWidget *parent = nullptr);

signals:


    // QOpenGLWidget interface
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

#endif // CHESSGLWIDGET_H
