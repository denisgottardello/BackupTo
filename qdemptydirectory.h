#ifndef QDEMPTYDIRECTORY_H
#define QDEMPTYDIRECTORY_H

#include <QDialog>
#include "QFileDialog"
#include "QMessageBox"
#include "QStandardItemModel"
#include "qthemptydirectory.h"
#include "QTimer"

namespace Ui {
class QDEmptyDirectory;
}

class QDEmptyDirectory : public QDialog
{
    Q_OBJECT
    
public:
    explicit QDEmptyDirectory(QWidget *parent = nullptr);
    ~QDEmptyDirectory();
    
private slots:
    void on_QLEDirectory_textChanged(const QString &arg1);
    void on_QPBClose_clicked();
    void on_QPBStart_clicked();
    void on_QPBStop_clicked();
    void on_QTBDirectory_clicked();
    void OnEnd();
    void OnGenericEvent(int Type, int Int0, int Int1);
    void OnLog(int Type, QString Log);
    void OnTimer();

private:
    Ui::QDEmptyDirectory *ui;
    QTimer Timer;
    QStandardItemModel *QSIMModel= nullptr;
    QThEmptyDirectory *ThEmptyDirectory= nullptr;

};

#endif // QDEMPTYDIRECTORY_H
