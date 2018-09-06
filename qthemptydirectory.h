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

#include <QThread>
#include "QDir"
#include "QFile"

#define EVENT_TYPE_JOB_STATUS 2

#define LOG_TYPE_ERROR 0
#define LOG_TYPE_STANDARD 1

class QThEmptyDirectory : public QThread
{
    Q_OBJECT

public:
    QThEmptyDirectory(QString Path);
    ~QThEmptyDirectory();

signals:
    void OnEnd();
    void OnGenericEvent(int Type, int Int0, int Int1);
    void OnLog(int Type, QString Log);

private:
    void run();
    bool DoStart;
    int FilesDeleted, DirectoriesDeleted;
    QString Path;
    void EmptyDirectory(QString Path);

};

#endif // THEMPTYDIRECTORY_H
