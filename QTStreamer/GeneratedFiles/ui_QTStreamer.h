/********************************************************************************
** Form generated from reading UI file 'QTStreamer.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTSTREAMER_H
#define UI_QTSTREAMER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include "qadvancedwidget.h"
#include "qgeneralwidget.h"
#include "qvideolistwidget.h"

QT_BEGIN_NAMESPACE

class Ui_QTStreamerClass
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_6;
    QLabel *appLabel;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *pushButton;
    QToolButton *toolButton_4;
    QToolButton *toolButton_5;
    QSpacerItem *horizontalSpacer_7;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_5;
    QToolButton *startOrPauseButton;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *stopRecordButton;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_2;
    QLabel *timeLabel;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QToolButton *micVolumeButton;
    QToolButton *systemVolumeButton;
    QSpacerItem *horizontalSpacer;
    QLabel *label_5;
    QStackedWidget *stackedWidget;
    QGeneralWidget *generalPage;
    QVideoListWidget *videoListPage;
    QWidget *advancedPage;
    QScrollArea *scrollArea;
    QAdvancedWidget *scrollAreaWidgetContents;
    QToolButton *generaButton;
    QToolButton *advancedButton;
    QToolButton *listButton;

    void setupUi(QWidget *QTStreamerClass)
    {
        if (QTStreamerClass->objectName().isEmpty())
            QTStreamerClass->setObjectName(QStringLiteral("QTStreamerClass"));
        QTStreamerClass->resize(890, 540);
        QTStreamerClass->setMaximumSize(QSize(890, 540));
        horizontalLayoutWidget = new QWidget(QTStreamerClass);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(-1, 0, 891, 61));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(4);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_6 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);

        appLabel = new QLabel(horizontalLayoutWidget);
        appLabel->setObjectName(QStringLiteral("appLabel"));

        horizontalLayout->addWidget(appLabel);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_8);

        pushButton = new QPushButton(horizontalLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMaximumSize(QSize(80, 40));

        horizontalLayout->addWidget(pushButton);

        toolButton_4 = new QToolButton(horizontalLayoutWidget);
        toolButton_4->setObjectName(QStringLiteral("toolButton_4"));

        horizontalLayout->addWidget(toolButton_4);

        toolButton_5 = new QToolButton(horizontalLayoutWidget);
        toolButton_5->setObjectName(QStringLiteral("toolButton_5"));

        horizontalLayout->addWidget(toolButton_5);

        horizontalSpacer_7 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);

        horizontalLayoutWidget_2 = new QWidget(QTStreamerClass);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(0, 439, 891, 101));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_5 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_5);

        startOrPauseButton = new QToolButton(horizontalLayoutWidget_2);
        startOrPauseButton->setObjectName(QStringLiteral("startOrPauseButton"));
        startOrPauseButton->setMinimumSize(QSize(54, 54));

        horizontalLayout_2->addWidget(startOrPauseButton);

        horizontalSpacer_3 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        stopRecordButton = new QToolButton(horizontalLayoutWidget_2);
        stopRecordButton->setObjectName(QStringLiteral("stopRecordButton"));
        stopRecordButton->setMinimumSize(QSize(48, 48));

        horizontalLayout_2->addWidget(stopRecordButton);

        horizontalSpacer_4 = new QSpacerItem(50, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        label_2 = new QLabel(horizontalLayoutWidget_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMaximumSize(QSize(50, 40));

        horizontalLayout_2->addWidget(label_2);

        timeLabel = new QLabel(horizontalLayoutWidget_2);
        timeLabel->setObjectName(QStringLiteral("timeLabel"));
        timeLabel->setMinimumSize(QSize(120, 0));
        timeLabel->setMaximumSize(QSize(300, 40));

        horizontalLayout_2->addWidget(timeLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        label_4 = new QLabel(horizontalLayoutWidget_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMaximumSize(QSize(50, 40));

        horizontalLayout_2->addWidget(label_4);

        micVolumeButton = new QToolButton(horizontalLayoutWidget_2);
        micVolumeButton->setObjectName(QStringLiteral("micVolumeButton"));
        micVolumeButton->setMaximumSize(QSize(34, 34));

        horizontalLayout_2->addWidget(micVolumeButton);

        systemVolumeButton = new QToolButton(horizontalLayoutWidget_2);
        systemVolumeButton->setObjectName(QStringLiteral("systemVolumeButton"));
        systemVolumeButton->setMaximumSize(QSize(34, 34));

        horizontalLayout_2->addWidget(systemVolumeButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label_5 = new QLabel(QTStreamerClass);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(0, 0, 900, 64));
        label_5->setStyleSheet(QStringLiteral("background-color: rgb(0, 159, 255);"));
        stackedWidget = new QStackedWidget(QTStreamerClass);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setGeometry(QRect(110, 70, 760, 340));
        generalPage = new QGeneralWidget();
        generalPage->setObjectName(QStringLiteral("generalPage"));
        stackedWidget->addWidget(generalPage);
        videoListPage = new QVideoListWidget();
        videoListPage->setObjectName(QStringLiteral("videoListPage"));
        stackedWidget->addWidget(videoListPage);
        advancedPage = new QWidget();
        advancedPage->setObjectName(QStringLiteral("advancedPage"));
        scrollArea = new QScrollArea(advancedPage);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(0, 0, 761, 331));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QAdvancedWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, -125, 750, 480));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        scrollAreaWidgetContents->setMinimumSize(QSize(750, 480));
        scrollArea->setWidget(scrollAreaWidgetContents);
        stackedWidget->addWidget(advancedPage);
        generaButton = new QToolButton(QTStreamerClass);
        generaButton->setObjectName(QStringLiteral("generaButton"));
        generaButton->setGeometry(QRect(5, 71, 95, 67));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(generaButton->sizePolicy().hasHeightForWidth());
        generaButton->setSizePolicy(sizePolicy1);
        generaButton->setMinimumSize(QSize(0, 0));
        generaButton->setMaximumSize(QSize(100, 67));
        advancedButton = new QToolButton(QTStreamerClass);
        advancedButton->setObjectName(QStringLiteral("advancedButton"));
        advancedButton->setGeometry(QRect(5, 213, 95, 67));
        sizePolicy.setHeightForWidth(advancedButton->sizePolicy().hasHeightForWidth());
        advancedButton->setSizePolicy(sizePolicy);
        advancedButton->setMinimumSize(QSize(90, 67));
        advancedButton->setMaximumSize(QSize(100, 67));
        listButton = new QToolButton(QTStreamerClass);
        listButton->setObjectName(QStringLiteral("listButton"));
        listButton->setGeometry(QRect(5, 142, 95, 67));
        sizePolicy1.setHeightForWidth(listButton->sizePolicy().hasHeightForWidth());
        listButton->setSizePolicy(sizePolicy1);
        listButton->setMinimumSize(QSize(0, 0));
        listButton->setMaximumSize(QSize(100, 67));
        label_5->raise();
        horizontalLayoutWidget->raise();
        horizontalLayoutWidget_2->raise();
        stackedWidget->raise();

        retranslateUi(QTStreamerClass);
        QObject::connect(generaButton, SIGNAL(clicked()), QTStreamerClass, SLOT(slotGeneralButton()));
        QObject::connect(listButton, SIGNAL(clicked()), QTStreamerClass, SLOT(slotVideoListButton()));
        QObject::connect(advancedButton, SIGNAL(clicked()), QTStreamerClass, SLOT(slotAdvanceButton()));

        QMetaObject::connectSlotsByName(QTStreamerClass);
    } // setupUi

    void retranslateUi(QWidget *QTStreamerClass)
    {
        QTStreamerClass->setWindowTitle(QApplication::translate("QTStreamerClass", "QTStreamer", nullptr));
        appLabel->setText(QApplication::translate("QTStreamerClass", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600; color:#ff0000;\">\345\275\225\345\261\217\350\275\257\344\273\266</span></p></body></html>", nullptr));
        pushButton->setText(QApplication::translate("QTStreamerClass", "\350\256\276\347\275\256", nullptr));
        toolButton_4->setText(QApplication::translate("QTStreamerClass", "\346\234\200\345\260\217\345\214\226", nullptr));
        toolButton_5->setText(QApplication::translate("QTStreamerClass", "\351\200\200\345\207\272", nullptr));
        startOrPauseButton->setText(QApplication::translate("QTStreamerClass", "\345\274\200\345\247\213", nullptr));
        stopRecordButton->setText(QApplication::translate("QTStreamerClass", "\345\201\234\346\255\242", nullptr));
        label_2->setText(QApplication::translate("QTStreamerClass", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">\346\227\266\351\225\277\357\274\232</span></p></body></html>", nullptr));
        timeLabel->setText(QApplication::translate("QTStreamerClass", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600; color:#5555ff;\">00:00:00</span></p></body></html>", nullptr));
        label_4->setText(QApplication::translate("QTStreamerClass", "<html><head/><body><p><span style=\" font-size:10pt;\">\345\243\260\351\237\263:</span></p></body></html>", nullptr));
        micVolumeButton->setText(QApplication::translate("QTStreamerClass", "\351\272\246", nullptr));
        systemVolumeButton->setText(QApplication::translate("QTStreamerClass", "\347\263\273\347\273\237", nullptr));
        label_5->setText(QString());
        generaButton->setText(QApplication::translate("QTStreamerClass", "\345\270\270\350\247\204", nullptr));
        advancedButton->setText(QApplication::translate("QTStreamerClass", "\351\253\230\347\272\247", nullptr));
        listButton->setText(QApplication::translate("QTStreamerClass", "\345\210\227\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QTStreamerClass: public Ui_QTStreamerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTSTREAMER_H
