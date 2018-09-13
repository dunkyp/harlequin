#pragma once

#include <QColor>

class Lut {
public:
    virtual QRgb lookup(QRgb in) = 0;
};
