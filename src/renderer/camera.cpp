#include "camera.hpp"

Q_LOGGING_CATEGORY(cameraCategory, "camera")

CameraLens::CameraLens(QObject * const parent)
    : QObject{parent}
{
    const auto o = metaObject();
    const int count = o->propertyCount() - 1;
    Q_ASSERT(!(count < 0));
    const auto projectionMatrixMetaProperty = o->property(count);
    Q_ASSERT(projectionMatrixMetaProperty.hasNotifySignal());
    const int projectionMatrixChangedIndex = projectionMatrixMetaProperty.notifySignalIndex();
    Q_ASSERT(o->indexOfSignal("projectionMatrixChanged()") == projectionMatrixChangedIndex);
    for (int i = o->propertyOffset(); i < count; ++i) {
        const auto metaProperty = o->property(i);
        Q_ASSERT(metaProperty.hasNotifySignal());
        if (!QMetaObject::connect(this, metaProperty.notifySignalIndex(), this, projectionMatrixChangedIndex)) {
            qCCritical(cameraCategory);
        }
    }
}

QMatrix4x4 CameraLens::projectionMatrix() const
{
    QMatrix4x4 projectionMatrix;
    projectionMatrix.setToIdentity();
    switch (projectionType) {
    case PerspectiveProjection :
        projectionMatrix.perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
        break;
    case OrthographicProjection :
        projectionMatrix.ortho(left, right, bottom, top, nearPlane, farPlane);
        break;
    case FrustumProjection :
        projectionMatrix.frustum(left, right, bottom, top, nearPlane, farPlane);
        break;
    default : {
        qCCritical(cameraCategory);
    }
    }
    return projectionMatrix;
}

Camera::Camera(QObject * const parent)
    : CameraLens{parent}
{
    connect(this, &CameraLens::projectionMatrixChanged, this, &Camera::transformationMatrixChanged);
    const auto o = metaObject();
    const int count = o->propertyCount() - 1;
    Q_ASSERT(!(count < 0));
    const auto transformationMatrixMetaProperty = o->property(count);
    Q_ASSERT(transformationMatrixMetaProperty.hasNotifySignal());
    const int transformationMatrixChangedIndex = transformationMatrixMetaProperty.notifySignalIndex();
    Q_ASSERT(o->indexOfSignal("transformationMatrixChanged()") == transformationMatrixChangedIndex);
    for (int i = o->propertyOffset(); i < count; ++i) {
        const auto metaProperty = o->property(i);
        Q_ASSERT(metaProperty.hasNotifySignal());
        if (!QMetaObject::connect(this, metaProperty.notifySignalIndex(), this, transformationMatrixChangedIndex)) {
            qCCritical(cameraCategory);
        }
    }
}

QMatrix4x4 Camera::transformationMatrix() const
{
    auto transformationMatrix = projectionMatrix();
    transformationMatrix.scale(scale);
    transformationMatrix.rotate(rotation);
    transformationMatrix.translate(position);
    return transformationMatrix;
}
