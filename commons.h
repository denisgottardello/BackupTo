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

#ifndef COMMONS_H
#define COMMONS_H

#include "QDateTime"
#include "QFileInfo"
#include "QStringList"
#ifdef Q_OS_UNIX
    #include <utime.h>
#endif
#ifdef Q_OS_WIN
    #include <Windows.h>
#endif

#define VERSION QString("1.0.12.0")

enum EventTypes {
    EVENT_TYPE_FILE,
    EVENT_TYPE_FILE_STATUS,
    EVENT_TYPE_JOB_STATUS,
};

enum LogTypes {
    LOG_TYPE_ERROR,
    LOG_TYPE_STANDARD,
};

enum Sides {
    SIDEA_SIDEB_MERGE= 0,
    SIDEA_TO_SIDEB= 1,
    SIDEB_TO_SIDEA= 2,
};

struct Profile {
    bool IgnoreFileDate, DeleteFilesOnTargetSide;
    Sides Direction;
    QString Description, SideA, SideB;
    QStringList QSLExclusionFiles;
};

bool SetFileDateTime(QString FileOut, QDateTime QDTCreation, QDateTime QDTLastAccess, QDateTime QDTLastWrite);
#ifdef Q_OS_WIN
    void TimetToFileTime(time_t t, LPFILETIME pft);
#endif

#endif // COMMONS_H
