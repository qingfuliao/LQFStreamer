#pragma once
#include <QtWidgets/QWidget>

#include "ui_VideoListWidget.h"
// 视频列表列宽度初始化系数 200+120+120+140+120=700
const int	k_column_num = 5;
// 视频名
const float  k_column_video_name_ratio = (1.0 * 200 / 700);
// 时长
const float  k_column_video_duration_ratio = (1.0 * 120 / 700);
// 大小
const float  k_column_video_size_ratio = (1.0 * 120 / 700);
// 日期
const float  k_column_video_date_ratio = (1.0 * 140 / 700);
// 更多
const float  k_column_video_more_ratio = (1.0 * 120 / 700);
class QVideoListWidget : public QWidget
{
	Q_OBJECT

public:
	QVideoListWidget(QWidget *parent = Q_NULLPTR);

private:
	Ui::Widget ui;

	void initTableWidget();
};
