#pragma once

#include <QQuickFramebufferObject>
#include <QUrl>


class HALD : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl clut READ clut WRITE setClut NOTIFY clutChanged)

public:
    Renderer *createRenderer() const;

public slots:
    QUrl source() const;
    QUrl clut() const;
    void setSource(QUrl image);
    void setClut(QUrl image);

signals:
    void sourceChanged();
    void clutChanged();

private:
    QUrl m_source;
    QUrl m_clut;
};
