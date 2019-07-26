/*
    settingsdialog.h

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Mike Krus <mike.krus@kdab.com>

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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <Kuesa/ToneMappingAndGammaCorrectionEffect>

class MainWindow;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY selectionColorChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(bool defaultClearColor READ defaultClearColor WRITE setDefaultClearColor NOTIFY defaultClearColorChanged)
    Q_PROPERTY(bool generateTangents READ generateTangents WRITE setGenerateTangents NOTIFY generateTangentsChanged)
    Q_PROPERTY(bool generateNormals READ generateNormals WRITE setGenerateNormals NOTIFY generateNormalsChanged)
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
    Q_PROPERTY(float exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
    Q_PROPERTY(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm READ toneMappingAlgorithm WRITE setToneMappingAlgorithm NOTIFY toneMappingAlgorithmChanged)
public:
    explicit SettingsDialog(MainWindow *parent = nullptr);
    ~SettingsDialog();

    QColor selectionColor() const;
    QColor clearColor() const;
    bool defaultClearColor() const;
    bool generateTangents() const;
    bool generateNormals() const;
    float gamma() const;
    float exposure() const;
    Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm() const;

public slots:
    void setSelectionColor(QColor selectionColor);
    void setClearColor(QColor clearColor);
    void setDefaultClearColor(bool defaultClearColor);
    void setGenerateTangents(bool generateTangents);
    void setGenerateNormals(bool generateNormals);
    void setGamma(float gamma);
    void setExposure(float exposure);
    void setToneMappingAlgorithm(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);

signals:
    void selectionColorChanged(QColor selectionColor);
    void clearColorChanged(QColor clearColor);
    void defaultClearColorChanged(bool defaultClearColor);
    void generateTangentsChanged(bool generateTangents);
    void generateNormalsChanged(bool generateNormals);
    void gammaChanged(float gamma);
    void exposureChanged(float exposure);
    void toneMappingAlgorithmChanged(Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping toneMappingAlgorithm);

private:
    Ui::SettingsDialog *ui;
    QColor m_selectionColor;
    QColor m_clearColor;
    bool m_defaultClearColor;
    bool m_generateTangents;
    bool m_generateNormals;
    float m_gamma;
    float m_exposure;
    Kuesa::ToneMappingAndGammaCorrectionEffect::ToneMapping m_toneMappingAlgorithm;
};

#endif // SETTINGSDIALOG_H
