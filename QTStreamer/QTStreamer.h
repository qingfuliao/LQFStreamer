#pragma once

#include <QtWidgets/QWidget>
#include "ui_QTStreamer.h"

class QGeneralWidget;

class QTStreamer : public QWidget
{
	Q_OBJECT

public:
	QTStreamer(QWidget *parent = Q_NULLPTR);

private slots:
	void slotGeneralButton();
	void slotVideoListButton();
	void slotAdvanceButton();
private:
	Ui::QTStreamerClass ui;
	QGeneralWidget *genera_widget_ = nullptr;
};
