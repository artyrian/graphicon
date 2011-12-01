#include "RayTrace.h"
#include <iostream>
#include <unistd.h>

MainWindow::MainWindow()
{
	scene1 = new Scene1();

	imageLabel = new QLabel;
	imageLabel->setBackgroundRole(QPalette::Base);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);

	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);
	setCentralWidget(scrollArea);

	createActions();
	createMenus();

	setWindowTitle(tr("Ray Trace"));
	resize(800, 800);
}

void MainWindow::createActions()
{
	drawImageAct1 = new QAction(tr("&Draw Scene 1"), this);
	drawImageAct1->setShortcut(tr("1"));
	connect(drawImageAct1, SIGNAL(triggered()), this, SLOT(init1()));

	drawImageAct2 = new QAction(tr("&Draw Scene 2"), this);
	drawImageAct2->setShortcut(tr("2"));
	connect(drawImageAct2, SIGNAL(triggered()), this, SLOT(init2()));


	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(drawImageAct1);
	fileMenu->addAction(drawImageAct2);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	menuBar()->addMenu(fileMenu);
}

void MainWindow::updateActions()
{
}

void MainWindow::init1()
{
	scene1->initScene1();
	drawImage();
}

void MainWindow::init2()
{
	scene1->initScene2();
	drawImage();

	QMessageBox::StandardButton reply;
	reply = QMessageBox::information(this, tr("Happy New Year!"), "It coming soon :)");

}

void MainWindow::drawImage()
{
	//Progressive Rendering
	int power_progressive = 4;

	imageLabel->setPixmap(QPixmap::fromImage(*(scene1->buildImage(power_progressive))));
	imageLabel->adjustSize();

	imageLabel->repaint();
	repaint();

	imageLabel->setPixmap(QPixmap::fromImage(*(scene1->buildImage2(power_progressive))));
	imageLabel->adjustSize();
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{

	switch (e->key()) {
	case Qt::Key_W:
		scene1->setRotation(0, DELTA_SHIFT);
		drawImage();
		break;
	case Qt::Key_D :
		scene1->setRotation(-DELTA_SHIFT, 0);
		drawImage();
		break;
	case Qt::Key_S :
		scene1->setRotation(0, -DELTA_SHIFT);
		drawImage();
		break;
	case Qt::Key_A:
		scene1->setRotation(DELTA_SHIFT, 0);
		drawImage();
		break;
	default:
		return;
	}


}

MainWindow::~MainWindow()
{
	delete scene1;
}
