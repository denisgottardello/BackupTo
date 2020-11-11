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

#ifndef QDTEXTINPUT_H
#define QDTEXTINPUT_H

#include "ui_qdtextinput.h"
#include <QDialog>
#include "ui_qdtextinput.h"

namespace Ui {
    class QDTextInput;
}

class QDTextInput : public QDialog
{
    Q_OBJECT

public:
    explicit QDTextInput(QWidget *parent = nullptr);
    ~QDTextInput();
    Ui::QDTextInput *ui;

private:

private slots:
    void on_QLETextInput_returnPressed();
    void on_QLETextInput_textChanged(QString );
    void on_QPBOk_clicked();
    void on_QPBCancel_clicked();

};

#endif // QDTEXTINPUT_H
