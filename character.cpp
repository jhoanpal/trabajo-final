#include "character.h"
#include "projectile.h"
#include <QPixmap>
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>

constexpr float GRAVITY = 2.5f;
constexpr int GROUND_Y = 300;

Character::Character(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent), m_health(500) {

    setPixmap(QPixmap("Goku/Goku_idle_0.png"));

    // Animación
    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &Character::updateAnimation);
    m_animTimer->start(100);

    // Combo
    m_comboTimer = new QTimer(this);
    m_comboTimer->setInterval(2000); // 2 segundos
    m_comboTimer->setSingleShot(true);
    connect(m_comboTimer, &QTimer::timeout, this, &Character::resetComboSlot);

    m_kiRegenTimer = new QTimer(this);
    connect(m_kiRegenTimer, &QTimer::timeout, this, [this]() {
        if (m_ki < 100) {
            m_ki = std::min(100, m_ki + 2);
            emit kiChanged(m_ki);
        }
    });
    m_kiRegenTimer->start(1000);

}

int Character::health() const {
    return m_health;
}

bool Character::blocking() const {
    return m_blocking;
}

void Character::setBlocking(bool b) {
    m_blocking = b;
    if (b) setState("Goku_block_", 2);
    else setState("Goku_idle_", 1);
}

int Character::ki() const {
    return m_ki;
}

void Character::consumeKi(int amount) {
    m_ki = std::max(0, m_ki - amount);
    emit kiChanged(m_ki);
}

void Character::jump() {
    if (m_onGround) {
        m_velocityY = -20;
        m_onGround = false;
    }
}


void Character::takeHit(int dmg) {
    if (m_blocking) return;

    m_health = std::max(0, m_health - dmg);
    emit healthChanged(m_health);

    m_comboCount++;
    emit comboUpdated(m_comboCount);
    m_comboTimer->start(); // reinicia el temporizador

    setState("Goku_hit_", 3);
}

void Character::punch() {
    setState("Goku_punch_", 3);
}

void Character::fireKi(QGraphicsScene *scene) {
    setState("Goku_ki_", 4);
    auto *p = new Projectile(true);
    p->setPos(x() + 40, y() + 20);
    p->setData(0, true); // true = jugador
    scene->addItem(p);
    connect(p, &Projectile::leftWorld, scene, [p](Projectile *pr) { delete pr; });
}

void Character::kamehameha(QGraphicsScene *scene) {
    setState("Goku_kame_", 6);
    QTimer::singleShot(600, [=]() {
        auto *p = new Projectile(true);
        p->setPixmap(QPixmap("Goku/kame_big.png"));
        p->setPos(x() + 50, y() + 10);
        p->setData(0, true);
        scene->addItem(p);
        connect(p, &Projectile::leftWorld, scene, [p](Projectile *pr) { delete pr; });
    });
}

void Character::setState(const QString &prefix, int maxFrames) {
    m_animPrefix = prefix;
    m_animMaxFrames = maxFrames;
    m_animFrame = 0;
}

void Character::updateAnimation() {
    QString path = QString("Goku/%1%2.png").arg(m_animPrefix).arg(m_animFrame);
    setPixmap(QPixmap(path));
    m_animFrame = (m_animFrame + 1) % m_animMaxFrames;
}

int Character::comboCount() const {
    return m_comboCount;
}

void Character::resetComboSlot() {
    m_comboCount = 0;
    emit comboUpdated(m_comboCount);
}

void Character::resetCombo() {
    resetComboSlot();
}

void Character::applyPhysics() {
    m_velocityY += GRAVITY;
    moveBy(0, m_velocityY);

    if (y() >= GROUND_Y) {
        setY(GROUND_Y);
        m_velocityY = 0;
        m_onGround = true;
    } else {
        m_onGround = false;
    }
}
