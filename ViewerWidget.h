#pragma once
#include <QtWidgets>
#include "Objekt.h"


struct zBuff {
	int z;
	QColor farbaPixelu;
};

class ViewerWidget :public QWidget {
	Q_OBJECT
private:
	QString name = "";
	QSize areaSize = QSize(0, 0);
	QImage* img = nullptr;
	QRgb* data = nullptr;
	QPainter* painter = nullptr;
	bool trebakreslit;

public:
	ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent = Q_NULLPTR);
	~ViewerWidget();
	void resizeWidget(QSize size);

	//Image functions
	bool setImage(const QImage& inputImg);
	QImage* getImage() { return img; };
	bool isEmpty();
	void kresliObjekty(QVector<objekt*> vsetko, QColor poz);
	void kresliObjekt(objekt* Objekt);
	void usecka_DDA(QPointF A, QPointF B, QColor color, int z);
	void kresliPriamku(QVector<QPointF> body, QColor color, int z);
	void kresliKruznicu(QVector<QPointF> body, QColor color, int z);
	void kresliKruznicuF(QVector<QPointF> body, QColor color, QColor vypln, bool vyplnaj, int z);
	void kresliPolygon(QVector<QPointF> body, QColor color, QColor vypln, bool vyplnaj, int z);
	void kresliKrivku(QVector <QPointF> body, QColor color, int z);
	void scanLine(QVector <QPointF> body, QColor color, int z); 
	void scanLineTri(QVector <QPointF> body, QColor farba, int z);
	QVector<QVector<zBuff>> buffer;
	void resetZbuff(QColor pozadie);
	void setZbuff(int x, int y, QColor farba, double z);
	void kresliZbuff();

	//Data functions
	QRgb* getData() { return data; }
	void setPixel(int x, int y, const QColor& color);
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	bool isInside(int x, int y) { return (x >= 0 && y >= 0 && x < img->width() && y < img->height()) ? true : false; }

	//Get/Set functions
	QString getName() { return name; }
	void setName(QString newName) { name = newName; }

	void setPainter() { painter = new QPainter(img); }
	void setDataPtr() { data = reinterpret_cast<QRgb*>(img->bits()); }

	int getImgWidth() { return img->width(); };
	int getImgHeight() { return img->height(); };

	void clear(QColor color = Qt::white);

public slots:
	void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
};