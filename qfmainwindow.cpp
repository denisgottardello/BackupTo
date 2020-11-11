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
#include "ui_qfmainwindow.h"

QFMainWindow::QFMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QFMainWindow) {
    ui->setupUi(this);
    QSIMModel= new QStandardItemModel(this);
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
    QSTITrayIcon= new QSystemTrayIcon(this);
    QSTITrayIcon->setToolTip(this->windowTitle());
    QSTITrayIcon->setIcon(this->windowIcon());
    QSTITrayIcon->setContextMenu(QMTrayIcon);
    QSTITrayIcon->setIcon(this->windowIcon());
    QSTITrayIcon->show();
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
            profile.Direction= QSSettings->value("Direction"+ QString::number(count), "0").toInt();
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
}

QFMainWindow::~QFMainWindow() {
    if (ThSynchronize) delete ThSynchronize;
    delete QSIMModel;
    delete QSTITrayIcon;
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

void QFMainWindow::on_QAAuthor_triggered() {
    QMessageBox::information(this, "Info", "Denis Gottardello\nwww.denisgottardello.it\ninfo@denisgottardello.it", "Ok");
}

void QFMainWindow::on_QADirectoryEmpty_triggered() {
    QDEmptyDirectory *EmptyDirectory= new QDEmptyDirectory(this); {
        EmptyDirectory->exec();
    }{
        delete EmptyDirectory;
    }
}

void QFMainWindow::on_QAFileAttributesChange_triggered() {
    QDFileAttributes *FileAttributes= new QDFileAttributes(this); {
        FileAttributes->exec();
    }{
        delete FileAttributes;
    }
}

void QFMainWindow::on_QAQtVersion_triggered() {
    QMessageBox::information(this, "Info", tr("Qt version: ")+ QT_VERSION_STR, "Ok");
}

void QFMainWindow::on_QAQuit_triggered() {
    ui->QPBQuit->click();
}

void QFMainWindow::on_QAVersion_triggered() {
    QMessageBox::information(this, "Info", tr("BackupTo, version ")+ ("1.0.8.0"), "Ok");
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
    QSIMModel->clear();
    ui->QLVLog->setModel(QSIMModel);
    ui->QLCDNDirectoriesCopied->display(0);
    ui->QLCDNDirectorieDeleted->display(0);
    ui->QLCDNFilesCopied->display(0);
    ui->QLCDNFilesDeleted->display(0);
}

void QFMainWindow::on_QPBProfileAdd_clicked() {
    QDTextInput textinput;
    textinput.ui->QLTextInput->setText(tr("Profile"));
    textinput.setWindowTitle(tr("New profile"));
    if (textinput.exec()== QDialog::Accepted) {
        Profile profile;
        profile.DeleteFilesOnTargetSide= true;
        profile.Description= textinput.ui->QLETextInput->text();
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
    ThSynchronize= new QThSynchronize(QVProfiles.at(ui->QLWProfiles->currentRow()), ui->QCBSimulation->isChecked());
    connect(ThSynchronize, SIGNAL(OnEnd()), this, SLOT(OnEnd()), Qt::QueuedConnection);
    connect(ThSynchronize, SIGNAL(OnGenericEvent(int,int,int,int,int,QString)), this, SLOT(OnGenericEvent(int,int,int,int,int,QString)));
    connect(ThSynchronize, SIGNAL(OnLog(int,QString)), this, SLOT(OnLog(int,QString)), Qt::QueuedConnection);
    ThSynchronize->start();
    ui->QPBStop->setEnabled(true);
}

void QFMainWindow::on_QPBStop_clicked() {
    ui->QPBStop->setEnabled(false);
    delete ThSynchronize;
    ThSynchronize= nullptr;
    QTIcons.stop();
    QSTITrayIcon->setIcon(this->windowIcon());
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

void QFMainWindow::OnGenericEvent(int Type, int Int0, int Int1, int Int2, int Int3, QString String0) {
    switch(Type) {
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

void QFMainWindow::OnLog(int Type, QString Log) {
    while (QSIMModel->rowCount()> 1024 * 8) QSIMModel->takeRow(0);
    QSIMModel->setRowCount(QSIMModel->rowCount()+ 1);
    QStandardItem *StandardItem0= new QStandardItem(Log);
    switch(Type) {
        case LOG_TYPE_ERROR: StandardItem0->setForeground(Qt::red); break;
        default: break;
    }
    QSIMModel->setItem(QSIMModel->rowCount()- 1, 0, StandardItem0);
    ui->QLVLog->setModel(QSIMModel);
}

void QFMainWindow::OnTimer() {
    if (ThSynchronize) ui->QLVLog->scrollToBottom();
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
        case 0: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/00.png")))); break;
        case 1: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/01.png")))); break;
        case 2: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/02.png")))); break;
        case 3: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/03.png")))); break;
        case 4: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/04.png")))); break;
        case 5: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/05.png")))); break;
        case 6: QSTITrayIcon->setIcon(QIcon(QPixmap(QString::fromUtf8(":/IconsWait/IconsWait/06.png")))); break;
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
