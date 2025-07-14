#include "piccolo.h"
#include <QPixmap>

Piccolo::Piccolo(QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_health(500) {

    m_animPrefix = "piccolo_idle_";
    m_animMaxFrames = 1;
    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &Piccolo::updateAnimation);
    m_animTimer->start(150);
    setPixmap(QPixmap("piccolo/piccolo_idle_0.png"));

    QTimer *ai = new QTimer(this);
    connect(ai, &QTimer::timeout, this, &Piccolo::aiStep);
    ai->start(750);
}

void Piccolo::takeHit(int dmg) {
    m_health = std::max(0, m_health - dmg);
    emit healthChanged(m_health);
    setState("piccolo_hit_", 3);
}

void Piccolo::resetComboSlot() {
    m_comboCount = 0;
    emit comboUpdated(m_comboCount);
}


void Piccolo::aiStep() {
    setState("piccolo_attack_", 3);
    QTimer::singleShot(300, this, [=]() {
        emit shoot(this);
        setState("piccolo_idle_", 4);
    });
}

void Piccolo::updateAnimation() {
    QString path = QString("piccolo/%1%2.png").arg(m_animPrefix).arg(m_animFrame);
    setPixmap(QPixmap(path));
    m_animFrame = (m_animFrame + 1) % m_animMaxFrames;
}

void Piccolo::setState(const QString &prefix, int frames) {
    m_animPrefix = prefix;
    m_animMaxFrames = frames;
    m_animFrame = 0;
}

