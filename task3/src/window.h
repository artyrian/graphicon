#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "GlWidget.h"


class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

protected:
	void keyPressEvent(QKeyEvent *);

private:
	GLWidget *glWidget;
};

#endif // WINDOW_H
