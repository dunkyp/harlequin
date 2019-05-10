#pragma once

#include "colourWheel.hpp"
#include <QQuickFramebufferObject>

class ColourSlice : public QQuickFramebufferObject {
    Q_OBJECT

public:
    explicit ColourSlice() {
        connect(this, &ColourSlice::hueChanged,
                this, &ColourSlice::update);
        connect(this, &ColourSlice::chromaChanged,
                this, &ColourSlice::update);
        connect(this, &ColourSlice::spaceChanged,
                this, &ColourSlice::update);
    }
    Q_PROPERTY(ColourWheel::Space space MEMBER m_space NOTIFY spaceChanged)
    Q_PROPERTY(float percentage MEMBER m_percentage NOTIFY percentageChanged)
    Q_PROPERTY(float greyFactor MEMBER m_greyFactor NOTIFY greyFactorChanged)
    Q_PROPERTY(float hue MEMBER m_hue NOTIFY hueChanged)
    Q_PROPERTY(float chroma MEMBER m_chroma NOTIFY chromaChanged)
    Renderer *createRenderer() const override;

signals:
    void spaceChanged(ColourWheel::Space);
    void percentageChanged(float);
    void greyFactorChanged(float);
    void hueChanged(float);
    void chromaChanged(float);

private:
    ColourWheel::Space m_space;
    float m_percentage;
    float m_greyFactor;
    float m_hue { 0.5 };
    float m_chroma { 0.5 };
};
