/*
    ssbobuffer.h

    This file is part of Kuesa.

    Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Paul Lemire <paul.lemire@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef SSBOBUFFER_H
#define SSBOBUFFER_H

#include <Qt3DRender/QBuffer>
#include <QSize>

class SSBOBuffer : public Qt3DRender::QBuffer
{
    Q_OBJECT
    Q_PROPERTY(bool isInteractive READ isInteractive WRITE setIsInteractive NOTIFY isInteractiveChanged)
    Q_PROPERTY(bool isComplete READ isComplete NOTIFY isCompleteChanged)
    Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY isInitializedChanged)
    Q_PROPERTY(QSize textureSize READ textureSize WRITE setTextureSize NOTIFY textureSizeChanged)
    Q_PROPERTY(QString outputFileName READ outputFileName WRITE setOutputFileName NOTIFY outputFileNameChanged)
public:
    explicit SSBOBuffer(Qt3DCore::QNode *parent = nullptr);

    void setTextureSize(QSize size);
    void setIsInteractive(bool interactive);
    void setOutputFileName(const QString &name);

    bool isInteractive() const { return m_isInteractive; }
    bool isComplete() const { return m_isComplete; }
    bool isInitialized() const { return m_wasInitialized; }
    QSize textureSize() const { return m_textureSize; }
    QString outputFileName() const { return m_outputFileName; }

signals:
    void textureSizeChanged();
    void isInteractiveChanged();
    void isInitializedChanged();
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
