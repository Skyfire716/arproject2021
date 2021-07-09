#include "archesswidget.h"
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QTransform>
#include <QLabel>
#include <Qt3DRender>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderSettings>
#include <QPropertyAnimation>
#include <qboxlayout.h>

archesswidget::archesswidget(QWidget *parent) : QWidget(parent)
{
    m_3d_window = new Qt3DExtras::Qt3DWindow();
    
    renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector();
    renderSurfaceSelector->setSurface(m_3d_window);
    
    // First branch clearing the buffers
    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(renderSurfaceSelector);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    
    // Second branch
    // Custom framegraph for background image
    Qt3DRender::QLayerFilter *backgroundLayerFilter = new Qt3DRender::QLayerFilter(renderSurfaceSelector);
    Qt3DRender::QLayer *backgroundLayer = new Qt3DRender::QLayer(backgroundLayerFilter);
    backgroundLayerFilter->addLayer(backgroundLayer);
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(backgroundLayer);
    Qt3DRender::QCamera *backgroundCamera = new Qt3DRender::QCamera(cameraSelector);
    backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    backgroundCamera->setPosition(QVector3D(0, 0, 0));
    backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    cameraSelector->setCamera(backgroundCamera);
    
    // Third branch
    // Framegraph for objects
    Qt3DRender::QLayerFilter *objectsLayerFilter = new Qt3DRender::QLayerFilter(renderSurfaceSelector);
    Qt3DRender::QLayer *objectsLayer = new Qt3DRender::QLayer(objectsLayerFilter);
    objectsLayerFilter->addLayer(objectsLayer);
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(objectsLayer);
    Qt3DRender::QCameraSelector *objectsCameraSelector = new Qt3DRender::QCameraSelector(viewport);
    objectsCamera = new Qt3DRender::QCamera(objectsCameraSelector);
    objectsCamera->lens()->setPerspectiveProjection(45.f, m_3d_window->geometry().width() / (float) m_3d_window->geometry().height(), 0.01f, 1000.f);
    objectsCamera->setPosition(QVector3D(0, 0, -10));
    objectsCamera->setViewCenter(QVector3D(0, 0, 0));
    objectsCamera->setUpVector(QVector3D(0, 1, 0));
    objectsCameraSelector->setCamera(objectsCamera);
    Qt3DRender::QClearBuffers *clearDepthBuffers = new Qt3DRender::QClearBuffers(objectsCamera);
    clearDepthBuffers->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    
    // Set the new framegraph
    m_3d_window->setActiveFrameGraph(renderSurfaceSelector);
    m_3d_window->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::Always);
    
    // Root
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    m_3d_window->setRootEntity(rootEntity);
    
    // Background
    Qt3DCore::QEntity *planeEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh(planeEntity);
    planeMesh->setHeight(2);
    planeMesh->setWidth(2);
    
    Qt3DExtras::QTextureMaterial *planeMaterial = new Qt3DExtras::QTextureMaterial(planeEntity);
    Qt3DRender::QTexture2D *planeTexture = new Qt3DRender::QTexture2D(planeMaterial);
    planeTextureImage = new archessbackgound(planeTexture);
    planeTextureImage->parent_widget = this;
    planeTextureImage->setSize(planeTextureImage->paint_area_size);
    planeTexture->addTextureImage(planeTextureImage);
    planeMaterial->setTexture(planeTexture);
    
    m_3d_window->setGeometry(QRect(0, 0, planeTextureImage->paint_area_size.width(), planeTextureImage->paint_area_size.height()));
    
    Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform(planeEntity);
    planeTransform->setRotationX(90);
    planeTransform->setTranslation(QVector3D(0, 0, 0));
    
    planeEntity->addComponent(planeMesh);
    planeEntity->addComponent(planeMaterial);
    planeEntity->addComponent(planeTransform);
    planeEntity->addComponent(backgroundLayer);


/*
    Qt3DCore::QEntity *monkeyEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMesh *monkeyMesh = new Qt3DRender::QMesh();
    monkeyMesh->setSource(QUrl("qrc:/models/resources/models/monkey.stl"));
    
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(monkeyEntity);
    material->setAmbient(QColor::fromRgbF(0.817308, 0.817308, 0.817308));
    material->setDiffuse(QColor::fromRgbF(0.800000, 0.242753, 0.008774));
    material->setSpecular(QColor::fromRgbF(0.817308, 0.817308, 0.817308));
    monkeyTransform = new Qt3DCore::QTransform;
    monkeyTransform->setScale3D(QVector3D(1, 1, 1));
    monkeyTransform->setTranslation(QVector3D(0, 0, 5));
    monkeyTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90.0f) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180.0f));
    monkeyEntity->addComponent(monkeyMesh);
    monkeyEntity->addComponent(monkeyTransform);
    monkeyEntity->addComponent(material);
    monkeyEntity->addComponent(objectsLayer);
*/


    Qt3DCore::QEntity *bauerEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMesh *bauerMesh = new Qt3DRender::QMesh();
    bauerMesh->setSource(QUrl("qrc:/models/resources/models/bauer.stl"));

    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(bauerEntity);
    material->setAmbient(QColor::fromRgbF(0.817308, 0.817308, 0.817308));
    material->setDiffuse(QColor::fromRgbF(0.800000, 0.242753, 0.008774));
    material->setSpecular(QColor::fromRgbF(0.817308, 0.817308, 0.817308));
    bauerTransform = new Qt3DCore::QTransform;
    bauerTransform->setScale3D(QVector3D(1, 1, 1));
    bauerTransform->setTranslation(QVector3D(0, 0, 0));
    bauerTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90.0f) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180.0f));
    bauerEntity->addComponent(bauerMesh);
    bauerEntity->addComponent(bauerTransform);
    bauerEntity->addComponent(material);
    bauerEntity->addComponent(objectsLayer);


