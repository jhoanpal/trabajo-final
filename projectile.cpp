#include "projectile.h"
#include <QPixmap>

constexpr int WINDOW_W = 800;
constexpr int FPS = 60;

Projectile::Projectile(bool fromPlayer, QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_fromPlayer(fromPlayer) {

    // Imagen por defecto
    setPixmap(QPixmap("Goku/ki.png"));

    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &Projectile::step);
    t->start(1000 / FPS);
}

void Projectile::step() {
    const int dx = m_fromPlayer ? m_speed : -m_speed;
    moveBy(dx, 0);
    if (x() < -50 || x() > WINDOW_W + 50)
        emit leftWorld(this);
}

void Projectile::setSpeed(int speed) {
    m_speed = speed;
}

void Projectile::setImage(const QString &path) {
    setPixmap(QPixmap(path));
}

