#define _USE_MATH_DEFINES

#include "colourWheel.hpp"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <cmath>
#include <map>
#include <qsgsimpletexturenode.h>

#include <QDebug>
#include <QPair>

class ColourWheelRenderer : public QQuickFramebufferObject::Renderer,
                            protected QOpenGLExtraFunctions {
  public:
    void synchronize(QQuickFramebufferObject *item) override {
        auto wheel = static_cast<ColourWheel *>(item);
        handleBrightnessChanged(wheel->brightness());
        handleGreyFactorChanged(wheel->greyFactor());
        handleSpaceChanged(wheel->space());
    }

    void handleBrightnessChanged(float brightness) {
        if (brightness != m_brightness) {
            m_brightness = brightness;
            update();
        }
    }

    void handleGreyFactorChanged(float greyFactor) {
        if (greyFactor != m_greyFactor) {
            m_greyFactor = greyFactor;
            update();
        }
    }

    void handleSpaceChanged(ColourWheel::Space space) {
        if (space != m_space) {
            m_space = space;
            program.removeAllShaders();
            program.addCacheableShaderFromSourceFile(
                QOpenGLShader::Vertex, ":/shaders/colour_wheel_vertex.glsl");
            program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
                                                     m_lut.at(m_space));
            program.link();
            update();
        }
    }

    ColourWheelRenderer(float brightness, float greyFactor,
                        ColourWheel::Space space, size_t height)
        : m_brightness(brightness), m_greyFactor(greyFactor), m_space(space),
          m_lut({{ColourWheel::HSP, ":/shaders/hsp_wheel_fragment.glsl"},
                 {ColourWheel::LAB, ":/shaders/lch_wheel_fragment.glsl"}}),
          m_height(height) {
        initialise();
    }

    void initialise() {
        initializeOpenGLFunctions();
        program.addCacheableShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/shaders/colour_wheel_vertex.glsl");
        program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
                                                 m_lut.at(m_space));
        program.link();
        glGenVertexArrays(1, &VertexArrayName);
        glBindVertexArray(VertexArrayName);
        glBindVertexArray(0);
    }

    void render() override {
        glClearColor(0.0, 0.0, 0.0, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        program.bind();
        program.setUniformValue("brightness", m_brightness);
        program.setUniformValue("greyFactor", m_greyFactor);
        glBindVertexArray(VertexArrayName);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
        program.release();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

    QOpenGLFramebufferObject *
    createFramebufferObject(const QSize &size) override {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        m_fbo = new QOpenGLFramebufferObject(size, format);
        return m_fbo;
    }

  private:
    float m_brightness;
    float m_greyFactor;
    ColourWheel::Space m_space;
    QOpenGLShaderProgram program;
    GLuint VertexArrayName;
    const std::map<ColourWheel::Space, const char *> m_lut;
    QOpenGLFramebufferObject *m_fbo;
    size_t m_height;
};

QQuickFramebufferObject::Renderer *ColourWheel::createRenderer() const {
    auto renderer =
        new ColourWheelRenderer(m_brightness, m_greyFactor, m_space, height());
    return renderer;
}

float ColourWheel::brightness() const { return m_brightness; }

float ColourWheel::greyFactor() const { return m_greyFactor; }

ColourWheel::Space ColourWheel::space() const { return m_space; }

void ColourWheel::setBrightness(float brightness) {
    m_brightness = brightness;
    emit brightnessChanged(brightness);
    update();
}

void ColourWheel::setGreyFactor(float greyFactor) {
    m_greyFactor = greyFactor;
    emit greyFactorChanged(greyFactor);
    update();
}

void ColourWheel::setSpace(ColourWheel::Space space) {
    m_space = space;
    emit spaceChanged(space);
    update();
}

void ColourWheel::dirty() { update(); }
