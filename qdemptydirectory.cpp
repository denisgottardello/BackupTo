#include "qdemptydirectory.h"
#include "ui_qdemptydirectory.h"

QDEmptyDirectory::QDEmptyDirectory(QWidget *parent) : QDialog(parent), ui(new Ui::QDEmptyDirectory) {
    ui->setupUi(this);
    pQStandardItemModel= new QStandardItemModel(this);
    connect(&Timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    Timer.start(500);
    move(parent->window()->mapToGlobal(parent->window()->rect().center())- mapToGlobal(rect().center()));
}

QDEmptyDirectory::~QDEmptyDirectory() {
    if (pQThEmptyDirectory) delete pQThEmptyDirectory;
    delete pQStandardItemModel;
    delete ui;
}

void QDEmptyDirectory::OnEnd() {
    ui->QPBStop->click();
    ui->QLVLog->scrollToBottom();
}

void QDEmptyDirectory::OnGenericEvent(EventTypes EventType, int Int0, int Int1) {
    switch(EventType) {
        case EVENT_TYPE_JOB_STATUS: {
            ui->QLCDNDirectorieDeleted->display(Int1);
            ui->QLCDNFilesDeleted->display(Int0);
            break;
        }
        default:
            break;
    }
}

void QDEmptyDirectory::OnLog(LogTypes Type, QString Log) {
    while (pQStandardItemModel->rowCount()> 1024 * 8) pQStandardItemModel->takeRow(0);
    pQStandardItemModel->setRowCount(pQStandardItemModel->rowCount()+ 1);
    QStandardItem *StandardItem0= new QStandardItem(Log);
    switch(Type) {
        case LOG_TYPE_ERROR: StandardItem0->setForeground(Qt::red); break;
        default: break;
    }
    pQStandardItemModel->setItem(pQStandardItemModel->rowCount()- 1, 0, StandardItem0);
    ui->QLVLog->setModel(pQStandardItemModel);
}

void QDEmptyDirectory::OnTimer() {
    if (pQThEmptyDirectory) ui->QLVLog->scrollToBottom();
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
        pQThEmptyDirectory= new QThEmptyDirectory(ui->QLEDirectory->text());
        connect(pQThEmptyDirectory, SIGNAL(OnEnd()), this, SLOT(OnEnd()), Qt::QueuedConnection);
        connect(pQThEmptyDirectory, SIGNAL(OnGenericEvent(EventTypes,int,int)), this, SLOT(OnGenericEvent(EventTypes,int,int)), Qt::QueuedConnection);
        connect(pQThEmptyDirectory, SIGNAL(OnLog(LogTypes,QString)), this, SLOT(OnLog(LogTypes,QString)), Qt::QueuedConnection);
        pQThEmptyDirectory->start();
        ui->QPBStop->setEnabled(true);
    }
}

void QDEmptyDirectory::on_QPBStop_clicked() {
    ui->QPBStop->setEnabled(false);
    delete pQThEmptyDirectory;
    pQThEmptyDirectory= nullptr;
    ui->QPBStart->setEnabled(true);
}

void QDEmptyDirectory::on_QTBDirectory_clicked() {
    QString Path= QFileDialog::getExistingDirectory(this, tr("Directory to empty"), "", QFileDialog::ShowDirsOnly);
    if (Path.length()> 0) ui->QLEDirectory->setText(Path);
}
