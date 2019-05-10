#pragma once

#include "colourWheel.hpp"
#include "lut.hpp"

#include <QObject>

#include <memory>

class HaldGenerator : public QObject {
    Q_OBJECT
  public:
    Q_PROPERTY(QImage hald READ hald NOTIFY haldChanged)
    Q_PROPERTY(ColourWheel::Space space READ space NOTIFY spaceChanged)

  public slots:
    ColourWheel::Space space() const;
    QImage hald() const;
    void setLut(std::shared_ptr<Lut> lut);
    void handleSpaceChanged(ColourWheel::Space space);
    void handleSamplesChanged(std::shared_ptr<Lut::OffsetSamples> samples);

  signals:
    void spaceChanged(ColourWheel::Space space);
    void haldChanged(QImage image);

  private:
    void generateHald(std::shared_ptr<Lut::OffsetSamples> samples);

    std::shared_ptr<Lut> m_lut;
    ColourWheel::Space m_space;
    QImage m_hald;
};
