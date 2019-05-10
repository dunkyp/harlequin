#pragma once

#include <QImage>
#include <QMap>
#include <QObject>
#include <QOpenGLTexture>

class Histogram : public QObject {
    Q_OBJECT
    Q_PROPERTY(
        QMap<int, QMap<int, QMap<int, float>>> values READ getValues CONSTANT)

  public:
    Histogram(QImage image, QObject *parent = nullptr);
    QMap<int, QMap<int, QMap<int, float>>> getValues(); // RGB percentage
    QOpenGLTexture *getTexture();

  private:
    QMap<int, QMap<int, QMap<int, float>>> m_values;
};
