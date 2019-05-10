#include "resultImage.hpp"

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

#include <QImage>

#include <cmath>
#include <iostream>
#include <vector>

class ResultImageRenderer : public QQuickFramebufferObject::Renderer,
                            protected QOpenGLExtraFunctions {
  public:
    void synchronize(QQuickFramebufferObject *item) override {
        auto result = static_cast<ResultImage *>(item);
        handleClutChanged(result->clutImage());
        handleSourceChanged(result->sourceImage());
        handleXCutChanged(result->xCut());
        handleYCutChanged(result->yCut());
    }

    void handleClutChanged(QImage);
    void handleSourceChanged(QImage);
    void handleXCutChanged(float x);
    void handleYCutChanged(float y);
    ResultImageRenderer(QImage source, QImage clut, float xCut, float yCut);
    void render() override;
    QOpenGLFramebufferObject *
    createFramebufferObject(const QSize &size) override;

  private:
    std::unique_ptr<QOpenGLTexture> textureFromResultImageImage();
    std::unique_ptr<QOpenGLTexture> textureFromSourceImage();
    void initialise();

    QOpenGLShaderProgram program;
    std::unique_ptr<QOpenGLTexture> m_sourceTexture;
    std::unique_ptr<QOpenGLTexture> m_clutTexture;
    GLuint VertexArrayName;
    QImage clut;
    QImage source;
    float xCut{0};
    float yCut{0};
    std::vector<float> clut_data;
};

void ResultImageRenderer::handleClutChanged(QImage image) {
    // Check that image is square before doing this
    clut = image;
    m_clutTexture = textureFromResultImageImage();
    update();
}

void ResultImageRenderer::handleSourceChanged(QImage image) {
    source = image;
    m_sourceTexture = textureFromSourceImage();
    update();
}

void ResultImageRenderer::handleXCutChanged(float x) {
    xCut = x;
    update();
}

void ResultImageRenderer::handleYCutChanged(float y) {
    yCut = y;
    update();
}

ResultImageRenderer::ResultImageRenderer(QImage source, QImage clut, float xCut,
                                         float yCut)
    : clut(clut), source(source), xCut(xCut), yCut(yCut) {
    initialise();
}

std::unique_ptr<QOpenGLTexture>
ResultImageRenderer::textureFromResultImageImage() {
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target3D);
    clut_data.resize(clut.width() * clut.width() * 3);
    float *data = clut_data.data();
    for (auto y = 0; y < clut.width(); y++) {
        for (auto x = 0; x < clut.width(); x++) {
            auto rgb = clut.pixel(x, y);
            auto red = qRed(rgb) / 255.0;
            auto green = qGreen(rgb) / 255.0;
            auto blue = qBlue(rgb) / 255.0;
            *data++ = red;
            *data++ = green;
            *data++ = blue;
        }
    }
    int level = std::cbrt(clut.width() * clut.width());
    texture->setSize(level, level, level);
    texture->setFormat(QOpenGLTexture::RGB32F);
    texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32,
                     (const void *)clut_data.data());
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    return texture;
}

std::unique_ptr<QOpenGLTexture> ResultImageRenderer::textureFromSourceImage() {
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    texture->setData(source, QOpenGLTexture::GenerateMipMaps);
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    return texture;
}

void ResultImageRenderer::initialise() {
    initializeOpenGLFunctions();
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,
                                             ":/shaders/hald_vertex.glsl");
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
                                             ":/shaders/hald_fragment.glsl");
    program.link();
    m_sourceTexture = textureFromSourceImage();
    m_clutTexture = textureFromResultImageImage();
    glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
    glBindVertexArray(0);
}

void ResultImageRenderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    program.bind();
    glActiveTexture(GL_TEXTURE1);
    m_sourceTexture->bind();
    glActiveTexture(GL_TEXTURE0);
    m_clutTexture->bind();
    program.setUniformValue("clut", 0);
    program.setUniformValue("source", 1);
    program.setUniformValue("xCut", xCut);
    program.setUniformValue("yCut", yCut);
    glBindVertexArray(VertexArrayName);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
    program.release();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

QOpenGLFramebufferObject *
ResultImageRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

QQuickFramebufferObject::Renderer *ResultImage::createRenderer() const {
    auto renderer =
        new ResultImageRenderer(m_sourceImage, m_clutImage, xCut(), yCut());
    return renderer;
}

QUrl ResultImage::source() const { return m_source; }

QUrl ResultImage::clut() const { return m_clut; }

QString cleanPath(QUrl in) {
    QString path;
    if (in.scheme() == "qrc") {
        path = ":" + in.path();
    } else {
        path = in.path();
    }
    return path;
}

void ResultImage::setSource(QUrl image) {
    m_source = image;
    emit sourceChanged(image);
    QString path = cleanPath(m_source);
    m_sourceImage = QImage(path);
    emit sourceImageChanged(m_sourceImage);
    update();
    m_sourceHistogram = new Histogram(m_sourceImage, this);
}

void ResultImage::setClut(QUrl image) {
    m_clut = image;
    emit clutChanged(image);
    QString path = cleanPath(m_clut);
    m_clutImage = QImage(path);
    emit clutImageChanged(m_clutImage);
    update();
}

float ResultImage::xCut() const { return m_xCut; }

float ResultImage::yCut() const { return m_yCut; }

void ResultImage::setxCut(float x) {
    m_xCut = x;
    emit xCutChanged(x);
    update();
}

void ResultImage::setyCut(float y) {
    m_yCut = y;
    emit yCutChanged(y);
    update();
}

QImage ResultImage::sourceImage() const { return m_sourceImage; }

QImage ResultImage::clutImage() const { return m_clutImage; }

Histogram *ResultImage::histogram() { return m_sourceHistogram; }
