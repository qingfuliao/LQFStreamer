#pragma once

#include <QtWidgets/QWidget>

#include "ui_GeneralWidget.h"
class QGeneralWidget : public QWidget
{
	Q_OBJECT

public:
	QGeneralWidget(QWidget *parent = Q_NULLPTR);

private:
	Ui::GeneralForm ui;
};

