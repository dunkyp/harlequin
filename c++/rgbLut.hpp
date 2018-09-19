#pragma once

#include "lut.hpp"

class RgbLut : public Lut {
public:
    QRgb lookup(QRgb in) override;
};
