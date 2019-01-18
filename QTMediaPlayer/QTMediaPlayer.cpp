#include "QTMediaPlayer.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>

#include "PlayerStyleSheet.h"
#include "MediaPlayer.h"


static int mediaPlayerCallback(PlayerEvent e, void *param, void *user)
{
	QTMediaPlayer *qmedia_player = (QTMediaPlayer *)user;
	qmedia_player->HandlePlayerEvent(e, param);
	return 0;
}
QTMediaPlayer::QTMediaPlayer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	initUI();

	player_ = new MediaPlayer(ui.openGLWidget);
	player_->RegisterEventCallback(mediaPlayerCallback, this);
	initPlayer();
}

void QTMediaPlayer::slotStop()
{
	qDebug() << "slotStop";
	player_->StopCommand();
}
void  QTMediaPlayer::slotPlayOrPause()
{
	qDebug() << "slotPlayOrPause";
	if (player_->IsStoped())		// 开始播放
	{
		if (!url_.empty())
		{
			player_->PlayCommand(url_.c_str());
			ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPauseBtn);
		}
	}
	else
	{
		player_->TogglePauseCommand();
		if (player_->IsPaused())
		{
			// 暂停状态则显示三角符号提示用户点击恢复播放
			ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPlayBtn);
		}
		else
		{
			ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPauseBtn);
		}
	}
}

void QTMediaPlayer::setProgressValue(int pos)
{
	// 发送比例  pos / progress_max_value_
}

void QTMediaPlayer::slotChangePlayButtonIcon(int value)
{
	if (1 == value)
	{
		ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPlayBtn);
	}
	else if(2 == value)
	{
		ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPlayBtn);
		ui.progressSlider->setValue(progress_max_value_);
	}
	else 
	{
		ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPauseBtn);
	}
}

void QTMediaPlayer::slotChangeTime(QString time)
{
	ui.timeLabel->setText(time);
}

void QTMediaPlayer::slotChangeProgressSlider(int value)
{
	ui.progressSlider->setValue(value);
}

void QTMediaPlayer::slotSeekProgress(int pos)
{
//	player_->SeekCommand(1.0 * pos /progress_max_value_);
}

void QTMediaPlayer::slotSeekProgress()
{
	int pos = ui.progressSlider->value();
	player_->SeekCommand(1.0 * pos / progress_max_value_);
	is_progress_press_ = false;
}

void QTMediaPlayer::slotProgressPress()
{
	is_progress_press_ = true;
}

void QTMediaPlayer::slotOpenFile()
{
	// 这里只是简单的打开一个文件
	QString file_url_ = QFileDialog::getOpenFileName(
		this, u8"选择要播放的文件",
		"/",//初始目录
		u8"视频文件 (*.flv *.rmvb *.avi *.MP4);; 所有文件 (*.*);; ");
	if (!file_url_.isEmpty())
	{
		file_url_.replace("/", "\\");
		url_ = file_url_.toStdString();
	}
}

void QTMediaPlayer::slotSlow()
{
	player_->DecelerateCommand();
}

void QTMediaPlayer::slotFast()
{
	player_->AccelerateCommand();
}

void QTMediaPlayer::slotVolume(int pos)
{
	float percent = 1.0*pos / volume_max_value_;
	player_->AdjustVolumeCommand(percent);
}

void QTMediaPlayer::HandlePlayerEvent(PlayerEvent e, void * param)
{
	switch (e)
	{
	case PLAYER_EVENT_PLAYBACK_START:
		// 通知修改播放暂停按钮的暂停
		handlePlay();
		break;
	case PLAYER_EVENT_PLAYBACK_STOPED:
		handleStop();
		break;
	case PLAYER_EVENT_UPDATE_DURATION:	// 更新当前文件的总时长
		handleDuration(param);
		break;
	case PLAYER_EVENT_UPDATE_PROGRESS:	// 更新当前的播放进度
		// (1)更新进度条
		// (2)更新时间的显示
		handleProgress(param);
		break;
	case PLAYER_EVENT_PLAYBACK_FINISHED:
		handlePlayFinish();
		break;
	default:
		break;
	}
}

