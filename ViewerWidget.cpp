#include   "ViewerWidget.h"

ViewerWidget::ViewerWidget(QString viewerName, QSize imgSize, QWidget* parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setMouseTracking(true);
	name = viewerName;
	if (imgSize != QSize(0, 0)) {
		img = new QImage(imgSize, QImage::Format_ARGB32);
		img->fill(Qt::white);
		resizeWidget(img->size());
		setPainter();
		setDataPtr();
	}
}
ViewerWidget::~ViewerWidget()
{
	delete painter;
	delete img;
}
void ViewerWidget::resizeWidget(QSize size)
{
	this->resize(size);
	this->setMinimumSize(size);
	this->setMaximumSize(size);
}

//Image functions
bool ViewerWidget::setImage(const QImage& inputImg)
{
	if (img != nullptr) {
		delete img;
	}
	img = new QImage(inputImg);
	if (!img) {
		return false;
	}
	resizeWidget(img->size());
	setPainter();
	update();

	return true;
}
bool ViewerWidget::isEmpty()
{
	if (img->size() == QSize(0, 0)) {
		return true;
	}
	return false;
}

//Data function
void ViewerWidget::setPixel(int x, int y, const QColor& color)
{
	if (isInside(x, y)) 
		data[x + y * img->width()] = color.rgb();
	else
		qDebug() << "nechce vykreslit bod" << x << "," << y << "\n";
}
void ViewerWidget::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if (isInside(x, y)) {
		QColor color(r, g, b);
		setPixel(x, y, color);
	}
}

void ViewerWidget::clear(QColor color)
{
	for (size_t x = 0; x < img->width(); x++)
	{
		for (size_t y = 0; y < img->height(); y++)
		{
			setPixel(x, y, color);
		}
	}
	update();
}

//Slots
void ViewerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QRect area = event->rect();
	painter.drawImage(area, *img, area);
}

//kresliace a vyfarbovacie funkcie

void ViewerWidget::kresliObjekty(QVector<objekt*> vsetko) {
	int i;
	resetZbuff();
	for (i = 0; i < vsetko.size(); i++) {
		kresliObjekt(vsetko[i]);
	}
	kresliZbuff();
}

