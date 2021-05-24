#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_ImageViewer.h"
#include "ViewerWidget.h"
#include "NewImageDialog.h"
#include "Objekt.h"

class ImageViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);

private:
	Ui::ImageViewerClass* ui;
	NewImageDialog* newImgDialog;

	QSettings settings;
	QMessageBox msgBox;

	//ViewerWidget functions
	ViewerWidget* getViewerWidget(int tabId);
	ViewerWidget* getCurrentViewerWidget();

	//Event filters
	bool eventFilter(QObject* obj, QEvent* event);

	//ViewerWidget Events
	bool ViewerWidgetEventFilter(QObject* obj, QEvent* event);
	void ViewerWidgetMouseButtonPress(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseButtonRelease(ViewerWidget* w, QEvent* event);
	void ViewerWidgetMouseMove(ViewerWidget* w, QEvent* event);
	void ViewerWidgetLeave(ViewerWidget* w, QEvent* event);
	void ViewerWidgetEnter(ViewerWidget* w, QEvent* event);
	void ViewerWidgetWheel(ViewerWidget* w, QEvent* event);

	//ImageViewer Events
	void closeEvent(QCloseEvent* event);

	//Image functions
	void openNewTabForImg(ViewerWidget* vW);
	bool openImage(QString filename);
	bool saveImage(QString filename);
	void clearImage();
	void setBackgroundColor(QColor color);

	//Inline functions
	inline bool isImgOpened() { return ui->tabWidget->count() == 0 ? false : true; }

	QPoint A, B;
	QVector <QPointF> pts, temp;
	bool prvybod = false;
	bool nakreslene = false;
	QColor farba = Qt::black;
	QColor vypln = Qt::white;
	QVector <float> rotacie;
	QVector<objekt*> objekty;
	objekt* aktualny = nullptr;
	void swapper(int i);
	void calcTrans();
	void calcSca();
	void calcRot();
 

private slots:
	//Tabs slots
	void on_tabWidget_tabCloseRequested(int tabId);
	void on_actionRename_triggered();

	//Image slots
	void on_actionNew_triggered();
	void newImageAccepted();
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_actionClear_triggered();
	void on_actionSet_background_color_triggered();
	void on_color_clicked();
	void on_fill_clicked();
	void on_comboBox_currentIndexChanged(int i);
	void on_mode_currentIndexChanged(int i);
	void on_up_clicked();
	void on_down_clicked();
	void on_filled_stateChanged(int i);
	void on_kresli_clicked();
	void on_imp_clicked();
	void on_exp_clicked();
	void on_save_clicked();

	/*
	void on_Clear_clicked();
	void on_Clear2_clicked();
	void on_kresli_clicked();
	void on_sym_clicked();
	void on_sko_clicked();
	void on_comboBox_2_currentIndexChanged(int i);
	void on_mode_currentIndexChanged(int i);
	void on_bod_currentIndexChanged(int i);
	void on_uhol_valueChanged(double i);*/
};
