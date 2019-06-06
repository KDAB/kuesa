/*
    tst_textureimagesmodel.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QtTest/QtTest>
#include "textureinspector.h"
#include <Qt3DRender/QTexture>

using namespace Kuesa;

class tst_TextureImagesModel : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkModel_data()
    {
        QTest::addColumn<Qt3DRender::QAbstractTexture *>("texture");

        {
            Qt3DRender::QAbstractTexture *t = new Qt3DRender::QTexture2D();
            QTest::addRow("empty") << t;
        }
        {
            Qt3DRender::QAbstractTexture *t = new Qt3DRender::QTexture2D();
            Qt3DRender::QTextureImage *img = new Qt3DRender::QTextureImage();
            img->setSource(QUrl("file:///something.png"));
            t->addTextureImage(img);
            QTest::addRow("single_image") << t;
        }
        {
            const QString faceSuffixes[] = {
                QStringLiteral("_posx"),
                QStringLiteral("_negx"),
                QStringLiteral("_posy"),
                QStringLiteral("_negy"),
                QStringLiteral("_posz"),
                QStringLiteral("_negz")
            };
            Qt3DRender::QAbstractTexture *t = new Qt3DRender::QTextureCubeMap();
            for (int i = 0; i < 6; ++i) {
                Qt3DRender::QTextureImage *img = new Qt3DRender::QTextureImage();
                img->setSource(QUrl(QStringLiteral("file:///something") + faceSuffixes + QStringLiteral(".png")));
                img->setFace(static_cast<Qt3DRender::QAbstractTexture::CubeMapFace>(Qt3DRender::QAbstractTexture::CubeMapPositiveX + i));
                t->addTextureImage(img);
            }
            QTest::addRow("cubemap") << t;
        }
    }

    void checkModel()
    {
        // GIVEN
        QFETCH(Qt3DRender::QAbstractTexture *, texture);
        TextureImagesModel model;

        // WHEN
        model.setTexture(texture);

        // THEN
        QCOMPARE(model.rowCount(), texture->textureImages().size());

        for (int i = 0; i < model.rowCount(); ++i) {
            Qt3DRender::QTextureImage *img = static_cast<Qt3DRender::QTextureImage *>(texture->textureImages().at(i));
            QCOMPARE(model.data(model.index(i, 0)).toString(), img->objectName());
            QCOMPARE(model.data(model.index(i, 1)).toInt(), img->mipLevel());
            QCOMPARE(model.data(model.index(i, 2)).toInt(), img->layer());
            QCOMPARE(model.data(model.index(i, 3)).toInt(), img->face());
            QCOMPARE(model.data(model.index(i, 4)).toUrl(), img->source());
            QCOMPARE(model.data(model.index(i, 5)).toBool(), img->isMirrored());
        }
    }

    void checkHeaderData()
    {
        // GIVEN
        TextureImagesModel model;

        // THEN
        QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString(), QVariant(QStringLiteral("ImageNameColumn")));
        QCOMPARE(model.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString(), QVariant(QStringLiteral("MipLevelColumn")));
        QCOMPARE(model.headerData(2, Qt::Horizontal, Qt::DisplayRole).toString(), QVariant(QStringLiteral("LayerColumn")));
        QCOMPARE(model.headerData(3, Qt::Horizontal, Qt::DisplayRole).toString(), QVariant(QStringLiteral("FaceColumn")));
        QCOMPARE(model.headerData(4, Qt::Horizontal, Qt::DisplayRole).toString(), QVariant(QStringLiteral("SourceColumn")));
        QCOMPARE(model.headerData(5, Qt::Horizontal, Qt::DisplayRole).toString(), QVariant(QStringLiteral("MirroredColumn")));
    }
};

QTEST_APPLESS_MAIN(tst_TextureImagesModel)

#include "tst_textureimagesmodel.moc"
