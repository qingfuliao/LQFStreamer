#include "QTStreamer.h"
#include "QGeneralWidget.h"
QTStreamer::QTStreamer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.stackedWidget->setCurrentIndex(0);
}
void QTStreamer::slotGeneralButton()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void QTStreamer::slotVideoListButton()
{
	ui.stackedWidget->setCurrentIndex(1);
}

void QTStreamer::slotAdvanceButton()
{
	ui.stackedWidget->setCurrentIndex(2);
}
