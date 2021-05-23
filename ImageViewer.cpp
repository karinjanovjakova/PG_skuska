#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::ImageViewerClass)
{
	ui->setupUi(this);
	openNewTabForImg(new ViewerWidget("default", QSize(500, 500)));
	ui->farba->setStyleSheet("background-color: black");
	ui->farba2->setStyleSheet("background-color: white");
}

//ViewerWidget functions
ViewerWidget* ImageViewer::getViewerWidget(int tabId)
{
	QScrollArea* s = static_cast<QScrollArea*>(ui->tabWidget->widget(tabId));
	if (s) {
		ViewerWidget* vW = static_cast<ViewerWidget*>(s->widget());
		return vW;
	}
	return nullptr;
}
ViewerWidget* ImageViewer::getCurrentViewerWidget()
{
	return getViewerWidget(ui->tabWidget->currentIndex());
}

// Event filters
bool ImageViewer::eventFilter(QObject* obj, QEvent* event)
{
	if (obj->objectName() == "ViewerWidget") {
		return ViewerWidgetEventFilter(obj, event);
	}
	return false;
}

//ViewerWidget Events
bool ImageViewer::ViewerWidgetEventFilter(QObject* obj, QEvent* event)
{
	ViewerWidget* w = static_cast<ViewerWidget*>(obj);

	if (!w) {
		return false;
	}

	if (event->type() == QEvent::MouseButtonPress) {
		ViewerWidgetMouseButtonPress(w, event);
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		ViewerWidgetMouseButtonRelease(w, event);
	}
	else if (event->type() == QEvent::MouseMove) {
		ViewerWidgetMouseMove(w, event);
	}
	else if (event->type() == QEvent::Leave) {
		ViewerWidgetLeave(w, event);
	}
	else if (event->type() == QEvent::Enter) {
		ViewerWidgetEnter(w, event);
	}
	else if (event->type() == QEvent::Wheel) {
		ViewerWidgetWheel(w, event);
	}

	return QObject::eventFilter(obj, event);
}
void ImageViewer::ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
	if (e->button() == Qt::LeftButton) {
		if (ui->mode->currentIndex() == 0) {				//priamka
			if (pts.size() == 1) {
				pts.append(e->pos());
				objekty.push_back(new objekt(0, objekty.size(), pts, Qt::black));
				getCurrentViewerWidget()->kresliObjekty(objekty);
				aktualny = objekty[objekty.size()-1];
				pts.clear();	pts.squeeze();
				QString str = "vrstva " + QString::number(objekty.size());
				ui->comboBox->addItem(str);
				ui->comboBox->setCurrentIndex(objekty.size() - 1);
			}
			else if (pts.size() == 0) {
				pts.append(e->pos());
			}
			else {
				msgBox.setText(u8"Prive¾a bodov pre zvolený algoritmus. Skúste znovu.");
				msgBox.setIcon(QMessageBox::Information);
				msgBox.exec();
			}
		}
		else if (ui->mode->currentIndex() == 1) {			//stvoruholnik
			if (pts.size() == 1) {
				pts.append(e->pos());
				objekty.push_back(new objektF(1, objekty.size(), pts, Qt::black, Qt::white, true));
				objekty[objekty.size() - 1]->dopocitaj();
				pts = objekty[objekty.size() - 1]->getBody();
				getCurrentViewerWidget()->kresliObjekty(objekty);
				aktualny = objekty[objekty.size() - 1];
				pts.clear();	pts.squeeze();
				QString str = "vrstva " + QString::number(objekty.size());
				ui->comboBox->addItem(str);
				ui->comboBox->setCurrentIndex(objekty.size() - 1);
			}
			else if (pts.size() == 0) {
				pts.append(e->pos());
			}
			else {
				msgBox.setText(u8"Prive¾a bodov pre zvolený algoritmus. Skúste znovu.");
				msgBox.setIcon(QMessageBox::Information);
				msgBox.exec();
			}
		}
		else if (ui->mode->currentIndex() == 2) {			//nuholnik
			pts.append(e->pos());
		}
		else if (ui->mode->currentIndex() == 3) {			//kruznica
			if (pts.size() == 1) {
				pts.append(e->pos());
				objekty.push_back(new objekt(3, objekty.size(), pts, Qt::black));
				getCurrentViewerWidget()->kresliObjekty(objekty);
				aktualny = objekty[objekty.size() - 1];
				pts.clear();	pts.squeeze();
				QString str = "vrstva " + QString::number(objekty.size());
				ui->comboBox->addItem(str);
				ui->comboBox->setCurrentIndex(objekty.size() - 1);
			}
			else if (pts.size() == 0) {
				pts.append(e->pos());
			}
			else {
				msgBox.setText(u8"Prive¾a bodov pre zvolený algoritmus. Skúste znovu.");
				msgBox.setIcon(QMessageBox::Information);
				msgBox.exec();
			}
		}
		else if (ui->mode->currentIndex() == 4) {			//bezier
			pts.append(e->pos());
		}
	}
	if (e->button() == Qt::RightButton) {
		if (ui->mode->currentIndex() == 2) {				//vykreslenie n-uholnika
			if (pts.size() >= 3) {
				//pts.append(pts[0]);
				objekty.push_back(new objektF(2, objekty.size(), pts, Qt::black, Qt::white, true));
				getCurrentViewerWidget()->kresliObjekty(objekty);
				aktualny = objekty[objekty.size() - 1];
				pts.clear();	pts.squeeze();
				QString str = "vrstva " + QString::number(objekty.size());
				ui->comboBox->addItem(str);
				ui->comboBox->setCurrentIndex(objekty.size() - 1);
			}
		}
		if (ui->mode->currentIndex() == 4) {				//vykreslenie bezierovej krivky
			if (pts.size() >= 2) {
				objekty.push_back(new objekt(4, objekty.size(), pts, Qt::black));
				getCurrentViewerWidget()->kresliObjekty(objekty);
				aktualny = objekty[objekty.size() - 1];
				pts.clear();	pts.squeeze();
				QString str = "vrstva " + QString::number(objekty.size());
				ui->comboBox->addItem(str);
				ui->comboBox->setCurrentIndex(objekty.size() - 1);
			}
		}
	}
}

