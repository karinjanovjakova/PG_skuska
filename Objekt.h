#pragma once
#include <QtWidgets>

class objekt {
protected:
	int typ, z;
	QVector<QPointF> body;
	QColor farba;
public: 
	objekt() { QVector<QPointF> volaco; typ = -1; z = -1; body = volaco; farba = Qt::white; };
	objekt(int i, int j, QVector<QPointF> pts, QColor f) { typ = i;  z = j; body = pts; farba = f; };
	int getTyp() { return typ; };
	int getZ() { return z; };
 	QVector<QPointF> getBody() { return body; };
	QColor getFarba() { return farba; };

	void setTyp(int i) { typ = i; };
	void setZ(int i) { z = i; };
	void setBody(QVector<QPointF> pts) { body = pts; };
	void setFarba(QColor f) { farba = f; };

	//virtual stvoruholnik
	virtual void dopocitaj() {};
	virtual QColor getFill() { return Qt::white; };
	virtual void setFill(QColor fill) {};
	virtual bool getVyplnaj() { return false; };
	virtual void setVyplnaj(bool vyp) {};

};

class objektF : public objekt {
protected : 
	QColor fill;
	bool vyplnaj;
public: 
	objektF() :objekt() { fill = Qt::white; vyplnaj = true; };
	objektF(int i, int j, QVector<QPointF> pts, QColor c, QColor f, bool vyp) : objekt(i, j, pts, c) { fill = f; vyplnaj = vyp; };
	void setFill(QColor f) { fill = f; };
	void setVyplnaj(bool vyp) { vyplnaj = vyp; };
	bool getVyplnaj() {
		if (vyplnaj)
			return true;
		else
			return false;
	};
	QColor getFill() { return fill; };
	void dopocitaj() {
		if (typ == 1 && body.size()==2) {
			QPointF A,B,C, D;
			C.setX(body[0].x());	C.setY(body[1].y());
			D.setX(body[1].x());	D.setY(body[0].y());
			body.append(C);		body.append(D);
			QVector<QPointF> newBody;
			newBody.append(body[0]); newBody.append(body[2]); newBody.append(body[1]); newBody.append(body[3]);
			body = newBody;
		}
	};
};
