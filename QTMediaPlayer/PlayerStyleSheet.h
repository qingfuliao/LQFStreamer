#pragma once
#ifndef PLAYER_STYLE_SHEET_H
#define PLAYER_STYLE_SHEET_H
#include <QString>

namespace PlayerStyleSheet
{

	/**
	是不是可以设置图片size
	const QString ToolBarPlayBtn = " \
	QPushButton{ border:none; background-image: url(:/toolbar/play);} \
	QPushButton:hover{ background-image: url(:/toolbar/play);background-color: rgb(50,150, 150);} \
	QPushButton:pressed{ background-image: url(:/toolbar/play);background-size:32px 32px;background-color: rgb(160, 50, 100);}";
	所以使用了border-image后，没必要再指定background-image。
	如果同时指定了两个属性，那么将会使用border-image 绘制覆盖掉background-image。
	*/

	const QString ToolBarPlayBtn = " \
				QPushButton{ border:none; border-image: url(:/toolbar/play);} \
				QPushButton:hover{ border-image: url(:/toolbar/play);background-color: rgb(70,70, 70);} \
				QPushButton:pressed{ border-image: url(:/toolbar/play);background-size:32px 32px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarPauseBtn = " \
                QPushButton{ border:none; border-image: url(:/toolbar/pause);} \
                QPushButton:hover{ border-image: url(:/toolbar/pause);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/pause);background-size:32px 32px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarPreviousButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/previous);} \
                QPushButton:hover{ border-image: url(:/toolbar/previous);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/previous);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarStopBtn = " \
                QPushButton{ border:none; border-image: url(:/toolbar/stop);} \
                QPushButton:hover{ border-image: url(:/toolbar/stop);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/stop);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarNextButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/next);} \
                QPushButton:hover{ border-image: url(:/toolbar/next);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/next);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarFullScreenButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/fullscreen);} \
                QPushButton:hover{ border-image: url(:/toolbar/fullscreen);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/fullscreen);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarSettingButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/extended_16px);} \
                QPushButton:hover{ border-image: url(:/toolbar/extended_16px);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/extended_16px);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarVolumeButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/volume-medium);} \
                QPushButton:hover{ border-image: url(:/toolbar/volume-medium);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/volume-medium);background-size:26px 26px;background-color: rgb(30, 30, 30);}";

	const QString ToolBarModeButtonRepeatOff = " \
                QPushButton{ border:none; border-image: url(:/list/repeat_off);} \
                QPushButton:hover{ border-image: url(:/list/repeat_off);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/list/repeat_off);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarRandomButton = " \
                QPushButton{ border:none; border-image: url(:/list/repeat_one);} \
                QPushButton:hover{ border-image: url(:/list/repeat_one);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/list/repeat_one);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarListButton = " \
                QPushButton{ border:none; border-image: url(:/list/playlist);} \
                QPushButton:hover{ border-image: url(:/list/playlist);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/list/playlist);background-size:26px 26px;background-color: rgb(30, 30, 30);}";


	const QString ToolBarSlowerButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/slower);} \
                QPushButton:hover{ border-image: url(:/toolbar/slower);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/slower);background-size:26px 20px;background-color: rgb(30, 30, 30);}";
	const QString ToolBarFasterButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/faster);} \
                QPushButton:hover{ border-image: url(:/toolbar/faster);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/faster);background-size:26px 20px;background-color: rgb(30, 30, 30);}";





	const QString TopMenuBtn = " \
        QPushButton{font:bold 14px;color: rgb(250,250,250); border:none;background-color: rgb(72,77, 80);} \
        QPushButton:hover{font:bold 14px; background-color: rgb(102,106, 110);} \
        QPushButton:pressed{font:bold 14px;color: rgb(220,220,220); background-color: rgb(50, 50, 50);}";

	const QString ListBtn = " \
        QPushButton{ border:none;background-color: rgb(32,32, 30);} \
        QPushButton:hover{ background-color: rgb(42,42, 42);} \
        QPushButton:pressed{ background-color: rgb(62, 62, 62);}";


	const QString  musicSlider = ("  \
             QSlider::add-page:Horizontal\
             {     \
                background-color: rgb(87, 97, 106);\
                height:4px;\
             }\
             QSlider::sub-page:Horizontal \
            {\
                background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(231,80,229, 255), stop:1 rgba(7,208,255, 255));\
                height:4px;\
             }\
            QSlider::groove:Horizontal \
            {\
                background:transparent;\
                height:6px;\
            }\
            QSlider::handle:Horizontal \
            {\
                height: 30px;\
                width:8px;\
                border-image: url(:/images/ic_music_thumb.png);\
                margin: -8 0px; \
            }\
            ");

	const QString TopMinButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/min);} \
                QPushButton:hover{ border-image: url(:/toolbar/min);background-color: rgb(70,70, 200);} \
                QPushButton:pressed{ border-image: url(:/toolbar/minmin);background-size:26px 26px;background-color: rgb(200, 30, 30);}";

	const QString TopEixtButton = " \
                QPushButton{ border:none; border-image: url(:/toolbar/clear);} \
                QPushButton:hover{ border-image: url(:/toolbar/clear);background-color: rgb(70,70, 70);} \
                QPushButton:pressed{ border-image: url(:/toolbar/clear);background-size:26px 26px;background-color: rgb(30, 30, 30);}";
	const QString  musicSlider1 = ("QSlider::groove:horizontal {  \
     border: 2px solid #999999;            \
     height: 2px;                           \
     margin: 0px 0;                 \
     left: 5px; right: 5px;}    \
    QSlider::add-page:horizontal{   \
    background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #bcbcbc, stop:0.25 #bcbcbc, stop:0.5 #bcbcbc, stop:1 #bcbcbc); }\
    QSlider::sub-page:horizontal{background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #439cf3, stop:0.25 #439cf3, \
    stop:0.5 #439cf3, stop:1 #439cf3);}\
    QSlider::handle:Horizontal{height: 50px; width:6px; background-color: rgb(200,200,200); margin: -4 0px; }");

	//首先是设置主体
	const QString  musicSlider2 = ("QSlider::groove:horizontal {  \
     border: 2px solid #999999;            \
     height: 2px;                           \
     margin: 0px 0;                 \
     left: 5px; right: 5px;}    \
    QSlider::add-page:horizontal{   \
    background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #bcbcbc, stop:0.25 #bcbcbc, stop:0.5 #bcbcbc, stop:1 #bcbcbc); }\
    QSlider::sub-page:horizontal{background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 #439cf3, stop:0.25 #439cf3, \
    stop:0.5 #439cf3, stop:1 #439cf3);}\
    QSlider::handle:Horizontal{height: 50px; width:6px; background-color: rgb(200,200,200); margin: -4 0px; }");


}
#endif // PLAYER_STYLE_SHEET_H
