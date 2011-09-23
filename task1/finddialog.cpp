#include <QtGui>
#include "finddialog.h"

//! [0]
FindDialog::FindDialog(int value, double ** &k, QWidget *parent)
    : QDialog(parent)
{
	size = value;
	kern = k;

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

void FindDialog::handleButton()
{
	checkValues();
}

void FindDialog::checkValues()
{
	bool correctTable = true;

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
		}
	}

	if (correctTable == false) {
		fprintf(stderr, "bad.\n");
	} else {
		fprintf(stderr, "good.\n");
		close();
	}
}
