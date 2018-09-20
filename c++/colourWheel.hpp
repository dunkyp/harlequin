#pragma once

#include "lut.hpp"
#include <QQuickFramebufferObject>
#include <QVariantList>
#include <QImage>
#include <QOpenGLTexture>
#include <memory>

class ColourWheel : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    enum Space {
        HSP,
        LAB
    };
    Q_ENUM(Space);
    Q_PROPERTY(float brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(Space space READ space WRITE setSpace NOTIFY spaceChanged)
    Renderer *createRenderer() const override;
    

public slots:
    float brightness() const;
    Space space() const;
    void setBrightness(float brightness);
    void setSpace(Space space);
    void setInputImage(QImage image);

signals:
    void brightnessChanged(float);
    void spaceChanged(Space);
    void samplesChanged(const QVariantList&);
    void inputImageChanged();
    void offsetSamplesChanged(std::shared_ptr<Lut::OffsetSamples>);

private:
    float m_brightness {70};
    Space m_space {LAB};
    QImage m_inputImage;

private slots:
    void handleSamplesChanged(const QVariantList&);
};

Q_DECLARE_METATYPE(ColourWheel::Space)
