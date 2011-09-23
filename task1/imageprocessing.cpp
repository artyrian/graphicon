#include <QtGui>
#include "imageprocessing.h"
#include <time.h>

// BT.709: Y = 0.2125·R + 0.7154·G + 0.0721·B
const double coeffRed = 0.2125;
const double coeffGreen = 0.7154;
const double coeffBlue = 0.0721;
const double pi = 3.1416;
const double sharpSigma = 1.0;
const double sigmaScale  = 0.5;

Position::Position(int x0, int y0, int x1, int y1)
{
	this->x0 = x0;
	this->y0 = y0;
	this->x1 = x1;
	this->y1 = y1;
}

struct PixelRGBSpecific
{
	double red;
	double green;
	double blue;
	double sumNormal;

	PixelRGBSpecific ()
		: red(0), green(0), blue(0), sumNormal(0)
	{};

	void add(double r, double g, double b)
	{
		red += r;
		green += g;
		blue += b;
	}

	void addSumNormal(double sum)
	{
		sumNormal += sum;
	}

	void normalize()
	{
		red /= sumNormal;
		green /= sumNormal;
		blue /= sumNormal;
	}
};

ImgInfo::ImgInfo (double r, double g, double b)
	: coeffR (r), coeffG(g), coeffB(b)
{
	intensityMin = 255.0;
	intensityMax = 0.0;
};

ImageProcessing::ImageProcessing()
{
	image = new QImage;
	imageLabel = new AreaImage;

	imageLabel->setBackgroundRole(QPalette::Dark);
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	imageLabel->setScaledContents(true);

	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);
	setCentralWidget(scrollArea);

	createActions();
	createMenus();

	setWindowTitle(tr("Image Processing"));
	resize (500, 400);
}

AreaImage::AreaImage()
{
	rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	selectedByMouse = false;
}

Position AreaImage::points(int maxX, int maxY)
{
	if (selectedByMouse) {
		if ( firstCorner.x() < 0 ) firstCorner.setX(0);
		if ( firstCorner.y() < 0 ) firstCorner.setY(0);
		if ( secondCorner.x() >= maxX ) secondCorner.setX(maxX);
		if ( secondCorner.y() >= maxY ) secondCorner.setY(maxY);
		return Position (firstCorner.x(), firstCorner.y(), secondCorner.x(), secondCorner.y());
	} else {
		return Position (0, 0, maxX, maxY);
	}
}

void AreaImage::setSelectedByMouse(bool isSelect)
{
	selectedByMouse = isSelect;
}

void AreaImage::mousePressEvent(QMouseEvent *event)
 {
	if (selectedByMouse) {
		rubberBand->hide();
		firstCorner = event->pos();
		rubberBand->setGeometry(QRect(firstCorner, QSize()));
		rubberBand->show();
	}
 }

void AreaImage::mouseMoveEvent(QMouseEvent *event)
 {
	if (selectedByMouse) {
		rubberBand->setGeometry(QRect(firstCorner, event->pos()));
	}
 }

void AreaImage::mouseReleaseEvent(QMouseEvent *event)
 {
	if (selectedByMouse) {
		secondCorner = event->pos();
	}
 }

