#include "mainwindow.h"

#include <QApplication>
#include <Qt3DCore>
#include <Qt3DCore/QTransform>
#include <Qt3DRender>
#include <Qt3DExtras>
#if Q_OS_ANDROID
#include <QtAndroid>
#endif

Qt3DCore::QEntity *createScene()
{
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;
    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    //torusEntity->addComponent(torusMesh);
    //torusEntity->addComponent(torusTransform);
    //torusEntity->addComponent(material);
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(3);
    sphereMesh->setGenerateTangents(true);

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;
    //OrbitTransformController *controller = new OrbitTransformController(sphereTransform);
    //controller->setTarget(sphereTransform);
    //controller->setRadius(20.0f);

    QPropertyAnimation *sphereRotateTransformAnimation = new QPropertyAnimation(sphereTransform);
    //sphereRotateTransformAnimation->setTargetObject(controller);
    sphereRotateTransformAnimation->setPropertyName("angle");
    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
    sphereRotateTransformAnimation->setDuration(10000);
    sphereRotateTransformAnimation->setLoopCount(-1);
    sphereRotateTransformAnimation->start();

    //sphereEntity->addComponent(sphereMesh);
    //sphereEntity->addComponent(sphereTransform);
    //sphereEntity->addComponent(material);

    Qt3DCore::QEntity *monkeyEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QMesh *monkeyMesh = new Qt3DRender::QMesh();
    monkeyMesh->setSource(QUrl("qrc:/models/resources/models/blender_monkey.stl"));

    Qt3DCore::QTransform *monkeyTransform = new Qt3DCore::QTransform;
    monkeyTransform->setScale3D(QVector3D(1, 1, 1));
    monkeyTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 1), 90.0f));
    monkeyEntity->addComponent(monkeyMesh);
    monkeyEntity->addComponent(monkeyTransform);
    monkeyEntity->addComponent(material);
    return rootEntity;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    Qt3DExtras::Qt3DWindow view;
    Qt3DCore::QEntity *scene = createScene();
    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.setRootEntity(scene);
    view.show();
#ifdef Q_OS_ANDROID
    auto  result = QtAndroid::checkPermission(QString("android.permission.CAMERA"));
    if(result == QtAndroid::PermissionResult::Denied){
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({"android.permission.CAMERA"}));
        if(resultHash["android.permission.CAMERA"] == QtAndroid::PermissionResult::Denied)
        {

        }
    }
#endif
    //w.show();
    return a.exec();
}
