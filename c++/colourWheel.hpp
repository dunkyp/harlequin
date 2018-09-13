#pragma once

#include <QQuickFramebufferObject>
#include <QVariantList>

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
    Renderer *createRenderer() const;
    

public slots:
    float brightness() const;
    Space space() const;
    void setBrightness(float brightness);
    void setSpace(Space space);

signals:
    void brightnessChanged(float);
    void spaceChanged(Space);
    void samplesChanged(const QVariantList&);

private:
    float m_brightness {70};
    Space m_space {LAB};
};

Q_DECLARE_METATYPE(ColourWheel::Space)
