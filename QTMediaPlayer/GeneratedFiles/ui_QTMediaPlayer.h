/********************************************************************************
** Form generated from reading UI file 'QTMediaPlayer.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTMEDIAPLAYER_H
#define UI_QTMEDIAPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "videowidget.h"

QT_BEGIN_NAMESPACE

class Ui_QTMediaPlayerClass
{
public:
    QAction *actionOpenFIle;
    QAction *actionOpenUrl;
    QAction *actionPlayPause;
    QAction *actionStop;
    QWidget *centralWidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    VideoWidget *openGLWidget;
    QWidget *bottomWidget;
    QWidget *bottomTopwidget;
    QSlider *progressSlider;
    QPushButton *showListButton;
    QPushButton *fasterButton;
    QWidget *bottomBottomwidget;
    QWidget *bottomBottomLeftWidget;
    QPushButton *stopButton;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QPushButton *playButton;
    QWidget *bottomBottomCenterWidget;
    QLabel *timeLabel;
    QPushButton *volumeButton;
    QSlider *volumeSlider;
    QWidget *bottomBottomRightWidget;
    QPushButton *modeButton;
    QPushButton *listButton;
    QPushButton *randomButton;
    QPushButton *settingButton;
    QPushButton *fullScreenButton;
    QSpacerItem *horizontalSpacer;
    QMenuBar *menuBar;
    QMenu *menuMedia;
    QMenu *menuPlay;

    void setupUi(QMainWindow *QTMediaPlayerClass)
    {
        if (QTMediaPlayerClass->objectName().isEmpty())
            QTMediaPlayerClass->setObjectName(QStringLiteral("QTMediaPlayerClass"));
        QTMediaPlayerClass->resize(1104, 709);
        actionOpenFIle = new QAction(QTMediaPlayerClass);
        actionOpenFIle->setObjectName(QStringLiteral("actionOpenFIle"));
        actionOpenUrl = new QAction(QTMediaPlayerClass);
        actionOpenUrl->setObjectName(QStringLiteral("actionOpenUrl"));
        actionPlayPause = new QAction(QTMediaPlayerClass);
        actionPlayPause->setObjectName(QStringLiteral("actionPlayPause"));
        actionStop = new QAction(QTMediaPlayerClass);
        actionStop->setObjectName(QStringLiteral("actionStop"));
        centralWidget = new QWidget(QTMediaPlayerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 10, 1101, 696));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new VideoWidget(verticalLayoutWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setEnabled(true);
        openGLWidget->setMinimumSize(QSize(0, 600));
        openGLWidget->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 0);"));

        verticalLayout->addWidget(openGLWidget);

        bottomWidget = new QWidget(verticalLayoutWidget);
        bottomWidget->setObjectName(QStringLiteral("bottomWidget"));
        bottomWidget->setMinimumSize(QSize(200, 60));
        bottomWidget->setStyleSheet(QStringLiteral("background-color: rgb(10, 10, 10);"));
        bottomTopwidget = new QWidget(bottomWidget);
        bottomTopwidget->setObjectName(QStringLiteral("bottomTopwidget"));
        bottomTopwidget->setGeometry(QRect(0, 0, 1101, 20));
        bottomTopwidget->setStyleSheet(QStringLiteral("background-color: rgb(10, 10, 10);"));
        progressSlider = new QSlider(bottomTopwidget);
        progressSlider->setObjectName(QStringLiteral("progressSlider"));
        progressSlider->setGeometry(QRect(40, 0, 1011, 20));
        progressSlider->setStyleSheet(QStringLiteral(""));
        progressSlider->setOrientation(Qt::Horizontal);
        showListButton = new QPushButton(bottomTopwidget);
        showListButton->setObjectName(QStringLiteral("showListButton"));
        showListButton->setGeometry(QRect(10, 0, 26, 20));
        showListButton->setIconSize(QSize(26, 20));
        fasterButton = new QPushButton(bottomTopwidget);
        fasterButton->setObjectName(QStringLiteral("fasterButton"));
        fasterButton->setGeometry(QRect(1060, 0, 26, 20));
        fasterButton->setMinimumSize(QSize(24, 18));
        fasterButton->setIconSize(QSize(26, 20));
        bottomBottomwidget = new QWidget(bottomWidget);
        bottomBottomwidget->setObjectName(QStringLiteral("bottomBottomwidget"));
        bottomBottomwidget->setGeometry(QRect(0, 20, 1012, 40));
        bottomBottomwidget->setStyleSheet(QStringLiteral(""));
        bottomBottomLeftWidget = new QWidget(bottomBottomwidget);
        bottomBottomLeftWidget->setObjectName(QStringLiteral("bottomBottomLeftWidget"));
        bottomBottomLeftWidget->setGeometry(QRect(1, 0, 220, 40));
        stopButton = new QPushButton(bottomBottomLeftWidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        stopButton->setGeometry(QRect(100, 7, 26, 26));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stopButton->sizePolicy().hasHeightForWidth());
        stopButton->setSizePolicy(sizePolicy);
        stopButton->setMinimumSize(QSize(26, 26));
        stopButton->setMaximumSize(QSize(26, 26));
        stopButton->setLayoutDirection(Qt::LeftToRight);
        stopButton->setIconSize(QSize(26, 26));
        nextButton = new QPushButton(bottomBottomLeftWidget);
        nextButton->setObjectName(QStringLiteral("nextButton"));
        nextButton->setGeometry(QRect(140, 7, 26, 26));
        nextButton->setIconSize(QSize(26, 26));
        previousButton = new QPushButton(bottomBottomLeftWidget);
        previousButton->setObjectName(QStringLiteral("previousButton"));
        previousButton->setGeometry(QRect(60, 7, 26, 26));
        previousButton->setIconSize(QSize(26, 26));
        playButton = new QPushButton(bottomBottomLeftWidget);
        playButton->setObjectName(QStringLiteral("playButton"));
        playButton->setGeometry(QRect(10, 4, 32, 32));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(playButton->sizePolicy().hasHeightForWidth());
        playButton->setSizePolicy(sizePolicy1);
        playButton->setMinimumSize(QSize(32, 32));
        playButton->setMaximumSize(QSize(32, 32));
        playButton->setStyleSheet(QStringLiteral(""));
        playButton->setIconSize(QSize(32, 32));
        bottomBottomCenterWidget = new QWidget(bottomBottomwidget);
        bottomBottomCenterWidget->setObjectName(QStringLiteral("bottomBottomCenterWidget"));
        bottomBottomCenterWidget->setGeometry(QRect(260, 0, 321, 40));
        bottomBottomCenterWidget->setMinimumSize(QSize(0, 34));
        timeLabel = new QLabel(bottomBottomCenterWidget);
        timeLabel->setObjectName(QStringLiteral("timeLabel"));
        timeLabel->setGeometry(QRect(5, 10, 161, 20));
        timeLabel->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        volumeButton = new QPushButton(bottomBottomCenterWidget);
        volumeButton->setObjectName(QStringLiteral("volumeButton"));
        volumeButton->setGeometry(QRect(160, 10, 21, 20));
        volumeSlider = new QSlider(bottomBottomCenterWidget);
        volumeSlider->setObjectName(QStringLiteral("volumeSlider"));
        volumeSlider->setGeometry(QRect(190, 10, 101, 20));
        volumeSlider->setStyleSheet(QLatin1String("border-color: rgb(29, 255, 63);\n"
"color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 178, 102, 255), stop:0.55 rgba(235, 148, 61, 255), stop:0.98 rgba(0, 0, 0, 255), stop:1 rgba(0, 0, 0, 0));"));
        volumeSlider->setOrientation(Qt::Horizontal);
        bottomBottomRightWidget = new QWidget(bottomBottomwidget);
        bottomBottomRightWidget->setObjectName(QStringLiteral("bottomBottomRightWidget"));
        bottomBottomRightWidget->setGeometry(QRect(761, 0, 250, 40));
        modeButton = new QPushButton(bottomBottomRightWidget);
        modeButton->setObjectName(QStringLiteral("modeButton"));
        modeButton->setGeometry(QRect(130, 7, 26, 26));
        modeButton->setIconSize(QSize(26, 26));
        listButton = new QPushButton(bottomBottomRightWidget);
        listButton->setObjectName(QStringLiteral("listButton"));
        listButton->setGeometry(QRect(210, 7, 26, 26));
        listButton->setIconSize(QSize(26, 26));
        randomButton = new QPushButton(bottomBottomRightWidget);
        randomButton->setObjectName(QStringLiteral("randomButton"));
        randomButton->setGeometry(QRect(170, 7, 26, 26));
        randomButton->setIconSize(QSize(26, 26));
        settingButton = new QPushButton(bottomBottomRightWidget);
        settingButton->setObjectName(QStringLiteral("settingButton"));
        settingButton->setGeometry(QRect(80, 7, 26, 26));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(settingButton->sizePolicy().hasHeightForWidth());
        settingButton->setSizePolicy(sizePolicy2);
        settingButton->setIconSize(QSize(26, 26));
        fullScreenButton = new QPushButton(bottomBottomRightWidget);
        fullScreenButton->setObjectName(QStringLiteral("fullScreenButton"));
        fullScreenButton->setGeometry(QRect(40, 7, 26, 26));
        fullScreenButton->setIconSize(QSize(26, 26));

        verticalLayout->addWidget(bottomWidget);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(horizontalSpacer);

        bottomWidget->raise();
        openGLWidget->raise();
        QTMediaPlayerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QTMediaPlayerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1104, 26));
        menuMedia = new QMenu(menuBar);
        menuMedia->setObjectName(QStringLiteral("menuMedia"));
        menuPlay = new QMenu(menuBar);
        menuPlay->setObjectName(QStringLiteral("menuPlay"));
        QTMediaPlayerClass->setMenuBar(menuBar);

        menuBar->addAction(menuMedia->menuAction());
        menuBar->addAction(menuPlay->menuAction());
        menuMedia->addAction(actionOpenFIle);
        menuMedia->addAction(actionOpenUrl);
        menuPlay->addAction(actionPlayPause);
        menuPlay->addAction(actionStop);

        retranslateUi(QTMediaPlayerClass);
        QObject::connect(playButton, SIGNAL(clicked()), QTMediaPlayerClass, SLOT(slotPlayOrPause()));
        QObject::connect(stopButton, SIGNAL(clicked()), QTMediaPlayerClass, SLOT(slotStop()));
        QObject::connect(progressSlider, SIGNAL(sliderReleased()), QTMediaPlayerClass, SLOT(slotSeekProgress()));
        QObject::connect(progressSlider, SIGNAL(sliderPressed()), QTMediaPlayerClass, SLOT(slotProgressPress()));
        QObject::connect(showListButton, SIGNAL(clicked()), QTMediaPlayerClass, SLOT(slotSlow()));
        QObject::connect(fasterButton, SIGNAL(clicked()), QTMediaPlayerClass, SLOT(slotFast()));
        QObject::connect(volumeSlider, SIGNAL(sliderMoved(int)), QTMediaPlayerClass, SLOT(slotVolume(int)));

        QMetaObject::connectSlotsByName(QTMediaPlayerClass);
    } // setupUi

    void retranslateUi(QMainWindow *QTMediaPlayerClass)
    {
        QTMediaPlayerClass->setWindowTitle(QApplication::translate("QTMediaPlayerClass", "QTMediaPlayer", nullptr));
        actionOpenFIle->setText(QApplication::translate("QTMediaPlayerClass", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        actionOpenUrl->setText(QApplication::translate("QTMediaPlayerClass", "\346\211\223\345\274\200\344\270\262\346\265\201", nullptr));
        actionPlayPause->setText(QApplication::translate("QTMediaPlayerClass", "\346\222\255\346\224\276", nullptr));
        actionStop->setText(QApplication::translate("QTMediaPlayerClass", "\345\201\234\346\255\242", nullptr));
        showListButton->setText(QString());
        fasterButton->setText(QString());
        stopButton->setText(QString());
        nextButton->setText(QString());
        previousButton->setText(QString());
        playButton->setText(QString());
        timeLabel->setText(QApplication::translate("QTMediaPlayerClass", "00:00:00/00:00:00", nullptr));
        volumeButton->setText(QString());
        modeButton->setText(QString());
        listButton->setText(QString());
        randomButton->setText(QString());
        settingButton->setText(QString());
        fullScreenButton->setText(QString());
        menuMedia->setTitle(QApplication::translate("QTMediaPlayerClass", "\345\252\222\344\275\223", nullptr));
        menuPlay->setTitle(QApplication::translate("QTMediaPlayerClass", "\346\222\255\346\224\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QTMediaPlayerClass: public Ui_QTMediaPlayerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTMEDIAPLAYER_H
