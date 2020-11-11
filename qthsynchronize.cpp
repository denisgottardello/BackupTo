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

#include "qthsynchronize.h"

QThSynchronize::QThSynchronize(Profile profile, bool Simulation) : QThread() {
    this->profile= profile;
    this->Simulation= Simulation;
    DoStart= true;
    FilesCopied= DirecoriesCopied= FilesDeleted= DirectoriesDeleted= 0;
}

QThSynchronize::~QThSynchronize() {
    DoStart= false;
    while (this->isRunning()) msleep(10);
}

void QThSynchronize::run() {
    emit OnLog(LOG_TYPE_STANDARD, tr("Begin synchronization"));
    switch (profile.Direction) {
        case SIDEA_SIDEB_MERGE: {
            DirectoryCheckToCopy(profile.SideA, profile.SideB, profile.SideA, profile.IgnoreFileDate, true);
            DirectoryCheckToCopy(profile.SideB, profile.SideA, profile.SideB, profile.IgnoreFileDate, true);
            break;
        }
        case SIDEA_TO_SIDEB: {
            if (profile.DeleteFilesOnTargetSide) DirectoryCheckToDelete(profile.SideB, profile.SideA, profile.SideB);
            DirectoryCheckToCopy(profile.SideA, profile.SideB, profile.SideA, profile.IgnoreFileDate, false);
            break;
        }
        case SIDEB_TO_SIDEA: {
            if (profile.DeleteFilesOnTargetSide) DirectoryCheckToDelete(profile.SideA, profile.SideB, profile.SideA);
            DirectoryCheckToCopy(profile.SideB, profile.SideA, profile.SideB, profile.IgnoreFileDate, false);
            break;
        }
    }
    if (DoStart) emit OnLog(LOG_TYPE_STANDARD, tr("Synchronization completed"));
    else emit OnLog(LOG_TYPE_STANDARD, tr("Synchronization interrupted"));
    if (DoStart) emit OnEnd();
}

void QThSynchronize::DirectoryCheckToCopy(QString Source, QString Destination, QString Path, bool IgnoreFileDate, bool OnlyIfBest) {
    QDir QDPathSource= QDir(Path);
    QDPathSource.setSorting(QDir::Name);
    QStringList QSLFiles= QDPathSource.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (int count= 0; count< QSLFiles.length(); count++) {
        QFileInfo QFIPathSource(QDPathSource.absolutePath()+ "/"+ QSLFiles.at(count));
        bool Finded= false;
        for (int count_2= 0; count_2< profile.QSLExclusionFiles.length(); count_2++) {
            if (QString(QDPathSource.absolutePath()+ "/"+ QSLFiles.at(count)).compare(profile.QSLExclusionFiles.at(count_2), Qt::CaseInsensitive)== 0) {
                Finded= true;
                break;
            }
        }
        if (!Finded) {
            QFileInfo QFIPathDestination(Destination+ "/"+ QFIPathSource.absoluteFilePath().right(QFIPathSource.absoluteFilePath().length()- Source.length()));
            if (DoStart && QFIPathSource.isDir()) {
                if (!QFIPathSource.isSymLink() &! QFIPathSource.isBundle()) {
                    if (!QFIPathDestination.exists()) {
                        if (Simulation) {
                            DirecoriesCopied++;
                            emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesCopied, DirecoriesCopied, FilesDeleted, DirectoriesDeleted, "");
                            emit OnLog(LOG_TYPE_STANDARD, QFIPathSource.absoluteFilePath()+ tr(" created"));
                        } else {
                            QDir QDPathDestination;
                            if (!QDPathDestination.mkpath(QFIPathDestination.absoluteFilePath())) emit OnLog(LOG_TYPE_ERROR, tr("Error on create folder ")+ QFIPathDestination.absoluteFilePath());
                            else {
                                if (!SetFileDateTime(QFIPathDestination.absoluteFilePath(), QFIPathSource.birthTime(), QFIPathSource.lastRead(), QFIPathSource.lastModified())) emit OnLog(LOG_TYPE_ERROR, tr("Error on change folder date time ")+ QFIPathDestination.absoluteFilePath());
                                DirecoriesCopied++;
                                emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesCopied, DirecoriesCopied, FilesDeleted, DirectoriesDeleted, "");
                                emit OnLog(LOG_TYPE_STANDARD, QFIPathSource.absoluteFilePath()+ tr(" created"));
                            }
                        }
                    }
                    DirectoryCheckToCopy(Source, Destination, QFIPathSource.absoluteFilePath(), IgnoreFileDate, OnlyIfBest);
                }
            } else if (DoStart && QFIPathSource.isFile()) {
                bool CanGo= false;
                if (!QFIPathDestination.exists()) CanGo= true;
                else {
                    if (!OnlyIfBest) {
                        if (QFIPathSource.size()!= QFIPathDestination.size() || QFIPathSource.lastModified().toString("yyyyMMddhhmmss").compare(QFIPathDestination.lastModified().toString("yyyyMMddhhmmss"))!= 0) CanGo= true;
                        else {
                            #ifdef Q_OS_WIN
                                if (QFIPathSource.created().toString("yyyyMMddhhmmss").compare(QFIPathDestination.created().toString("yyyyMMddhhmmss"))!= 0) CanGo= true;
                            #endif
                        }
                    } else {
                        if (QFIPathSource.lastModified().toString("yyyyMMddhhmmss").compare(QFIPathDestination.lastModified().toString("yyyyMMddhhmmss"))> 0) {
                            CanGo= true;
                            qDebug() << QFIPathSource.lastModified().toString("yyyyMMddhhmmss") << QFIPathDestination.lastModified().toString("yyyyMMddhhmmss");
                        }
                        else {
                            #ifdef Q_OS_WIN
                                if (QFIPathSource.created().toString("yyyyMMddhhmmss").compare(QFIPathDestination.created().toString("yyyyMMddhhmmss"))> 0) CanGo= true;
                            #endif
                        }
                    }
                }
                if (CanGo) {
                    if (Simulation) {
                        FilesCopied++;
                        emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesCopied, DirecoriesCopied, FilesDeleted, DirectoriesDeleted, "");
                        emit OnLog(LOG_TYPE_STANDARD, QFIPathSource.absoluteFilePath()+ tr(" copied"));
                    } else {
                        QFile QFSource(QFIPathSource.absoluteFilePath());
                        QFile QFDestination(QFIPathDestination.absoluteFilePath());
                        if (QFSource.open(QIODevice::ReadOnly)) {
                            if (QFDestination.open(QIODevice::WriteOnly)) {
                                qint64 Position= 0;
                                emit OnGenericEvent(EVENT_TYPE_FILE_STATUS, 0, 0, 0, 0, "");
                                emit OnGenericEvent(EVENT_TYPE_FILE, 0, 0, 0, 0, QFSource.fileName());
                                QDateTime QDTLastEvent= QDateTime::currentDateTime();
                                while(DoStart && !QFSource.atEnd()) {
                                    Position+= 4096;
                                    if (QDTLastEvent.msecsTo(QDateTime::currentDateTime())> 250) {
                                        emit OnGenericEvent(EVENT_TYPE_FILE_STATUS, static_cast<int>((Position * 100) / QFSource.size()), 0, 0, 0, "");
                                        QDTLastEvent= QDateTime::currentDateTime();
                                    }
                                    QFDestination.write(QFSource.read(4096));
                                    QFDestination.flush();
                                }
                                emit OnGenericEvent(EVENT_TYPE_FILE_STATUS, 100, 0, 0, 0, "");
                                QFDestination.close();
                                FilesCopied++;
                                emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesCopied, DirecoriesCopied, FilesDeleted, DirectoriesDeleted, "");
                                if (!SetFileDateTime(QFIPathDestination.absoluteFilePath(), QFIPathSource.birthTime(), QFIPathSource.lastRead(), QFIPathSource.lastModified())) emit OnLog(LOG_TYPE_ERROR, QFDestination.fileName()+ tr(" error on change file date time!"));
                                else emit OnLog(LOG_TYPE_STANDARD, QFSource.fileName()+ tr(" copied"));
                            } else OnLog(LOG_TYPE_ERROR, QFSource.fileName()+ tr(" error on copy to ")+ QFDestination.fileName());
                            QFSource.close();
                        } else OnLog(LOG_TYPE_ERROR, QFSource.fileName()+ tr(" error on copy to ")+ QFDestination.fileName());
                    }
                }
            }
        }
    }
}

