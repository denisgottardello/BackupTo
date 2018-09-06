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

#include "QString"
#include "QStringList"
#ifdef Q_OS_WIN
    #include "windows.h"
#else
    #include <utime.h>
#endif
#include "QDateTime"

#define SIDEA_SIDEB_MERGE 0
#define SIDEA_TO_SIDEB 1
#define SIDEB_TO_SIDEA 2

struct Profile {
    bool IgnoreFileDate, DeleteFilesOnTargetSide;
    int Direction;
    QString Description, SideA, SideB;
    QStringList QSLExclusionFiles;
};

bool SetFileDateTime(QString FileOut, QDateTime QDTCreation, QDateTime QDTLastAccess, QDateTime QDTLastWrite);
#ifdef Q_OS_WIN
    FILETIME ToWindowsFileTime(const QDateTime QDTValue);
#endif

#endif // COMMONS_H
