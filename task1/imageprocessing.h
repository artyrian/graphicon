#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QMainWindow>
#define MAX_LENGHT_HISTOGRAM 256

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QAction;
class QLabel;
class QScrollArea;
QT_END_NAMESPACE

class ImageProcessing : public QMainWindow
{
	 Q_OBJECT

public:
        ImageProcessing();

private slots:
        void open();
        void save();
	void linearStretchingRGB();
	void linearStretchingR();
	void linearStretchingG();
	void linearStretchingB();

private:
	void createActions();
	void createMenus();
	void linearStretching(int isR, int isG, int isB);

        QLabel *imageLabel;
        QImage *image;

	QScrollArea *scrollArea;
        QAction *openAct;
        QAction *saveAct;
	QAction *exitAct;
	QAction *linearStretchingRGBAct;
	QAction *linearStretchingRAct;
	QAction *linearStretchingGAct;
	QAction *linearStretchingBAct;

	QMenu *fileMenu;
        QMenu *contrastMenu;
};

#endif // IMAGEPROCESSING_H