void QThSynchronize::DirectoryCheckToDelete(QString Source, QString Destination, QString Path) {
    QDir QDPathSource= QDir(Path);
    QDPathSource.setSorting(QDir::Name);
    QStringList QSLFiles= QDPathSource.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (int count= 0; count< QSLFiles.length(); count++) {
        QFileInfo QFIPathSource(QDPathSource.absolutePath()+ "/"+ QSLFiles.at(count));
        QFileInfo QFIPathDestination(Destination+ "/"+ QFIPathSource.absoluteFilePath().right(QFIPathSource.absoluteFilePath().length()- Source.length()));
        if (DoStart && QFIPathSource.isDir()) {
            if (!QFIPathSource.isSymLink() &! QFIPathSource.isBundle()) {
                if (!QFIPathDestination.exists()) {
                    QDir QDPathSource(QFIPathSource.absoluteFilePath());
                    if (Simulation || QDPathSource.removeRecursively()) {
                        DirectoriesDeleted++;
                        emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesCopied, DirecoriesCopied, FilesDeleted, DirectoriesDeleted, "");
                        emit OnLog(LOG_TYPE_STANDARD, QFIPathSource.absoluteFilePath()+ tr(" deleted"));
                    } else emit OnLog(LOG_TYPE_ERROR, QFIPathSource.absoluteFilePath()+ tr(" error on delete!"));
                } else DirectoryCheckToDelete(Source, Destination, QFIPathSource.absoluteFilePath());
            }
        } else if (DoStart && QFIPathSource.isFile()) {
            if (!QFIPathDestination.exists()) {
                QFile QFSource(QFIPathSource.absoluteFilePath());
                if (Simulation || QFSource.remove()) {
                    FilesDeleted++;
                    emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesCopied, DirecoriesCopied, FilesDeleted, DirectoriesDeleted, "");
                    emit OnLog(LOG_TYPE_STANDARD, QFIPathSource.absoluteFilePath()+ tr(" deleted"));
                } else emit OnLog(LOG_TYPE_ERROR, QFIPathSource.absoluteFilePath()+ tr(" error on delete!"));
            }
        }
    }
}
