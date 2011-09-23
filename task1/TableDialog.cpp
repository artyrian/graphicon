#include <QtGui>
#include "TableDialog.h"

//! [0]
TableDialog::TableDialog(int value, double ** &k, bool *statusOk, QWidget *parent)
    : QDialog(parent)
{
	size = value;
	kern = k;
	isOk = statusOk;

	table = new QTableWidget(value, value, this);
	table->setShown(true);

	/* init table */
	for(int i = 0; i < value; i++) {
		for(int j = 0; j < value; j++) {
			QTableWidgetItem *newItem = new QTableWidgetItem(tr("0"));
			table->setItem(i, j, newItem);
		}
	}


    okButton = new QPushButton(tr("&Process"));
    okButton->setDefault(true);
	connect(okButton, SIGNAL(released()),this, SLOT(handleButton()));

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(table);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(leftLayout, 0, 0);
    mainLayout->addWidget(buttonBox, 0, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Extension"));
	resize(400, 400);
}

void TableDialog::handleButton()
{
	checkValues();
}

void TableDialog::checkValues()
{
	bool correctTable = true;

	double sum = 0;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			bool conversionOk = true;
			table->item(i,j)->text().toDouble (&conversionOk);
			if (table->item(i,j)->text() == "" || !conversionOk) {
				table->item(i,j)->setText(tr("0"));
				QMessageBox::information(this, tr("Image Viewer"), tr("Error in item (%1,%2)").arg(i+1).arg(j+1));
				correctTable = false;
			}
			kern[i][j] = table->item(i, j)->text().toDouble();
			sum += kern[i][j];
		}
	}

	if (sum < 0.9 || sum > 1.1) {
	    correctTable = false;
	    QMessageBox::information(this, tr("Image Viewer"), tr("Sum of items not in [0.9, 1.1]."));
	}

	if (correctTable == false) {
		fprintf(stderr, "bad.\n");
		*isOk = false;
	} else {
		fprintf(stderr, "good.\n");
		*isOk = true;
		close();
	}
}
