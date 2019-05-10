#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QQuickFramebufferObject>
#include <QUrl>
#include <QColor>
#include <memory>

class ExtendedImage : public QQuickFramebufferObject {
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QImage sourceImage READ sourceImage NOTIFY sourceImageChanged)
    Q_PROPERTY(int sourceHeight READ sourceHeight NOTIFY sourceHeightChanged)
    Q_PROPERTY(int sourceWidth READ sourceWidth NOTIFY sourceWidthChanged)

  public:
    Renderer *createRenderer() const override;

  public slots:
    QUrl source() const;
    QImage sourceImage() const;
    void setSource(QUrl image);
    int sourceHeight() const;
    int sourceWidth() const;
    QColor color(int x, int y) const;

  signals:
    void sourceChanged(QUrl);
    void sourceImageChanged(QImage image);
    void sourceWidthChanged();
    void sourceHeightChanged();

  private:
    QUrl m_source;
    QImage m_sourceImage;
};
