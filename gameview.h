#pragma once
#include <QSoundEffect>
#include <QEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include "character.h"
#include "piccolo.h"
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>


class GameView : public QGraphicsView {
    QSoundEffect* clickSound = nullptr;
    Q_OBJECT
public:
    explicit GameView(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

signals:
    void hitPiccolo(int dmg);

private slots:
    void piccoloShoot(Piccolo *);
    void tick();
    void gameLoop();
    void updateGokuCombo(int count);
    void updatePiccoloCombo(int count);
    void togglePause();
    void updateKiBars();





private:
    void loadLevel(int index);
    void updateHealthBars();

    QGraphicsScene     *m_scene {nullptr};
    Character          *m_goku  {nullptr};
    Piccolo            *m_piccolo {nullptr};
    QTimer             *m_clock {nullptr};
    int                 m_secondsLeft {0};

    QGraphicsRectItem  *gokuHealthBar = nullptr;
    QGraphicsRectItem  *piccoloHealthBar = nullptr;
    QGraphicsTextItem  *comboText = nullptr;
    QGraphicsTextItem  *timerText = nullptr;
    QGraphicsPixmapItem *background = nullptr;
    QGraphicsRectItem* gokuKiBar = nullptr;
    QGraphicsRectItem* piccoloKiBar = nullptr;
    QPushButton* pauseButton = nullptr;
    QWidget* pauseMenu = nullptr;
    QTimer* sprintCooldown = nullptr;
    bool paused = false;
    QWidget* mainMenu = nullptr;
    QPushButton* playButton = nullptr;
    QGraphicsPixmapItem* logoItem = nullptr;
    QGraphicsTextItem* versionText = nullptr;
     QGraphicsTextItem* levelText = nullptr;

    // Métodos
    void showMainMenu();

    void startGame();
    void showLevelBanner(const QString& levelName);


    int                 m_level = 0;

};

