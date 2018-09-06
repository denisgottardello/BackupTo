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

#include "qdtextinput.h"
#include "ui_qdtextinput.h"

QDTextInput::QDTextInput(QWidget *parent) : QDialog(parent), ui(new Ui::QDTextInput) {
    ui->setupUi(this);
}

QDTextInput::~QDTextInput() {
    delete ui;
}

void QDTextInput::on_QPBCancel_clicked() {
    this->reject();
}

void QDTextInput::on_QPBOk_clicked() {
    this->accept();
}

void QDTextInput::on_QLETextInput_textChanged(QString ) {
    if (ui->QLETextInput->text().size()> 0) ui->QPBOk->setEnabled(true);
    else ui->QPBOk->setEnabled(false);
}

void QDTextInput::on_QLETextInput_returnPressed() {
    this->accept();
}
