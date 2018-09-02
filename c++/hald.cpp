#include "hald.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

#include <QImage>

#include <iostream>
#include <cmath>
#include <vector>

class HALDRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions, public QObject
{
public slots:
    void handleClutChanged(QUrl image) {
        // Check that image is square before doing this
        delete m_clutTexture;
        clut = image;
        m_clutTexture = textureFromHALDImage();
        update();
    }

    void handleSourceChanged(QUrl image) {
        delete m_sourceTexture;
        source = image;
        m_sourceTexture = textureFromSourceImage();
        update();
    }

public:
    HALDRenderer(QUrl source, QUrl clut) : clut(clut), source(source)
    {
        initialise();
    }

    QOpenGLTexture *textureFromHALDImage() {
        QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
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

    QOpenGLTexture *textureFromSourceImage() {
        auto texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture->setData(QImage(source.path()), QOpenGLTexture::DontGenerateMipMaps);
        texture->setMagnificationFilter(QOpenGLTexture::Nearest);
        texture->setMinificationFilter(QOpenGLTexture::Nearest);
        return texture;
    }

    void initialise() {
        initializeOpenGLFunctions();
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/hald_vertex.glsl");
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/hald_fragment.glsl");
        program.link();
        sourceUniformLocation = program.uniformLocation("source");
        clutUniformLocation = program.uniformLocation("clut");
        m_sourceTexture = textureFromSourceImage();
        m_clutTexture = textureFromHALDImage();
        glGenVertexArrays(1, &VertexArrayName);
        glBindVertexArray(VertexArrayName);
        glBindVertexArray(0);
    }
    
    void render() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        program.bind();
        glActiveTexture(GL_TEXTURE1);
        m_sourceTexture->bind();
        glActiveTexture(GL_TEXTURE0);
        m_clutTexture->bind();
        program.setUniformValue("clut", 0);
        program.setUniformValue("source", 1);
        glBindVertexArray(VertexArrayName);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
        program.release();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        update();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

private:
    QOpenGLShaderProgram program;
    int sourceUniformLocation;
    int clutUniformLocation;
    QOpenGLTexture *m_sourceTexture;
    QOpenGLTexture *m_clutTexture;
    GLuint VertexArrayName;
    QUrl clut;
    QUrl source;
    std::vector<float> clut_data;
};

QQuickFramebufferObject::Renderer *HALD::createRenderer() const
{
    auto renderer = new HALDRenderer(m_source, m_clut);
    connect(this, &HALD::clutChanged, renderer, &HALDRenderer::handleClutChanged);
    connect(this, &HALD::sourceChanged, renderer, &HALDRenderer::handleSourceChanged);
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
