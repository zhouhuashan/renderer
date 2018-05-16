#pragma once

#include "engine.hpp"

#include <QtQuick>

Q_DECLARE_LOGGING_CATEGORY(frameBufferRendererCategory)

class FrameBufferRenderer
        : public QQuickFramebufferObject::Renderer
{

    bool autoRefresh;
    Engine engine;

    QPointer< QObject > rendererInterface = Q_NULLPTR;

    QElapsedTimer elapsedTimer;

public :

    FrameBufferRenderer(bool autoRefresh, QUrl source);

    QOpenGLFramebufferObject * createFramebufferObject(const QSize & size) Q_DECL_OVERRIDE;
    void synchronize(QQuickFramebufferObject * const renderItem) Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

};
