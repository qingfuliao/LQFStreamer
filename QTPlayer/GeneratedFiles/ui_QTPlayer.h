/********************************************************************************
** Form generated from reading UI file 'QTPlayer.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTPLAYER_H
#define UI_QTPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTPlayerClass
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QTPlayerClass)
    {
        if (QTPlayerClass->objectName().isEmpty())
            QTPlayerClass->setObjectName(QStringLiteral("QTPlayerClass"));
        QTPlayerClass->resize(906, 496);
        centralWidget = new QWidget(QTPlayerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(70, 270, 93, 28));
        QTPlayerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QTPlayerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 906, 26));
        QTPlayerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QTPlayerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QTPlayerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QTPlayerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QTPlayerClass->setStatusBar(statusBar);

        retranslateUi(QTPlayerClass);
        QObject::connect(pushButton, SIGNAL(clicked()), QTPlayerClass, SLOT(slotPlay()));

        QMetaObject::connectSlotsByName(QTPlayerClass);
    } // setupUi

    void retranslateUi(QMainWindow *QTPlayerClass)
    {
        QTPlayerClass->setWindowTitle(QApplication::translate("QTPlayerClass", "QTPlayer", nullptr));
        pushButton->setText(QApplication::translate("QTPlayerClass", "play", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QTPlayerClass: public Ui_QTPlayerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTPLAYER_H
