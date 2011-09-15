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

struct ImgInfo
{
	double coeffR;
	double coeffG;
	double coeffB;
	double intensityMin;
	double intensityMax;
	ImgInfo (double r, double g, double b);
};

struct Position
{
	int x0;
	int y0;
	int x1;
	int y1;
};


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
	void filterSmoothing();
	void filterSharpness();

	void wavesH();
	void wavesV();
private:
	Position pos;
	void createActions();
	void createMenus();
	void extremumsIntensity(ImgInfo *);
	void setWorkArea(int, int, int, int);

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
	QAction *filterSmoothingAct;
	QAction *filterSharpnessAct;

	QAction *wavesHAct;
	QAction *wavesVAct;

	QMenu *fileMenu;
        QMenu *contrastMenu;

	QMenu *effectsMenu;
};



#endif // IMAGEPROCESSING_H
