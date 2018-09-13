#pragma once

#include "lut.hpp"
#include <QVariantList>
#include <QImage>

class LabLut : public Lut {
    LabLut(double height, const QVariantList &list, QImage image);
    QRgb lookup(QRgb in) override;
};
