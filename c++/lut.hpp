#pragma once

#include <QColor>
#include <QMap>
#include <QObject>

class Lut : public QObject{
public:
    virtual QRgb lookup(QRgb in) = 0;
public slots:
    virtual void updateMappings(QMap<QRgb, QRgb> mappings) = 0;
};
