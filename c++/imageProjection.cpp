#include "imageProjection.hpp"

#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <QDebug>

struct VertexData {
    float a;
    float b;
    float c;
};

class ImageProjectionRenderer : public QQuickFramebufferObject::Renderer,
                                protected QOpenGLExtraFunctions {
  public:
    ImageProjectionRenderer() {
        initializeOpenGLFunctions();
        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/shaders/image_projection_vertex.glsl");
        m_program->addCacheableShaderFromSourceFile(
            QOpenGLShader::Fragment,
            ":/shaders/image_projection_fragment.glsl");
        m_program->link();
    }

    virtual void synchronize(QQuickFramebufferObject *item) override {
        auto space = static_cast<ImageProjection *>(item)->space();
        if (space != m_space) {
            m_space = space;
            m_program->removeAllShaders();
            if (space == ColourWheel::Space::LAB) {
                m_program->addCacheableShaderFromSourceFile(
                    QOpenGLShader::Vertex,
                    ":/shaders/lch_image_projection_vertex.glsl");
            } else {
                m_program->addCacheableShaderFromSourceFile(
                    QOpenGLShader::Vertex,
                    ":/shaders/image_projection_vertex.glsl");
            }
            m_program->addCacheableShaderFromSourceFile(
                QOpenGLShader::Fragment,
                ":/shaders/image_projection_fragment.glsl");
            m_program->link();
        }
        auto image = static_cast<ImageProjection *>(item)->image();
        if (!image.isNull()) {
            if (image != m_image) {
                m_image = image.scaledToHeight(512);
                imageToBuffer();
            }
        }
        m_greyFactor = static_cast<ImageProjection *>(item)->greyFactor();
        update();
    }

    void render() override {
        // Clear
        glClear(GL_COLOR_BUFFER_BIT);

        // Render using our shader
        m_program->bind();
        {
            m_program->setUniformValue("greyFactor", m_greyFactor);
            m_object.bind();
            glDrawArrays(GL_POINTS, 0, 512 * 512);
            m_object.release();
        }
        m_program->release();
    }

    virtual QOpenGLFramebufferObject *
    createFramebufferObject(const QSize &size) override {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        return new QOpenGLFramebufferObject(size, format);
    }

    void imageToBuffer() {
        m_program->bind();
        std::vector<VertexData> vertices;
        for (auto i = 0; i < 512; i++) {
            for (auto j = 0; j < m_image.width(); j++) {
                auto colour = m_image.pixelColor(j, i);
                VertexData v{static_cast<float>(colour.redF()),
                             static_cast<float>(colour.greenF()),
                             static_cast<float>(colour.blueF())};
                vertices.push_back(v);
            }
        }

        if (!m_vertex.isCreated())
            m_vertex.create();
        m_vertex.bind();
        m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_vertex.allocate(vertices.data(),
                          sizeof(VertexData) * vertices.size());
        m_object.destroy();
        m_object.create();
        m_object.bind();
        m_program->enableAttributeArray(0);
        m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(VertexData));

        m_object.release();
        m_vertex.release();
        m_program->release();
    }

  private:
    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_object;
    QOpenGLShaderProgram *m_program;
    QImage m_image;
    ColourWheel::Space m_space;
    float m_greyFactor;
};

ColourWheel::Space ImageProjection::space() const { return m_space; }

QImage ImageProjection::image() const { return m_image; }

void ImageProjection::setSpace(Space space) {
    m_space = space;
    emit spaceChanged(space);
    update();
}

void ImageProjection::setImage(QImage image) {
    m_image = image.convertToFormat(QImage::Format_RGBX8888);
    emit imageChanged();
    update();
}

QQuickFramebufferObject::Renderer *ImageProjection::createRenderer() const {
    return new ImageProjectionRenderer();
}

void ImageProjection::setGreyFactor(float greyFactor) {
    m_greyFactor = greyFactor;
    update();
    emit greyFactorChanged();
}

float ImageProjection::greyFactor() const { return m_greyFactor; }
