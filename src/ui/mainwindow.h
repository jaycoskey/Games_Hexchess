/********************************************************************************
** Form generated from reading UI file 'hexchess.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <iostream>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "fen.h"
#include "game_outcome.h"
#include "move.h"
#include "player_action.h"
#include "util.h"
#include "util_hexchess.h"

#include "boardwidget.h"


QT_BEGIN_NAMESPACE

using hexchess::NotImplementedException;
using hexchess::print;

using hexchess::core::Color;
using hexchess::core::Fen;
using hexchess::core::GameOutcome;
using hexchess::core::Index;
using hexchess::core::Move;
using hexchess::core::PlayerAction;
using hexchess::core::PlayerActionEnum;
using hexchess::core::Scope;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow()
    {
        if (objectName().isEmpty()) {
            setObjectName(QString::fromUtf8("MainWindow"));
        }
        resize(1400, 800);
        actionNewGame = new QAction();
        actionNewGame->setObjectName(QString::fromUtf8("actionNewGame"));

        actionLoadGame = new QAction{};
        actionLoadGame->setObjectName(QString::fromUtf8("actionLoadGame"));
        actionSaveGame = new QAction{};
        actionSaveGame->setObjectName(QString::fromUtf8("actionSaveGame"));
        actionSaveImage = new QAction{};
        actionSaveImage->setObjectName(QString::fromUtf8("actionSaveImage"));
        actionClose = new QAction{};
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionQuit = new QAction{};
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionPreferences = new QAction{};
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        actionCopyPgn = new QAction{};
        actionCopyPgn->setObjectName(QString::fromUtf8("actionCopyPgn"));
        actionCopyFen = new QAction{};
        actionCopyFen->setObjectName(QString::fromUtf8("actionCopyFen"));
        actionAbort = new QAction{};
        actionAbort->setObjectName(QString::fromUtf8("actionAbort"));
        // actionOfferAdjournment = new QAction{};
        // actionOfferAdjournment->setObjectName(QString::fromUtf8("actionOfferAdjournment"));
        actionOfferDraw = new QAction{};
        actionOfferDraw->setObjectName(QString::fromUtf8("actionOfferDraw"));
        // actionRequestResignation = new QAction{};
        // actionRequestResignation->setObjectName(QString::fromUtf8("actionRequestResignation"));
        actionPause = new QAction{};
        actionPause->setObjectName(QString::fromUtf8("actionPause"));
        actionResume = new QAction{};
        actionResume->setObjectName(QString::fromUtf8("actionResume"));
        actionUndo = new QAction{};
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        actionResign = new QAction{};
        actionResign->setObjectName(QString::fromUtf8("actionResign"));
        actionHowToPlay = new QAction{};
        actionHowToPlay->setObjectName(QString::fromUtf8("actionHowToPlay"));
        actionAbout = new QAction{};
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));

        mainWidget = new QWidget{};
        mainWidget->setObjectName(QString::fromUtf8("mainWidget"));

        verticalLayout_2 = new QVBoxLayout(mainWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout->setContentsMargins(15, 15, 15, 15);

        headerGroupBox = new QGroupBox(mainWidget);
        headerGroupBox->setObjectName(QString::fromUtf8("headerGroupBox"));

        verticalLayout->addWidget(headerGroupBox);

        horizontalLayout = new QHBoxLayout{};
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(15, 15, 15, 15);

        boardWidget = new BoardWidget{};
        boardWidget->setObjectName(QString::fromUtf8("boardWidget"));

        horizontalLayout->addWidget(boardWidget);

        sideViewWidget = new QTabWidget{mainWidget};
        sideViewWidget->setObjectName(QString::fromUtf8("sideViewWidget"));

        movesTab = new QWidget();
        movesTab->setObjectName(QString::fromUtf8("movesTab"));
        movesTable = new QTableView(movesTab);
        movesTable->setObjectName(QString::fromUtf8("movesTable"));
        movesTable->setGeometry(QRect(20, 20, 451, 641));
        movesTable->setAlternatingRowColors(true);
        sideViewWidget->addTab(movesTab, QString());
        rulesTab = new QWidget();
        rulesTab->setObjectName(QString::fromUtf8("rulesTab"));
        rulesWidget = new QGraphicsView(rulesTab);
        rulesWidget->setObjectName(QString::fromUtf8("rulesWidget"));
        rulesWidget->setGeometry(QRect(20, 10, 451, 641));
        sideViewWidget->addTab(rulesTab, QString());

        horizontalLayout->addWidget(sideViewWidget);


        verticalLayout->addLayout(horizontalLayout);

        footerGroupBox = new QGroupBox(mainWidget);
        footerGroupBox->setObjectName(QString::fromUtf8("footerGroupBox"));

        verticalLayout->addWidget(footerGroupBox);

        verticalLayout_2->addLayout(verticalLayout);

        settingsDialog = new QWidget(mainWidget);
        settingsDialog->setObjectName(QString::fromUtf8("settingsDialog"));
        settingsOkButton = new QDialogButtonBox(settingsDialog);
        settingsOkButton->setObjectName(QString::fromUtf8("settingsOkButton"));
        settingsOkButton->setGeometry(QRect(20, 0, 166, 25));
        settingsOkButton->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(settingsDialog);

        setCentralWidget(mainWidget);
        menubar = new QMenuBar{this};
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1062, 22));
        menuHexchess = new QMenu(menubar);
        menuHexchess->setObjectName(QString::fromUtf8("menuHexchess"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuActions = new QMenu(menubar);
        menuActions->setObjectName(QString::fromUtf8("menuActions"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));

        setMenuBar(menubar);
        statusbar = new QStatusBar{this};
        statusbar->setObjectName(QString::fromUtf8("statusbar"));

        setStatusBar(statusbar);

        menubar->addAction(menuHexchess->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuActions->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuHexchess->addAction(actionNewGame);
        menuHexchess->addAction(actionLoadGame);
        menuHexchess->addAction(actionSaveGame);
        menuHexchess->addAction(actionSaveImage);
        menuHexchess->addSeparator();
        menuHexchess->addAction(actionClose);
        menuHexchess->addAction(actionQuit);
        menuEdit->addAction(actionCopyPgn);
        menuEdit->addAction(actionCopyFen);
        menuEdit->addSeparator();
        menuEdit->addAction(actionPreferences);
        menuActions->addAction(actionUndo);
        menuActions->addSeparator();
        menuActions->addAction(actionOfferDraw);
        // menuActions->addAction(actionRequestResignation);
        // menuActions->addAction(actionOfferAdjournment);
        menuActions->addSeparator();
        menuActions->addAction(actionResign);
        menuActions->addSeparator();
        menuActions->addAction(actionPause);
        menuActions->addAction(actionResume);
        menuHelp->addAction(actionHowToPlay);
        menuHelp->addAction(actionAbout);

        retranslateUi();

        sideViewWidget->setCurrentIndex(0);

        QMetaObject::connectSlotsByName(this);
    }

    QAction *actionNewGame;
    QAction *actionLoadGame;
    QAction *actionSaveGame;
    QAction *actionSaveImage;
    QAction *actionClose;
    QAction *actionQuit;
    QAction *actionPreferences;
    QAction *actionCopyPgn;
    QAction *actionCopyFen;
    QAction *actionAbort;
    QAction *actionOfferAdjournment;
    QAction *actionOfferDraw;
    QAction *actionOfferResign;
    QAction *actionPause;
    QAction *actionResume;
    QAction *actionUndo;
    QAction *actionResign;
    QAction *actionHowToPlay;
    QAction *actionAbout;
    QWidget *mainWidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QGroupBox *headerGroupBox;
    QHBoxLayout *horizontalLayout;

    BoardWidget *boardWidget;

    QTabWidget *sideViewWidget;
    QWidget *movesTab;
    QTableView *movesTable;
    QWidget *rulesTab;
    QGraphicsView *rulesWidget;
    QGroupBox *footerGroupBox;
    QWidget *settingsDialog;
    QDialogButtonBox *settingsOkButton;
    QMenuBar *menubar;
    QMenu *menuHexchess;
    QMenu *menuEdit;
    QMenu *menuActions;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void retranslateUi()
    {
        setWindowTitle(QApplication::translate("MainWindow", "Hexagonal Chess", nullptr));
        actionNewGame->setText(QApplication::translate("MainWindow", "New Game", nullptr));
        actionLoadGame->setText(QApplication::translate("MainWindow", "Load Game (PGN)", nullptr));
        actionSaveGame->setText(QApplication::translate("MainWindow", "Save Game (PGN)", nullptr));
        actionSaveImage->setText(QApplication::translate("MainWindow", "Save Board Image", nullptr));
        actionClose->setText(QApplication::translate("MainWindow", "Close", nullptr));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        actionPreferences->setText(QApplication::translate("MainWindow", "Preferences", nullptr));
        actionCopyPgn->setText(QApplication::translate("MainWindow", "Copy PGN", nullptr));
        actionCopyFen->setText(QApplication::translate("MainWindow", "Copy FEN", nullptr));
        actionAbort->setText(QApplication::translate("MainWindow", "Abort", nullptr));
        // actionOfferAdjournment->setText(QApplication::translate("MainWindow", "Offer Adjournment", nullptr));
        actionOfferDraw->setText(QApplication::translate("MainWindow", "Offer Draw", nullptr));
        // actionRequestResignation->setText(QApplication::translate("MainWindow", "Offer Resignation", nullptr));
        actionPause->setText(QApplication::translate("MainWindow", "Pause", nullptr));
        actionResume->setText(QApplication::translate("MainWindow", "Resume", nullptr));
        actionUndo->setText(QApplication::translate("MainWindow", "Undo", nullptr));
        actionResign->setText(QApplication::translate("MainWindow", "Resign", nullptr));
        actionHowToPlay->setText(QApplication::translate("MainWindow", "How To Play", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        headerGroupBox->setTitle(QApplication::translate("MainWindow", "GroupBox", nullptr));
#ifndef QT_NO_ACCESSIBILITY
        boardWidget->setAccessibleName(QApplication::translate("MainWindow", "Board Widget", nullptr));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sideViewWidget->setAccessibleName(QApplication::translate("MainWindow", "Tab Widget", nullptr));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        movesTable->setAccessibleName(QApplication::translate("MainWindow", "Moves Table", nullptr));
#endif // QT_NO_ACCESSIBILITY
        sideViewWidget->setTabText(sideViewWidget->indexOf(movesTab), QApplication::translate("MainWindow", "Moves", nullptr));
#ifndef QT_NO_ACCESSIBILITY
        rulesWidget->setAccessibleName(QApplication::translate("MainWindow", "Rules Widget", nullptr));
#endif // QT_NO_ACCESSIBILITY
        sideViewWidget->setTabText(sideViewWidget->indexOf(rulesTab), QApplication::translate("MainWindow", "Rules", nullptr));
        footerGroupBox->setTitle(QApplication::translate("MainWindow", "GroupBox", nullptr));
        menuHexchess->setTitle(QApplication::translate("MainWindow", "Hexagonal Chess", nullptr));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", nullptr));
        menuActions->setTitle(QApplication::translate("MainWindow", "Actions", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
    }

public slots:
    void receiveBoardInitializationFromPlayer(const Fen<Glinski>& fen) {
        Scope scope{"MainWindow::receiveBoardInitializationFromPlayer", true};
        boardWidget->initializeBoard(fen);
    }

    void receiveActionRequestFromPlayer(Color mover) {
        Scope scope{"MainWindow::displayActionRequest"};
        // throw NotImplementedException{"MainWindow::displayActionRequest"};
    }

    void receiveActionFromPlayer(Color mover, const PlayerAction& action) {
        if (action.playerActionEnum() == PlayerActionEnum::Move) {
            boardWidget->execMove(action.move());
        }
    }

    void receiveCheckFromPlayer(Color checked, Index kingInd) {
        std::cout << "MainWindow::displayCheck: Entering\n";
        throw NotImplementedException{"MainWindow::displayCheck"};
    }

    void receiveGameOutcomeFromPlayer(Color reader, const GameOutcome& gameOutcome) {
        std::cout << "MainWindow::displayGameOutcome: Entering\n";
        // throw NotImplementedException{"MainWindow::displayGameOutcome"};
    }

signals:
    void sendActionToPlayer(Color mover, const PlayerAction& action);
};

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
