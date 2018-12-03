#ifndef SSBOBUFFER_H
#define SSBOBUFFER_H

#include <Qt3DRender/QBuffer>
#include <QSize>

class SSBOBuffer : public Qt3DRender::QBuffer
{
    Q_OBJECT
    Q_PROPERTY(bool isInteractive READ isInteractive WRITE setIsInteractive NOTIFY isInteractiveChanged)
    Q_PROPERTY(bool isComplete READ isComplete NOTIFY isCompleteChanged)
    Q_PROPERTY(QSize textureSize READ textureSize WRITE setTextureSize NOTIFY textureSizeChanged)
    Q_PROPERTY(QString outputFileName READ outputFileName WRITE setOutputFileName NOTIFY outputFileNameChanged)
public:
    explicit SSBOBuffer(Qt3DCore::QNode *parent = nullptr);

    void setTextureSize(QSize size);
    void setIsInteractive(bool interactive);
    void setOutputFileName(const QString &name);

    bool isInteractive() const { return m_isInteractive; }
    bool isComplete() const { return m_isComplete; }
    QSize textureSize() const { return m_textureSize; }
    QString outputFileName() const { return m_outputFileName; }

signals:
    void textureSizeChanged();
    void isInteractiveChanged();
    void isCompleteChanged();
    void outputFileNameChanged();

private:
    void onDataChanged(const QByteArray &newData);
    Q_INVOKABLE void saveImage();

    bool m_wasInitialized;
    bool m_isInteractive;
    bool m_isComplete;
    QSize m_textureSize;
    QString m_outputFileName;
};
#endif // SSBOBUFFER_H