void ViewerWidget::kresliObjekt(objekt* Objekt) {
	int typ = Objekt->getTyp(); QVector<QPointF> body = Objekt->getBody(); QColor fill = Objekt->getFill(); QColor color = Objekt->getFarba(); bool vyplnaj = Objekt->getVyplnaj(); int z = Objekt->getZ();
	if (typ == 0 && body.size()==2) {						//priamka
		kresliPriamku(body, color, z);
	}
	else if (typ==1 || typ == 2 && body.size()>2) {			//n-uholnik
		kresliPolygon(body, color, fill, vyplnaj, z);
	}
	else if (typ == 3 && body.size()==2) {					//kruznica
		kresliKruznicu(body, color, z);
	}
	else if (typ == 4 && body.size()>=2) {					//bezier
		kresliKrivku(body, color, z);
	}
	else {//message o nespravnom pocte
		QMessageBox msgBox;
		msgBox.setText(u8"Nesprávny poèet bodov pre zvolený algoritmus.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}

void ViewerWidget::usecka_DDA(QPointF A, QPointF B, QColor color, int z) {
	int ax = (int)A.x(), ay = (int)A.y(), bx = (int)B.x(), by = (int)B.y();
	A.setX(ax); A.setY(ay); B.setX(bx); B.setY(by);
	int x, y, x2, y2 ;
	double chyba = 0;
	if (B.y() == A.y()) {
		int delta = abs(A.x() - B.x()), x1;
		if (A.x() < B.x())
			x1 = A.x();
		else
			x1 = B.x();
		for (x = 0; x < delta; x++)
			setZbuff(x+x1, A.y(), color, z);
	}
	else {
		double smernica = (B.y() - A.y()) / (double)(B.x() - A.x());
		double posun=0;
		if (abs(smernica) < 1) {											//riadiaca os x
			if (A.x() < B.x()) {
				x = A.x(); y = A.y(); x2 = B.x();
			}
			else if (A.x() > B.x()) {
				x = B.x(); y = B.y(); x2 = A.x();
			}
			else if (A.x() == B.x()) {
				int delta = abs(A.y() - B.y()), y1;
				if (A.y() < B.y())
					y1 = A.y();
				else
					y1 = B.y();
				for (y = 0; y < delta; y++)
					setZbuff(y + y1, A.x(), color, z);
			}
				
			for (x; x < x2; x++) {
				setZbuff(x, (int)y, color, z);
				posun += smernica;
				if (posun > 0.5) {
					y += (int)smernica;
					if ((int)smernica == 0)
						y++;
					posun += -1;
				}
				if (posun < -0.5) {
					y+=-(int) smernica;
					if ((int)smernica == 0)
						y--;
					posun += 1;
				}
			}
		}
		else {																//riadiaca os y
			if (A.y() > B.y())
				 y = A.y(), x = A.x(), y2 = B.y();
			else if (A.y() < B.y())
				 y = B.y(), x = B.x(), y2 = A.y();
			for (y; y > y2; y--) {
				setZbuff(x, (int)y, color, z);
				posun += 1 / smernica;
				if (posun > 0.5) {
					x--;
					posun += -1;
				}
				if (posun < -0.5) {
					x++;
					posun += 1;
				}
			}
		}
	}
	update();
}

void ViewerWidget::kresliPriamku(QVector<QPointF> body, QColor color, int z) {
	QVector<QPointF> A;
	//najdeme priesecniky s okrajmi platna
	QVector<QPointF> rohy;
	QPointF d, E;
	int i = 0;
	E.setX(0);
	E.setY(0);
	rohy.push_back(E);
	E.setX(img->width() - 1);
	E.setY(0);
	rohy.push_back(E);
	E.setX(img->width() - 1);
	E.setY(img->height() - 1);
	rohy.push_back(E);
	E.setX(0);
	E.setY(img->height() - 1);
	rohy.push_back(E);
	d.setX(body[1].x() - body[0].x());
	d.setY(body[1].y() - body[0].y());

	QPoint priesecnik;
	int x, y;
	int x1 = body[0].x(), y1 = body[0].y(), x2 = body[1].x(), y2 = body[1].y(), x3, x4, y3, y4;
	for (i = 0; i < rohy.size(); i++) {
		x3 = rohy[i].x();
		y3 = rohy[i].y();
		x4 = rohy[(i + 1) % 4].x();
		y4 = rohy[(i + 1) % 4].y();
		float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
		float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
		float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;
		priesecnik.setX(x);			//priesecnik priamok
		priesecnik.setY(y);
		if (priesecnik.x() >= 0 && priesecnik.x() < img->width() && priesecnik.y() >= 0 && priesecnik.y() < img->height()) {
			A.append(priesecnik);
		}
	}
	if (A.size() == 2) {
		usecka_DDA(A[0], A[1], color, z);
		A.clear();	A.squeeze();
	}
	else {
		QMessageBox msgBox;
		msgBox.setText(u8"Nepodarilo sa najst 2 priesecniky.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}

void ViewerWidget::kresliKruznicu(QVector<QPointF> body, QColor color, int z) {
	QPointF A(body[0]), B(body[1]);
	int ax = (int)A.x(), ay = (int)A.y(), bx = (int)B.x(), by = (int)B.y();
	A.setX(ax); A.setY(ay); B.setX(bx); B.setY(by);
	int a = abs(A.x() - B.x()), b = abs(A.y() - B.y()), r = sqrt((a * a) + (b * b)),
		p = 1 - r, x = 0, y = r, dvaX = 3, dvaY = 2 * r + 2;
	do {
		setZbuff(x + A.x(), y + A.y(), color, z);
		setZbuff(-x + A.x(), y + A.y(), color, z);
		setZbuff(-x + A.x(), -y + A.y(), color, z);
		setZbuff(x + A.x(), -y + A.y(), color, z);
		setZbuff(y + A.x(), x + A.y(), color, z);
		setZbuff(-y + A.x(), x + A.y(), color, z);
		setZbuff(-y + A.x(), -x + A.y(), color, z);
		setZbuff(y + A.x(), -x + A.y(), color,z);
		/*
		setPixel(x+A.x(), y+A.y(), color);
		setPixel(-x + A.x(), y + A.y(), color);
		setPixel(-x + A.x(), -y + A.y(), color);
		setPixel(x + A.x(), -y + A.y(), color);
		setPixel(y + A.x(), x + A.y(), color);
		setPixel(-y + A.x(), x + A.y(), color);
		setPixel(-y + A.x(), -x + A.y(), color);
		setPixel(y + A.x(), -x + A.y(), color);*/
		if (p > 0) {
			p += -dvaY;
			y--;
			dvaY += -2;
		}
		p += dvaX;
		dvaX += 2;
		x++;
	} while (x < y);
	update();
}

void ViewerWidget::kresliPolygon(QVector<QPointF> body, QColor color, QColor vypln, bool vyplnaj, int z) {
	//polygon s orezanim
	QVector<QPointF> W, V;
	V = body;
		if (V.size() > 2) {
			QPointF S, P, A, B;
			int Xmin[4] = { 0, 0, -img->width()+1, -img->height()+1 };
			int xmin = 0, j = 0, i=0;
			for (j = 0; j < 4; j++) {
				xmin = Xmin[j];
				if (V.size()>0)
					S = V[V.size() - 1];
				for (i = 0; i < V.size(); i++) {
					if (V[i].x() >= xmin) {
						if (S.x() >= xmin) {
							W.push_back(V[i]);
						}
						else {
							P.setX(xmin);
							P.setY(S.y() + (xmin - S.x()) * (V[i].y() - S.y()) / (V[i].x() - S.x()));
							W.push_back(P);
							W.push_back(V[i]);
						}
					}
					else {
						if (S.x() >= xmin) {
							P.setX(xmin);
							P.setY(S.y() + (xmin - S.x()) * (V[i].y() - S.y()) / (V[i].x() - S.x()));
							W.push_back(P);
						}
					}
					S = V[i];
				}
				V.clear();	V.squeeze();
				for (i = 0; i < W.size(); i++) {
					P.setX(W[i].y());
					P.setY(-W[i].x());
					V.push_back(P);
				}
				W.clear();	W.squeeze();
			}
			for (i = 0; i < V.size(); i++) {
				A.setX(V[i].x());
				A.setY(V[i].y());
				B.setX(V[(i + 1) % (V.size())].x());
				B.setY(V[(i + 1) % (V.size())].y());

				usecka_DDA(A, B, color, z);
			}
			if (vyplnaj && V.size()>3)
				scanLine(V, vypln, z);
			else if (vyplnaj && V.size() == 3) {
				scanLineTri(V, vypln, z);
			}

		}
	update();
}

void ViewerWidget::scanLineTri(QVector <QPointF> body, QColor farba, double z) {
	//QVector<edge> hrany (3);
																//usporiadanie pola bodov
	int i, j, k, y = 0, Ymax = 0;
	float Xmin = 0, Xmax = 0, m;
	//qDebug() << body;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2; j++) {
			if (body[j].y() > body[j + 1].y()) {
				QPointF temp = body[j];
				body[j] = body[j + 1];
				body[j + 1] = temp;
			}
			else if (body[j].y() == body[j + 1].y()) {
				if (body[j].x() > body[j + 1].x()) {
					QPointF temp = body[j];
					body[j] = body[j + 1];
					body[j + 1] = temp;
				}
			}
		}
	}
	//qDebug() << body;
	QVector<QPointF> e1(2), e2(2);								//vytvorenie "podtrojuholnikov"
	float w1, w2;
	bool ibajeden = false;
	if (body[0].y() == body[1].y()) {				//iba spodny
		ibajeden = true;
		e1[0] = body[0];
		e1[1] = body[2];

		e2[0] = body[1];
		e2[1] = body[2];

		Xmin = e1[0].x();
		Xmax = e2[0].x();

		y = e1[0].y();
		Ymax = e1[1].y();
	}
	else if (body[1].y() == body[2].y()) {			//iba vrchny
		ibajeden = true;
		e1[0] = body[0];
		e1[1] = body[1];

		e2[0] = body[0];
		e2[1] = body[2];

		Xmin = e1[0].x();
		Xmax = e2[0].x();

		y = e1[0].y();
		Ymax = e1[1].y();
	}
	else {
		float m = (body[2].y() - body[0].y()) / (float)(body[2].x() - body[0].x());
		int px = ((body[1].y() - body[0].y()) / m) + body[0].x();
		QPointF P;
		P.setX((int)px);
		P.setY(body[1].y());

		if (body[1].x() < P.x()) {
			QVector<QPointF> e3(2), e4(2);
			float w3, w4;
			e1[0] = body[0];														//nehorizontalne hrany 
			e1[1] = body[1];

			e2[0] = body[0];
			e2[1] = P;

			e3[0] = body[1];
			e3[1] = body[2];

			e4[0] = P;
			e4[1] = body[2];

			m = (e3[1].y() - e3[0].y()) / (double)(e3[1].x() - e3[0].x());			//nastavenie premennych pre dolny 
			w3 = 1.0 / m;
			m = (e4[1].y() - e4[0].y()) / (double)(e4[1].x() - e4[0].x());
			w4 = 1.0 / m;
			Xmin = e3[0].x();
			Xmax = e4[0].x();
			y = e3[0].y();
			Ymax = e3[1].y();

			int dx = Xmax - Xmin;
			while (y < Ymax) {														//kreslenie dolneho
				for (i = 1; i <= dx; i++) {
					setZbuff((int)Xmin + i, (int)y, farba, z); 
				}
				Xmin += w3;
				Xmax += w4;
				y++;
				dx = Xmax - Xmin;
			}

			Xmin = e1[0].x();														//nastavenie premennych pre horny 
			Xmax = e2[0].x();

			y = e1[0].y();
			Ymax = e1[1].y();
			//qDebug() << "pre dolny " << Xmin << Xmax;
		}

		else {
			QVector<QPointF> e3(2), e4(2);
			float w3, w4;

			e1[0] = body[0];														//nehorizontalne hrany 
			e1[1] = P;

			e2[0] = body[0];
			e2[1] = body[1];

			e3[0] = P;
			e3[1] = body[2];

			e4[0] = body[1];
			e4[1] = body[2];


			m = (e3[1].y() - e3[0].y()) / (double)(e3[1].x() - e3[0].x());			//nastavenie premennych pre dolny 
			w3 = 1.0 / m;
			m = (e4[1].y() - e4[0].y()) / (double)(e4[1].x() - e4[0].x());
			w4 = 1.0 / m;
			Xmin = e3[0].x();
			Xmax = e4[0].x();
			y = e3[0].y();
			Ymax = e3[1].y();

			int dx = Xmax - Xmin;
			while (y < Ymax) {														//kreslenie dolneho	
				for (i = 1; i <= dx; i++) {
					setZbuff((int)Xmin + i, (int)y, farba, z);
				}
				Xmin += w3;
				Xmax += w4;
				y++;
				dx = Xmax - Xmin;
			}
			Xmin = e1[0].x();														//nastavenie premennych pre horny 
			Xmax = e2[0].x();
			y = e1[0].y();
			Ymax = e1[1].y();
		}
		m = (e1[1].y() - e1[0].y()) / (double)(e1[1].x() - e1[0].x());				//kreslenie horneho
		w1 = 1.0 / m;
		m = (e2[1].y() - e2[0].y()) / (double)(e2[1].x() - e2[0].x());
		w2 = 1.0 / m;



		int dx = Xmax - Xmin;
		while (y < Ymax) {														//kreslenie dolneho	
			for (i = 1; i <= dx; i++) {
				setZbuff((int)Xmin + i, (int)y, farba, z);
			}
			Xmin += w1;
			Xmax += w2;
			y++;
			dx = Xmax - Xmin;
		}
	}

	if (ibajeden) {
		m = (e1[1].y() - e1[0].y()) / (double)(e1[1].x() - e1[0].x());
		w1 = 1.0 / m;
		m = (e2[1].y() - e2[0].y()) / (double)(e2[1].x() - e2[0].x());
		w2 = 1.0 / m;

		int dx = Xmax - Xmin;
		while (y < Ymax) {
			for (i = 1; i <= dx; i++) {
				setZbuff((int)Xmin + i, (int)y, farba, z);
			}
			Xmin += w1;
			Xmax += w2;
			y++;
			dx = Xmax - Xmin;
		}
	}
}

void ViewerWidget::kresliKrivku(QVector <QPointF> body, QColor color, int z) {
	int i, j;
	QPointF Q0, Q1;
	float deltaT = 0.01;
	float T = deltaT;

	QVector<QVector<QPointF>> P(body.size());
	for (i = 0; i < body.size(); i++) {
		P[i].resize(body.size() - i);
		P[0][i] = body[i];
	}
	Q0.setX(body[0].x());
	Q0.setY(body[0].y());
	while (T < 1) {
		for (i = 1; i < body.size(); i++) {
			for (j = 0; j < body.size() - i; j++) {
				P[i][j].setX((1.0 - T) * P[i - 1][j].x() + T * P[i - 1][j + 1].x());
				P[i][j].setY((1.0 - T) * P[i - 1][j].y() + T * P[i - 1][j + 1].y());
			}
		}
		Q1.setX(P[body.size() - 1][0].x());
		Q1.setY(P[body.size() - 1][0].y());

		usecka_DDA(Q0, Q1, color, z);

		Q0 = Q1;
		T += deltaT;
	}
	Q1.setX(body[body.size() - 1].x());
	Q1.setY(body[body.size() - 1].y());
	usecka_DDA(Q0, Q1, color, z);
}

void ViewerWidget::scanLine(QVector<QPointF> body, QColor vypln, int z) {
	struct edge {
		float xs, xk, ys, yk, dy;
		double m, w = 1.0 / m;
		bool operator< (const edge& e) const {
			return ys < e.ys;
		}
	};
	QVector <edge> hrany(body.size());
	int i, j, k;
	for (i = 0; i < body.size(); i++) {									//vytvorim si pole hran, hrana bude zacinat v bode s mensim y
		if (body[i].y() < body[(i + 1) % body.size()].y()) {
			hrany[i].xs = body[i].x();
			hrany[i].xk = body[(i + 1) % body.size()].x();
			hrany[i].ys = body[i].y();
			hrany[i].yk = body[(i + 1) % body.size()].y();

		}
		else {		//touto podmienkou odignoruje hrany so smernicou m=0
			hrany[i].xk = body[i].x();
			hrany[i].xs = body[(i + 1) % body.size()].x();
			hrany[i].yk = body[i].y();
			hrany[i].ys = body[(i + 1) % body.size()].y();
		}
		if (hrany[i].xk != hrany[i].xs) {
			hrany[i].m = (hrany[i].yk - hrany[i].ys) / (double)(hrany[i].xk - hrany[i].xs);
			hrany[i].w = 1.0 / hrany[i].m;
		}
		else
			hrany[i].w = 0.0;
		hrany[i].yk += -1;
		hrany[i].dy = hrany[i].yk - hrany[i].ys;
	}
	for (i = 0; i < hrany.size(); i++) {							//vymazanie horizontalnych hran
		if (hrany[i].dy < 0) {
			hrany.removeAt(i);
			i--;
		}
	}

	for ( i = 0; i < hrany.size(); i++) {						//zoradi hrany podla y
		for ( j = 0; j < hrany.size() - i - 1; j++) {
			if (hrany[j + 1] < hrany[j]) {
				edge temp = hrany[j];
				hrany[j] = hrany[j + 1];
				hrany[j + 1] = temp;
			}
		}
	}

	int Ymin = round(hrany[0].ys), Ymax = 0;								//nastavi Ymax, Ymin
	for (i = 0; i < hrany.size(); i++) {
		if (hrany[i].yk > Ymax)
			Ymax = round(hrany[i].yk);
	}

	QVector<QList<edge>> TH;									//vytvori tabulku hran
	TH.resize(Ymax - Ymin + 1);
	for (i = 0; i < hrany.size(); i++) {
		TH[hrany[i].ys - Ymin].push_back(hrany[i]);
	}
	/*
   for (k = 0; k < TH.size(); k++) {
	   for (i = 0; i < TH[k].size(); i++)
		   qDebug() << TH[k][i].ys << TH[k][i].yk;
   }*/

	QList<edge> ZAH;
	int y = Ymin;
	//qDebug() << "IDEM DO CYKLUUUUUU";
	for (i = 0; i < TH.size(); i++) {
		if (!TH[i].isEmpty()) {														//aktivne hrany pre aktualny riadok presunie do zah
			for (j = 0; j < TH[i].size(); j++) {
				ZAH.push_back(TH[i][j]);
			}
		}

		for ( i = 0; i < ZAH.size(); i++) {										//zoradi hrany podla x
			for ( j = 0; j < ZAH.size() - i - 1; j++) {
				if (ZAH[j + 1].xs < ZAH[j].xs) {
					edge TempHrana = ZAH[j];
					ZAH[j] = ZAH[j + 1];
					ZAH[j + 1] = TempHrana;
				}
			}
		}

		for (j = 0; j < ZAH.size(); j += 2) {										//vykreslovanie
			if (round(ZAH[j].xs) != round(ZAH[j + 1].xs)) {
				for (k = 0; k < round(ZAH[j + 1].xs) - round(ZAH[j].xs); k++) {
					setZbuff(round(ZAH[j].xs + k), y, vypln, z);
				}
			}
		}

		for (j = 0; j < ZAH.size(); j++) {											//zmazanie uz neaktivnych hran, posun o riadok
			if (ZAH[j].dy == 0) {
				ZAH.removeAt(j);
				j--;
			}
			else {
				ZAH[j].dy += -1;
				ZAH[j].xs += ZAH[j].w;
			}
		}
		y++;
	}
}

void ViewerWidget::resetZbuff() {
	int i, j;
	buffer.resize(500);
	for (i = 0; i < 500; i++)
		buffer[i].resize(500);

	for (i = 0; i < buffer.size(); i++) {
		for (j = 0; j < buffer[i].size(); j++) {
			buffer[i][j].farbaPixelu.setRed(255);
			buffer[i][j].farbaPixelu.setGreen(255);
			buffer[i][j].farbaPixelu.setBlue(255);
			//buf[i][j].z = INTMAX_MIN;
			buffer[i][j].z = -74237489;
		}
	}
	//qDebug() << "resetlo sa";
}

void ViewerWidget::setZbuff(int x, int y, QColor farba, double z) {
	if (x < 500 && y < 500 && x>0 && y>0) {
		if (z > buffer[x][y].z) {
			buffer[x][y].z = z;
			buffer[x][y].farbaPixelu = QColor(farba);
		}
	}
}

void ViewerWidget::kresliZbuff() {
	int i, j;
	img->fill(Qt::white);
	update();
	for (i = 0; i < img->width(); i++) {
		for (j = 0; j < img->height(); j++) {
			setPixel(i, j, buffer[i][j].farbaPixelu);
		}
	}
	update();
}
