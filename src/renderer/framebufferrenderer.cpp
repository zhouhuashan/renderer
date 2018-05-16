#include "framebufferrenderer.hpp"

Q_LOGGING_CATEGORY(frameBufferRendererCategory, "frameBufferRenderer")

FrameBufferRenderer::FrameBufferRenderer(bool autoRefresh, QUrl source)
    : autoRefresh{autoRefresh}
    , engine{source}
{ ; }

QOpenGLFramebufferObject * FrameBufferRenderer::createFramebufferObject(const QSize & size)
{
    QOpenGLFramebufferObjectFormat format;
    const auto fbo = new (std::nothrow) QOpenGLFramebufferObject{size, format};
    Q_CHECK_PTR(fbo);
    engine.init(fbo->size());
    return fbo;
}

void FrameBufferRenderer::synchronize(QQuickFramebufferObject * const renderItem)
{
    rendererInterface = renderItem->property("renderer").value< QObject * >();
    Q_CHECK_PTR(rendererInterface);
    autoRefresh = rendererInterface->property("autoRefresh").toBool();
    engine.setTransformationMatrix(renderItem->property("camera").value< QObject * >()->property("transformationMatrix").value< QMatrix4x4 >());
    engine.setSource(renderItem->property("source").toUrl());
}

void FrameBufferRenderer::render()
{
    elapsedTimer.start();
    engine.render();
    if (rendererInterface) {
        if (!QMetaObject::invokeMethod(rendererInterface, "updateProperty", Q_ARG(QString, "dt"), Q_ARG(QVariant, float(elapsedTimer.nsecsElapsed() * 1E-9)))) {
            qCCritical(frameBufferRendererCategory);
        }
    }
    if (autoRefresh) {
        update();
    }
}

