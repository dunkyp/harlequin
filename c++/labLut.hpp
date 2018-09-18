#pragma once

#include "lut.hpp"

class LabLut : public Lut {
public:
    LabLut();
    QRgb lookup(QRgb in) override;
public slots:
    void updateMappings(QMap<QRgb, QRgb> mappings) override;

private:
    QMap<QRgb, QRgb> m_mappings;
};
