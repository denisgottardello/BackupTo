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

#ifndef THSYNCHRONIZE_H
#define THSYNCHRONIZE_H

#include "commons.h"
#include "QDebug"
#include "QDir"
#include <QThread>

class QThSynchronize : public QThread
{
    Q_OBJECT

public:
    QThSynchronize(Profile profile, bool Simulation);
    ~QThSynchronize();

signals:
    void OnEnd();
    void OnGenericEvent(EventTypes EventType, int Int0, int Int1, int Int2, int Int3, QString String0);
    void OnLog(LogTypes Type, QString Log);

private:
    bool DoStart= true, Simulation;
    int FilesCopied= 0, DirecoriesCopied= 0, FilesDeleted= 0, DirectoriesDeleted= 0;
    Profile profile;
    bool FileCopy(QFileInfo &QFIPathSource, QFileInfo &QFIPathDestination);
    void DirectoryCheckToCopy(QString Source, QString Destination, QString Path, bool IgnoreFileDate, bool OnlyIfBest);
    void DirectoryCheckToDelete(QString Source, QString Destination, QString Path);
    void run();

};

#endif // THSYNCHRONIZE_H
