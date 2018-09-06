#include "qdemptydirectory.h"
#include "ui_qdemptydirectory.h"

QDEmptyDirectory::QDEmptyDirectory(QWidget *parent) : QDialog(parent), ui(new Ui::QDEmptyDirectory) {
    ui->setupUi(this);
    ThEmptyDirectory= NULL;
    move(parent->window()->mapToGlobal(parent->window()->rect().center())- mapToGlobal(rect().center()));
}

QDEmptyDirectory::~QDEmptyDirectory() {
    if (ThEmptyDirectory) delete ThEmptyDirectory;
    delete ui;
}

void QDEmptyDirectory::OnEnd() {
    ui->QPBStop->click();
}

void QDEmptyDirectory::OnGenericEvent(int Type, int Int0, int Int1) {
    switch(Type) {
        case EVENT_TYPE_JOB_STATUS: {
            ui->QLCDNDirectorieDeleted->display(Int1);
            ui->QLCDNFilesDeleted->display(Int0);
            break;
        }
    }
}

void QDEmptyDirectory::OnLog(int Type, QString Log) {
    while (ui->QLWLog->count()> 1024) ui->QLWLog->takeItem(0);
    ui->QLWLog->addItem(Log);
    switch(Type) {
        case LOG_TYPE_ERROR: ui->QLWLog->item(ui->QLWLog->count()-1)->setForeground(Qt::red); break;
        default: ui->QLWLog->item(ui->QLWLog->count()-1)->setForeground(Qt::black); break;
    }
    ui->QLWLog->scrollToBottom();
}

void QDEmptyDirectory::on_QLEDirectory_textChanged(const QString &) {
    if (ui->QLEDirectory->text().length()> 0) ui->QPBStart->setEnabled(true);
}

void QDEmptyDirectory::on_QPBClose_clicked() {
    if (ui->QPBStop->isEnabled()) on_QPBStop_clicked();
    this->accept();
}

void QDEmptyDirectory::on_QPBStart_clicked() {
    if (QMessageBox::question(this, tr("Warning!"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No)== QMessageBox::Yes) {
        ui->QPBStart->setEnabled(false);
        ThEmptyDirectory= new QThEmptyDirectory(ui->QLEDirectory->text());
        connect(ThEmptyDirectory, SIGNAL(OnEnd()), this, SLOT(OnEnd()), Qt::QueuedConnection);
        connect(ThEmptyDirectory, SIGNAL(OnGenericEvent(int,int,int)), this, SLOT(OnGenericEvent(int,int,int)));
        connect(ThEmptyDirectory, SIGNAL(OnLog(int,QString)), this, SLOT(OnLog(int,QString)), Qt::QueuedConnection);
        ThEmptyDirectory->start();
        ui->QPBStop->setEnabled(true);
    }
}

void QDEmptyDirectory::on_QPBStop_clicked() {
    ui->QPBStop->setEnabled(false);
    delete ThEmptyDirectory;
    ThEmptyDirectory= NULL;
    ui->QPBStart->setEnabled(true);
}

void QDEmptyDirectory::on_QTBDirectory_clicked() {
    QString Path= QFileDialog::getExistingDirectory(this, tr("Directory to empty"), "", QFileDialog::ShowDirsOnly);
    if (Path!= NULL) ui->QLEDirectory->setText(Path);
}
