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

#include "qfmainwindow.h"

QFMainWindow::QFMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QFMainWindow) {
    ui->setupUi(this);
    qRegisterMetaType<EventTypes>("EventTypes");
    qRegisterMetaType<LogTypes>("LogTypes");
    pQStandardItemModel= new QStandardItemModel(this);
    QMenu *QMAdd= new QMenu(this);
    QMenu *QMRemove= new QMenu(this);
    QAction *QAAddDirectories= new QAction(tr("Add directories"), this);
    QAction *QAAddFiles= new QAction(tr("Add files"), this);
    QAction *QARemove= new QAction(tr("Remove"), this);
    connect(QAAddDirectories, SIGNAL(triggered()), this, SLOT(AddDirectories()));
    connect(QAAddFiles, SIGNAL(triggered()), this, SLOT(AddFiles()));
    connect(QARemove, SIGNAL(triggered()), this, SLOT(Remove()));
    QMAdd->addAction(QAAddDirectories);
    QMAdd->addAction(QAAddFiles);
    QMRemove->addAction(QARemove);
    ui->QTBExclusionListAdd->setMenu(QMAdd);
    ui->QTBExclusionListRemove->setMenu(QMRemove);
    UpdateInAction= true;
    ui->widget_2->setEnabled(false);
    ui->widget_7->setEnabled(false);
    QAction *QAMinimize= new QAction(tr("Minimize"), this);
    QAction *QARestore= new QAction(tr("Restore"), this);
    QAction *QAQuit= new QAction(tr("Quit"), this);
    connect(QAMinimize, SIGNAL(triggered()), this, SLOT(hide()));
    connect(QARestore, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect(QAQuit, SIGNAL(triggered()), this, SLOT(on_QPBQuit_clicked()));
    QMenu *QMTrayIcon= new QMenu(this);
    QMTrayIcon->addAction(QAMinimize);
    QMTrayIcon->addAction(QARestore);
    QMTrayIcon->addSeparator();
    QMTrayIcon->addAction(QAQuit);
    pQSystemTrayIcon= new QSystemTrayIcon(this);
    pQSystemTrayIcon->setToolTip(this->windowTitle());
    pQSystemTrayIcon->setIcon(this->windowIcon());
    pQSystemTrayIcon->setContextMenu(QMTrayIcon);
    pQSystemTrayIcon->setIcon(this->windowIcon());
    pQSystemTrayIcon->show();
    LastIcon= 0;
    UpdateInAction= false;
    ui->QPBFile->hide();
    ui->QLFile->hide();
    QTIcons.setInterval(100);
    connect(&QTIcons, SIGNAL(timeout()), this, SLOT(UpdateIcons()));
    QSettings *QSSettings= new QSettings(QDir::homePath()+ QDir::separator()+ ".BackupTo"+ QDir::separator()+ "BackupTo.ini", QSettings::IniFormat); {
        QSSettings->beginGroup("Profiles");
        int count= 0;
        while (true) {
            Profile profile;
            profile.Description= QSSettings->value("Description"+ QString::number(count), "").toString();
            profile.DeleteFilesOnTargetSide= QSSettings->value("DeleteFilesOnTargetSide"+ QString::number(count), "true").toBool();
            profile.Direction= static_cast<Sides>(QSSettings->value("Direction"+ QString::number(count), SIDEA_SIDEB_MERGE).toInt());
            profile.IgnoreFileDate= QSSettings->value("IgnoreFileDate"+ QString::number(count), "false").toBool();
            profile.SideA= QSSettings->value("SideA"+ QString::number(count), "").toString();
            profile.SideB= QSSettings->value("SideB"+ QString::number(count), "").toString();
            int count_2= 0;
            while (true) {
                QString Path= QSSettings->value("ExclusionList"+ QString::number(count)+ "."+ QString::number(count_2), "").toString();
                if (Path.trimmed().compare("")!= 0) profile.QSLExclusionFiles.append(Path);
                else break;
                count_2++;
            }
            if (profile.Description.trimmed().compare("")== 0) break;
            else {
                ui->QLWProfiles->addItem(profile.Description);
                QVProfiles.append(profile);
                count++;
            }
        }
    }{
        delete QSSettings;
    }
    connect(&Timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    Timer.start(500);
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    QRect Rect= QApplication::screens().at(0)->geometry();
    move(Rect.center()- rect().center());
    pQNetworkAccessManager= new QNetworkAccessManager(this);
    connect(pQNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    pQNetworkAccessManager->get(QNetworkRequest(QUrl("http://www.denisgottardello.it/BackupTo/BackupToVersion.txt")));
}

QFMainWindow::~QFMainWindow() {
    if (pQNetworkAccessManager) delete pQNetworkAccessManager;
    if (pQThSynchronize) delete pQThSynchronize;
    delete pQStandardItemModel;
    delete pQSystemTrayIcon;
    delete ui;
}

void QFMainWindow::AddFiles() {
    QFileDialog FileDialog;
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setFileMode(QFileDialog::ExistingFiles);
    if (FileDialog.exec()== QDialog::Accepted) {
        QVProfiles[ui->QLWProfiles->currentRow()].QSLExclusionFiles.append(FileDialog.selectedFiles());
        ui->QLWExclusionList->clear();
        ui->QLWExclusionList->addItems(QVProfiles.at(ui->QLWProfiles->currentRow()).QSLExclusionFiles);
        ui->QPBSave->setEnabled(true);
    }
}

void QFMainWindow::AddDirectories() {
    QFileDialog FileDialog;
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (FileDialog.exec()== QDialog::Accepted) {
        QVProfiles[ui->QLWProfiles->currentRow()].QSLExclusionFiles.append(FileDialog.selectedFiles());
        ui->QLWExclusionList->clear();
        ui->QLWExclusionList->addItems(QVProfiles.at(ui->QLWProfiles->currentRow()).QSLExclusionFiles);
        ui->QPBSave->setEnabled(true);
    }
}

void QFMainWindow::finished(QNetworkReply *reply) {
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()== 200) {
        if (QString(reply->readAll().trimmed()).compare(VERSION)!= 0) {
            if (QMessageBox::information(this, tr("Info"), tr("New version available. Do you want to open the BackupTo download page?"), QMessageBox::Yes | QMessageBox::No)== QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl("http://www.denisgottardello.it/BackupTo"));
            }
        }
    }
}

void QFMainWindow::on_QAAuthor_triggered() {
    QMessageBox::information(this, "Info", "Denis Gottardello\nwww.denisgottardello.it\ninfo@denisgottardello.it", "Ok");
}

void QFMainWindow::on_QADirectoryEmpty_triggered() {
    QDEmptyDirectory EmptyDirectory(this);
    EmptyDirectory.exec();
}

void QFMainWindow::on_QAFileAttributesChange_triggered() {
    QDFileAttributes FileAttributes(this);
    FileAttributes.exec();
}

void QFMainWindow::on_QAQtVersion_triggered() {
    QMessageBox::information(this, "Info", tr("Qt version: ")+ QT_VERSION_STR, "Ok");
}

void QFMainWindow::on_QAQuit_triggered() {
    ui->QPBQuit->click();
}

void QFMainWindow::on_QAVersion_triggered() {
    QMessageBox::information(this, "Info", tr("BackupTo, version ")+ VERSION, "Ok");
}

void QFMainWindow::on_QCBDeleteFilesOnTargetSide_toggled(bool checked) {
    on_QRBSideAEqualToSideB_toggled(checked);
}

void QFMainWindow::on_QCBIgnoreFileDate_toggled(bool checked) {
    on_QRBSideAEqualToSideB_toggled(checked);
}

void QFMainWindow::on_QLEDescription_textChanged(const QString &) {
    on_QRBSideAEqualToSideB_toggled(false);
}

void QFMainWindow::on_QLESideA_textChanged(const QString &) {
    on_QRBSideAEqualToSideB_toggled(false);
}

void QFMainWindow::on_QLESideB_textChanged(const QString &) {
    on_QRBSideAEqualToSideB_toggled(false);
}

void QFMainWindow::on_QLVLog_doubleClicked(const QModelIndex &index) {
    if (pQStandardItemModel->item(index.row(), index.column())->text().contains("[") && pQStandardItemModel->item(index.row(), index.column())->text().contains("]")) {
        QString Value= pQStandardItemModel->item(index.row(), index.column())->text().midRef(pQStandardItemModel->item(index.row(), index.column())->text().indexOf("[")).toString();
        Value= pQStandardItemModel->item(index.row(), index.column())->text().midRef(1, pQStandardItemModel->item(index.row(), index.column())->text().indexOf("]")- 1).toString();
        if (Value.length()> 0) {
            QFileInfo FileInfo(Value);
            QDesktopServices::openUrl(QUrl("file:///"+ FileInfo.absoluteDir().absolutePath()));
        }
    }
}

void QFMainWindow::on_QLWProfiles_clicked(const QModelIndex &index) {
    on_QLWProfiles_currentRowChanged(index.row());
}

void QFMainWindow::on_QLWProfiles_currentRowChanged(int currentRow) {
    if (currentRow> -1 && currentRow< ui->QLWProfiles->count()) {
        UpdateInAction= true;
        ui->QLEDescription->setText(QVProfiles.at(currentRow).Description);
        ui->QCBDeleteFilesOnTargetSide->setChecked(QVProfiles.at(currentRow).DeleteFilesOnTargetSide);
        ui->QCBIgnoreFileDate->setChecked(QVProfiles.at(currentRow).IgnoreFileDate);
        ui->QLESideA->setText(QVProfiles.at(currentRow).SideA);
        ui->QLESideB->setText(QVProfiles.at(currentRow).SideB);
        switch (QVProfiles.at(currentRow).Direction) {
            case SIDEA_SIDEB_MERGE: {
                ui->QRBSideAEqualToSideB->setChecked(true);
                break;
            }
            case SIDEA_TO_SIDEB: {
                ui->QRBSideAToSideB->setChecked(true);
                break;
            }
            case SIDEB_TO_SIDEA: {
                ui->QRBSideBToSideA->setChecked(true);
                break;
            }
        }
        ui->QLWExclusionList->clear();
        ui->QLWExclusionList->addItems(QVProfiles.at(currentRow).QSLExclusionFiles);
        ui->widget_2->setEnabled(true);
        ui->widget_7->setEnabled(true);
        ui->QPBProfileRemove->setEnabled(true);
        UpdateInAction= false;
    }
}

void QFMainWindow::on_QLWExclusionList_clicked(const QModelIndex &index) {
    on_QLWExclusionList_currentRowChanged(index.row());
}

void QFMainWindow::on_QLWExclusionList_currentRowChanged(int currentRow) {
    ui->QTBExclusionListRemove->setEnabled(false);
    if (currentRow> -1 && currentRow< QVProfiles.at(ui->QLWProfiles->currentRow()).QSLExclusionFiles.size()) ui->QTBExclusionListRemove->setEnabled(true);
}

void QFMainWindow::on_QPBClear_clicked() {
    pQStandardItemModel->clear();
    ui->QLVLog->setModel(pQStandardItemModel);
    ui->QLCDNDirectoriesCopied->display(0);
    ui->QLCDNDirectorieDeleted->display(0);
    ui->QLCDNFilesCopied->display(0);
    ui->QLCDNFilesDeleted->display(0);
}

void QFMainWindow::on_QPBProfileAdd_clicked() {
    QDTextInput TextInput(this);
    TextInput.ui->QLTextInput->setText(tr("Profile"));
    TextInput.setWindowTitle(tr("New profile"));
    if (TextInput.exec()== QDialog::Accepted) {
        Profile profile;
        profile.DeleteFilesOnTargetSide= true;
        profile.Description= TextInput.ui->QLETextInput->text();
        profile.Direction= SIDEA_TO_SIDEB;
        profile.IgnoreFileDate= false;
        ui->QLWProfiles->addItem(profile.Description);
        QVProfiles.append(profile);
        ui->QPBSave->setEnabled(true);
    }
}

void QFMainWindow::on_QPBProfileRemove_clicked() {
    if (QMessageBox::question(this, tr("Warning!"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No)== QMessageBox::Yes) {
        ui->QPBProfileRemove->setEnabled(false);
        ui->widget_2->setEnabled(false);
        ui->widget_7->setEnabled(false);
        ui->QPBProfileRemove->setEnabled(false);
        QVProfiles.remove(ui->QLWProfiles->row(ui->QLWProfiles->currentItem()));
        ui->QLWProfiles->takeItem(ui->QLWProfiles->row(ui->QLWProfiles->currentItem()));
        ui->QPBSave->setEnabled(true);
    }
}

void QFMainWindow::on_QPBQuit_clicked() {
    if (ui->QPBSave->isEnabled()) {
        if (QMessageBox::question(this, tr("Warning!"), tr("Are you sure you want to exit without save?"), QMessageBox::Yes | QMessageBox::No)== QMessageBox::Yes) {
            this->close();
        }
    } else this->close();
}

void QFMainWindow::on_QPBSave_clicked() {
    QSettings *QSSettings= new QSettings(QDir::homePath()+ QDir::separator()+ ".BackupTo"+ QDir::separator()+ "BackupTo.ini", QSettings::IniFormat); {
        QSSettings->beginGroup("Profiles");
        QSSettings->remove("");
        for (int count= 0; count< QVProfiles.count(); count++) {
            QSSettings->setValue("Description"+ QString::number(count), QVProfiles.at(count).Description);
            QSSettings->setValue("Direction"+ QString::number(count), QVProfiles.at(count).Direction);
            QSSettings->setValue("IgnoreFileDate"+ QString::number(count), QVProfiles.at(count).IgnoreFileDate);
            QSSettings->setValue("SideA"+ QString::number(count), QVProfiles.at(count).SideA);
            QSSettings->setValue("SideB"+ QString::number(count), QVProfiles.at(count).SideB);
            QSSettings->setValue("DeleteFilesOnTargetSide"+ QString::number(count), QVProfiles.at(count).DeleteFilesOnTargetSide);
            for (int count_2= 0; count_2< QVProfiles.at(count).QSLExclusionFiles.size(); count_2++) {
                QSSettings->setValue("ExclusionList"+ QString::number(count)+ "."+ QString::number(count_2), QVProfiles.at(count).QSLExclusionFiles.at(count_2));
            }
        }
        QSSettings->endGroup();
    }{
        delete QSSettings;
    }
    ui->QPBSave->setEnabled(false);
}

void QFMainWindow::on_QPBStart_clicked() {
    ui->QPBStart->setEnabled(false);
    ui->QPBClear->click();
    ui->QPBFile->setValue(0);
    ui->QPBFile->show();
    ui->QLFile->clear();
    ui->QLFile->show();
    QTIcons.start();
    pQThSynchronize= new QThSynchronize(QVProfiles.at(ui->QLWProfiles->currentRow()), ui->QCBSimulation->isChecked());
    connect(pQThSynchronize, SIGNAL(OnEnd()), this, SLOT(OnEnd()), Qt::QueuedConnection);
    connect(pQThSynchronize, SIGNAL(OnGenericEvent(EventTypes,int,int,int,int,QString)), this, SLOT(OnGenericEvent(EventTypes,int,int,int,int,QString)), Qt::QueuedConnection);
    connect(pQThSynchronize, SIGNAL(OnLog(LogTypes,QString)), this, SLOT(OnLog(LogTypes,QString)), Qt::QueuedConnection);
    pQThSynchronize->start();
    ui->QPBStop->setEnabled(true);
}

void QFMainWindow::on_QPBStop_clicked() {
    ui->QPBStop->setEnabled(false);
    delete pQThSynchronize;
    pQThSynchronize= nullptr;
    QTIcons.stop();
    pQSystemTrayIcon->setIcon(this->windowIcon());
    ui->QLUpdate->clear();
    ui->QPBFile->hide();
    ui->QLFile->hide();
    ui->QPBStart->setEnabled(true);
}

void QFMainWindow::on_QRBSideAEqualToSideB_toggled(bool ) {
    if (!UpdateInAction) {
        QVProfiles[ui->QLWProfiles->currentRow()].DeleteFilesOnTargetSide= ui->QCBDeleteFilesOnTargetSide->isChecked();
        QVProfiles[ui->QLWProfiles->currentRow()].Description= ui->QLEDescription->text();
        QVProfiles[ui->QLWProfiles->currentRow()].IgnoreFileDate= ui->QCBIgnoreFileDate->isChecked();
        QVProfiles[ui->QLWProfiles->currentRow()].SideA= ui->QLESideA->text();
        QVProfiles[ui->QLWProfiles->currentRow()].SideB= ui->QLESideB->text();
        if (ui->QRBSideAEqualToSideB->isChecked()) QVProfiles[ui->QLWProfiles->currentRow()].Direction= SIDEA_SIDEB_MERGE;
        else if (ui->QRBSideAToSideB->isChecked()) QVProfiles[ui->QLWProfiles->currentRow()].Direction= SIDEA_TO_SIDEB;
        else QVProfiles[ui->QLWProfiles->currentRow()].Direction= SIDEB_TO_SIDEA;
        ui->QPBSave->setEnabled(true);
    }
}

void QFMainWindow::on_QRBSideAToSideB_toggled(bool checked) {
    on_QRBSideAEqualToSideB_toggled(checked);
}

void QFMainWindow::on_QRBSideBToSideA_toggled(bool checked) {
    on_QRBSideAEqualToSideB_toggled(checked);
}

void QFMainWindow::on_QTBSideA_clicked() {
    UpdateInAction= true;
    QString Path= QFileDialog::getExistingDirectory(this, tr("Side A"), QVProfiles[ui->QLWProfiles->row(ui->QLWProfiles->currentItem())].SideA, QFileDialog::ShowDirsOnly);
    if (Path.length()> 0) {
        QVProfiles[ui->QLWProfiles->currentRow()].SideA= Path;
        ui->QLESideA->setText(QVProfiles.at(ui->QLWProfiles->currentRow()).SideA);
        ui->QPBSave->setEnabled(true);
    }
    UpdateInAction= false;
}

void QFMainWindow::on_QTBSideB_clicked() {
    UpdateInAction= true;
    QString Path= QFileDialog::getExistingDirectory(this, tr("Side B"), QVProfiles[ui->QLWProfiles->row(ui->QLWProfiles->currentItem())].SideB, QFileDialog::ShowDirsOnly);
    if (Path.length()> 0) {
        QVProfiles[ui->QLWProfiles->currentRow()].SideB= Path;
        ui->QLESideB->setText(QVProfiles.at(ui->QLWProfiles->currentRow()).SideB);
        ui->QPBSave->setEnabled(true);
    }
    UpdateInAction= false;
}

void QFMainWindow::OnEnd() {
    ui->QPBStop->click();
    ui->QLVLog->scrollToBottom();
}

void QFMainWindow::OnGenericEvent(EventTypes EventType, int Int0, int Int1, int Int2, int Int3, QString String0) {
    switch(EventType) {
        case EVENT_TYPE_FILE: {
            ui->QLFile->setText(String0);
            break;
        }
        case EVENT_TYPE_FILE_STATUS: {
            ui->QPBFile->setValue(Int0);
            break;
        }
        case EVENT_TYPE_JOB_STATUS: {
            ui->QLCDNDirectoriesCopied->display(Int1);
            ui->QLCDNDirectorieDeleted->display(Int3);
            ui->QLCDNFilesCopied->display(Int0);
            ui->QLCDNFilesDeleted->display(Int2);
            break;
        }
    }
}

void QFMainWindow::OnLog(LogTypes Type, QString Log) {
    while (pQStandardItemModel->rowCount()> 1024 * 8) pQStandardItemModel->takeRow(0);
    pQStandardItemModel->setRowCount(pQStandardItemModel->rowCount()+ 1);
    QStandardItem *StandardItem0= new QStandardItem(Log);
    switch(Type) {
        case LOG_TYPE_ERROR: {
            StandardItem0->setForeground(Qt::red);
            break;
        }
        default:
            break;
    }
    pQStandardItemModel->setItem(pQStandardItemModel->rowCount()- 1, 0, StandardItem0);
    ui->QLVLog->setModel(pQStandardItemModel);
}

void QFMainWindow::OnTimer() {
    if (pQThSynchronize) ui->QLVLog->scrollToBottom();
}

void QFMainWindow::Remove() {
    if (QMessageBox::question(this, tr("Warning!"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No)== QMessageBox::Yes) {
        ui->QTBExclusionListRemove->setEnabled(false);
        QVProfiles[ui->QLWProfiles->currentRow()].QSLExclusionFiles.removeAt(ui->QLWExclusionList->currentRow());
        ui->QLWExclusionList->clear();
        ui->QLWExclusionList->addItems(QVProfiles.at(ui->QLWProfiles->currentRow()).QSLExclusionFiles);
        ui->QPBSave->setEnabled(true);
    }
}

void QFMainWindow::UpdateIcons() {
    switch (LastIcon) {
        case 0: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/00.png")))); break;
        case 1: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/01.png")))); break;
        case 2: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/02.png")))); break;
        case 3: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/03.png")))); break;
        case 4: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/04.png")))); break;
        case 5: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/05.png")))); break;
        case 6: pQSystemTrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/06.png")))); break;
    }
    switch (LastIcon) {
        case 0: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/00.png"))); break;
        case 1: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/01.png"))); break;
        case 2: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/02.png"))); break;
        case 3: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/03.png"))); break;
        case 4: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/04.png"))); break;
        case 5: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/05.png"))); break;
        case 6: ui->QLUpdate->setPixmap(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/06.png"))); break;
    }
    LastIcon++;
    if (LastIcon> 6) LastIcon= 0;
}
