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

    animation = new QAction("Animation [on | off]", this);
    animation->setShortcut(tr("A"));
    connect(animation, SIGNAL(triggered()), this, SLOT(setAnimation()));

    setPresentation = new QAction("Presentation [on | off]", this);
    setPresentation->setShortcut((tr("P")));
    connect(setPresentation, SIGNAL(triggered()), this, SLOT(presentation()));

    mirror = new QAction("Mirror: [on | off ]", this);
    mirror->setShortcut((tr("M")));
    connect(mirror, SIGNAL(triggered()), this, SLOT(setMirror()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(tr("Esc"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void Window::setAnimation()
{
    glWidget->animation();
}

void Window::presentation()
{
    glWidget->rotation();

    setMode();
}

void Window::createMenus()
{
    menu = new QMenu(tr("Main menu"), this);
    menu->addAction(fullScreen);
    menu->addAction(normalScreen);
    menu->addAction(animation);
    menu->addAction(setPresentation);
    menu->addAction(mirror);
    menu->addAction(exitAct);

    menubar->addMenu(menu);
}

void Window::setMirror()
{
    glWidget->showMirror = !glWidget->showMirror;
    glWidget->updateGL();
}

void Window::setMode()
{
    glWidget->lolMode = !glWidget->lolMode;
    glWidget->updateGL();
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
        case Qt::Key_Space :
            glWidget->rotation();
            break;
	default:
	    QWidget::keyPressEvent(e);
    }
}
