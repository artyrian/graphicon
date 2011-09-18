#ifndef FILTER_DIALOG_H
#define FILTER_DIALOG_H

#include <qdialog.h>

class FilterDialog : public QDialog
{
	Q_OBJECT;
public:
	FilterDialog (QWidget *parent = 0, const char *name = 0);
signals:

private slots:

private:
	int sigma;
};

#endif // FILTER_DIALOG_H
