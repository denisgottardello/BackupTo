#ifndef QDEMPTYDIRECTORY_H
#define QDEMPTYDIRECTORY_H

#include <QDialog>
#include "QFileDialog"
#include "qthemptydirectory.h"
#include "QMessageBox"

namespace Ui {
class QDEmptyDirectory;
}

class QDEmptyDirectory : public QDialog
{
    Q_OBJECT
    
public:
    explicit QDEmptyDirectory(QWidget *parent = 0);
    ~QDEmptyDirectory();
    
private slots:
    void OnEnd();
    void OnGenericEvent(int Type, int Int0, int Int1);
    void OnLog(int Type, QString Log);
    void on_QLEDirectory_textChanged(const QString &arg1);
    void on_QPBClose_clicked();
    void on_QPBStart_clicked();
    void on_QPBStop_clicked();
    void on_QTBDirectory_clicked();

private:
    Ui::QDEmptyDirectory *ui;
    QThEmptyDirectory *ThEmptyDirectory;

};

#endif // QDEMPTYDIRECTORY_H
