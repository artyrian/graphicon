#include <QtGui>
#include "filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent, const char *name)
{
	setWindowTitle(tr("Filters"));

	QTableWidget *filesTable = new QTableWidget(2, 2);
	filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	filesTable->setShowGrid(true);
	filesTable->setShown(true);

	resize(50, 100);
}

