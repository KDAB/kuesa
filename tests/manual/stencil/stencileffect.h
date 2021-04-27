#ifndef OPACITYEFFECT_H
#define OPACITYEFFECT_H

#include <Kuesa/AbstractPostProcessingEffect>
#include <Qt3DRender/QParameter>
#include <QColor>

class StencilEffect : public Kuesa::AbstractPostProcessingEffect
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    explicit StencilEffect();

    QColor color() const;
    FrameGraphNodePtr frameGraphSubTree() const;
    void setInputTexture(Qt3DRender::QAbstractTexture *texture);
    void setDepthTexture(Qt3DRender::QAbstractTexture *texture);
    QVector<Qt3DRender::QLayer *> layers() const;

public Q_SLOTS:
    void setColor(QColor color);

Q_SIGNALS:
    void colorChanged(QColor color);

private:
    Qt3DRender::QParameter *m_color;
    FrameGraphNodePtr m_rootFramegraphNode;
    Qt3DRender::QParameter *m_inputTexture;
    Qt3DRender::QParameter *m_depthTexture;
    QVector<Qt3DRender::QLayer *> m_layers;
};

#endif // OPACITYEFFECT_H
