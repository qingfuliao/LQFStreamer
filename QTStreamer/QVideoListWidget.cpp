#include "QVideoListWidget.h"

#include <QStringList>
#include <QDateTime>
QVideoListWidget::QVideoListWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initTableWidget();
}


void QVideoListWidget::initTableWidget()
{
	// 设置列数和行高
	int width = this->width();
	// 5列 系数比例
	ui.tableWidget->setColumnCount(k_column_num);
	ui.tableWidget->setColumnWidth(0, k_column_video_name_ratio * width);
	ui.tableWidget->setColumnWidth(1, k_column_video_duration_ratio * width);
	ui.tableWidget->setColumnWidth(2, k_column_video_size_ratio * width);
	ui.tableWidget->setColumnWidth(3, k_column_video_date_ratio * width);
	ui.tableWidget->setColumnWidth(4, k_column_video_more_ratio * width);

	QStringList head_text;
	head_text << u8"视频名" << u8"时长" << u8"大小" << u8"日期" << u8"更多";
	ui.tableWidget->setHorizontalHeaderLabels(head_text);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget->setAlternatingRowColors(true);
	ui.tableWidget->verticalHeader()->setVisible(false);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);

	//设置行数
	ui.tableWidget->setRowCount(100);

	for (int i = 0; i < 100; i++) 
	{
		ui.tableWidget->setRowHeight(i, 24);

		QTableWidgetItem *item_video_name = new QTableWidgetItem(QString("%1.mp4").arg(i + 1));
		QTableWidgetItem *item_video_duration = new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss"));
		QTableWidgetItem *item_video_size = new QTableWidgetItem(QString("%1 M").arg(i + 1));
		QTableWidgetItem *item_video_date = new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
		QTableWidgetItem *item_video_more = new QTableWidgetItem(u8"更多");

		ui.tableWidget->setItem(i, 0, item_video_name);
		ui.tableWidget->setItem(i, 1, item_video_duration);
		ui.tableWidget->setItem(i, 2, item_video_size);
		ui.tableWidget->setItem(i, 3, item_video_date);
		ui.tableWidget->setItem(i, 4, item_video_more);
	}
}
