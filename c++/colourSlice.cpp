#include "colourSlice.hpp"
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QDebug>

class ColourSliceRenderer : public QQuickFramebufferObject::Renderer,
                            protected QOpenGLExtraFunctions {
public:
    void synchronize(QQuickFramebufferObject *item) override {
        auto slice = static_cast<ColourSlice *>(item);
        handleChromaChanged(slice->property("chroma").toFloat());
        handleHueChanged(slice->property("hue").toFloat());
        handleSpaceChanged(slice->property("space").value<ColourWheel::Space>());
    }

    void handleChromaChanged(float chroma) {
        if(chroma != m_chroma) {
            m_chroma = chroma;
            update();
        }
    }

    void handleHueChanged(float hue) {
        if(hue != m_hue) {
            m_hue = hue;
            update();
        }
    }

    void handleSpaceChanged(ColourWheel::Space space) {
        if(space != m_space) {
            program.removeAllShaders();
            program.addCacheableShaderFromSourceFile(
                                                     QOpenGLShader::Vertex, ":/shaders/colour_wheel_vertex.glsl");
            program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,
                                                     m_lut.at(m_space));
            program.link();
            update();
        }
    }

    ColourSliceRenderer() {
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
        program.setUniformValue("chroma", m_chroma);
        program.setUniformValue("greyFactor", m_greyFactor);
        program.setUniformValue("hue", m_hue);
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
    float m_chroma;
    float m_greyFactor { 1 };
    float m_hue;
    QOpenGLShaderProgram program;
    GLuint VertexArrayName;
    const std::map<ColourWheel::Space, const char *> m_lut {{ColourWheel::HSP, ":/shaders/hsp_slice_fragment.glsl"},
                                                            {ColourWheel::LAB, ":/shaders/lch_slice_fragment.glsl"}};
    QOpenGLFramebufferObject *m_fbo;
    ColourWheel::Space m_space;
};

QQuickFramebufferObject::Renderer *ColourSlice::createRenderer() const {
    return new ColourSliceRenderer();
}
