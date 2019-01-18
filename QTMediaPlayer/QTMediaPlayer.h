#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTMediaPlayer.h"

#include <QCloseEvent>

#include "PlayerInfo.h"
class MediaPlayer;
class QTMediaPlayer : public QMainWindow
{
	Q_OBJECT

public:
	QTMediaPlayer(QWidget *parent = Q_NULLPTR);
	void HandlePlayerEvent(PlayerEvent e, void *param);
protected:       
	void closeEvent(QCloseEvent *event);
private:
	void initUI();
	void initPlayer();
	void handlePlay();
	void handleStop();
	void handleDuration(void *param);
	void handleProgress(void *param);
	void handlePlayFinish();
signals:
	void signalChangePlayButtonIcon(int);
	void signalChangeTotalDuration(QString);
	void signalChangeProgressSlider(int);
private slots:
	void slotStop();
	void slotPlayOrPause();
	void setProgressValue(int);
	void slotChangePlayButtonIcon(int);
	void slotChangeTime(QString);
	void slotChangeProgressSlider(int);
	void slotSeekProgress(int);
	void slotSeekProgress();
	void slotProgressPress();
	void slotOpenFile();		//打开文件
	void slotSlow();			// 慢放
	void slotFast();			// 快放
	void slotVolume(int);
private:
	Ui::QTMediaPlayerClass ui;

	MediaPlayer *player_ = nullptr;

	int progress_max_value_ = 1000;
	int volume_max_value_ = 20;
	QString total_time_;	// 完整的时间长度
	QString cur_time_;		// 当前时间
	std::string url_ = "shahai45.mp4";			// 打开文件链接
	bool is_progress_press_ = false;
	float volume_default_ratio_ = 0.5; // 范围从0~1.0 对应最小~最大声音
};