void ImageProcessing::createActions()
{
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	saveAct = new QAction(tr("&Save..."), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setDisabled(true);
	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

	linearStretchingRGBAct = new QAction(tr("&Linear RGB histogram stretching"), this);
	linearStretchingRGBAct->setShortcut((tr("Ctrl+1")));
	connect(linearStretchingRGBAct, SIGNAL(triggered()), this, SLOT(linearStretchingRGB()));
/*
	linearStretchingRAct = new QAction(tr("&Linear R histogram stretching"), this);
	connect(linearStretchingRAct, SIGNAL(triggered()), this, SLOT(linearStretchingR()));

	linearStretchingGAct = new QAction(tr("&Linear G histogram stretching"), this);
	connect(linearStretchingGAct, SIGNAL(triggered()), this, SLOT(linearStretchingG()));

	linearStretchingBAct = new QAction(tr("&Linear B histogram stretching"), this);
	connect(linearStretchingBAct, SIGNAL(triggered()), this, SLOT(linearStretchingB()));
*/
	linearStretchingChannelsAct = new QAction(tr("&Linear RGB (in turn) histogram stretching"), this);
	connect(linearStretchingChannelsAct, SIGNAL(triggered()), this, SLOT(linearStretchingChannels()));

	grayWorldAct = new QAction(tr("Grayworld"), this);
	connect(grayWorldAct, SIGNAL(triggered()), this, SLOT(grayWorld()));

	filterSmoothingSimpleAct = new QAction(tr("Smoothing simple filter"), this);
	connect(filterSmoothingSimpleAct, SIGNAL(triggered()), this, SLOT(filterSmoothingSimple()));

	filterSmoothingAct = new QAction(tr("Smoothing filter..."), this);
	filterSmoothingAct->setShortcut((tr("Ctrl+6")));
	connect(filterSmoothingAct, SIGNAL(triggered()), this, SLOT(filterSmoothing()));

	filterSharpnessAct = new QAction(tr("Sharpness filter"), this);
	filterSharpnessAct->setShortcut((tr("Ctrl+7")));
	connect(filterSharpnessAct, SIGNAL(triggered()), this, SLOT(filterSharpness()));

	filterMedianAct = new QAction(tr("Median filter"), this);
	connect(filterMedianAct, SIGNAL(triggered()), this, SLOT(filterMedian()));

	filterOptionalAct = new QAction(tr("Optional filter"), this);
	connect(filterOptionalAct, SIGNAL(triggered()), this, SLOT(filterOptional()));

	wavesShortEffectAct = new QAction(tr("Waves (short)"), this);
	connect(wavesShortEffectAct, SIGNAL(triggered()), this, SLOT(wavesShortEffect()));

	wavesLongEffectAct = new QAction(tr("Waves (long)"), this);
	connect(wavesLongEffectAct, SIGNAL(triggered()), this, SLOT(wavesLongEffect()));

	glassEffectAct = new QAction(tr("&Glass"), this);
	connect(glassEffectAct, SIGNAL(triggered()), this, SLOT(glassEffect()));

	rotateAct = new QAction(tr("&Rotate..."), this);
	connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));

	scaleAct = new QAction(tr("&Scale..."), this);
	connect(scaleAct, SIGNAL(triggered()), this, SLOT(scale()));

	selectByMouseAct = new QAction(tr("Select area by mouse"), this);
	selectByMouseAct->setCheckable(true);
	connect(selectByMouseAct, SIGNAL(triggered()), this, SLOT(selectByMouse()));

	exitAct = new QAction(tr("&Exit"), this);
	exitAct->setShortcut(tr("Ctrl+E"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void ImageProcessing::createMenus()
{
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	filterMenu = new QMenu(tr("Fi&lters"), this);
	filterMenu->setDisabled(true);
	filterMenu->addAction(linearStretchingRGBAct);
/*
	filterMenu->addAction(linearStretchingRAct);
	filterMenu->addAction(linearStretchingGAct);
	filterMenu->addAction(linearStretchingBAct);
*/
	filterMenu->addAction(linearStretchingChannelsAct);
	filterMenu->addSeparator();
	filterMenu->addAction(grayWorldAct);
	filterMenu->addSeparator();
	filterMenu->addAction(filterSmoothingSimpleAct);
	filterMenu->addAction(filterSmoothingAct);
	filterMenu->addAction(filterSharpnessAct);
	filterMenu->addAction(filterMedianAct);
	filterMenu->addAction(filterOptionalAct);

	effectsMenu = new QMenu(tr("&Effects"), this);
	effectsMenu->setDisabled(true);
	effectsMenu->addAction(wavesShortEffectAct);
	effectsMenu->addAction(wavesLongEffectAct);
	effectsMenu->addAction(glassEffectAct);

	transformationMenu = new QMenu(tr("&Transorm"), this);
	transformationMenu->setDisabled(true);
	transformationMenu->addAction(rotateAct);
	transformationMenu->addAction(scaleAct);

	optionsMenu = new QMenu(tr("&Options"), this);
	optionsMenu->setDisabled(true);
	optionsMenu->addAction(selectByMouseAct);

	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(filterMenu);
	menuBar()->addMenu(effectsMenu);
	menuBar()->addMenu(transformationMenu);
	menuBar()->addMenu(optionsMenu);
}

/* Open file. */
void ImageProcessing::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
				    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
	image->load(fileName, "BMP");
	if (image->isNull()) {
	    QMessageBox::information(this, tr("Image Viewer"),
				     tr("Cannot load %1.").arg(fileName));
	    return;
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
	imageLabel->adjustSize();

	saveAct->setEnabled(true);
	filterMenu->setEnabled(true);
	effectsMenu->setEnabled(true);
	transformationMenu->setEnabled(true);
	optionsMenu->setEnabled(true);
	imageLabel->rubberBand->hide();
	imageLabel->firstCorner = QPoint (0,0);
	imageLabel->secondCorner = QPoint (0,0);
    }
}


/* Save to path. Dialog in function. */
void ImageProcessing::save()
{
	QString fileName = QFileDialog::getSaveFileName(this,
							tr("Save File"),
							QDir::currentPath());

	if (fileName.isEmpty()) {
		return;
	}

	if (!fileName.endsWith(".bmp")) {
		fileName.append (".bmp");
	}

	image->save(fileName, "BMP");
}


/* Linear strething RGB. Used extremumsIntensity() */
void ImageProcessing::linearStretchingRGB()
{
	ImgInfo img(coeffRed, coeffGreen, coeffBlue);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
                for (int x = pos.x0; x < pos.x1; x++) {
                        double redValue = qRound(255.0 * (qRed(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
                        double greenValue = qRound(255.0 * (qGreen(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
                        double blueValue = qRound(255.0 * (qBlue(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			image->setPixel(x, y, qRgb(normalizeColorValue(redValue), normalizeColorValue(greenValue), normalizeColorValue(blueValue)));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* Linear strething channels in queue. */
void ImageProcessing::linearStretchingChannels()
{
	linearStretchingR();
	linearStretchingG();
	linearStretchingB();
}


/* Linear strething R. Used extremumsIntensity() */
void ImageProcessing::linearStretchingR()
{
	ImgInfo img(1, 0 , 0);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
                        double redValue = qRound(255 * (qRed(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			double greenValue = qGreen(image->pixel(x,y));
			double blueValue = qBlue(image->pixel(x,y));
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* Linear strething G. Used extremumsIntensity() */
void ImageProcessing::linearStretchingG()
{
	ImgInfo img(0, 1 , 0);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int redValue = qRed(image->pixel(x,y));
                        double greenValue = qRound(255 * (qGreen(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			int blueValue = qBlue(image->pixel(x,y));
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* Linear strething B. Used extremumsIntensity() */
void ImageProcessing::linearStretchingB()
{
	ImgInfo img(0, 0 , 1);
	extremumsIntensity (&img);

	if (!(img.intensityMax - img.intensityMin)) {
		return;
	}

	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int redValue = qRed(image->pixel(x,y));
			int greenValue = qGreen(image->pixel(x,y));
                        double blueValue = qRound(255 * (qBlue(image->pixel(x,y)) - img.intensityMin) / (img.intensityMax - img.intensityMin));
			image->setPixel(x, y, qRgb(redValue, greenValue, blueValue));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* Grayworld. For regions also. Allright, now only practice tests. */
void ImageProcessing::grayWorld()
{
	Position pos = imageLabel->points(image->width(), image->height());

	double coeffPixels = 1.0 / ((pos.x1 - pos.x0) * (pos.y1 - pos.y0));
	double Rsum = 0;
	double Gsum = 0;
	double Bsum = 0;

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			Rsum += qRed(image->pixel(x, y));
			Gsum += qGreen(image->pixel(x, y));
			Bsum += qBlue(image->pixel(x, y));
		}
	}

	Rsum *= coeffPixels;
	Gsum *= coeffPixels;
	Bsum *= coeffPixels;

	double avg = (Rsum + Gsum + Bsum)/ 3.0;
	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			double R = avg * qRed(image->pixel(x, y)) / Rsum;
			double G = avg * qGreen(image->pixel(x, y)) / Gsum;
			double B = avg * qBlue(image->pixel(x, y)) / Bsum;
			image->setPixel(x, y, qRgb(normalizeColorValue(R),
						normalizeColorValue(G),
						normalizeColorValue(B)));
		}
	}

	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* if value of color < 0 or >255 write allowed nearest value */
int ImageProcessing::normalizeColorValue (double color)
{
	if (color > 255) {
		return 255;
	} else if (color < 0 ) {
		return 0;
	} else return color;
}


/* Count intensity value with reading coefficients in ImgInfo::coeffR(G, B) */
void ImageProcessing::extremumsIntensity(ImgInfo * img)
{
	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			double intensity = img->coeffR * qRed(image->pixel(x,y)) +
					img->coeffG * qGreen(image->pixel(x,y)) +
					img->coeffB * qBlue(image->pixel(x,y));
			if (intensity > img->intensityMax) {
				img->intensityMax = intensity;
			}
			if (intensity < img->intensityMin) {
				img->intensityMin = intensity;
			}
		}
	}
}


double **ImageProcessing::kernelGauss2D(double sigma, double coeffSharp)
{
	double s2 = 2 * sigma * sigma;
        int partLine = qRound(3 * sigma + 0.5);
	int size = 2 * partLine + 1;

	fprintf(stderr, "size %d \n", size);

	double **kernel = new double* [size];
	for(int i = 0; i < size; i++) {
		kernel[i] = new double [size];
	}

	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
                        kernel[x][y] = coeffSharp * exp (-(qPow(partLine - x, 2)+ qPow(partLine-y, 2))/ s2);
		}
	}
	return kernel;
}


/* Simple realization of Gblur with 2d kernel. */
void ImageProcessing::gaussBlurSimple(int partLine, double **kernel)
{
	int size = 2 * partLine + 1;

	Position pos = imageLabel->points(image->width(), image->height());
	QImage *imageBuf = new QImage (pos.x1 - pos.x0, pos.y1 - pos.y0, QImage::Format_RGB888);

	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			PixelRGBSpecific pix;
			for (int m = 0; m < size; m++) {
				int j = y + m - partLine;
				for (int n = 0; n < size; n++) {
					int i = x + n - partLine;
					QRgb neibPix = realPixel(i, j);
					pix.add(qRed(neibPix) * kernel[n][m],
						qGreen(neibPix) * kernel[n][m],
						qBlue(neibPix) * kernel[n][m]);
				}
			}
			imageBuf->setPixel(x - pos.x0, y - pos.y0, qRgb(normalizeColorValue(pix.red),
									normalizeColorValue(pix.green),
									normalizeColorValue(pix.blue)));
		}
	}
	copyImageBuf(imageBuf);
	delete(imageBuf);
}


void ImageProcessing::normalizeKernel2D(double **kernel, int size)
{
	double sum = 0;
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			sum += kernel[x][y];
		}
	}
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			kernel[x][y] /= sum;
		}
	}
}


/* Need to know wheere is sigma allowed. */
void ImageProcessing::filterSmoothingSimple()
{
	bool statusOk;
	double sigma = QInputDialog::getDouble(this, tr("Sigma"),
				     tr("Input sigma[0.1; 5.0]:"), 2.0, 0.1, 5.0, 1, &statusOk);
	if (statusOk) {
                int partLine = qRound(3 * sigma + 0.5);
		int size = 2 * partLine + 1;
		double **kernel= kernelGauss2D(sigma, 1);
		normalizeKernel2D(kernel, size);
		gaussBlurSimple(partLine, kernel);
		deleteKernel2D(kernel, size);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}

void ImageProcessing::deleteKernel2D(double **kernel, int size)
{
	/* Free */
	for (int i = 0; i < size; i++) {
		delete [] kernel[i];
	}
	delete [] kernel;
}



/* Work, but need to know right sigma diaposon */
double *ImageProcessing::kernelGauss1D (double sigma)
{
	double s2 = 2 * sigma * sigma;
        int partLine = qRound(3 * sigma + 0.5);
	int size = 2 * partLine + 1;

	double *result = new double [size];

	double sum = 0;
	for(int i = 0; i < size; i++) {
                result[i] = exp(- qPow(partLine - i, 2)/s2);
		sum += result[i];
	}

	for(int i = 0; i < size; i++) {
		result[i] /= sum;
	}

	return result;
}


/* work, but kernel1D maybe not creating rightly. */
void ImageProcessing::gaussBlur(double sigma)
{
        int partLine = qRound(3*sigma + 0.5);
	int size = 2 * partLine + 1;
	double *normalLine = kernelGauss1D(sigma);

	Position pos = imageLabel->points(image->width(), image->height());
	fprintf(stderr, "pos.x0 = %d, pos.y0 = %d, pos.x1 = %d, pos.y1 = %d",
			pos.x0, pos.y0, pos.x1, pos.x1);

	/* 	Used effect of mirror near the border with using realPixel(x,y),
		so additional normalizing doesn't need.
		So maybe some slowly. Don't kill me :)
	 */

	// Horizontal;
	QRgb *pixHArray = new QRgb [pos.x1 - pos.x0];
	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			PixelRGBSpecific pix;
			for (int i = 0; i < size; i++) {
				int j = x + i - partLine;
				QRgb neibPix = realPixel(j, y);
				pix.add(qRed(neibPix) * normalLine[i],
					qGreen(neibPix) * normalLine[i],
					qBlue(neibPix) * normalLine[i]);
			}
			pixHArray[x - pos.x0] = qRgb(normalizeColorValue(pix.red),
						normalizeColorValue(pix.green),
						normalizeColorValue(pix.blue));
		}
		for (int x = pos.x0; x < pos.x1; x++) {
			image->setPixel(x, y, pixHArray[x - pos.x0]);
		}
	}

	// Vertical;
	QRgb *pixVArray = new QRgb [pos.y1 - pos.y0];
	for(int x = pos.x0; x < pos.x1; x++) {
		for(int y = pos.y0; y < pos.y1; y++) {
			PixelRGBSpecific pix;
			for (int i = 0; i < size; i++) {
				int j = y + i - partLine;
				QRgb neibPix = realPixel (x, j);
				pix.add(qRed(neibPix) * normalLine[i],
					qGreen(neibPix) *normalLine[i],
					qBlue(neibPix) * normalLine[i]);
			}
			pixVArray[y - pos.y0] = qRgb(normalizeColorValue(pix.red),
						normalizeColorValue(pix.green),
						normalizeColorValue(pix.blue));
		}
		for (int y = pos.y0; y < pos.y1; y++) {
			image->setPixel(x, y, pixVArray[y - pos.y0]);
		}
	}

	delete [] normalLine;
	delete [] pixHArray;
	delete [] pixVArray;
}


/* Need to know wheere is sigma allowed. */
void ImageProcessing::filterSmoothing()
{
	bool statusOk;
	double sigma = QInputDialog::getDouble(this, tr("Sigma"),
				     tr("Input sigma[0.5; 5.0]:"), 2.0, 0.1, 5.0, 1, &statusOk);
	if (statusOk) {
		gaussBlur(sigma);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}


/* Work with simple gauss. (kernel2D with coeff alpha)
	Need ask alpha and right sigma */
void ImageProcessing::filterSharpness()
{
	bool statusOk;
	double alpha = QInputDialog::getDouble(this, tr("Sigma"),
				     tr("Input alpha[0.1; 1.0]:"), 0.5, 0.01, 1.0, 2, &statusOk);
	if (statusOk) {
		double sigma = sharpSigma;	// not truethfull value. need to check.;
		int partLine =  qRound(3 * sigma + 0.5);
		int size = 2 * partLine + 1;
		double **kernel = kernelGauss2D(sigma, -alpha);
		// Emulate single filter and sum.
		kernel[partLine][partLine] += 1 + alpha;
		normalizeKernel2D(kernel, size);
		gaussBlurSimple(partLine, kernel);
		deleteKernel2D(kernel, size);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}


/* Compare two elements. Need for qsort() in median filter. */
int compare(const void *a, const void *b)
{
	return (*(char*)a -*(char*)b);
}

/* Compare two elements. Need for qsort() in median filter. */
int compare_double(const void *a, const void *b)
{
	return (*(double*)a -*(double*)b);
}

/* Realization of median filter*/
void ImageProcessing::medianProcess(int size)
{
	char *rArray = new char[size*size];
	char *gArray = new char[size*size];
	char *bArray = new char[size*size];
	int halfSize = (size%2 == 0) ? size/2 - 1 : (size - 1) / 2;

	Position pos = imageLabel->points(image->width(), image->height());
	QImage *imageBuf = new QImage (pos.x1-pos.x0, pos.y1-pos.y0, QImage::Format_RGB888);

	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			for(int m = 0; m < size; m++) {
				int j = y + m - halfSize;
				for(int n = 0; n < size; n++) {
					int i = x + n - halfSize;
					rArray[size*m + n] = qRed(realPixel(i, j));
					gArray[size*m + n] = qGreen(realPixel(i, j));
					bArray[size*m + n] = qBlue(realPixel(i, j));
				}
			}
			qsort(rArray, size*size, sizeof(char), compare);
			qsort(gArray, size*size, sizeof(char), compare);
			qsort(bArray, size*size, sizeof(char), compare);
			int median = (size%2 == 0) ? (size*size - 1)/2: ((size-1)*(size-1)-1)/2;
			int red = rArray[median];
			int green = gArray[median];
			int blue = bArray[median];
			imageBuf->setPixel(x - pos.x0, y - pos.y0, qRgb(red, green, blue));
		}
	}
	copyImageBuf(imageBuf);
	delete(imageBuf);
	delete [] rArray;
	delete [] gArray;
	delete [] bArray;
	fprintf (stderr, "x\n");
}


/* Need a size of median filter. now 2...7 */
void ImageProcessing::filterMedian()
{
	bool statusOk;
	int size = QInputDialog::getInt(this, tr("Input size of median filter"),
				     tr("Size of median filter:"), 3, 2, 7, 1, &statusOk);

	if (statusOk) {
		medianProcess(size);
		imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}


/* Need normalize, or not?! To be or not to be.. */
void ImageProcessing::filterOptional()
{
	bool ok;
	QStringList items;
	items << tr("3") << tr("5") << tr("7");
	QString item = QInputDialog::getItem(this, tr("Input size kernel"),
					     tr("Size of kernel:"), items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		int value = item.toInt();
                int partLine = qRound(value*1.0/2 - 0.5);

		double **kernel = new double* [value];
		for(int i = 0; i < value; i++) {
			kernel[i] = new double [value];
		}

		bool statusKernel;
		TableDialog dialog(value, kernel, &statusKernel);
		dialog.exec();

		if (statusKernel) {
		    gaussBlurSimple(partLine, kernel);
		}
		deleteKernel2D(kernel, value);
	}
}


/* Waves effect type 1. */
void ImageProcessing::wavesShortEffect()
{
    bool lOk =false;
    int l = QInputDialog::getInt(this, tr("Input lenght"),
				 tr("Lenght:"), 1, 1, 200, 1, &lOk);
    bool ampOk =false;
    int amp = QInputDialog::getInt(this, tr("Input amplitude"),
				 tr("Amplitude:"), 1, 1, 200, 1, &ampOk);

    if (lOk && ampOk) {

	QImage *imageBuf = new QImage(*image);
	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + l * sin(2 * pi * x / amp);
			int newY = y;
			if (newX >= pos.x0 && newX < pos.x1) {
				image->setPixel(x, y, imageBuf->pixel(newX, newY));
			}
		}
	}

	delete (imageBuf);
	imageLabel->setPixmap(QPixmap::fromImage(*image));
    }
   }


/* Waves effect type 2. */
void ImageProcessing::wavesLongEffect()
{
    bool lOk =false;
    int l = QInputDialog::getInt(this, tr("Input lenght"),
				 tr("Lenght:"), 1, 1, 200, 1, &lOk);
    bool ampOk =false;
    int amp = QInputDialog::getInt(this, tr("Input amplitude"),
				 tr("Amplitude:"), 1, 1, 200, 1, &ampOk);

    if (lOk && ampOk) {

	QImage *imageBuf = new QImage(*image);
	Position pos = imageLabel->points(image->width(), image->height());

	for (int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			int newX = x + l * sin(2 * pi * y / amp);
			int newY = y;
			if (newX >= pos.x0 && newX < pos.x1) {
				image->setPixel(x, y, imageBuf->pixel(newX, newY));
			}
		}
	}
	delete (imageBuf);
	imageLabel->setPixmap(QPixmap::fromImage(*image));
    }
}


/* Glass effect */
void ImageProcessing::glassEffect()
{
	bool radiusOk =false;
	int size = QInputDialog::getInt(this, tr("Input radius"),
				     tr("Size of radius:"), 1, 1, 10, 1, &radiusOk);
	size++;

	if (radiusOk) {
	    QImage *imageBuf = new QImage(*image);
	    Position pos = imageLabel->points(image->width(), image->height());
	    srand((unsigned)time(0));

	    for (int y = pos.y0; y < pos.y1; y++) {
		    for (int x = pos.x0; x < pos.x1; x++) {
			    int newX = x + (rand()%size - 0.5) * 10;
			    int newY = y + (rand()%size - 0.5) * 10;
			    if (newX >= 0 && newX < image->width() && newY >= 0 && newY < image->height()) {
				image->setPixel(x, y, imageBuf->pixel(newX, newY));
			    }
		    }
	    }
		delete(imageBuf);
	    imageLabel->setPixmap(QPixmap::fromImage(*image));
	}
}


/* Bilinear interpolation. Round x, y up and down. If one of point not in region return blackpoint */
QRgb ImageProcessing:: bilinearInterpolation(double x, double y, QImage *img)
{
	int leftX = x;
	int rightX = leftX + 1;
	double deltaX = x - leftX;

	int upY  = y;
	int downY = upY + 1;
	double deltaY = y - upY;

	int red = (1 - deltaX) * (1 - deltaY) * qRed(img->pixel(leftX, upY)) +
		deltaX * (1 - deltaY) * qRed(img->pixel(rightX, upY)) +
		(1 - deltaX) * deltaY * qRed(img->pixel(leftX, downY)) +
		deltaX * deltaY * qRed(img->pixel(rightX, downY));
	int green = (1 - deltaX) * (1 - deltaY) * qGreen(img->pixel(leftX, upY)) +
		deltaX * (1 - deltaY) * qGreen(img->pixel(rightX, upY)) +
		(1 - deltaX) * (deltaY) * qGreen(img->pixel(leftX, downY)) +
		deltaX * deltaY * qGreen(img->pixel(rightX, downY));
	int blue = (1 - deltaX) * (1 - deltaY) * qBlue(img->pixel(leftX, upY)) +
		deltaX * (1 - deltaY) * qBlue(img->pixel(rightX, upY)) +
		(1 - deltaX) * deltaY * qBlue(img->pixel(leftX, downY)) +
		deltaX * deltaY * qBlue(img->pixel(rightX, downY));
	return qRgb(normalizeColorValue(red), normalizeColorValue(green), normalizeColorValue(blue));
}


/* Create big region points */
QRect ImageProcessing::setBigRegion(int xCenter, int yCenter, double r)
{
	Position pos = imageLabel->points(image->width(), image->height());
	double xArray[4];
	double yArray[4];

	xArray[0] = ((pos.x0 - xCenter) * cos(r) - (pos.y0 - yCenter) * sin(r)) + xCenter;
	yArray[0] = ((pos.x0 - xCenter) * sin(r) + (pos.y0 - yCenter) * cos(r)) + yCenter;
	xArray[1] = ((pos.x0 - xCenter) * cos(r) - (pos.y1 - yCenter) * sin(r)) + xCenter;
	yArray[1] = ((pos.x0 - xCenter) * sin(r) + (pos.y1 - yCenter) * cos(r)) + yCenter;
	xArray[2] = ((pos.x1 - xCenter) * cos(r) - (pos.y0 - yCenter) * sin(r)) + xCenter;
	yArray[2] = ((pos.x1 - xCenter) * sin(r) + (pos.y0 - yCenter) * cos(r)) + yCenter;
	xArray[3] = ((pos.x1 - xCenter) * cos(r) - (pos.y1 - yCenter) * sin(r)) + xCenter;
	yArray[3] = ((pos.x1 - xCenter) * sin(r) + (pos.y1 - yCenter) * cos(r)) + yCenter;

	qsort(xArray, 4, sizeof(double), compare_double);
	qsort(yArray, 4, sizeof(double), compare_double);

	xArray[0] = (xArray[0] < 0) ? 0 : xArray[0];
	yArray[0] = (yArray[0] < 0) ? 0 : yArray[0];
	xArray[3] = (xArray[3] >= image->width()) ? image->width() : xArray[3];
	yArray[3] = (yArray[3] >= image->height()) ? image->height() : yArray[3];

	return QRect(QPoint(xArray[0], yArray[0]), QPoint(xArray[3], yArray[3]));
}


/* Rotate -180.. 180 (also for region */
void ImageProcessing::rotate()
{
	bool statusOk;
	int value = QInputDialog::getInt(this, tr("Transform angel"),
				     tr("Input angel:"), 90, -180, 180, 1, &statusOk);
	if (!statusOk) {
		return;
	}

	Position pos = imageLabel->points(image->width(), image->height());
	QImage *imageBuf = new QImage(pos.x1 - pos.x0, pos.y1 - pos.y0, QImage::Format_RGB888);
	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			imageBuf->setPixel(x - pos.x0,y - pos.y0, image->pixel(x,y));
			image->setPixel(x, y, qRgb(0, 0, 0));
		}
	}

	int xCenter = pos.x0 + (pos.x1 - pos.x0) / 2;
	int yCenter = pos.y0 + (pos.y1 - pos.y0) / 2;
	double r = -value * pi / 180;

	QRect region = setBigRegion(xCenter, yCenter, r);

	fprintf(stderr, "top %d; bottom %d; left %d; right %d; x0 %d; y0%d;.\n", region.top(), region.bottom(), region.left(), region.right(), pos.x0, pos.y0);

	for (int y = region.top(); y < region.bottom(); y++) {
		for (int x = region.left(); x < region.right(); x++) {
			double xOld= (x - xCenter) * cos(r) - (y - yCenter) * sin(r) + xCenter - pos.x0;
			double yOld= (x - xCenter) * sin(r) + (y - yCenter) * cos(r) + yCenter - pos.y0;
			if (xOld >= 0  && xOld < imageBuf->width() - 1 && yOld >= 0 && yOld < imageBuf->height() - 1) {
				QRgb pix = bilinearInterpolation(xOld, yOld, imageBuf);
				image->setPixel(x, y, pix);
			}
		}
	}

	delete(imageBuf);
	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* Scale. Zoom in and zoom out. Handle paint dark rectangle if scale < 1.0.*/
void ImageProcessing::scale()
{
	bool statusOk;
	double value = QInputDialog::getDouble(this, tr("Scale factor"),
				     tr("Input scale factor:"), 1.0, 0.3, 3.0, 1, &statusOk);
	if (!statusOk) {
		return;
	}

	if (value < 1) {
		gaussBlur(sigmaScale);
	}

	Position pos = imageLabel->points(image->width(), image->height());
	QImage *imageBuf = new QImage(pos.x1 - pos.x0, pos.y1 - pos.y0, QImage::Format_RGB888);
	for(int y = pos.y0; y < pos.y1; y++) {
		for(int x = pos.x0; x < pos.x1; x++) {
			imageBuf->setPixel(x - pos.x0,y - pos.y0, image->pixel(x,y));
			if (value < 1) {
				image->setPixel(x, y, qRgb(0, 0, 0));
			}
		}
	}

	int xCenter = pos.x0 + (pos.x1 - pos.x0) / 2;
	int yCenter = pos.y0 + (pos.y1 - pos.y0) / 2;
	int newX0 = xCenter - (xCenter - pos.x0) * value;
	int newX1 = xCenter + (xCenter - pos.x0) * value;
	int newY0 = yCenter - (yCenter - pos.y0) * value;
	int newY1 = yCenter + (yCenter - pos.y0) * value;
	newX0 = (newX0 < 0) ? 0 : (newX0 > image->width()) ? image->width() : newX0;
	newX1 = (newX1 < 0) ? 0 : (newX1 > image->width()) ? image->width() : newX1;
	newY0 = (newY0 < 0) ? 0 : (newY0 > image->height()) ? image->height() : newY0;
	newY1 = (newY1 < 0) ? 0 : (newY1 > image->height()) ? image->height() : newY1;

	for (int y = newY0; y < newY1; y++) {
		for (int x = newX0; x < newX1; x++) {
			double xOld = (x - xCenter) / value + xCenter - pos.x0;
			double yOld = (y - yCenter) / value + yCenter - pos.y0;
			if (xOld >= 0  && xOld < imageBuf->width() - 1 && yOld >= 0 && yOld < imageBuf->height() - 1) {
				QRgb pix = bilinearInterpolation(xOld, yOld, imageBuf);
				image->setPixel(x, y, pix);
			}
		}
	}
	delete(imageBuf);
	imageLabel->setPixmap(QPixmap::fromImage(*image));
}


/* Action in menu for select by mouse. Work with class AreaImage *imageLabel. Set value also there. */
void ImageProcessing::selectByMouse()
{
	bool selectByMouse = selectByMouseAct->isChecked();

	if (!selectByMouse) {
		selectByMouseAct->setChecked(false);
		imageLabel->rubberBand->hide();
		imageLabel->setSelectedByMouse(false);
	} else {
		selectByMouseAct->setChecked(true);
		imageLabel->setSelectedByMouse(true);
	}
}


/* Now in used by gauss, median etc. */
void ImageProcessing::copyImageBuf(QImage *imgBuf)
{
	Position pos = imageLabel->points(image->width(), image->height());
	for(int y = pos.y0; y < pos.y1; y++) {
		for (int x = pos.x0; x < pos.x1; x++) {
			image->setPixel(x, y, imgBuf->pixel(x - pos.x0, y - pos.y0));
		}
	}
}


/* Need for creating effect of mirror image near the border */
QRgb ImageProcessing::realPixel(int x, int y)
{
	return QRgb(image->pixel(realX(x), realY(y)));
}


/* for realPixel() */
int ImageProcessing::realX(int x)
{
	if (x < 0) {
		return realX(-x - 1);
	} else if (x >= image->width()){
		return realX(2 * image->width() - 1 - x);
	} else {
		return x;
	}
}


/* for realPixel() */
int ImageProcessing::realY(int y)
{
	if  (y < 0) {
		return realY(-y - 1);
	} else if (y >= image->height()) {
		return realY(2 * image->height() - 1 - y);
	} else {
		return y;
	}
}
