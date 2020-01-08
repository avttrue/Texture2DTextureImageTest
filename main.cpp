#include <QGuiApplication>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

int main(int argc, char *argv[])
{
    QGuiApplication application(argc, argv);
    Qt3DExtras::Qt3DWindow window;

    // root entity
    auto scene = new Qt3DCore::QEntity;
    window.setRootEntity(scene);

    // entity
    auto powerup = new Qt3DCore::QEntity(scene);

    // mesh
    Qt3DRender::QMesh* mesh = new Qt3DRender::QMesh(scene);
    mesh->setSource(QUrl::fromLocalFile(":/powerup.obj"));

    // material
    auto material = new Qt3DExtras::QDiffuseSpecularMaterial(powerup);
    material->setAmbient(Qt::white);
    material->setShininess(200.f);
    material->setAlphaBlendingEnabled(false);
    material->setTextureScale(1.0f);
    material->setSpecular(QVariant::fromValue<QColor>(Qt::lightGray));

    // texture
    auto texture2d = new Qt3DRender::QTexture2D(scene);
    auto textureImage = new Qt3DRender::QTextureImage(texture2d);

    texture2d->setMinificationFilter(Qt3DRender::QAbstractTexture::LinearMipMapLinear);
    texture2d->setMagnificationFilter(Qt3DRender::QAbstractTexture::Linear);
    texture2d->setFormat(Qt3DRender::QAbstractTexture::Automatic);

    Qt3DRender::QTextureWrapMode wm;
    wm.setX(Qt3DRender::QTextureWrapMode::ClampToEdge);
    wm.setY(Qt3DRender::QTextureWrapMode::ClampToEdge);
    wm.setZ(Qt3DRender::QTextureWrapMode::ClampToEdge);
    texture2d->setWrapMode(wm);

    textureImage->setMirrored(true);

    QObject::connect(textureImage, &Qt3DRender::QTextureImage::statusChanged,
                     [=](Qt3DRender::QTextureImage::Status s){qDebug() << "textureImage status =" << s; });
    QObject::connect(textureImage, &Qt3DRender::QTextureImage::sourceChanged,
                     [=](QUrl s){ qDebug() << "textureImage source =" << s.path(); });
    QObject::connect(texture2d, &Qt3DRender::QAbstractTexture::statusChanged,
                     [=](Qt3DRender::QAbstractTexture::Status s){ qDebug() << "texture2d status =" << s; });

    textureImage->setSource(QUrl::fromLocalFile(":/powerup.png"));
    texture2d->addTextureImage(textureImage);

    material->setDiffuse(QVariant::fromValue<Qt3DRender::QTexture2D*>(texture2d));

    // transform
    auto transform = new Qt3DCore::QTransform;
    transform->setTranslation(QVector3D(0.0f, 0.0f, -1.0f));
    transform->setScale3D(QVector3D(10, 10, 10));

    // picker
    auto picker = new Qt3DRender::QObjectPicker;
    picker->setHoverEnabled(false);
    picker->setEnabled(true);

    powerup->addComponent(transform);
    powerup->addComponent(material);
    powerup->addComponent(mesh);
    powerup->addComponent(picker);

    QObject::connect(picker, &Qt3DRender::QObjectPicker::clicked, [=](){ qDebug() << "clicked"; });

    // camera
    auto camera = window.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.0f, 0.0f, 100.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    auto camController = new Qt3DExtras::QFirstPersonCameraController(scene);
    camController->setCamera(camera);

    window.show();
    return application.exec();
}

