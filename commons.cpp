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

#include "commons.h"

bool SetFileDateTime(QString FileOut, QDateTime QDTCreation, QDateTime QDTLastAccess, QDateTime QDTLastWrite) {
    bool Result= false;
    #ifdef Q_OS_WIN
        LPCWSTR lpFileName= (const wchar_t*)FileOut.utf16();
        HANDLE hFile= CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (hFile== INVALID_HANDLE_VALUE) Result= false;
        else {
            CONST FILETIME lpCreationTime= ToWindowsFileTime(QDTCreation);
            CONST FILETIME lpLastAccessTime= ToWindowsFileTime(QDTLastAccess);
            CONST FILETIME lpLastWriteTime= ToWindowsFileTime(QDTLastWrite);
            Result= SetFileTime(hFile, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime);
            CloseHandle(hFile);
        }
    #else
        struct utimbuf NewTime;
        NewTime.actime= QDTLastAccess.toTime_t();
        NewTime.modtime= QDTLastWrite.toTime_t();
        if (utime(FileOut.toStdString().c_str(), &NewTime)< 0) Result= false;
        else Result= true;
    #endif
    return Result;
}

#ifdef Q_OS_WIN
    FILETIME ToWindowsFileTime(const QDateTime QDTValue) {
        // Definition of FILETIME from MSDN:
        // Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
        QDateTime origin(QDate(1601, 1, 1), QTime(0, 0, 0, 0), Qt::UTC);
        // Get offset - note we need 100-nanosecond intervals, hence we multiply by
        // 10000.
        qint64 _100nanosecs = 10000 * origin.msecsTo(QDTValue);
        // Pack _100nanosecs into the structure.
        FILETIME fileTime;
        fileTime.dwLowDateTime = _100nanosecs;
        fileTime.dwHighDateTime = (_100nanosecs >> 32);
        return fileTime;
    }
#endif
