#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QMainWindow>
#include <QLabel>
#include <QRubberBand>
#define MAX_LENGHT_HISTOGRAM 256

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QAction;
class QLabel;
class QPoint;
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
	Position(int, int, int, int);
};

class AreaImage : public QLabel
{
	friend class ImageProcessing;
	Q_OBJECT
private:
	bool selectedByMouse;
public:
	QPoint firstCorner;
	QPoint secondCorner;
	AreaImage();
	void setSelectedByMouse(bool);
	Position points();
protected:
	QRubberBand *rubberBand;
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
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
	void linearStretchingChannels();
	void grayWorld();
	int normalizeColorValue (double color);
	void filterSmoothingSimple();
	void filterSmoothing();
	void filterSharpness();
	void filterMedian();
	void filterOptional();
	void wavesShortEffect();
	void wavesLongEffect();
	void glassEffect();
	void selectByMouse();
	void rotate();


private:
	double **kernelGauss2D(double, double);
	void gaussBlurSimple(double, double **);
	double *kernelGauss1D (double);
	void gaussBlur(double);
	void medianProcess(int);
	void createActions();
	void createMenus();
	void extremumsIntensity(ImgInfo *);
	void copyImageBuf(QImage *);
	QRgb realPixel(int, int);
	int realX(int);
	int realY(int);

	AreaImage *imageLabel;
	QImage *image;

	QScrollArea *scrollArea;
        QAction *openAct;
        QAction *saveAct;
	QAction *exitAct;
	QAction *linearStretchingRGBAct;
	QAction *linearStretchingRAct;
	QAction *linearStretchingGAct;
	QAction *linearStretchingBAct;
	QAction *linearStretchingChannelsAct;
	QAction *grayWorldAct;
	QAction *filterSmoothingSimpleAct;
	QAction *filterSmoothingAct;
	QAction *filterSharpnessAct;
	QAction *filterMedianAct;
	QAction *filterOptionalAct;
	QAction *wavesShortEffectAct;
	QAction *wavesLongEffectAct;
	QAction *glassEffectAct;
	QAction *rotateAct;
	QAction *selectByMouseAct;

	QMenu *fileMenu;
	QMenu *filterMenu;
	QMenu *effectsMenu;
	QMenu *transformationMenu;
	QMenu *optionsMenu;
};



#endif // IMAGEPROCESSING_H
