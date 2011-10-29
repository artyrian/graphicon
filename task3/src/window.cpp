#include <QtGui>

#include "window.h"
#include "GlWidget.h"


Window::Window()
{
    glWidget = new GLWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);
    setWindowTitle("Watch");
}


void Window::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
	case Qt::Key_Escape :
	    close();
	    break;

	default:
	    QWidget::keyPressEvent(e);
    }
}
