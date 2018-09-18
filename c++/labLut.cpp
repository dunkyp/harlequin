#include "labLut.hpp"

#include <cmath>

LabLut::LabLut() {
}

QRgb LabLut::lookup(QRgb in) {
    return {};
}

void LabLut::updateMappings(QMap<QRgb, QRgb> mappings) {
    m_mappings = mappings;
}
