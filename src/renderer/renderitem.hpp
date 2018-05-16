#pragma once

#include "camera.hpp"
#include "rendererinterface.hpp"

#include <QtQuick>

Q_DECLARE_LOGGING_CATEGORY(renderItemCategory)

class RenderItem
        : public QQuickFramebufferObject
{

    Q_OBJECT

    Q_PROPERTY(Camera * camera MEMBER camera CONSTANT)
    Q_PROPERTY(RendererInterface * renderer MEMBER rendererInterface CONSTANT)

    Q_PROPERTY(float lookSpeed MEMBER lookSpeed NOTIFY lookSpeedChanged)
    Q_PROPERTY(float linearSpeed MEMBER linearSpeed NOTIFY linearSpeedChanged)

    Q_PROPERTY(QUrl source MEMBER source NOTIFY sourceChanged)

public :

    explicit RenderItem(QQuickItem * const parent = Q_NULLPTR);

Q_SIGNALS :

    void lookSpeedChanged(float lookSpeed);
    void linearSpeedChanged(float linearSpeed);

    void sourceChanged(QUrl source);

private :

    Camera * const camera = new (std::nothrow) Camera{this};
    RendererInterface * const rendererInterface = new (std::nothrow) RendererInterface{this};

    float lookSpeed = 5E-3f;
    float linearSpeed = 5E-3f;

    QUrl source;

    Renderer * createRenderer() const Q_DECL_OVERRIDE;

    QSet< Qt::Key > pressedKeys;
    Qt::KeyboardModifiers keyboardModifiers = Qt::NoModifier;
    Qt::MouseButtons pressedMouseButtons = Qt::NoButton;
    QPoint startPos;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;

    void clearControlsState();

    void onKeyEvent(QKeyEvent * event, bool pressed);

    void focusInEvent(QFocusEvent * event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent * event) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent * event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent * event) Q_DECL_OVERRIDE;

    void mouseUngrabEvent() Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    void hoverEnterEvent(QHoverEvent * event) Q_DECL_OVERRIDE;
    void hoverMoveEvent(QHoverEvent * event) Q_DECL_OVERRIDE;
    void hoverLeaveEvent(QHoverEvent * event) Q_DECL_OVERRIDE;

    void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;

};
