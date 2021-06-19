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

archesswidget::archesswidget(QWidget *parent) : QWidget(parent)
{
    m_3d_window = new Qt3DExtras::Qt3DWindow();
    m_3d_window->setGeometry(this->geometry());

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector();
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
    backgroundCamera->setPosition(QVector3D(0, 0, 1));
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
    objectsCamera->lens()->setPerspectiveProjection(45.f, this->geometry().width() / (float) this->geometry().height(), 0.01f, 1000.f);
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
    planeTextureImage->setSize(this->geometry().size());
    planeTexture->addTextureImage(planeTextureImage);
    planeMaterial->setTexture(planeTexture);

    Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform(planeEntity);
    planeTransform->setRotationX(90);
    planeTransform->setTranslation(QVector3D(0, 0, 0));

    planeEntity->addComponent(planeMesh);
    planeEntity->addComponent(planeMaterial);
    planeEntity->addComponent(planeTransform);
    planeEntity->addComponent(backgroundLayer);

    Qt3DCore::QEntity *monkeyEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMesh *monkeyMesh = new Qt3DRender::QMesh();
    monkeyMesh->setSource(QUrl("qrc:/models/resources/models/monkey.stl"));

    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(monkeyEntity);
    material->setAmbient(QColor::fromRgbF(0.817308, 0.817308, 0.817308));
    material->setDiffuse(QColor::fromRgbF(0.800000, 0.242753, 0.008774));
    material->setSpecular(QColor::fromRgbF(0.817308, 0.817308, 0.817308));
    Qt3DCore::QTransform *monkeyTransform = new Qt3DCore::QTransform;
    monkeyTransform->setScale3D(QVector3D(1, 1, 1));
    monkeyTransform->setTranslation(QVector3D(3, 0, 10));
    monkeyTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 90.0f) * QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), 180.0f));
    monkeyEntity->addComponent(monkeyMesh);
    monkeyEntity->addComponent(monkeyTransform);
    monkeyEntity->addComponent(material);
    monkeyEntity->addComponent(objectsLayer);

    m_3d_window_container = QWidget::createWindowContainer(m_3d_window);
    m_3d_window_container->setGeometry(this->geometry());
    m_3d_window_container->setParent(this);
    planeTextureImage->update();
    this->update();
}


void archesswidget::resizeEvent(QResizeEvent *event)
{
    m_3d_window_container->setGeometry(this->geometry());
    m_3d_window->setGeometry(this->geometry());
    objectsCamera->lens()->setPerspectiveProjection(45.f, this->geometry().width() / (float) this->geometry().height(), 0.01f, 1000.f);
    planeTextureImage->setSize(this->geometry().size());
    planeTextureImage->update();
    this->update();
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
    QPixmap map = background.scaled(this->parent_widget->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
    painter->drawPixmap((this->parent_widget->size().width() / 2.0) - (map.size().width()/2.0), 0, map);
}

void archessbackgound::receive_image(QPixmap img)
{
    this->background = img;
    this->update();
}
