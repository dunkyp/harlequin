#pragma once

#include <QColor>
#include <vector>

class Lut {
public:
    using Point = std::tuple<double, double, double>;
    using OffsetSamples = std::vector<std::pair<Point, Point>>;
    virtual QRgb lookup(QRgb in) = 0;
};
