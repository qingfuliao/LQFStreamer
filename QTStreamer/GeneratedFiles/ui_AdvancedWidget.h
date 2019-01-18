/********************************************************************************
** Form generated from reading UI file 'AdvancedWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADVANCEDWIDGET_H
#define UI_ADVANCEDWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AdvancedForm
{
public:
    QGroupBox *groupBox;
    QLabel *label;
    QPushButton *pushButton;
    QLabel *label_2;
    QLabel *label_3;
    QSlider *horizontalSlider;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QSlider *horizontalSlider_2;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QGroupBox *groupBox_2;
    QSplitter *splitter;
    QToolButton *toolButton;
    QToolButton *toolButton_2;
    QToolButton *toolButton_4;
    QToolButton *toolButton_3;
    QToolButton *toolButton_5;
    QToolButton *toolButton_6;
    QToolButton *toolButton_7;
    QGroupBox *groupBox_3;
    QSplitter *splitter_2;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_4;
    QCheckBox *checkBox_5;

    void setupUi(QWidget *AdvancedForm)
    {
        if (AdvancedForm->objectName().isEmpty())
            AdvancedForm->setObjectName(QStringLiteral("AdvancedForm"));
        AdvancedForm->resize(738, 474);
        AdvancedForm->setMinimumSize(QSize(738, 474));
        AdvancedForm->setMaximumSize(QSize(738, 474));
        groupBox = new QGroupBox(AdvancedForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 720, 140));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 30, 81, 16));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(100, 30, 93, 28));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 60, 81, 16));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 90, 81, 16));
        horizontalSlider = new QSlider(groupBox);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(90, 60, 541, 22));
        horizontalSlider->setOrientation(Qt::Horizontal);
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(650, 60, 70, 19));
        checkBox_2 = new QCheckBox(groupBox);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
        checkBox_2->setGeometry(QRect(650, 90, 70, 19));
        horizontalSlider_2 = new QSlider(groupBox);
        horizontalSlider_2->setObjectName(QStringLiteral("horizontalSlider_2"));
        horizontalSlider_2->setGeometry(QRect(90, 90, 541, 22));
        horizontalSlider_2->setOrientation(Qt::Horizontal);
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(80, 120, 51, 16));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(600, 120, 51, 16));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(350, 110, 20, 20));
        groupBox_2 = new QGroupBox(AdvancedForm);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 170, 720, 110));
        splitter = new QSplitter(groupBox_2);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setGeometry(QRect(20, 30, 671, 71));
        splitter->setOrientation(Qt::Horizontal);
        toolButton = new QToolButton(splitter);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolButton->sizePolicy().hasHeightForWidth());
        toolButton->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton);
        toolButton_2 = new QToolButton(splitter);
        toolButton_2->setObjectName(QStringLiteral("toolButton_2"));
        sizePolicy.setHeightForWidth(toolButton_2->sizePolicy().hasHeightForWidth());
        toolButton_2->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton_2);
        toolButton_4 = new QToolButton(splitter);
        toolButton_4->setObjectName(QStringLiteral("toolButton_4"));
        sizePolicy.setHeightForWidth(toolButton_4->sizePolicy().hasHeightForWidth());
        toolButton_4->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton_4);
        toolButton_3 = new QToolButton(splitter);
        toolButton_3->setObjectName(QStringLiteral("toolButton_3"));
        sizePolicy.setHeightForWidth(toolButton_3->sizePolicy().hasHeightForWidth());
        toolButton_3->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton_3);
        toolButton_5 = new QToolButton(splitter);
        toolButton_5->setObjectName(QStringLiteral("toolButton_5"));
        sizePolicy.setHeightForWidth(toolButton_5->sizePolicy().hasHeightForWidth());
        toolButton_5->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton_5);
        toolButton_6 = new QToolButton(splitter);
        toolButton_6->setObjectName(QStringLiteral("toolButton_6"));
        sizePolicy.setHeightForWidth(toolButton_6->sizePolicy().hasHeightForWidth());
        toolButton_6->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton_6);
        toolButton_7 = new QToolButton(splitter);
        toolButton_7->setObjectName(QStringLiteral("toolButton_7"));
        sizePolicy.setHeightForWidth(toolButton_7->sizePolicy().hasHeightForWidth());
        toolButton_7->setSizePolicy(sizePolicy);
        splitter->addWidget(toolButton_7);
        groupBox_3 = new QGroupBox(AdvancedForm);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 310, 720, 151));
        splitter_2 = new QSplitter(groupBox_3);
        splitter_2->setObjectName(QStringLiteral("splitter_2"));
        splitter_2->setGeometry(QRect(20, 30, 417, 101));
        splitter_2->setOrientation(Qt::Vertical);
        checkBox_3 = new QCheckBox(splitter_2);
        checkBox_3->setObjectName(QStringLiteral("checkBox_3"));
        splitter_2->addWidget(checkBox_3);
        checkBox_4 = new QCheckBox(splitter_2);
        checkBox_4->setObjectName(QStringLiteral("checkBox_4"));
        splitter_2->addWidget(checkBox_4);
        checkBox_5 = new QCheckBox(splitter_2);
        checkBox_5->setObjectName(QStringLiteral("checkBox_5"));
        splitter_2->addWidget(checkBox_5);

        retranslateUi(AdvancedForm);

        QMetaObject::connectSlotsByName(AdvancedForm);
    } // setupUi

    void retranslateUi(QWidget *AdvancedForm)
    {
        AdvancedForm->setWindowTitle(QApplication::translate("AdvancedForm", "Form", nullptr));
        groupBox->setTitle(QApplication::translate("AdvancedForm", "\345\243\260\351\237\263\344\274\230\345\214\226", nullptr));
        label->setText(QApplication::translate("AdvancedForm", "\351\272\246\345\205\213\351\243\216\350\256\276\347\275\256", nullptr));
        pushButton->setText(QApplication::translate("AdvancedForm", "\350\256\276\347\275\256", nullptr));
        label_2->setText(QApplication::translate("AdvancedForm", "\351\237\263\351\242\221\351\231\215\345\231\252", nullptr));
        label_3->setText(QApplication::translate("AdvancedForm", "\351\237\263\351\242\221\345\212\240\345\274\272", nullptr));
        checkBox->setText(QApplication::translate("AdvancedForm", "\345\274\200\345\220\257", nullptr));
        checkBox_2->setText(QApplication::translate("AdvancedForm", "\345\274\200\345\220\257", nullptr));
        label_4->setText(QApplication::translate("AdvancedForm", "\345\207\217\345\274\261", nullptr));
        label_5->setText(QApplication::translate("AdvancedForm", "\345\212\240\345\274\272", nullptr));
        label_6->setText(QApplication::translate("AdvancedForm", "0", nullptr));
        groupBox_2->setTitle(QApplication::translate("AdvancedForm", "\344\270\223\344\272\253\345\212\237\350\203\275", nullptr));
        toolButton->setText(QApplication::translate("AdvancedForm", "\350\247\206\351\242\221\345\212\240\351\200\237", nullptr));
        toolButton_2->setText(QApplication::translate("AdvancedForm", "\350\247\206\351\242\221\344\277\256\345\244\215", nullptr));
        toolButton_4->setText(QApplication::translate("AdvancedForm", "\351\253\230\346\270\205\350\275\254\347\240\201", nullptr));
        toolButton_3->setText(QApplication::translate("AdvancedForm", "\345\210\266\344\275\234gif\345\212\250\347\224\273", nullptr));
        toolButton_5->setText(QApplication::translate("AdvancedForm", "\344\270\262\346\265\201\345\255\230\347\233\230", nullptr));
        toolButton_6->setText(QApplication::translate("AdvancedForm", "\347\233\264\346\222\255\350\247\206\351\242\221", nullptr));
        toolButton_7->setText(QApplication::translate("AdvancedForm", "\347\241\254\344\273\266\351\207\207\351\233\206", nullptr));
        groupBox_3->setTitle(QApplication::translate("AdvancedForm", "\345\212\237\350\203\275\350\247\243\351\224\201", nullptr));
        checkBox_3->setText(QApplication::translate("AdvancedForm", "\346\234\254\345\234\260\347\233\264\346\222\255\350\247\202\347\234\213\347\224\250\346\210\267\344\270\212\351\231\220\346\217\220\345\215\207\350\207\26315\344\272\272", nullptr));
        checkBox_4->setText(QApplication::translate("AdvancedForm", "\346\221\204\345\203\217\345\244\264\346\241\214\351\235\242\346\202\254\346\265\256\345\275\225\345\210\266", nullptr));
        checkBox_5->setText(QApplication::translate("AdvancedForm", "\346\226\207\345\255\227\346\260\264\345\215\260\357\274\214\345\233\276\347\211\207\346\260\264\345\215\260\357\274\214\346\221\204\345\203\217\345\244\264\345\265\214\345\205\245\344\275\277\347\224\250\350\256\276\347\275\256\345\217\257\344\277\235\345\255\230\346\234\254\345\234\260\351\205\215\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdvancedForm: public Ui_AdvancedForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADVANCEDWIDGET_H
