#pragma once

#include "lut.hpp"

class HspLut : public Lut {
  public:
    virtual QRgb lookup(QRgb in) override;
};
