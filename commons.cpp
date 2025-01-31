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
    QFileInfo QFIFileOut(FileOut);
    if (QFIFileOut.isFile()) {
        QFile QFFileOut(FileOut);
        if (QFFileOut.open(QIODevice::WriteOnly | QFile::Append)) {
            if (QFFileOut.setFileTime(QDTLastAccess, QFileDevice::FileAccessTime)
                    #ifdef Q_OS_WIN
                        && QFFileOut.setFileTime(QDTCreation, QFileDevice::FileBirthTime)
                    #endif
                    && QFFileOut.setFileTime(QDTLastWrite, QFileDevice::FileModificationTime)) Result= true;
            QFFileOut.close();
        }
    } else if (QFIFileOut.isDir()) {
        #ifdef Q_OS_UNIX
            utimbuf times;
            qint64 Temp= QDTLastAccess.toMSecsSinceEpoch() / 1000;
            qint64 Temp2= QDTLastWrite.toMSecsSinceEpoch() / 1000;
            if (quint64(Temp)>= Q_UINT64_C(0xffffffff)) times.actime= -1;
            else times.actime= uint(Temp);
            if (quint64(Temp2)>= Q_UINT64_C(0xffffffff)) times.modtime= -1;
            else times.modtime= uint(Temp2);
            Result= utime(FileOut.toStdString().c_str(), &times)== 0;
        #endif
        #ifdef Q_OS_WIN
            FILETIME FTCreation;
            FILETIME FTLastAccess;
            FILETIME FTLastWrite;
            qint64 Temp= QDTCreation.toMSecsSinceEpoch() / 1000;
            qint64 Temp2= QDTLastAccess.toMSecsSinceEpoch() / 1000;
            qint64 Temp3= QDTLastWrite.toMSecsSinceEpoch() / 1000;
            TimetToFileTime(Temp, &FTCreation);
            TimetToFileTime(Temp2, &FTLastAccess);
            TimetToFileTime(Temp3, &FTLastWrite);
            HANDLE h;
            h= CreateFileA(FileOut.toStdString().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
            if (h!= INVALID_HANDLE_VALUE) {
                Result= SetFileTime(h, &FTCreation, &FTLastAccess, &FTLastWrite);
                CloseHandle(h);
            }
        #endif
    }
    return Result;
}

#ifdef Q_OS_WIN
    void TimetToFileTime(time_t t, LPFILETIME pft) {
        ULARGE_INTEGER time_value;
        time_value.QuadPart= ULONGLONG((t * 10000000LL)+ 116444736000000000LL);
        pft->dwLowDateTime= time_value.LowPart;
        pft->dwHighDateTime= time_value.HighPart;
    }
#endif
