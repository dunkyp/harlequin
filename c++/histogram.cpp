#include "histogram.hpp"

Histogram::Histogram(QImage image, QObject *parent) : QObject(parent) {
    auto totalPixels = image.width() * image.height();
    for (auto x = 0; x < image.width(); x++) {
        for (auto y = 0; y < image.height(); y++) {
            auto colour = image.pixelColor(x, y);
            m_values[colour.red()][colour.green()][colour.blue()] +=
                1.0 / totalPixels;
        }
    }
}

QMap<int, QMap<int, QMap<int, float>>> Histogram::getValues() {
    return m_values;
}

QOpenGLTexture *Histogram::getTexture() {
    auto texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
    texture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float32);
    return texture;
}
