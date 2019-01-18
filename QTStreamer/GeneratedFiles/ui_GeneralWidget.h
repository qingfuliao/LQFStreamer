/********************************************************************************
** Form generated from reading UI file 'GeneralWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERALWIDGET_H
#define UI_GENERALWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeneralForm
{
public:
    QWidget *widget;
    QPushButton *pushButton_3;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QSplitter *splitter;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_4;
    QGroupBox *groupBox_2;
    QSplitter *splitter_2;
    QRadioButton *radioButton_7;
    QRadioButton *radioButton_8;
    QRadioButton *radioButton_6;
    QRadioButton *radioButton_5;
    QGroupBox *groupBox_3;
    QSplitter *splitter_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;

    void setupUi(QWidget *GeneralForm)
    {
        if (GeneralForm->objectName().isEmpty())
            GeneralForm->setObjectName(QStringLiteral("GeneralForm"));
        GeneralForm->resize(773, 334);
        GeneralForm->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        widget = new QWidget(GeneralForm);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(370, 0, 400, 300));
        pushButton_3 = new QPushButton(GeneralForm);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(670, 300, 80, 30));
        layoutWidget = new QWidget(GeneralForm);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 381, 321));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMaximumSize(QSize(16777215, 36));

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMaximumSize(QSize(16777215, 36));

        horizontalLayout->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        groupBox = new QGroupBox(layoutWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 180));
        splitter = new QSplitter(groupBox);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(30, 20, 130, 130));
        splitter->setOrientation(Qt::Vertical);
        radioButton = new QRadioButton(splitter);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        splitter->addWidget(radioButton);
        radioButton_2 = new QRadioButton(splitter);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        splitter->addWidget(radioButton_2);
        radioButton_3 = new QRadioButton(splitter);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));
        splitter->addWidget(radioButton_3);
        radioButton_4 = new QRadioButton(splitter);
        radioButton_4->setObjectName(QStringLiteral("radioButton_4"));
        splitter->addWidget(radioButton_4);

        horizontalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(layoutWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        splitter_2 = new QSplitter(groupBox_2);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setGeometry(QRect(30, 20, 130, 130));
        splitter_2->setOrientation(Qt::Vertical);
        radioButton_7 = new QRadioButton(splitter_2);
        radioButton_7->setObjectName(QStringLiteral("radioButton_7"));
        splitter_2->addWidget(radioButton_7);
        radioButton_8 = new QRadioButton(splitter_2);
        radioButton_8->setObjectName(QStringLiteral("radioButton_8"));
        splitter_2->addWidget(radioButton_8);
        radioButton_6 = new QRadioButton(splitter_2);
        radioButton_6->setObjectName(QStringLiteral("radioButton_6"));
        splitter_2->addWidget(radioButton_6);
        radioButton_5 = new QRadioButton(splitter_2);
        radioButton_5->setObjectName(QStringLiteral("radioButton_5"));
        splitter_2->addWidget(radioButton_5);

        horizontalLayout_2->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        groupBox_3 = new QGroupBox(layoutWidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMaximumSize(QSize(16777215, 80));
        splitter_3 = new QSplitter(groupBox_3);
        splitter_3->setObjectName(QStringLiteral("splitter_3"));
        splitter_3->setGeometry(QRect(5, 20, 370, 28));
        splitter_3->setOrientation(Qt::Horizontal);
        pushButton_4 = new QPushButton(splitter_3);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        splitter_3->addWidget(pushButton_4);
        pushButton_5 = new QPushButton(splitter_3);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        splitter_3->addWidget(pushButton_5);
        pushButton_6 = new QPushButton(splitter_3);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        splitter_3->addWidget(pushButton_6);
        pushButton_7 = new QPushButton(splitter_3);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));
        splitter_3->addWidget(pushButton_7);

        verticalLayout->addWidget(groupBox_3);


        retranslateUi(GeneralForm);

        QMetaObject::connectSlotsByName(GeneralForm);
    } // setupUi

    void retranslateUi(QWidget *GeneralForm)
    {
        GeneralForm->setWindowTitle(QString());
        pushButton_3->setText(QApplication::translate("GeneralForm", "\347\274\226\350\276\221\345\234\272\346\231\257", nullptr));
        pushButton->setText(QApplication::translate("GeneralForm", "\346\234\254\345\234\260\345\275\225\345\210\266", nullptr));
        pushButton_2->setText(QApplication::translate("GeneralForm", "\345\234\250\347\272\277\346\222\255\346\224\276", nullptr));
        groupBox->setTitle(QApplication::translate("GeneralForm", "\350\247\206\351\242\221", nullptr));
        radioButton->setText(QApplication::translate("GeneralForm", "\345\205\250\345\261\217\345\275\225\345\210\266", nullptr));
        radioButton_2->setText(QApplication::translate("GeneralForm", "\351\200\211\345\214\272\345\275\225\345\210\266", nullptr));
        radioButton_3->setText(QApplication::translate("GeneralForm", "\346\221\204\345\203\217\345\244\264\345\275\225\345\210\266", nullptr));
        radioButton_4->setText(QApplication::translate("GeneralForm", "\344\270\215\345\275\225\350\247\206\351\242\221", nullptr));
        groupBox_2->setTitle(QApplication::translate("GeneralForm", "\351\237\263\351\242\221", nullptr));
        radioButton_7->setText(QApplication::translate("GeneralForm", "\344\273\205\351\272\246\345\205\213\351\243\216", nullptr));
        radioButton_8->setText(QApplication::translate("GeneralForm", "\344\273\205\347\263\273\347\273\237\345\243\260\351\237\263", nullptr));
        radioButton_6->setText(QApplication::translate("GeneralForm", "\351\272\246\345\222\214\347\263\273\347\273\237\345\243\260\351\237\263", nullptr));
        radioButton_5->setText(QApplication::translate("GeneralForm", "\344\270\215\345\275\225\351\237\263\351\242\221", nullptr));
        groupBox_3->setTitle(QApplication::translate("GeneralForm", "\350\276\205\345\212\251\345\267\245\345\205\267", nullptr));
        pushButton_4->setText(QApplication::translate("GeneralForm", "\345\233\276\347\211\207\346\260\264\345\215\260", nullptr));
        pushButton_5->setText(QApplication::translate("GeneralForm", "\346\226\207\345\255\227\346\260\264\345\215\260", nullptr));
        pushButton_6->setText(QApplication::translate("GeneralForm", "\345\265\214\345\205\245\346\221\204\345\203\217\345\244\264", nullptr));
        pushButton_7->setText(QApplication::translate("GeneralForm", "\346\234\254\345\234\260\347\233\264\346\222\255", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GeneralForm: public Ui_GeneralForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERALWIDGET_H
