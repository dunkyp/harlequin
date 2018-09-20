#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QQuickFramebufferObject>
#include <QUrl>
#include <memory>


class ResultImage : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl clut READ clut WRITE setClut NOTIFY clutChanged)
    Q_PROPERTY(float xCut READ xCut WRITE setxCut NOTIFY xCutChanged)
    Q_PROPERTY(float yCut READ yCut WRITE setyCut NOTIFY yCutChanged)
    Q_PROPERTY(QImage sourceImage READ sourceImage NOTIFY sourceImageChanged)
    Q_PROPERTY(QImage clutImage READ clutImage NOTIFY clutImageChanged)

public:
    Renderer *createRenderer() const;

public slots:
    QUrl source() const;
    QImage sourceImage() const;
    QUrl clut() const;
    QImage clutImage() const;
    float xCut() const;
    float yCut() const;
    void setSource(QUrl image);
    void setClut(QUrl image);
    void setxCut(float x);
    void setyCut(float y);

signals:
    void sourceChanged(QUrl);
    void sourceImageChanged(QImage image);
    void clutChanged(QUrl);
    void clutImageChanged(QImage);
    void xCutChanged(float);
    void yCutChanged(float);

private:
    QUrl m_source;
    QUrl m_clut;
    float m_xCut {0};
    float m_yCut {0};
    QImage m_sourceImage;
    QImage m_clutImage;
};
