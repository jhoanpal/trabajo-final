#pragma once
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>

class Projectile : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    explicit Projectile(bool fromPlayer, QGraphicsItem *parent = nullptr);

    void setSpeed(int speed);
    void setImage(const QString &path);

signals:
    void leftWorld(Projectile *p);

private slots:
    void step();

private:
    bool m_fromPlayer;
    int m_speed = 12;
};
