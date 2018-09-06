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

#ifndef QDFILEATTRIBUTES_H
#define QDFILEATTRIBUTES_H

#include <QDialog>
#include "QDateTime"
#include "commons.h"
#include "QMessageBox"
#include "QFileDialog"

namespace Ui {
class QDFileAttributes;
}

class QDFileAttributes : public QDialog
{
    Q_OBJECT
    
public:
    explicit QDFileAttributes(QWidget *parent = 0);
    ~QDFileAttributes();
    
private slots:
    void on_QDTECreation_dateTimeChanged(const QDateTime &date);
    void on_QDTELastAccess_dateTimeChanged(const QDateTime &date);
    void on_QDTELastModified_dateTimeChanged(const QDateTime &date);
    void on_QPBApply_clicked();
    void on_QPBClose_clicked();
    void on_QPBSelectFile_clicked();

private:
    Ui::QDFileAttributes *ui;
    void ReadAttributes();

};

#endif // QDFILEATTRIBUTES_H
