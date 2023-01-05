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

#ifndef THEMPTYDIRECTORY_H
#define THEMPTYDIRECTORY_H

#include "commons.h"
#include "QDir"
#include "QFile"
#include <QThread>

class QThEmptyDirectory : public QThread
{
    Q_OBJECT

public:
    QThEmptyDirectory(QString Path);
    ~QThEmptyDirectory();

signals:
    void OnEnd();
    void OnGenericEvent(EventTypes EventType, int Int0, int Int1);
    void OnLog(LogTypes Type, QString Log);

private:
    void run();
    bool DoStart= true;
    int FilesDeleted= 0, DirectoriesDeleted= 0;
    QString Path;
    void EmptyDirectory(QString Path);

};

#endif // THEMPTYDIRECTORY_H
