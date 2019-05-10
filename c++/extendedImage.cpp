#include "extendedImage.hpp"

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

#include <QImage>

#include <cmath>
#include <iostream>
#include <vector>

class ExtendedImageRenderer : public QQuickFramebufferObject::Renderer,
                              protected QOpenGLExtraFunctions {
  public:
    void synchronize(QQuickFramebufferObject *item) override {
        auto result = static_cast<ExtendedImage *>(item);
        handleSourceChanged(result->sourceImage());
    }

    void handleSourceChanged(QImage);

    ExtendedImageRenderer(QImage source);
    void render() override;
    QOpenGLFramebufferObject *
    createFramebufferObject(const QSize &size) override;

  private:
    std::unique_ptr<QOpenGLTexture> textureFromSourceImage();
    void initialise();

    QOpenGLShaderProgram program;
    std::unique_ptr<QOpenGLTexture> m_sourceTexture;
    GLuint VertexArrayName;
    QImage m_source;
};

void ExtendedImageRenderer::handleSourceChanged(QImage image) {
    m_source = image;
    m_sourceTexture = textureFromSourceImage();
    update();
}

ExtendedImageRenderer::ExtendedImageRenderer(QImage source) : m_source(source) {
    initialise();
}

std::unique_ptr<QOpenGLTexture>
ExtendedImageRenderer::textureFromSourceImage() {
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    texture->setData(m_source, QOpenGLTexture::GenerateMipMaps);
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    return texture;
}

void ExtendedImageRenderer::initialise() {
    initializeOpenGLFunctions();
    program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Vertex, ":/shaders/extended_image_vertex.glsl");
    program.addCacheableShaderFromSourceFile(
        QOpenGLShader::Fragment, ":/shaders/extended_image_fragment.glsl");
    program.link();
    m_sourceTexture = textureFromSourceImage();
    glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
    glBindVertexArray(0);
}

void ExtendedImageRenderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    program.bind();
    glActiveTexture(GL_TEXTURE0);
    m_sourceTexture->bind();
    program.setUniformValue("source", 0);
    glBindVertexArray(VertexArrayName);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
    program.release();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

QOpenGLFramebufferObject *
ExtendedImageRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

QQuickFramebufferObject::Renderer *ExtendedImage::createRenderer() const {
    auto renderer = new ExtendedImageRenderer(m_sourceImage);
    return renderer;
}

QUrl ExtendedImage::source() const { return m_source; }

namespace {
QString cleanPath(QUrl in) {
    QString path;
    if (in.scheme() == "qrc") {
        path = ":" + in.path();
    } else {
        path = in.path();
    }
    return path;
}
} // namespace

void ExtendedImage::setSource(QUrl image) {
    m_source = image;
    emit sourceChanged(image);
    QString path = cleanPath(m_source);
    m_sourceImage = QImage(path);
    emit sourceImageChanged(m_sourceImage);
    emit sourceHeightChanged();
    emit sourceWidthChanged();
    update();
}

QImage ExtendedImage::sourceImage() const { return m_sourceImage; }

int ExtendedImage::sourceHeight() const { return m_sourceImage.height(); }

int ExtendedImage::sourceWidth() const { return m_sourceImage.width(); }

QColor ExtendedImage::color(int x, int y) const { return m_sourceImage.pixelColor(x, y); }
