#pragma once

#include "colourWheel.hpp"
#include <QImage>
#include <QQuickFramebufferObject>

class ImageProjection : public QQuickFramebufferObject {
    using Space = ColourWheel::Space;
    Q_OBJECT
  public:
    Q_PROPERTY(Space space READ space WRITE setSpace NOTIFY spaceChanged)
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(float greyFactor READ greyFactor WRITE setGreyFactor NOTIFY
                   greyFactorChanged)
    Renderer *createRenderer() const override;

  public slots:
    Space space() const;
    QImage image() const;
    float greyFactor() const;
    void setSpace(Space space);
    void setImage(QImage image);
    void setGreyFactor(float greyFactor);

  signals:
    void spaceChanged(Space);
    void imageChanged();
    void greyFactorChanged();

  private:
    QImage m_image;
    Space m_space;
    float m_greyFactor;
};
