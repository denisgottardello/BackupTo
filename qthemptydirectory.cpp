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

#include "qthemptydirectory.h"

QThEmptyDirectory::QThEmptyDirectory(QString Path) : QThread() {
    this->Path= Path;
    DoStart= true;
    FilesDeleted= DirectoriesDeleted= 0;
}

QThEmptyDirectory::~QThEmptyDirectory() {
    DoStart= false;
    while (this->isRunning()) msleep(10);
}

void QThEmptyDirectory::run() {
    emit OnLog(LOG_TYPE_STANDARD, tr("Directory empty begin"));
    EmptyDirectory(Path);
    if (DoStart) emit OnLog(LOG_TYPE_STANDARD, tr("Directory empty completed"));
    else emit OnLog(LOG_TYPE_STANDARD, tr("Directory empty interrupted"));
    if (DoStart) emit OnEnd();
}

void QThEmptyDirectory::EmptyDirectory(QString Path) {
    QDir QDPath= QDir(Path);
    QDPath.setSorting(QDir::Name);
    QStringList QSLFiles= QDPath.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    for (int count= 0; count< QSLFiles.size(); count++) {
        QFileInfo QFIPath(Path+ QDir::separator()+ QSLFiles.at(count));
        if (QFIPath.isDir()) {
            if (DoStart) EmptyDirectory(QFIPath.absoluteFilePath());
            QDir Dir;
            if (Dir.rmdir(QFIPath.absoluteFilePath())) {
                DirectoriesDeleted++;
                emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesDeleted, DirectoriesDeleted);
                if (DoStart) emit OnLog(LOG_TYPE_STANDARD, "Directory "+ QFIPath.absoluteFilePath()+ tr(" removed"));
            } else emit OnLog(LOG_TYPE_ERROR, tr("Error removing directory ")+ QFIPath.absoluteFilePath()+ "!");
        } else {
            QFile File(QFIPath.absoluteFilePath());
            if (!File.isWritable()) File.setPermissions(QFile::WriteOwner);
            if (File.remove()) {
                FilesDeleted++;
                emit OnGenericEvent(EVENT_TYPE_JOB_STATUS, FilesDeleted, DirectoriesDeleted);
                if (DoStart) emit OnLog(LOG_TYPE_STANDARD, "File "+ QFIPath.absoluteFilePath()+ tr(" removed"));
            } else emit OnLog(LOG_TYPE_ERROR, tr("Error removing file ")+ QFIPath.absoluteFilePath()+ "!");
        }
        if (!DoStart) break;
    }
}