void QTMediaPlayer::closeEvent(QCloseEvent * event)
{
	QMessageBox::StandardButton button;       
	button = QMessageBox::question(this, tr(u8"退出程序"),
		QString(tr(u8"确认退出程序")), QMessageBox::Yes | QMessageBox::No);
	if (button == QMessageBox::No) 
	{
		event->ignore(); // 忽略退出信号，程序继续进行       
	}       
	else if(button==QMessageBox::Yes)       
	{           
		event->accept(); // 接受退出信号，程序退出 
		if (!player_->IsStoped())
		{
			player_->StopCommand();
			while (true)
			{
				if (player_->IsStoped())
					break;
				Sleep(30);
			}
			delete player_;
			player_ = nullptr;
		}
	}  
}

void QTMediaPlayer::initUI()
{
	ui.playButton->setStyleSheet(PlayerStyleSheet::ToolBarPlayBtn);
	ui.previousButton->setStyleSheet(PlayerStyleSheet::ToolBarPreviousButton);
	ui.stopButton->setStyleSheet(PlayerStyleSheet::ToolBarStopBtn);
	ui.nextButton->setStyleSheet(PlayerStyleSheet::ToolBarNextButton);

	// 全屏按钮
	ui.fullScreenButton->setStyleSheet(PlayerStyleSheet::ToolBarFullScreenButton);
	// 拓展设置
	ui.settingButton->setStyleSheet(PlayerStyleSheet::ToolBarSettingButton);

	// 打开播放列表
	ui.listButton->setStyleSheet(PlayerStyleSheet::ToolBarListButton);
	// 设置循环模式
	ui.modeButton->setStyleSheet(PlayerStyleSheet::ToolBarModeButtonRepeatOff);
	//
	ui.randomButton->setStyleSheet(PlayerStyleSheet::ToolBarRandomButton);

	//设置播放速度
	ui.showListButton->setStyleSheet(PlayerStyleSheet::ToolBarSlowerButton);
	ui.fasterButton->setStyleSheet(PlayerStyleSheet::ToolBarFasterButton);

	// 设置音量键
	ui.volumeButton->setStyleSheet(PlayerStyleSheet::ToolBarVolumeButton);

	// 设置进度条的值
	ui.progressSlider->setMaximum(progress_max_value_);
	ui.progressSlider->setSingleStep(1);
	// 初始化时间
	total_time_.sprintf("%02d:%02d:%02d", 0, 0, 0);
	cur_time_.sprintf("%02d:%02d:%02d", 0, 0, 0);
	// 绑定按钮显示的不同
	connect(this, SIGNAL(signalChangePlayButtonIcon(int)), this, SLOT(slotChangePlayButtonIcon(int)));
	// 更新总时间
	connect(this, SIGNAL(signalChangeTotalDuration(QString)), this, SLOT(slotChangeTime(QString)));
	// 更新进度条
	connect(this, SIGNAL(signalChangeProgressSlider(int)), this, SLOT(slotChangeProgressSlider(int)));

	// 打开文件
	connect(ui.actionOpenFIle, SIGNAL(triggered()), this, SLOT(slotOpenFile()));

	// 音量控制
	ui.volumeSlider->setMaximum(volume_max_value_);
}

void QTMediaPlayer::initPlayer()
{
	ui.volumeSlider->setValue(volume_max_value_ * volume_default_ratio_);
	player_->AdjustVolumeCommand(volume_default_ratio_);
}

void QTMediaPlayer::handlePlay()
{
	emit signalChangePlayButtonIcon(0);	// 0 播放中, 1暂停
}

void QTMediaPlayer::handleStop()
{
	emit signalChangePlayButtonIcon(1);	// 0 播放中, 1暂停
}

void QTMediaPlayer::handleDuration(void * param)
{
	PlayerDuration *duration = (PlayerDuration *)param;
	QString time;
	total_time_.sprintf("%02d:%02d:%02d", duration->hour, duration->minute, duration->second);
	time = cur_time_ + "\/" + total_time_; //.sprintf("%s//%s", cur_time_.constData(), total_time_.constData());
	emit signalChangeTotalDuration(time);
}

void QTMediaPlayer::handleProgress(void * param)
{
	PlayerProgress *progress = (PlayerProgress *)param;
	QString time;
	cur_time_.sprintf("%02d:%02d:%02d", progress->hour, progress->minute, progress->second);
	time = cur_time_ + "\/" + total_time_;
	emit signalChangeTotalDuration(time);
	int pos = progress->percentage * progress_max_value_;
	if(!is_progress_press_)		// 进度条按下时不能更新进度条
	emit signalChangeProgressSlider(pos);
}

void QTMediaPlayer::handlePlayFinish()
{
	emit signalChangePlayButtonIcon(2);	// 0 播放中, 1暂停 , 2播放完毕
}
