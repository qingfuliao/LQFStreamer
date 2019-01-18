#pragma once
#include <QtWidgets/QWidget>

#include "ui_AdvancedWidget.h"

class QAdvancedWidget : public QWidget
{
	Q_OBJECT

public:
	QAdvancedWidget(QWidget *parent = Q_NULLPTR);

private:
	Ui::AdvancedForm ui;
};

