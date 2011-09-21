#include <QtGui>
#include "filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent, const char *name)
{
	createInputForm();


	setWindowTitle(tr("Filters"));
}

void FilterDialog::createInputForm()
{
	inputForm = new QGroupBox(tr("Input form"));
	QHBoxLayout *layout = new QHBoxLayout;

	okButton = new QPushButton();
	okButton->setShown(true);


	int value = 3;
	matrixTable = new QTableWidget(value, value);
	matrixTable->setShown(true);

	for(int i = 0; i < value; i++) {
		for(int j = 0; j < value; j++) {
			QTableWidgetItem *newItem = new QTableWidgetItem(tr("0"));
			matrixTable->setItem(i, j, newItem);
		}
	}


	inputForm->setLayout(layout);
}
