#pragma once

#include <QtGui>

Q_DECLARE_LOGGING_CATEGORY(engineCategory)

class Engine
        : protected QOpenGLFunctions
{

    QOpenGLShaderProgram program;

    int vertexLocation = -1;
    int textureLocation = -1;

    QOpenGLBuffer vbo;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer pixelUnpackBuffer{QOpenGLBuffer::PixelUnpackBuffer};
    QOpenGLTexture texture{QOpenGLTexture::Target2D};
    void * cudaBuf = Q_NULLPTR;

    QMatrix4x4 inverseTransformationMatrix;
    QUrl source;
    QFile sourceFile;
    uchar * f = Q_NULLPTR;
    void * scene = Q_NULLPTR;

    bool map();
    bool unmap();

public :

    Engine(QUrl source);
    ~Engine();

    void init(const QSize & size);
    void render();

    void setTransformationMatrix(const QMatrix4x4 & transformationMatrix);
    bool setSource(QUrl source);

};
