#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMenuBar>
#include <QWidget>
#include "GlWidget.h"

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

private slots:
        void setAnimation();
        void presentation();
        void setMirror();
        void setMode();

protected:
	void keyPressEvent(QKeyEvent *);


private:
        void createActions();
        void createMenus();

	GLWidget *glWidget;

        QAction *fullScreen;
        QAction *normalScreen;
        QAction *animation;
        QAction *setPresentation;
        QAction *mirror;
        QAction *exitAct;

        QMenu *menu;
        QTimer *timer;
        QMenuBar *menubar;

};

#endif // WINDOW_H
