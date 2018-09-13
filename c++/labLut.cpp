#include "labLut.hpp"

#include <cmath>

LabLut::LabLut(double height, const QVariantList &pointList, QImage image) {
    auto sampleCount = pointList.size();
    double rings = std::sqrt(pointList.size() - 1);
    auto radius = (height / 2.0);
    for(double ring = 0; ring < rings; ring++) {
        for(auto i = 0; i < rings; i++) {
            auto initialX = std::cos(((M_PI * 2) / rings) * i) * (radius / rings * (ring + 1)) + radius;
            auto initialY = std::cos(((M_PI * 2) / rings) * i) * (radius / rings * (ring + 1)) + radius;
            auto point =  pointList[ring * rings + i].toList();
            auto newX = point[0].toDouble();
            auto newY = point[1].toDouble();
        }
    }
}

QRgb LabLut::lookup(QRgb in) {
    return {};
}
