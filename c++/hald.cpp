#include "hald.hpp"

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

#include <QImage>

#include <iostream>
#include <cmath>
#include <vector>

void HALDRenderer::handleClutChanged(QUrl image) {
    // Check that image is square before doing this
    clut = image;
    m_clutTexture = textureFromHALDImage();
    update();
}

void HALDRenderer::handleSourceChanged(QUrl image) {
    source = image;
    m_sourceTexture = textureFromSourceImage();
    update();
    emit sourceTextureChanged(m_sourceTexture.get());
}

void HALDRenderer::handleXCutChanged(float x) {
    x_cut = x;
}

void HALDRenderer::handleYCutChanged(float y) {
    y_cut = y;
}


HALDRenderer::HALDRenderer(QUrl source, QUrl clut, float x_cut, float y_cut) : clut(clut), source(source),
                                                                              x_cut(x_cut), y_cut(y_cut)
{
    initialise();
}

std::shared_ptr<QOpenGLTexture> HALDRenderer::textureFromHALDImage() {
    auto texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target3D);
    QImage clut_image(clut.path());
    clut_data.resize(clut_image.width() * clut_image.width() * 3);
    float* data = clut_data.data();
    for(auto y = 0; y < clut_image.width(); y++) {
        for(auto x = 0; x < clut_image.width(); x++) {
            auto rgb = clut_image.pixel(x, y);
            auto red = qRed(rgb) / 255.0;
            auto green = qGreen(rgb) / 255.0;
            auto blue = qBlue(rgb) / 255.0;
            *data++ = red;
            *data++ = green;
            *data++ = blue;
        }
    }
    int level = std::cbrt(clut_image.width() * clut_image.width());
    texture->setSize(level, level, level);
    texture->setFormat(QOpenGLTexture::RGB32F);
    texture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
    texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, (void*) clut_data.data());
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    return texture;
}

std::shared_ptr<QOpenGLTexture> HALDRenderer::textureFromSourceImage() {
    auto texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D);
    texture->setData(QImage(source.path()), QOpenGLTexture::DontGenerateMipMaps);
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
    program.setUniformValue("x_cut", x_cut);
    program.setUniformValue("y_cut", y_cut);
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
    auto renderer = new HALDRenderer(m_source, m_clut, x_cut(), y_cut());
    connect(this, &HALD::clutChanged, renderer, &HALDRenderer::handleClutChanged);
    connect(this, &HALD::sourceChanged, renderer, &HALDRenderer::handleSourceChanged);
    connect(this, &HALD::x_cutChanged, renderer, &HALDRenderer::handleXCutChanged);
    connect(this, &HALD::y_cutChanged, renderer, &HALDRenderer::handleYCutChanged);
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
}

void HALD::setClut(QUrl image) {
    m_clut = image;
    emit clutChanged(image);
}


float HALD::x_cut() const {
    return m_x_cut;
}

float HALD::y_cut() const {
    return m_y_cut;
}

void HALD::setx_cut(float x) {
    m_x_cut = x;
    emit x_cutChanged(x);
}

void HALD::sety_cut(float y) {
    m_y_cut = y;
    emit y_cutChanged(y);
}
