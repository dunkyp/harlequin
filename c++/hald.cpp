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

class HALDRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions
{
public:
    HALDRenderer(QUrl source, QUrl clut) : clut(clut), source(source)
    {
        initialise();
    }

    QOpenGLTexture *textureFromHALDImage() {
        QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
        QImage clut_image(clut.path());
        float* data = new float[clut_image.width() * clut_image.width() * 3];
        auto array = data;
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
        texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, (void*) array);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        return texture;
    }

    void initialise() {
        initializeOpenGLFunctions();
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/hald_vertex.glsl");
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/hald_fragment.glsl");
        program.link();
        sourceUniformLocation = program.uniformLocation("source");
        clutUniformLocation = program.uniformLocation("clut");
        m_sourceTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_sourceTexture->setData(QImage(source.path()), QOpenGLTexture::DontGenerateMipMaps);
        m_sourceTexture->setMagnificationFilter(QOpenGLTexture::Nearest);
        m_sourceTexture->setMinificationFilter(QOpenGLTexture::Nearest);
        
        // auto level = 64;
        // auto cube_size = level * level;
        // auto data_size = level * level * level;
        // auto storage_size = data_size * 3;
        // float *data = new float[storage_size];
        // float *array = data;
        // for(auto blue = 0; blue < level; blue++) {
        //     for(auto green  = 0; green < level; green++) {
        //         for(auto red = 0; red < level; red++) {
        //             auto r = (float)red / (float)(level - 1);
        //             auto g = (float)green / (float)(level - 1);
        //             auto b = (float)blue / (float)(level - 1);
        //             *data++ = r;
        //             *data++ = g;
        //             *data++ = b;
        //         }
        //     }
        // }
        // uchar *image_data = new uchar[storage_size];
        // for(auto i = 0; i < storage_size; i += 3) {
        //     image_data[i] = (int)array[i] * 255.0;
        //     image_data[i+1] = (int)array[i+1] * 255.0;
        //     image_data[i+2] = (int)array[i+2] * 255.0;
        // }
        //QImage image((uchar*)image_data, 10, 10, QImage::Format_RGB888);
        //auto saved = image.save("clut.png");
        // m_clutTexture = new QOpenGLTexture(QOpenGLTexture::Target3D);
        // m_clutTexture->setSize(level, level, level);
        // m_clutTexture->setFormat(QOpenGLTexture::RGB32F);
        // m_clutTexture->allocateStorage(QOpenGLTexture::RGB, QOpenGLTexture::Float32);
        // m_clutTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
        // m_clutTexture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, (void*) array);
        // m_clutTexture->setMagnificationFilter(QOpenGLTexture::Linear);
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
};

QQuickFramebufferObject::Renderer *HALD::createRenderer() const
{
    return new HALDRenderer(m_source, m_clut);
}


QUrl HALD::source() const {
    return m_source;
}

QUrl HALD::clut() const {
    return m_clut;
}

void HALD::setSource(QUrl image) {
    m_source = image;
    emit sourceChanged();
}

void HALD::setClut(QUrl image) {
    m_clut = image;
    emit clutChanged();
}
