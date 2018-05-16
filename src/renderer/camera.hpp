#pragma once

#include <QtGui>

Q_DECLARE_LOGGING_CATEGORY(cameraCategory)

class CameraLens
        : public QObject
{

    Q_OBJECT

    Q_PROPERTY(ProjectionType projectionType MEMBER projectionType NOTIFY projectionTypeChanged)

    Q_PROPERTY(float fieldOfView MEMBER fieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio MEMBER aspectRatio NOTIFY aspectRatioChanged)

    Q_PROPERTY(float nearPlane MEMBER nearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane MEMBER farPlane NOTIFY farPlaneChanged)

    Q_PROPERTY(float left MEMBER left NOTIFY leftChanged)
    Q_PROPERTY(float right MEMBER right NOTIFY rightChanged)
    Q_PROPERTY(float bottom MEMBER bottom NOTIFY bottomChanged)
    Q_PROPERTY(float top MEMBER top NOTIFY topChanged)

    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix NOTIFY projectionMatrixChanged STORED false)

public :

    explicit
    CameraLens(QObject * const parent = Q_NULLPTR);

    enum ProjectionType {
        PerspectiveProjection,
        OrthographicProjection,
        FrustumProjection
    };
    Q_ENUM(ProjectionType)

    QMatrix4x4 projectionMatrix() const;

Q_SIGNALS :

    void projectionTypeChanged(ProjectionType projectionType);

    void fieldOfViewChanged(float fieldOfView);
    void aspectRatioChanged(float aspectRatio);

    void nearPlaneChanged(float nearPlane);
    void farPlaneChanged(float farPlane);

    void leftChanged(float left);
    void rightChanged(float right);
    void bottomChanged(float bottom);
    void topChanged(float top);

    void projectionMatrixChanged();

private :

    ProjectionType projectionType = PerspectiveProjection;

    float fieldOfView = 90.0f;
    float aspectRatio = 1.0f;

    float nearPlane = 0.01f;
    float farPlane = 100.0f;

    float left = -0.5f;
    float right = 0.5f;
    float bottom = -0.5f;
    float top = 0.5f;

};

class Camera
        : public CameraLens
{

    Q_OBJECT

    Q_PROPERTY(QVector3D position MEMBER position NOTIFY positionChanged)
    Q_PROPERTY(QVector3D scale MEMBER scale NOTIFY scaleChanged)
    Q_PROPERTY(QQuaternion rotation MEMBER rotation NOTIFY rotationChanged)

    Q_PROPERTY(QMatrix4x4 transformationMatrix READ transformationMatrix NOTIFY transformationMatrixChanged STORED false)

public :

    explicit
    Camera(QObject * const parent = Q_NULLPTR);

    Q_INVOKABLE
    QMatrix4x4 transformationMatrix() const;

Q_SIGNALS :

    void positionChanged(QVector3D position);
    void scaleChanged(QVector3D scale);
    void rotationChanged(QQuaternion rotation);

    void transformationMatrixChanged();

private :

    QVector3D position = {0.0f, 0.0f, 0.0f};
    QVector3D scale = {1.0f, 1.0f, 1.0f};
    QQuaternion rotation;

};
