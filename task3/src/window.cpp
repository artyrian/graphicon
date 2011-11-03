#include <QtGui>

#include "window.h"
#include "GlWidget.h"
#include <QtGui>

Window::Window()
{
    glWidget = new GLWidget;
    menubar = new QMenuBar;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glWidget);
    mainLayout->setMenuBar(menubar);

    setLayout(mainLayout);
    setWindowTitle("Watch");

    createActions();
    createMenus();

    timer = new QTimer(this);
    connect(timer,  SIGNAL(timeout()), this, SLOT(update()));
}

void Window::createActions()
{
    fullScreen = new QAction("Full Screen", this);
    fullScreen->setShortcut(tr("F"));
    connect(fullScreen, SIGNAL(triggered()), this, SLOT(showFullScreen()));

    normalScreen = new QAction("Window Screen", this);
    normalScreen->setShortcut(tr("W"));
    connect(normalScreen, SIGNAL(triggered()), this, SLOT(showNormal()));

    animation = new QAction("Animation", this);
    connect(animation, SIGNAL(triggered()), this, SLOT(setAnimation()));

    setPresentation = new QAction("Presentation", this);
    connect(setPresentation, SIGNAL(triggered()), this, SLOT(presentation()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(tr("Esc"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void Window::setAnimation()
{
    glWidget->isAnimation = !glWidget->isAnimation;
}

void Window::presentation()
{
    timer->start(1000);
    glWidget->presentation();
}

void Window::createMenus()
{
    menu = new QMenu(tr("Main menu"), this);
    menu->addAction(fullScreen);
    menu->addAction(normalScreen);
    menu->addAction(animation);
    menu->addAction(setPresentation);
    menu->addAction(exitAct);

    menubar->addMenu(menu);
}


void Window::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
	case Qt::Key_Escape :
	    close();
	    break;
        case Qt::Key_F :
            showFullScreen();
            show();
            break;
        case Qt::Key_W :
            showMaximized();
            show();
            break;
	default:
	    QWidget::keyPressEvent(e);
    }
}
