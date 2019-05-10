#include "haldGenerator.hpp"
#include "hspLut.hpp"
#include "labLut.hpp"
#include "rgbLut.hpp"

#include <QDebug>

ColourWheel::Space HaldGenerator::space() const { return m_space; }

QImage HaldGenerator::hald() const { return m_hald; }

void HaldGenerator::setLut(std::shared_ptr<Lut> lut) { m_lut = lut; }

void HaldGenerator::handleSamplesChanged(
    std::shared_ptr<Lut::OffsetSamples> samples) {
    generateHald(samples);
}

void HaldGenerator::handleSpaceChanged(ColourWheel::Space space) {
    m_space = space;
    switch (space) {
    case ColourWheel::LAB:
        setLut(std::make_shared<LabLut>());
        break;
    case ColourWheel::RGB:
        setLut(std::make_shared<RgbLut>());
        break;
    case ColourWheel::HSP:
        setLut(std::make_shared<HspLut>());
        break;
    }
    emit spaceChanged(m_space);
}

void HaldGenerator::generateHald(std::shared_ptr<Lut::OffsetSamples> samples) {
    m_lut->update(samples);
    m_hald = {};
}
