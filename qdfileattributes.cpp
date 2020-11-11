/***************************************************************************
*   Copyright (C) 2011-2017 by Denis Gottardello                          *
*   info@denisgottardello.it                                              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "qdfileattributes.h"
#include "ui_qdfileattributes.h"

QDFileAttributes::QDFileAttributes(QWidget *parent) : QDialog(parent), ui(new Ui::QDFileAttributes) {
    ui->setupUi(this);
    move(parent->window()->mapToGlobal(parent->window()->rect().center())- mapToGlobal(rect().center()));
    #ifdef Q_OS_LINUX
        ui->QDTECreation->setEnabled(false);
    #endif
}

QDFileAttributes::~QDFileAttributes() {
    delete ui;
}

void QDFileAttributes::on_QDTECreation_dateTimeChanged(const QDateTime &date) {
    on_QDTELastModified_dateTimeChanged(date);
}

void QDFileAttributes::on_QDTELastAccess_dateTimeChanged(const QDateTime &date) {
    on_QDTELastModified_dateTimeChanged(date);
}

void QDFileAttributes::on_QDTELastModified_dateTimeChanged(const QDateTime &) {
    ui->QPBApply->setEnabled(true);
}

void QDFileAttributes::on_QPBApply_clicked() {
    if (!SetFileDateTime(ui->QLEFile->text(), ui->QDTECreation->dateTime(), ui->QDTELastAccess->dateTime(), ui->QDTELastModified->dateTime())) QMessageBox::critical(this, tr("Error!!!"), tr("Unable to apply the attributes!"), "Ok");
    else ReadAttributes();
}

void QDFileAttributes::on_QPBClose_clicked() {
    this->accept();
}

void QDFileAttributes::on_QPBSelectFile_clicked() {
    QFileDialog FileDialog;
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setDirectory(QFileInfo(ui->QLEFile->text()).dir().absolutePath());
    if (FileDialog.exec()== QDialog::Accepted) {
        ui->QLEFile->setText(FileDialog.selectedFiles().at(0));
        ReadAttributes();
    }
}

void QDFileAttributes::ReadAttributes() {
    QFileInfo QFIPathSource(ui->QLEFile->text());
    ui->QDTECreation->setDateTime(QFIPathSource.birthTime());
    ui->QDTELastAccess->setDateTime(QFIPathSource.lastRead());
    ui->QDTELastModified->setDateTime(QFIPathSource.lastModified());
    ui->QPBApply->setEnabled(false);
}
