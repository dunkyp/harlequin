#include "colourWheel.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>
#include <qsgsimpletexturenode.h>
#include <map>

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

public:
    ColourWheelRenderer(float brightness, ColourWheel::Space space) : m_brightness(brightness), m_space(space),
                                                                      m_lut({{ColourWheel::HSP, ":/shaders/hsp_wheel_fragment.glsl"},
                                                                                  {ColourWheel::LAB, ":/shaders/lab_wheel_fragment.glsl"}})
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
        return new QOpenGLFramebufferObject(size, format);
    }
    
private:
    float m_brightness;
    ColourWheel::Space m_space;
    QOpenGLShaderProgram program;
    GLuint VertexArrayName;
    const std::map<ColourWheel::Space, const char*> m_lut;
};

QQuickFramebufferObject::Renderer *ColourWheel::createRenderer() const {
    auto renderer = new ColourWheelRenderer(m_brightness, m_space);
    connect(this, &ColourWheel::brightnessChanged, renderer, &ColourWheelRenderer::handleBrightnessChanged);
    connect(this, &ColourWheel::spaceChanged, renderer, &ColourWheelRenderer::handleSpaceChanged);
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
