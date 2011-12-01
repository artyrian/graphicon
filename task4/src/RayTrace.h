#ifndef RAYTRACE_H
#define RAYTRACE_H

#include <QtGui>
#include "scene1.h"

#define DELTA_SHIFT 30

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu *fileMenu;
    QAction *drawImageAct1;
    QAction *drawImageAct2;
    QAction *exitAct;

    QScrollArea *scrollArea;
    QLabel *imageLabel;

    void createActions();
    void createMenus();
    void updateActions();

    Scene1 *scene1;

public:
    MainWindow();
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void init1();
    void init2();
    void drawImage();
};

#endif // RAYTRACE_H
