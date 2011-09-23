#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QTableWidget>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QGroupBox;
class QPushButton;
QT_END_NAMESPACE

class TableDialog : public QDialog
{
    Q_OBJECT

public:
    TableDialog(int p_value, double ** &, bool *, QWidget *parent = 0);

private slots:
	void handleButton();


private:
    QDialogButtonBox *buttonBox;
    QPushButton *okButton;
	QTableWidget *table;
	int size;
	double **kern;
	bool *isOk;
	void checkValues();
};

#endif // TABLEDIALOG_H
