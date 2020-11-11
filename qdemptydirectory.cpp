#include "qdemptydirectory.h"
#include "ui_qdemptydirectory.h"

QDEmptyDirectory::QDEmptyDirectory(QWidget *parent) : QDialog(parent), ui(new Ui::QDEmptyDirectory) {
    ui->setupUi(this);
    QSIMModel= new QStandardItemModel(this);
    connect(&Timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    Timer.start(500);
    move(parent->window()->mapToGlobal(parent->window()->rect().center())- mapToGlobal(rect().center()));
}

QDEmptyDirectory::~QDEmptyDirectory() {
    if (ThEmptyDirectory) delete ThEmptyDirectory;
    delete QSIMModel;
    delete ui;
}

void QDEmptyDirectory::OnEnd() {
    ui->QPBStop->click();
    ui->QLVLog->scrollToBottom();
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
    while (QSIMModel->rowCount()> 1024 * 8) QSIMModel->takeRow(0);
    QSIMModel->setRowCount(QSIMModel->rowCount()+ 1);
    QStandardItem *StandardItem0= new QStandardItem(Log);
    switch(Type) {
        case LOG_TYPE_ERROR: StandardItem0->setForeground(Qt::red); break;
        default: break;
    }
    QSIMModel->setItem(QSIMModel->rowCount()- 1, 0, StandardItem0);
    ui->QLVLog->setModel(QSIMModel);
}

void QDEmptyDirectory::OnTimer() {
    if (ThEmptyDirectory) ui->QLVLog->scrollToBottom();
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
    ThEmptyDirectory= nullptr;
    ui->QPBStart->setEnabled(true);
}

void QDEmptyDirectory::on_QTBDirectory_clicked() {
    QString Path= QFileDialog::getExistingDirectory(this, tr("Directory to empty"), "", QFileDialog::ShowDirsOnly);
    if (Path.length()> 0) ui->QLEDirectory->setText(Path);
}
