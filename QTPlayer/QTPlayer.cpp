#include "QTPlayer.h"

QTPlayer::QTPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void QTPlayer::slotPlay()
{
	play_enable_ = true;
}