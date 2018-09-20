#define _USE_MATH_DEFINES

#include "lut.hpp"
#include "colourWheel.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>
#include <qsgsimpletexturenode.h>
#include <map>
#include <cmath>

#include <QPair>

class ColourWheelRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLExtraFunctions, public QObject
{
public slots:
    void handleBrightnessChanged(float brightness) {
        m_brightness = brightness;
    }

    void handleSpaceChanged(ColourWheel::Space space) {
        m_space = space;
        program.removeAllShaders();
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/colour_wheel_vertex.glsl");
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, m_lut.at(m_space));
        program.link();
    }

    void handleInputImageChanged(const QImage image) {
        m_inputImageTexture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target3D);
        m_inputImageTexture->setFormat(QOpenGLTexture::R8I);
        m_inputImageTexture->setSize(64, 64, 64);
        m_inputImageTexture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Int8);
        m_inputImageTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
        m_inputImageTexture->setMagnificationFilter(QOpenGLTexture::Linear);
        // create a lookup for occupancy at each coordinate in RGB
        // texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, (void*) clut_data.data());
    }

public:
    ColourWheelRenderer(float brightness, ColourWheel::Space space, size_t height) : m_brightness(brightness), m_space(space),
                                                                      m_lut({{ColourWheel::HSP, ":/shaders/hsp_wheel_fragment.glsl"},
                                                                                  {ColourWheel::LAB, ":/shaders/lab_wheel_fragment.glsl"}}),
                                                                                     m_height(height)
{
        initialise();
    }

    void initialise() {
        initializeOpenGLFunctions();
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/colour_wheel_vertex.glsl");
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, m_lut.at(m_space));
        program.link();
        glGenVertexArrays(1, &VertexArrayName);
        glBindVertexArray(VertexArrayName);
        glBindVertexArray(0);
    }

    void render() override {
        glClearColor( 0.0, 0.0, 0.0, 0.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.bind();
        program.setUniformValue("brightness", m_brightness);
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
        m_fbo = new QOpenGLFramebufferObject(size, format);
        return m_fbo;
    }
    
private:
    float m_brightness;
    ColourWheel::Space m_space;
    QOpenGLShaderProgram program;
    GLuint VertexArrayName;
    const std::map<ColourWheel::Space, const char*> m_lut;
    QOpenGLFramebufferObject *m_fbo;
    size_t m_height;
    QImage m_inputImage;
    std::unique_ptr<QOpenGLTexture> m_inputImageTexture;
};

QQuickFramebufferObject::Renderer *ColourWheel::createRenderer() const {
    auto renderer = new ColourWheelRenderer(m_brightness, m_space, height());
    connect(this, &ColourWheel::brightnessChanged, renderer, &ColourWheelRenderer::handleBrightnessChanged);
    connect(this, &ColourWheel::spaceChanged, renderer, &ColourWheelRenderer::handleSpaceChanged);
    connect(this, &ColourWheel::samplesChanged, this, &ColourWheel::handleSamplesChanged);
    return renderer;
}


float ColourWheel::brightness() const {
    return m_brightness;
}

ColourWheel::Space ColourWheel::space() const {
    return m_space;
}

void ColourWheel::setBrightness(float brightness) {
    m_brightness = brightness;
    emit brightnessChanged(brightness);
}

void ColourWheel::setSpace(ColourWheel::Space space) {
    m_space = space;
    emit spaceChanged(space);
}

void ColourWheel::setInputImage(QImage image) {
    m_inputImage = image;
}

void ColourWheel::handleSamplesChanged(const QVariantList &list) {
    // this needs to deal with a vertical sample rate use a single vertical sample just now
    auto offsetSamples = std::make_shared<Lut::OffsetSamples>();
    float rings = std::sqrt(list.size() - 1);
    auto radius = (height() / 2.0);
    auto centroid = QPoint(radius, radius);
    for(auto ring = 0; ring < rings; ring++) {
        for(auto i = 0; i < rings; i++) {
            auto initialX = std::cos(((M_PI * 2) / rings) * i) * (radius / rings * (ring + 1)) + radius;
            auto initialY = std::cos(((M_PI * 2) / rings) * i) * (radius / rings * (ring + 1)) + radius;
            auto point =  list[ring * rings + i].toList();
            auto newX = point[0].toDouble();
            auto newY = point[1].toDouble();
            offsetSamples->push_back({{0, initialX, initialY}, {0, newX, newY}});
        }
    }
    emit offsetSamplesChanged(offsetSamples);
}
