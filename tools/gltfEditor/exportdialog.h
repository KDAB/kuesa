/*
    exportdialog.h

    This file is part of Kuesa.

    Copyright (C) 2018-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDialog;
}

namespace Kuesa {
class GLTF2Exporter;
}
class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(Kuesa::GLTF2Exporter &exporter,
                          const QString &originalPath,
                          QWidget *parent = nullptr);
    ~ExportDialog();

private:
    void onSave();
    void setTargetFile(const QString &p);
    Ui::ExportDialog *ui;
    Kuesa::GLTF2Exporter &m_exporter;
    QString m_originalFile;
    QString m_targetFile;
    QString m_warningText;
};

#endif // EXPORTDIALOG_H
