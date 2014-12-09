/********************************************************************************
** Form generated from reading UI file 'aboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_aboutDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label_4;
    QSpacerItem *verticalSpacer;
    QPushButton *close;

    void setupUi(QDialog *aboutDialog)
    {
        if (aboutDialog->objectName().isEmpty())
            aboutDialog->setObjectName(QStringLiteral("aboutDialog"));
        aboutDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(aboutDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(aboutDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setTextFormat(Qt::AutoText);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        label_3 = new QLabel(aboutDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_3);

        label_2 = new QLabel(aboutDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setTextFormat(Qt::AutoText);
        label_2->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        label_2->setWordWrap(true);

        verticalLayout->addWidget(label_2);

        label_4 = new QLabel(aboutDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignCenter);
        label_4->setWordWrap(true);

        verticalLayout->addWidget(label_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        close = new QPushButton(aboutDialog);
        close->setObjectName(QStringLiteral("close"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(close->sizePolicy().hasHeightForWidth());
        close->setSizePolicy(sizePolicy);
        close->setMaximumSize(QSize(100, 16777215));
        close->setCursor(QCursor(Qt::PointingHandCursor));
        close->setMouseTracking(false);
        close->setLayoutDirection(Qt::LeftToRight);

        verticalLayout->addWidget(close, 0, Qt::AlignHCenter);


        retranslateUi(aboutDialog);

        QMetaObject::connectSlotsByName(aboutDialog);
    } // setupUi

    void retranslateUi(QDialog *aboutDialog)
    {
        aboutDialog->setWindowTitle(QApplication::translate("aboutDialog", "About", 0));
        label->setText(QApplication::translate("aboutDialog", "<b>Technical University of Ko\305\241ice</b>", 0));
        label_3->setText(QApplication::translate("aboutDialog", "Name: UWB - coordinate generator\n"
"Year: 2014/2015\n"
"Version: 1.0\n"
"Author: Peter Mikula\n"
"", 0));
        label_2->setText(QApplication::translate("aboutDialog", "This is simple program for generating [x, y] coordinates for targets moving in specific area monitored by UWB sensors. Program allows to draw trajectories and place as many radar sensors as needed with possibility of graphic simulation. Coordinates will be automatically transformated into system of coordinates of individual sensors. Finally generation of *.txt files with all calculated values is implemented.", 0));
        label_4->setText(QApplication::translate("aboutDialog", "This program was created as a part of bachelor thesis by Peter Mikula in Qt Framework 5.2.1 (MSVC 2010, 32 bit).\n"
"\n"
"All source codes are free to be modified.", 0));
        close->setText(QApplication::translate("aboutDialog", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class aboutDialog: public Ui_aboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
