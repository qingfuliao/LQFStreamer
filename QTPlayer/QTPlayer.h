#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTPlayer.h"

class QTPlayer : public QMainWindow
{
	Q_OBJECT

public:
	QTPlayer(QWidget *parent = Q_NULLPTR);
// 	private slots:
// 	void slotPlay();
private slots:
	void slotPlay();
private:
	Ui::QTPlayerClass ui;

	bool play_enable_ = false;
};
