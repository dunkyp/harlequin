#pragma once

#include <QOpenGLTexture>
#include <QQuickFramebufferObject>

class ColourWheel : public QQuickFramebufferObject {
    Q_OBJECT
  public:
    enum Space { HSP, LAB };
    Q_ENUM(Space);
    Q_PROPERTY(float brightness READ brightness WRITE setBrightness NOTIFY
                   brightnessChanged)
    Q_PROPERTY(float greyFactor READ greyFactor WRITE setGreyFactor NOTIFY
                   greyFactorChanged)
    Q_PROPERTY(Space space READ space WRITE setSpace NOTIFY spaceChanged)
    Renderer *createRenderer() const override;

  public slots:
    float brightness() const;
    float greyFactor() const;
    Space space() const;
    void setBrightness(float brightness);
    void setGreyFactor(float greyFactor);
    void setSpace(Space space);
    Q_INVOKABLE void dirty();

  signals:
    void brightnessChanged(float);
    void greyFactorChanged(float);
    void spaceChanged(Space);
    void samplesChanged(const QVariantList &);

  private:
    float m_brightness{70};
    float m_greyFactor{1};
    Space m_space{LAB};
};

Q_DECLARE_METATYPE(ColourWheel::Space)
