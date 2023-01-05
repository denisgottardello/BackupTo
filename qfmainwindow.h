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

#ifndef QFMAINWINDOW_H
#define QFMAINWINDOW_H

#include "commons.h"
#include "qdfileattributes.h"
#include "QDir"
#include "qdemptydirectory.h"
#include "QDesktopServices"
#include "qdtextinput.h"
#include "QFileDialog"
#include <QMainWindow>
#include "QMenu"
#include "QMessageBox"
#include "QNetworkAccessManager"
#include "QNetworkReply"
#include "QScreen"
#include "QSettings"
#include "QSystemTrayIcon"
#include "QTimer"
#include "ui_qfmainwindow.h"
#ifndef THSYNCHRONIZE_H
    class QFMainWindow;
    #include "qthsynchronize.h"
#endif

namespace Ui {
class QFMainWindow;
}

class QFMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QFMainWindow(QWidget *parent = nullptr);
    ~QFMainWindow();

private slots:
    void AddFiles();
    void AddDirectories();
    void on_QAAuthor_triggered();
    void on_QADirectoryEmpty_triggered();
    void on_QAFileAttributesChange_triggered();
    void on_QAQtVersion_triggered();
    void on_QAQuit_triggered();
    void on_QAVersion_triggered();
    void on_QCBDeleteFilesOnTargetSide_toggled(bool checked);
    void on_QCBIgnoreFileDate_toggled(bool checked);
    void on_QLEDescription_textChanged(const QString &arg1);
    void on_QLESideA_textChanged(const QString &arg1);
    void on_QLESideB_textChanged(const QString &arg1);
    void on_QLVLog_doubleClicked(const QModelIndex &index);
    void on_QLWProfiles_clicked(const QModelIndex &index);
    void on_QLWProfiles_currentRowChanged(int currentRow);
    void on_QLWExclusionList_clicked(const QModelIndex &index);
    void on_QLWExclusionList_currentRowChanged(int currentRow);
    void on_QPBClear_clicked();
    void on_QPBProfileAdd_clicked();
    void on_QPBProfileRemove_clicked();
    void on_QPBQuit_clicked();
    void on_QPBSave_clicked();
    void on_QPBStart_clicked();
    void on_QPBStop_clicked();
    void on_QRBSideAEqualToSideB_toggled(bool checked);
    void on_QRBSideAToSideB_toggled(bool checked);
    void on_QRBSideBToSideA_toggled(bool checked);
    void on_QTBSideA_clicked();
    void on_QTBSideB_clicked();
    void finished(QNetworkReply *reply);
    void OnEnd();
    void OnGenericEvent(EventTypes EventType, int Int0, int Int1, int Int2, int Int3, QString String0);
    void OnLog(LogTypes Type, QString Log);
    void OnTimer();
    void Remove();
    void UpdateIcons();

private:
    Ui::QFMainWindow *ui;
    bool UpdateInAction;
    int LastIcon;
    QNetworkAccessManager *pQNetworkAccessManager= nullptr;
    QSystemTrayIcon *pQSystemTrayIcon= nullptr;
    QStandardItemModel *pQStandardItemModel= nullptr;
    QThSynchronize *pQThSynchronize= nullptr;
    QTimer QTIcons, Timer;
    QVector<Profile> QVProfiles;

};

#endif // QFMAINWINDOW_H
