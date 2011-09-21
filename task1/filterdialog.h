#ifndef FILTER_DIALOG_H
#define FILTER_DIALOG_H

#include <qdialog.h>
#include <QGroupBox>
#include <QTableWidget>
#include <QPushButton>

class FilterDialog : public QDialog
{
	Q_OBJECT;
public:
	FilterDialog (QWidget *parent = 0, const char *name = 0);
signals:

private slots:
	void createInputForm();

private:
	QGroupBox *inputForm;
	QTableWidget *matrixTable;
	QPushButton *okButton;
};

#endif // FILTER_DIALOG_H
