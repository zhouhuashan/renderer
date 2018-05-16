#include "renderitem.hpp"

#include "framebufferrenderer.hpp"
#include "utility.hpp"

Q_LOGGING_CATEGORY(renderItemCategory, "renderItem")

RenderItem::RenderItem(QQuickItem * const parent)
    : QQuickFramebufferObject{parent}
{
    Q_CHECK_PTR(camera);
    Q_CHECK_PTR(rendererInterface);

    connect(camera, &Camera::transformationMatrixChanged, this, &QQuickFramebufferObject::update);
    connect(rendererInterface, &RendererInterface::autoRefreshChanged, this, &QQuickFramebufferObject::update);

    setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    //setAcceptHoverEvents(true);
}

auto RenderItem::createRenderer() const -> Renderer *
{
    const auto autoRefresh = rendererInterface->property("autoRefresh").toBool();
    const auto frameBufferRenderer = new (std::nothrow) FrameBufferRenderer{autoRefresh, source};
    Q_CHECK_PTR(frameBufferRenderer);
    connect(rendererInterface, &RendererInterface::autoRefreshChanged, this, &RenderItem::update);
    connect(camera, &Camera::transformationMatrixChanged, this, &RenderItem::update);
    connect(this, &RenderItem::sourceChanged, this, &RenderItem::update);
    return frameBufferRenderer;
}

void RenderItem::clearControlsState()
{
    pressedKeys.clear();
    keyboardModifiers = Qt::NoModifier;
    pressedMouseButtons = 0;
    unsetCursor();
}

void RenderItem::onKeyEvent(QKeyEvent * event, bool pressed)
{
    keyboardModifiers = event->modifiers();
    if (!event->isAutoRepeat()) {
        const auto key = Qt::Key(event->key());
        switch (key) {
        case Qt::Key_W :
        case Qt::Key_A :
        case Qt::Key_S :
        case Qt::Key_D :

        case Qt::Key_PageUp :
        case Qt::Key_PageDown :

        case Qt::Key_Left :
        case Qt::Key_Right :
        case Qt::Key_Up :
        case Qt::Key_Down :

        case Qt::Key_Space :
            if (pressed) {
                if (pressedKeys.contains(key)) {
                    qCWarning(renderItemCategory)
                            << QStringLiteral("Unable to add key %1: already pressed")
                               .arg(toString(key));
                } else {
                    pressedKeys.insert(key);
                }
            } else {
                if (!pressedKeys.remove(key)) {
                    qCWarning(renderItemCategory)
                            << QStringLiteral("Unable to remove key %1: not pressed")
                               .arg(toString(key));
                }
            }
            event->accept();
            break;
        default :
            break;
        }
    }
}

void RenderItem::focusInEvent(QFocusEvent * event)
{
    clearControlsState();
    return QQuickFramebufferObject::focusInEvent(event);
}

void RenderItem::focusOutEvent(QFocusEvent * event)
{
    clearControlsState();
    return QQuickFramebufferObject::focusOutEvent(event);
}

void RenderItem::keyPressEvent(QKeyEvent * event)
{
    onKeyEvent(event, true);
    if (!event->isAccepted()) {
        return QQuickFramebufferObject::keyPressEvent(event);
    }
}

void RenderItem::keyReleaseEvent(QKeyEvent * event)
{
    onKeyEvent(event, false);
    if (!event->isAccepted()) {
        return QQuickFramebufferObject::keyReleaseEvent(event);
    }
}

void RenderItem::mouseUngrabEvent()
{
    pressedMouseButtons = Qt::NoButton;
}

void RenderItem::mousePressEvent(QMouseEvent * event)
{
    pressedMouseButtons = event->buttons();
    switch (event->button()) {
    case Qt::MouseButton::LeftButton :
        setCursor(Qt::BlankCursor);
        startPos = QCursor::pos();
        event->accept();
        break;
    default :
        break;
    }
    if (!event->isAccepted()) {
        return QQuickFramebufferObject::mousePressEvent(event);
    }
}

void RenderItem::mouseMoveEvent(QMouseEvent * event)
{
    pressedMouseButtons = event->buttons();
    if (event->buttons() & Qt::MouseButton::LeftButton) {
        const auto posDelta = QCursor::pos() - startPos;
        if (!posDelta.isNull()) {
            const auto angularSpeed = camera->property("fieldOfView").toFloat() * lookSpeed;
            yaw += (posDelta.x() * angularSpeed) * camera->property("aspectRatio").toFloat(); // pan
            if (yaw > +180.0f) {
                yaw -= 360.0f;
            } else if (yaw < -180.0f) {
                yaw += 360.0f;
            }
            pitch -= (posDelta.y() * angularSpeed); // tilt
            constexpr float pitch_max = 89.9f;
            if (pitch > +pitch_max) {
                pitch = +pitch_max;
            } else if (pitch < -pitch_max) {
                pitch = -pitch_max;
            }
            const auto rotation = QQuaternion::fromEulerAngles(pitch, yaw, roll);
            if (qIsNaN(rotation.lengthSquared())) {
                qCWarning(renderItemCategory) << "rotation versor contains NaN: reset to default";
            } else {
                if (!camera->setProperty("rotation", rotation)) {
                    qCCritical(renderItemCategory) << "unable to set 'rotation' property of 'camera'";
                }
            }
            //qDebug() << camera->property("rotation").value< QQuaternion >().toEulerAngles();
        }
        QCursor::setPos(startPos);
        event->accept();
    }
    if (!event->isAccepted()) {
        return QQuickFramebufferObject::mouseMoveEvent(event);
    }
}

void RenderItem::mouseReleaseEvent(QMouseEvent * event)
{
    pressedMouseButtons = event->buttons();
    switch (event->button()) {
    case Qt::MouseButton::LeftButton :
        unsetCursor();
        event->accept();
        break;
    default :
        break;
    }
    if (!event->isAccepted()) {
        return QQuickFramebufferObject::mouseReleaseEvent(event);
    }
}

void RenderItem::mouseDoubleClickEvent(QMouseEvent * event)
{
    switch (event->button()) {
    case Qt::MouseButton::LeftButton :
        event->accept();
        break;
    default :
        break;
    }
    if (!event->isAccepted()) {
        return QQuickFramebufferObject::mouseDoubleClickEvent(event);
    }
}

void RenderItem::hoverEnterEvent(QHoverEvent * event)
{
    return QQuickFramebufferObject::hoverEnterEvent(event);
}

void RenderItem::hoverMoveEvent(QHoverEvent * event)
{
    return QQuickFramebufferObject::hoverMoveEvent(event);
}

void RenderItem::hoverLeaveEvent(QHoverEvent * event)
{
    return QQuickFramebufferObject::hoverLeaveEvent(event);
}

void RenderItem::wheelEvent(QWheelEvent * event)
{
    qDebug() << Q_FUNC_INFO;
    return QQuickFramebufferObject::wheelEvent(event);
}
