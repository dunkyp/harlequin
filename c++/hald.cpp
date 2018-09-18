#include "hald.hpp"

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

#include <QImage>

#include <iostream>
#include <cmath>
#include <vector>

void HALDRenderer::handleClutChanged(QImage image) {
    // Check that image is square before doing this
    clut = image;
    m_clutTexture = textureFromHALDImage();
    update();
}

void HALDRenderer::handleSourceChanged(QImage image) {
    source = image;
    m_sourceTexture = textureFromSourceImage();
    update();
}

void HALDRenderer::handleXCutChanged(float x) {
    xCut = x;
}

void HALDRenderer::handleYCutChanged(float y) {
    yCut = y;
}


HALDRenderer::HALDRenderer(QImage source, QImage clut, float xCut, float yCut) : clut(clut), source(source),
                                                                                 xCut(xCut), yCut(yCut)
{
    initialise();
}

std::unique_ptr<QOpenGLTexture> HALDRenderer::textureFromHALDImage() {
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target3D);
    clut_data.resize(clut.width() * clut.width() * 3);
    float* data = clut_data.data();
    for(auto y = 0; y < clut.width(); y++) {
        for(auto x = 0; x < clut.width(); x++) {
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
    texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, (void*) clut_data.data());
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    return texture;
}

std::unique_ptr<QOpenGLTexture> HALDRenderer::textureFromSourceImage() {
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    texture->setData(source, QOpenGLTexture::GenerateMipMaps);
    texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture->setMinificationFilter(QOpenGLTexture::Nearest);
    return texture;
}

void HALDRenderer::initialise() {
    initializeOpenGLFunctions();
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/hald_vertex.glsl");
    program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/hald_fragment.glsl");
    program.link();
    m_sourceTexture = textureFromSourceImage();
    m_clutTexture = textureFromHALDImage();
    glGenVertexArrays(1, &VertexArrayName);
    glBindVertexArray(VertexArrayName);
    glBindVertexArray(0);
}
    
void HALDRenderer::render() {
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
    update();
}

QOpenGLFramebufferObject *HALDRenderer::createFramebufferObject(const QSize &size)  {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

QQuickFramebufferObject::Renderer *HALD::createRenderer() const
{
    auto renderer = new HALDRenderer(m_sourceImage, m_clutImage, xCut(), yCut());
    connect(this, &HALD::clutImageChanged, renderer, &HALDRenderer::handleClutChanged);
    connect(this, &HALD::sourceImageChanged, renderer, &HALDRenderer::handleSourceChanged);
    connect(this, &HALD::xCutChanged, renderer, &HALDRenderer::handleXCutChanged);
    connect(this, &HALD::yCutChanged, renderer, &HALDRenderer::handleYCutChanged);
    return renderer;
}


QUrl HALD::source() const {
    return m_source;
}

QUrl HALD::clut() const {
    return m_clut;
}

void HALD::setSource(QUrl image) {
    m_source = image;
    emit sourceChanged(image);
    m_sourceImage = QImage(m_source.path());
    emit sourceImageChanged(m_sourceImage);
}

void HALD::setClut(QUrl image) {
    m_clut = image;
    emit clutChanged(image);
    m_clutImage = QImage(m_clut.path());
    emit clutImageChanged(m_clutImage);
}


float HALD::xCut() const {
    return m_xCut;
}

float HALD::yCut() const {
    return m_yCut;
}

void HALD::setxCut(float x) {
    m_xCut = x;
    emit xCutChanged(x);
}

void HALD::setyCut(float y) {
    m_yCut = y;
    emit yCutChanged(y);
}

QImage HALD::sourceImage() const {
    return m_sourceImage;
}

QImage HALD::clutImage() const {
    return m_clutImage;
}
