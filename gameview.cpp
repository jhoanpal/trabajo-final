#include "gameview.h"
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>
#include <QBrush>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QSoundEffect>
#include "projectile.h"

constexpr int WINDOW_W = 800;
constexpr int WINDOW_H = 480;
constexpr int FPS = 60;
constexpr int LEVEL_TIME = 90;

GameView::GameView(QWidget *parent)
    : QGraphicsView(parent) {
    setFixedSize(WINDOW_W, WINDOW_H);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scene = new QGraphicsScene(0, 0, WINDOW_W, WINDOW_H, this);
    setScene(m_scene);

    showMainMenu();

    clickSound = new QSoundEffect(this);
    clickSound->setSource(QUrl::fromLocalFile("sounds/play.mp3"));
    clickSound->setVolume(0.5);

    m_secondsLeft = LEVEL_TIME;
    m_clock = new QTimer(this);
    connect(m_clock, &QTimer::timeout, this, &GameView::tick);

    QTimer *loop = new QTimer(this);
    connect(loop, &QTimer::timeout, this, &GameView::gameLoop);
    loop->start(1000 / FPS);
}

void GameView::showMainMenu() {
    m_scene->clear();

    background = new QGraphicsPixmapItem();
    background->setPixmap(QPixmap("maps/menu_background.png").scaled(WINDOW_W, WINDOW_H));
    m_scene->addItem(background);

    logoItem = new QGraphicsPixmapItem(QPixmap("logo_game.png").scaled(360, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoItem->setPos((WINDOW_W - 500) / 2, 60);
    m_scene->addItem(logoItem);

    versionText = m_scene->addText("versión: 1.0");
    versionText->setDefaultTextColor(Qt::cyan);
    versionText->setFont(QFont("Arial", 10, QFont::Bold));
    versionText->setPos(WINDOW_W - 180, WINDOW_H - 30);

    playButton = new QPushButton("Play", this);
    playButton->setGeometry((WINDOW_W - 100) / 2, 250, 100, 40);
    playButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #1c1c1c; color: white; font-weight: bold; font-size: 16px;"
        "  border: 2px solid white; border-radius: 5px; }"
        "QPushButton:hover {"
        "  background-color: #333333; font-size: 18px; }"
        );
    connect(playButton, &QPushButton::clicked, this, &GameView::startGame);
    playButton->show();
}

void GameView::startGame() {
    if (clickSound) clickSound->play();
    if (playButton) playButton->hide();
    if (logoItem) m_scene->removeItem(logoItem);
    if (versionText) m_scene->removeItem(versionText);

    levelText = m_scene->addText("Nivel 1");
    levelText->setFont(QFont("Arial", 28, QFont::Bold));
    levelText->setDefaultTextColor(Qt::white);
    levelText->setPos((WINDOW_W - levelText->boundingRect().width()) / 2, 180);

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect;
    levelText->setGraphicsEffect(effect);

    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(2000);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->start(QPropertyAnimation::DeleteWhenStopped);

    connect(anim, &QPropertyAnimation::finished, this, [=]() {
        m_scene->removeItem(levelText);
        loadLevel(0);
    });
}

void GameView::loadLevel(int index) {
    m_scene->clear();
    m_level = index;
    m_secondsLeft = LEVEL_TIME;

    background = new QGraphicsPixmapItem(QPixmap(index == 0 ? "maps/level1.png" : "maps/level2.png").scaled(WINDOW_W, WINDOW_H));
    m_scene->addItem(background);

    gokuKiBar = m_scene->addRect(10, 30, 200, 8, Qt::NoPen, Qt::blue);
    piccoloKiBar = m_scene->addRect(580, 30, 200, 8, Qt::NoPen, Qt::cyan);

    m_goku = new Character();
    m_goku->setPixmap(QPixmap("sprites/goku_idle_0.png"));
    m_goku->setPos(50, 300);
    m_scene->addItem(m_goku);

    m_piccolo = new Piccolo();
    m_piccolo->setPixmap(QPixmap("sprites/piccolo_idle_0.png"));
    m_piccolo->setPos(600, 300);
    m_scene->addItem(m_piccolo);

    connect(m_goku, &Character::kiChanged, this, &GameView::updateKiBars);
    connect(m_piccolo, &Piccolo::shoot, this, &GameView::piccoloShoot);
    connect(m_goku, &Character::healthChanged, this, &GameView::updateHealthBars);
    connect(m_piccolo, &Piccolo::healthChanged, this, &GameView::updateHealthBars);
    connect(m_goku, &Character::comboUpdated, this, &GameView::updateGokuCombo);
    connect(m_piccolo, &Piccolo::comboUpdated, this, &GameView::updatePiccoloCombo);

    gokuHealthBar = m_scene->addRect(10, 10, 200, 15, Qt::NoPen, Qt::green);
    piccoloHealthBar = m_scene->addRect(580, 10, 200, 15, Qt::NoPen, Qt::red);

    comboText = m_scene->addText("Combo: 0");
    comboText->setPos(330, 10);
    comboText->setDefaultTextColor(Qt::yellow);
    comboText->setFont(QFont("Arial", 14, QFont::Bold));

    timerText = m_scene->addText(QString::number(m_secondsLeft));
    timerText->setPos(390, 30);
    timerText->setDefaultTextColor(Qt::white);
    timerText->setFont(QFont("Arial", 12));

    pauseButton = new QPushButton("Pause", this);
    pauseButton->setGeometry(700, 10, 80, 30);
    connect(pauseButton, &QPushButton::clicked, this, &GameView::togglePause);
    pauseButton->show();

    m_clock->start(1000);
}

void GameView::tick() {
    m_secondsLeft--;
    timerText->setPlainText(QString::number(m_secondsLeft));

    if (m_goku && m_goku->health() <= 0) {
        m_clock->stop();
        QGraphicsTextItem *lostText = m_scene->addText("\u00a1Perdiste el juego!");
        lostText->setFont(QFont("Arial", 28, QFont::Bold));
        lostText->setDefaultTextColor(Qt::red);
        lostText->setPos((WINDOW_W - lostText->boundingRect().width()) / 2, 180);

        QTimer::singleShot(4000, this, [=]() {
            showMainMenu();
        });
        return;
    }

    if (m_secondsLeft <= 0) {
        m_clock->stop();

        QGraphicsTextItem *transitionText = m_scene->addText("\u00a1Nivel completado!");
        transitionText->setFont(QFont("Arial", 24, QFont::Bold));
        transitionText->setDefaultTextColor(Qt::green);
        transitionText->setPos((WINDOW_W - transitionText->boundingRect().width()) / 2, 180);

        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect;
        transitionText->setGraphicsEffect(effect);

        QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(3000);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        anim->start(QPropertyAnimation::DeleteWhenStopped);

        connect(anim, &QPropertyAnimation::finished, this, [=]() {
            m_scene->removeItem(transitionText);
            m_level = (m_level + 1) % 2;
            loadLevel(m_level);
        });
    }
}


void GameView::gameLoop() {
    if (paused) return;
    for (auto item : m_scene->items()) {
        auto *proj = dynamic_cast<Projectile *>(item);
        if (!proj) continue;

        if (proj->collidesWithItem(m_piccolo) && proj->data(0).toBool()) {
            m_piccolo->takeHit(20);
            proj->deleteLater();
        }
        if (proj->collidesWithItem(m_goku) && !proj->data(0).toBool()) {
            m_goku->takeHit(10);
            proj->deleteLater();
        }
    }
    if (m_goku) m_goku->applyPhysics();
}

void GameView::piccoloShoot(Piccolo *) {
    auto *p = new Projectile(false);
    p->setPos(m_piccolo->x() - 20, m_piccolo->y() + 20);
    m_scene->addItem(p);
    connect(p, &Projectile::leftWorld, m_scene, [p](Projectile *pr) { delete pr; });
}

void GameView::keyPressEvent(QKeyEvent *e) {
    if (!m_goku) return;

    if (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right) {
        if (m_goku->m_recentKeys.contains(e->key())) m_goku->m_sprinting = true;
        int key = e->key();
        m_goku->m_recentKeys.insert(key);
        QTimer::singleShot(250, this, [=]() {
            m_goku->m_recentKeys.remove(key);
        });
    }

    switch (e->key()) {
    case Qt::Key_Left:
        m_goku->moveBy(-8, 0);
        m_goku->setState("Goku_run_", 6);
        break;
    case Qt::Key_Right:
        m_goku->moveBy(8, 0);
        m_goku->setState("Goku_run_", 6);
        break;
    case Qt::Key_Z:
        m_goku->punch();
        break;
    case Qt::Key_X:
        m_goku->fireKi(m_scene);
        break;
    case Qt::Key_C:
        m_goku->kamehameha(m_scene);
        break;
    case Qt::Key_Down:
        m_goku->setBlocking(true);
        break;
    }
}

void GameView::keyReleaseEvent(QKeyEvent *e) {
    if (!m_goku) return;
    if (e->key() == Qt::Key_Down)
        m_goku->setBlocking(false);
}

void GameView::togglePause() {
    paused = !paused;
    if (paused) {
        m_clock->stop();
        pauseMenu = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(pauseMenu);
        QPushButton* resume = new QPushButton("Resume");
        QPushButton* restart = new QPushButton("Restart");
        QPushButton* quit = new QPushButton("Quit");
        layout->addWidget(resume);
        layout->addWidget(restart);
        layout->addWidget(quit);
        pauseMenu->setLayout(layout);
        pauseMenu->setGeometry(250, 100, 300, 200);
        pauseMenu->show();
        connect(resume, &QPushButton::clicked, [=]() {
            pauseMenu->hide(); paused = false; m_clock->start(1000);
        });
        connect(restart, &QPushButton::clicked, [=]() {
            pauseMenu->hide(); loadLevel(m_level);
        });
        connect(quit, &QPushButton::clicked, [=]() {
            QApplication::quit();
        });
    } else {
        if (pauseMenu) pauseMenu->hide();
        m_clock->start(1000);
    }
}

void GameView::updateHealthBars() {
    if (m_goku)
        gokuHealthBar->setRect(10, 10, 200.0 * m_goku->health() / 500.0, 15);
    if (m_piccolo)
        piccoloHealthBar->setRect(580, 10, 200.0 * m_piccolo->health() / 500.0, 15);
    if (m_goku)
        gokuKiBar->setRect(10, 30, 200.0 * m_goku->ki() / 100.0, 8);
}

void GameView::updateKiBars() {
    if (m_goku && gokuKiBar)
        gokuKiBar->setRect(10, 30, 200.0 * m_goku->ki() / 100.0, 8);
    if (m_piccolo && piccoloKiBar)
        piccoloKiBar->setRect(580, 30, 200.0, 8); // Placeholder for Piccolo's KI if needed
}

void GameView::updateGokuCombo(int count) {
    comboText->setPlainText(QString("Combo Goku: %1").arg(count));
}

void GameView::updatePiccoloCombo(int count) {
    comboText->setPlainText(QString("Combo Piccolo: %1").arg(count));
}
