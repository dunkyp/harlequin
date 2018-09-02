#pragma once

#include <QQuickFramebufferObject>
#include <QUrl>


class HALD : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl clut READ clut WRITE setClut NOTIFY clutChanged)
    Q_PROPERTY(float x_cut READ x_cut WRITE setx_cut NOTIFY x_cutChanged)
    Q_PROPERTY(float y_cut READ y_cut WRITE sety_cut NOTIFY y_cutChanged)

public:
    Renderer *createRenderer() const;

public slots:
    QUrl source() const;
    QUrl clut() const;
    float x_cut() const;
    float y_cut() const;
    void setSource(QUrl image);
    void setClut(QUrl image);
    void setx_cut(float x);
    void sety_cut(float y);

signals:
    void sourceChanged(QUrl);
    void clutChanged(QUrl);
    void x_cutChanged(float);
    void y_cutChanged(float);

private:
    QUrl m_source;
    QUrl m_clut;
    float m_x_cut {0};
    float m_y_cut {0};
};
