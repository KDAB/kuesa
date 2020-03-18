/*
    texturewidget.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TEXTUREWIDGET_H
#define TEXTUREWIDGET_H

#include <QWidget>
#include <QScrollArea>

class RenderedPreviewWidget;
class QVBoxLayout;
class QAbstractItemModel;
class QDataWidgetMapper;
class TextureImageDelegateWidget;
class TextureInspector;
class QQmlContext;

namespace Ui {
class TextureWidget;
}

class FixedWidthScrollArea : public QScrollArea
{
public:
    explicit FixedWidthScrollArea(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
};

class TextureWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TextureWidget(QWidget *parent = nullptr);
    ~TextureWidget();

    void setTextureInspector(TextureInspector *inspector);
    void setPreviewRenderContext(QQmlContext *context);

private Q_SLOTS:
    void updateData();

private:
    TextureInspector *m_inspector;
    Ui::TextureWidget *ui;
    RenderedPreviewWidget *m_previewWidget;
};

class TextureImageViewer : public QWidget
{
    Q_OBJECT
public:
    TextureImageViewer(QAbstractItemModel *model, QWidget *parent = nullptr);
    virtual ~TextureImageViewer();

private Q_SLOTS:
    void updateDelegates();

private:
    QAbstractItemModel *m_model;
    QList<TextureImageDelegateWidget *> m_delegates;
    QVBoxLayout *m_mainLayout;
};

class TextureImageDelegateWidget : public QWidget
{
    Q_OBJECT
public:
    TextureImageDelegateWidget(QAbstractItemModel *model, QWidget *parent = nullptr);
    virtual ~TextureImageDelegateWidget();

    void setRow(int row);

private:
    QDataWidgetMapper *m_mapper;
};

#endif // TEXTUREWIDGET_H