void ImageViewer::ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}

void ImageViewer::ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event)
{
	QMouseEvent* e = static_cast<QMouseEvent*>(event);
}

void ImageViewer::ViewerWidgetLeave(ViewerWidget* w, QEvent* event)
{
}

void ImageViewer::ViewerWidgetEnter(ViewerWidget* w, QEvent* event)
{
}

void ImageViewer::ViewerWidgetWheel(ViewerWidget* w, QEvent* event)
{
	QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
	
}

//ImageViewer Events
void ImageViewer::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Close Confirmation", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No))
	{
		event->accept();
	}
	else {
		event->ignore();
	}
}

//Image functions
void ImageViewer::openNewTabForImg(ViewerWidget* vW)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(vW);

	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidgetResizable(true);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	vW->setObjectName("ViewerWidget");
	vW->installEventFilter(this);

	QString name = vW->getName();

	ui->tabWidget->addTab(scrollArea, name);
}
bool ImageViewer::openImage(QString filename)
{
	QFileInfo fi(filename);

	QString name = fi.baseName();
	openNewTabForImg(new ViewerWidget(name, QSize(0, 0)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);

	ViewerWidget* w = getCurrentViewerWidget();

	QImage loadedImg(filename);
	return w->setImage(loadedImg);
}
bool ImageViewer::saveImage(QString filename)
{
	QFileInfo fi(filename);
	QString extension = fi.completeSuffix();
	ViewerWidget* w = getCurrentViewerWidget();

	QImage* img = w->getImage();
	return img->save(filename, extension.toStdString().c_str());
}
void ImageViewer::clearImage()
{
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear();
}
void ImageViewer::setBackgroundColor(QColor color)
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	ViewerWidget* w = getCurrentViewerWidget();
	w->clear(color);
}

//Slots

