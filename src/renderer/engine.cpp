#include "engine.hpp"

#include "rt.cuh"

#include <utility>

Q_LOGGING_CATEGORY(engineCategory, "engine")

static constexpr auto vert = "attribute mediump vec2 position;\n"
                             "varying mediump vec2 texcoord;\n"
                             "void main()\n"
                             "{\n"
                             "    gl_Position = vec4(position.xy, 0.0, 1.0);\n"
                             "    texcoord = (position + 1.0) / 2.0;\n"
                             "}\n";
static constexpr auto frag = "uniform sampler2D texture;\n"
                             "varying mediump vec2 texcoord;\n"
                             "void main()\n"
                             "{\n"
                             "    gl_FragColor = texture2D(texture, texcoord);\n"
                             "}\n";

static const QVector< QVector2D > triangle = {{-1.0f, -1.0f}, {3.0f, -1.0f}, {-1.0f, 3.0f}};

inline
QVector3D rotatedVector(const QQuaternion & q, QVector3D v)
{
    auto vector = q.vector();
    auto cross = QVector3D::crossProduct(vector, v);
    cross += cross;
    v += q.scalar() * cross;
    v += QVector3D::crossProduct(qMove(vector), cross);
    return v;
}

bool Engine::map()
{
    Q_ASSERT(!f);
    f = sourceFile.map(0, sourceFile.size(), QFile::MapPrivateOption);
    if (!f) {
        qCWarning(engineCategory) << QStringLiteral("unable to map file %1 to memory").arg(sourceFile.fileName());
        return false;
    }
    scene = CUDA_registerBuffer(f, sourceFile.size());
    return true;
}

bool Engine::unmap()
{
    if (!f) {
        return true;
    }
    scene = Q_NULLPTR;
    bool success = true;
    if (!CUDA_unregisterBuffer(f)) {
        qCCritical(engineCategory) << QStringLiteral("unable to unregister memory mapped buffer for file %1").arg(sourceFile.fileName());
        success = false;
    }
    if (!sourceFile.unmap(std::exchange(f, Q_NULLPTR))) {
        qCCritical(engineCategory) << QStringLiteral("unable to unmap file %1").arg(sourceFile.fileName());
        success = false;
    }
    return success;
}

Engine::Engine(QUrl source)
{
    initializeOpenGLFunctions();
    if (!program.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vert)) {
        qCCritical(engineCategory);
    }
    if (!program.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, frag)) {
        qCCritical(engineCategory);
    }
    if (!program.link()) {
        qCCritical(engineCategory);
    }
    vertexLocation = program.attributeLocation("position");
    Q_ASSERT(!(vertexLocation < 0));
    textureLocation = program.uniformLocation("texture");
    Q_ASSERT(!(textureLocation < 0));
    if (!pixelUnpackBuffer.create()) {
        qCCritical(engineCategory);
    }
    {
        if (!vbo.create()) {
            qCCritical(engineCategory);
        }
        if (!vao.create()) {
            qCCritical(engineCategory);
        }
        QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
        if (!vbo.bind()) {
            qCCritical(engineCategory);
        }
        vbo.allocate(2 * sizeof(GLfloat) * triangle.size());
        vbo.write(0, triangle.constData(), vbo.size());
        program.enableAttributeArray(vertexLocation);
        program.setAttributeArray(vertexLocation, (const GLfloat *)Q_NULLPTR, 2);
        vbo.release();
    }
    if (!CUDA_init()) {
        qCCritical(engineCategory);
    }
    setSource(source);
}

Engine::~Engine()
{
    unmap();
    if (cudaBuf) {
        if (!CUDA_unregisterGLBuffer(cudaBuf)) {
            qCCritical(engineCategory());
        }
    }
    vao.destroy();
    vbo.destroy();
    if (texture.isCreated()) {
        texture.destroy();
    }
    pixelUnpackBuffer.destroy();
}

void Engine::init(const QSize & size)
{
    if (cudaBuf) {
        if (!CUDA_unregisterGLBuffer(cudaBuf)) {
            qCCritical(engineCategory);
        }
    }
    if (texture.isCreated()) {
        texture.destroy();
        if (!texture.create()) {
            qCCritical(engineCategory);
        }
    }
    texture.setFormat(QOpenGLTexture::TextureFormat::RGB32F);
    texture.setSize(size.width(), size.height());
    texture.setAutoMipMapGenerationEnabled(true);
    texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
    texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture.allocateStorage(QOpenGLTexture::PixelFormat::RGB, QOpenGLTexture::PixelType::Float32);
    if (!pixelUnpackBuffer.bind()) {
        qCCritical(engineCategory);
    }
    pixelUnpackBuffer.allocate(texture.width() * texture.height() * 3 * sizeof(GLfloat));
    cudaBuf = CUDA_registerGLBuffer(pixelUnpackBuffer.bufferId());
    Q_ASSERT(cudaBuf);
    pixelUnpackBuffer.release();
}

void Engine::render()
{
    Q_ASSERT(cudaBuf);
    if (!CUDA_render(cudaBuf, scene, texture.width(), texture.height())) {
        qCCritical(engineCategory);
    }
    if (!program.bind()) {
        qCCritical(engineCategory);
    }
    if (!pixelUnpackBuffer.bind()) {
        qCCritical(engineCategory);
    }
    texture.bind();
    texture.setData(QOpenGLTexture::PixelFormat::RGB, QOpenGLTexture::PixelType::Float32, static_cast< const void * >(Q_NULLPTR));
    program.setUniformValue(textureLocation, 0);
    {
        QOpenGLVertexArrayObject::Binder vaoBinder(&vao);
        glDrawArrays(GL_TRIANGLES, 0, triangle.size());
    }
    texture.release();
    pixelUnpackBuffer.release();
    program.release();
}

void Engine::setTransformationMatrix(const QMatrix4x4 & transformationMatrix)
{
    QMatrix4x4 viewport;
    viewport.viewport(0, 0, texture.width(), texture.height());
    bool invertible = false;
    viewport *= transformationMatrix.inverted(&invertible);
    if (!invertible) {
        return;
    }
    inverseTransformationMatrix = qMove(viewport);
}

bool Engine::setSource(QUrl source)
{
    if (this->source == source) {
        return true;
    }
    this->source = source;
    if (sourceFile.isOpen()) {
        sourceFile.close();
        qCInfo(engineCategory) << QStringLiteral("file %1 is closed").arg(sourceFile.fileName());
        if (!unmap()) {
            return false;
        }
    }
    if (source.isEmpty()) {
        return true;
    }
    if (!source.isValid()) {
        qCWarning(engineCategory) << QStringLiteral("URL %1 is invalid").arg(source.toString());
        return false;
    }
    if (!source.isLocalFile()) {
        qCWarning(engineCategory) << QStringLiteral("URL %1 is not local file").arg(source.toString());
        return false;
    }
    sourceFile.setFileName(source.toLocalFile());
    if (!sourceFile.open(QFile::ReadOnly)) {
        qCWarning(engineCategory) << QStringLiteral("unable to open file %1 to read").arg(sourceFile.fileName());
        return false;
    }
    if (!map()) {
        return false;
    }
    qCInfo(engineCategory) << QStringLiteral("file %1 is open").arg(sourceFile.fileName());
    return true;
}