/*
    Qt3DCore::QEntity *fieldEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QPlaneMesh *fieldMesh = new Qt3DExtras::QPlaneMesh(fieldEntity);
    fieldMesh->setHeight(3.2);
    fieldMesh->setWidth(3.2);

    Qt3DExtras::QPhongMaterial *fieldMaterial = new Qt3DExtras::QPhongMaterial(fieldEntity);
    fieldMaterial->setAmbient(QColor::fromRgbF(1.0, 1.0, 1.0, 1.0));
    fieldMaterial->setDiffuse(QColor::fromRgbF(1.0, 1.0, 1.0, 1.0));
    fieldMaterial->setSpecular(QColor::fromRgbF(1.0, 1.0, 1.0, 1.0));
    fieldTransform = new Qt3DCore::QTransform(fieldEntity);
    fieldTransform->setScale(1);
    fieldTransform->setRotationX(-90);
    fieldTransform->setTranslation(QVector3D(0, 0, 3));

    fieldEntity->addComponent(fieldMesh);
    fieldEntity->addComponent(fieldMaterial);
    fieldEntity->addComponent(fieldTransform);
    fieldEntity->addComponent(objectsLayer);
*/

    m_3d_window_container = QWidget::createWindowContainer(m_3d_window);
    m_3d_window_container->setGeometry(QRect(0, 0, planeTextureImage->paint_area_size.width(), planeTextureImage->paint_area_size.height()));
    m_3d_window->installEventFilter(this);
    layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->addItem(new QSpacerItem(0, 0));
    layout->addWidget(m_3d_window_container);
    layout->addItem(new QSpacerItem(0, 0));
    layout->setContentsMargins(0, 0, 0, 0);
    planeTextureImage->update();

}

void archesswidget::new_ar_transform_singels(float scalar, float xpos, float ypos, float zpos, float x, float y, float z)
{
    //QQuaternion q(scalar, xpos, ypos, zpos);
    //QVector3D trans(x, y, z);
    //qDebug() << "Applying Trans";
    //bauerTransform->setRotation(q);
    //bauerTransform->setTranslation(trans);
}

void archesswidget::resizeEvent(QResizeEvent *event)
{
    float arWidth = planeTextureImage->paint_area_size.width();
    float arHeight = planeTextureImage->paint_area_size.height();
    float thisAspectRatio = (float)event->size().width() / event->size().height();
    int widgetStretch, outerStretch;
    if (thisAspectRatio > (arWidth/arHeight))
    {
        layout->setDirection(QBoxLayout::LeftToRight);
        widgetStretch = height() * (arWidth/arHeight);
        outerStretch = (width() - widgetStretch) / 2 + 0.5;
    }
    else
    {
        layout->setDirection(QBoxLayout::TopToBottom);
        widgetStretch = width() * (arHeight/arWidth);
        outerStretch = (height() - widgetStretch) / 2 + 0.5;
    }
    layout->setStretch(0, outerStretch);
    layout->setStretch(1, widgetStretch);
    layout->setStretch(2, outerStretch);
    objectsCamera->lens()->setPerspectiveProjection(45.f, m_3d_window->geometry().width() / (float) m_3d_window->geometry().height(), 0.01f, 1000.f);
    m_3d_window_container->move(0, 0);
    m_3d_window_container->setGeometry(m_3d_window->geometry());
    planeTextureImage->window_area = m_3d_window->geometry();
    renderSurfaceSelector->setSurface(m_3d_window);
}

void archesswidget::mousePressEvent(QMouseEvent *event)
{
    emit value_click_changed(event->x(), event->y());
    event->accept();
}

bool archesswidget::eventFilter(QObject *watched, QEvent *event)
{
    //TODO Here you can receive other events from the image screen
    if(watched == m_3d_window && event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        emit value_click_changed(mouseEvent->x() + m_3d_window->position().x(), mouseEvent->y() + m_3d_window->position().y());
    }
    return false;
}

archessbackgound::archessbackgound(Qt3DCore::QNode *parent) : Qt3DRender::QPaintedTextureImage(parent)
{
    
}

void archessbackgound::paint(QPainter *painter)
{
    painter->fillRect(painter->viewport(), QColor(0, 0, 0, 0));
    painter->setCompositionMode (QPainter::CompositionMode_Source);
    painter->fillRect(painter->viewport(), Qt::transparent);
    painter->setCompositionMode (QPainter::CompositionMode_SourceOver);
    painter->drawPixmap(0, 0, background);
}

void archessbackgound::receive_image(QPixmap img)
{
    if(img.size() != this->background.size()){
        this->parent_widget->resize(1, 1);
        this->parent_widget->resize(this->background.size());
    }
    this->background = img;
    this->paint_area_size = this->background.size();
    this->setSize(this->paint_area_size);
    this->update();
}