//Tabs slots
void ImageViewer::on_tabWidget_tabCloseRequested(int tabId)
{
	ViewerWidget* vW = getViewerWidget(tabId);
	delete vW; //vW->~ViewerWidget();
	ui->tabWidget->removeTab(tabId);
}
void ImageViewer::on_actionRename_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	ViewerWidget* w = getCurrentViewerWidget();
	bool ok;
	QString text = QInputDialog::getText(this, QString("Rename image"), tr("Image name:"), QLineEdit::Normal, w->getName(), &ok);
	if (ok && !text.trimmed().isEmpty())
	{
		w->setName(text);
		ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), text);
	}
}

//Image slots
void ImageViewer::on_actionNew_triggered()
{
	newImgDialog = new NewImageDialog(this);
	connect(newImgDialog, SIGNAL(accepted()), this, SLOT(newImageAccepted()));
	newImgDialog->exec();
}
void ImageViewer::newImageAccepted()
{
	NewImageDialog* newImgDialog = static_cast<NewImageDialog*>(sender());

	int width = newImgDialog->getWidth();
	int height = newImgDialog->getHeight();
	QString name = newImgDialog->getName();
	openNewTabForImg(new ViewerWidget(name, QSize(width, height)));
	ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}
void ImageViewer::on_actionOpen_triggered()
{
	QString folder = settings.value("folder_img_load_path", "").toString();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", folder, fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_load_path", fi.absoluteDir().absolutePath());

	if (!openImage(fileName)) {
		msgBox.setText("Unable to open image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}
void ImageViewer::on_actionSave_as_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image to save.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QString folder = settings.value("folder_img_save_path", "").toString();

	ViewerWidget* w = getCurrentViewerWidget();

	QString fileFilter = "Image data (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm .*xbm .* xpm);;All files (*)";
	QString fileName = QFileDialog::getSaveFileName(this, "Save image", folder + "/" + w->getName(), fileFilter);
	if (fileName.isEmpty()) { return; }

	QFileInfo fi(fileName);
	settings.setValue("folder_img_save_path", fi.absoluteDir().absolutePath());

	if (!saveImage(fileName)) {
		msgBox.setText("Unable to save image.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	else {
		msgBox.setText(QString("File %1 saved.").arg(fileName));
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
	}
}
void ImageViewer::on_actionClear_triggered()
{
	if (!isImgOpened()) {
		msgBox.setText("No image is opened.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	clearImage();
	objekty.clear();	objekty.squeeze();	ui->comboBox->clear();

}
void ImageViewer::on_actionSet_background_color_triggered()
{
	QColor backgroundColor = QColorDialog::getColor(Qt::white, this, "Select color of background");
	if (backgroundColor.isValid()) {
		setBackgroundColor(backgroundColor);
	}
}

void ImageViewer::on_color_clicked() {
	if (aktualny == NULL) {
		msgBox.setText(u8"Pre transformáciu objektu najskôr zvo¾te objekt.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QColor Color = QColorDialog::getColor(Qt::white, this, "Select color");
	if (Color.isValid()) {
		farba = Color;
		aktualny->setFarba(Color);
		QString qss = QString("background-color: %1 ").arg(Color.name());
		ui->farba->setStyleSheet(qss);
	}
	getCurrentViewerWidget()->kresliObjekty(objekty);
}

void ImageViewer::on_fill_clicked() {
	if (aktualny == NULL) {
		msgBox.setText(u8"Pre transformáciu objektu najskôr zvo¾te objekt.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	if (aktualny->getTyp() == 2 || aktualny->getTyp() == 1) {
		QColor Color = QColorDialog::getColor(Qt::white, this, "Select fill color");
		if (Color.isValid()) {
			vypln = Color;
			aktualny->setFill(Color);
			QString qss = QString("background-color: %1 ").arg(Color.name());
			ui->farba2->setStyleSheet(qss);
		}
		getCurrentViewerWidget()->kresliObjekty(objekty);
	}
}

void ImageViewer::calcTrans() {
	if (aktualny == NULL) {
		msgBox.setText(u8"Pre transformáciu objektu najskôr zvo¾te objekt.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	int i;
	QVector<QPointF> poly = aktualny->getBody();
	for (i = 0; i < poly.size(); i++) {
		poly[i].setX(poly[i].x() + ui->transX->value());
	}
	for (i = 0; i < poly.size(); i++) {
		poly[i].setY(poly[i].y() + ui->transY->value());
	}
	aktualny->setBody(poly);
}


void ImageViewer::calcRot() {
	if (aktualny == NULL) {
		msgBox.setText(u8"Pre transformáciu objektu najskôr zvo¾te objekt.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QVector<QPointF> poly = aktualny->getBody();
	double uhol = ui->rot_spin->value() / 180.0 * M_PI;
	double sX = poly[0].x(), sY = poly[0].y(), i = 0, x ,y;
	
	if (ui->rot_spin->value() < 0)
	{
		for (int i = 1; i < poly.size(); i++)
		{
			x = poly[i].x();
			y = poly[i].y();

			poly[i].setX(((x - sX) * qCos(uhol) + (y - sY) * qSin(uhol) + sX));
			poly[i].setY(-(x - sX) * qSin(uhol) + (y - sY) * qCos(uhol) + sY);
		}
	}
	else if (ui->rot_spin->value() > 0)
	{
		uhol = 2 * M_PI - uhol;
		for (int i = 1; i < poly.size(); i++)
		{
			x = poly[i].x();
			y = poly[i].y();

			poly[i].setX((x - sX) * qCos(uhol) - (y - sY) * qSin(uhol) + sX);
			poly[i].setY((x - sX) * qSin(uhol ) + (y - sY) * qCos(uhol ) + sY);
		}
	}
	aktualny->setBody(poly);
}

void ImageViewer::calcSca() {
	int i;
	if (aktualny == NULL) {
		msgBox.setText(u8"Pre transformáciu objektu najskôr zvo¾te objekt.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	QVector<QPointF> poly = aktualny->getBody();
	for (i = 0; i < poly.size(); i++) {
		poly[i].setX(poly[0].x() + ((poly[i].x() - poly[0].x()) * ui->sca_spin->value()));
		poly[i].setY(poly[0].y() + ((poly[i].y() - poly[0].y()) * ui->sca_spin->value()));
	}
	aktualny->setBody(poly);
}

void ImageViewer::on_kresli_clicked() {
	calcRot();
	calcSca();
	calcTrans();
	ui->transX->setValue(0);
	ui->transY->setValue(0);
	ui->sca_spin->setValue(1.0);
	ui->rot_spin->setValue(0);
	getCurrentViewerWidget()->kresliObjekty(objekty);
}

void ImageViewer::on_comboBox_currentIndexChanged(int i) {
	if (!objekty.isEmpty()) {
		aktualny = objekty[i];
		QColor Color = aktualny->getFill();
		QString qss = QString("background-color: %1 ").arg(Color.name());
		ui->farba2->setStyleSheet(qss);
		Color = aktualny->getFarba();
		qss = QString("background-color: %1 ").arg(Color.name());
		ui->farba->setStyleSheet(qss);
		ui->rot_spin->setValue(0.0);
		ui->sca_spin->setValue(1);
		ui->transX->setValue(0);
		ui->transY->setValue(0);
	}
}

void ImageViewer::on_mode_currentIndexChanged(int i) {
	pts.clear();	pts.squeeze();
}

void ImageViewer::swapper(int i) {				//swapne smerom dole
	if (objekty.size() <= i || objekty.size()<2 || i==0) {
		msgBox.setText(u8"Minnimálny poèet bodov je 2. Zadajte ïalšie body.");
		msgBox.setIcon(QMessageBox::Information);
		msgBox.exec();
		return;
	}
	else {
		objekt* temp = objekty[i];
		objekty[i] = objekty[i - 1];
		objekty[i - 1] = temp;
		objekty[i]->setZ(objekty[i]->getZ() + 1);
		objekty[i-1]->setZ(objekty[i]->getZ() - 1);
	}
}

void ImageViewer::on_up_clicked() {
	int i = ui->comboBox->currentIndex() + 1;
	swapper(i);
	getCurrentViewerWidget()->kresliObjekty(objekty);
}

void ImageViewer::on_down_clicked() {
	int i = ui->comboBox->currentIndex();
	swapper(i);
	getCurrentViewerWidget()->kresliObjekty(objekty);
}

void ImageViewer::on_filled_stateChanged(int i) {
	aktualny->setVyplnaj(i);
	getCurrentViewerWidget()->kresliObjekty(objekty);
}


void ImageViewer::on_imp_clicked() {
	ui->comboBox->clear();
	objekty.clear();
	int typ, pts;
	QPointF A;
	QVector<QPointF> body;
	QColor col, fill;
	QString fileName = QFileDialog::getOpenFileName(this, "Load ", "", "data (*.vtk *.txt);;All files (*)");
	if (fileName.isEmpty()) { return; }
	QFile file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		msgBox.setText("Unable to open file.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
	QString line;
	line = file.readLine();
	if (line != "#pocet objektov\r\n") {
		msgBox.setText(u8"Hlavièka súboru nie je správna.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}
	
	line = file.readLine();
	int obj = line.toInt(); //pocet objektov
	for (int i = 0; i < obj; i++) {
		line = file.readLine();
		typ=line.toInt();
		line = file.readLine();
		pts = line.toInt();
		for (int j = 0; j < pts; j++) {
			line = file.readLine();
			A.setX(line.split(" ").at(0).toFloat());	A.setY(line.split(" ").at(1).toFloat());
			body.append(A);
		}
		line = file.readLine();		//farba
		col.setRed(line.split(" ").at(0).toInt()); col.setGreen(line.split(" ").at(0).toInt()); col.setBlue(line.split(" ").at(0).toInt());
		line = file.readLine();		//fill
		fill.setRed(line.split(" ").at(0).toInt()); fill.setGreen(line.split(" ").at(0).toInt()); fill.setBlue(line.split(" ").at(0).toInt());
		line = file.readLine();		//vyplnaj
		if (typ == 0 || typ == 3 || typ == 4) {
			objekty.push_back(new objekt(typ, i, body, col));
		}
		else if (typ == 1 || typ == 2) {
			if (typ == 1) {

			}
			if (line.toInt() == 0) {
				objekty.push_back(new objektF(typ, i, body, col, fill, false));
			}
			else
				objekty.push_back(new objektF(typ, i, body, col, fill, true));
		}
		QString str = "vrstva " + QString::number(i);
		ui->comboBox->addItem(str);
		qDebug() << body.size();
		body.clear(); body.squeeze();
	}
	getCurrentViewerWidget()->kresliObjekty(objekty);
}

void ImageViewer::on_exp_clicked() {
	if (objekty.isEmpty()) {
		msgBox.setText(u8"Najskôr vytvorte objekty.");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return;
	}
	int i,j; 
	QFile file("out2.vtk");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file);
	out << "#pocet objektov\n" << objekty.size() << "\n";
	for (i = 0; i < objekty.size(); i++) {
		out << objekty[i]->getTyp() << "\n";
		out << objekty[i]->getBody().size() << "\n";
		for (j = 0; j < objekty[i]->getBody().size(); j++) {
			out << objekty[i]->getBody()[j].x() << " " << objekty[i]->getBody()[j].y() << "\n";
		}
		out << objekty[i]->getFarba().red()<< " " << objekty[i]->getFarba().green() << " " << objekty[i]->getFarba().blue() << "\n";
		out << objekty[i]->getFill().red() << " " << objekty[i]->getFill().green() << " " << objekty[i]->getFill().blue() << "\n";
		if(objekty[i]->getVyplnaj()==false)
			out << 0 << "\n";
		else 
			out << 1 << "\n";

	}
}


/*
void ImageViewer::on_Clear_clicked() {
	nakreslene = false;
	prvybod = false;
	rotacie.clear();
	rotacie.squeeze();
	pts.clear();
	pts.squeeze();
	temp.clear();
	temp.squeeze();
	ui->polygon->setEnabled(true);
	ui->comboBox->setEnabled(true);
	ui->transform->setVisible(false);
	ui->fill->setVisible(false);
	if (ui->mode->currentIndex()==0)
		ui->poly->setVisible(true);
	else
		ui->poly->setVisible(false);
	ui->comboBox->setEnabled(true);
	ui->mode->setEnabled(true);
	clearImage();
	update();
}

void ImageViewer::on_Clear2_clicked() {
	nakreslene = false;
	prvybod = false;
	rotacie.clear();
	rotacie.squeeze();
	//ui->bod->clear();
	//for (int i=0; i<poly.size();i++)
		//ui->bod->removeItem(0);
	poly.clear();
	poly.squeeze();
	temp.clear();
	temp.squeeze();
	ui->comboBox->setEnabled(true);
	ui->hermit->setVisible(false);
	ui->mode->setEnabled(true);
	ui->transform->setVisible(false);
	ui->fill->setVisible(false);
	if (ui->mode->currentIndex() == 0)
		ui->poly->setVisible(true);
	else
		ui->poly->setVisible(false);
	ui->krivka->setEnabled(true);
	ui->bod->clear();
	clearImage();
	update();
}

void ImageViewer::on_kresli_clicked() {
	if (ui->krivka->currentIndex() == 0) {
		if (poly.size() < 2) {
			msgBox.setText(u8"Minnimálny poèet bodov je 2. Zadajte ïalšie body.");
			msgBox.setIcon(QMessageBox::Information);
			msgBox.exec();
			return;
		}
		else {
			getCurrentViewerWidget()->kresliKrivku(poly, ui->krivka->currentIndex(), ui->comboBox->currentIndex(),rotacie);
			ui->hermit->setVisible(true);
		}
	}
	if (ui->krivka->currentIndex() == 1) {
		if (poly.size() < 3) {
			msgBox.setText(u8"Minnimálny poèet bodov je 3. Zadajte ïalšie body.");
			msgBox.setIcon(QMessageBox::Information);
			msgBox.exec();
			return;
		}
		else {
			getCurrentViewerWidget()->kresliKrivku(poly, ui->krivka->currentIndex(), ui->comboBox->currentIndex(),rotacie);
		}
	}
	if (ui->krivka->currentIndex() == 2) {
		if (poly.size() < 4) {
			msgBox.setText(u8"Minnimálny poèet bodov je 4. Zadajte ïalšie body.");
			msgBox.setIcon(QMessageBox::Information);
			msgBox.exec();
			return;
		}
		else {
			getCurrentViewerWidget()->kresliKrivku(poly, ui->krivka->currentIndex(), ui->comboBox->currentIndex(),rotacie);
		}
	}
	nakreslene = true;
	ui->krivka->setEnabled(false);
	update();
}

void ImageViewer::on_comboBox_2_currentIndexChanged(int i) {
	getCurrentViewerWidget()->kresliPolygon(poly, farba, ui->comboBox->currentIndex(), vypln, i);
}

void ImageViewer::on_mode_currentIndexChanged(int i) {
	if (ui->mode->currentIndex() == 0) {
		ui->transform->setVisible(false);
		ui->poly->setVisible(true);
		ui->fill->setVisible(false);
		ui->krivky->setVisible(false);
		poly.clear();
		poly.squeeze();
	}
	if (ui->mode->currentIndex() == 1) {
		ui->transform->setVisible(false);
		ui->poly->setVisible(false);
		ui->fill->setVisible(false);
		ui->krivky->setVisible(true);
		ui->hermit->setVisible(false);
		poly.clear();
		poly.squeeze();
	}
}

void ImageViewer::on_bod_currentIndexChanged(int i) {
	if (!rotacie.isEmpty())
		ui->uhol->setValue(rotacie[i]);
}

void ImageViewer::on_uhol_valueChanged(double i) {
	if (!rotacie.isEmpty()) {
		rotacie[ui->bod->currentIndex()] = i;
		clearImage();
		getCurrentViewerWidget()->kresliKrivku(poly, ui->krivka->currentIndex(), ui->comboBox->currentIndex(), rotacie);
	}	
}*/