#pragma once
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>

class Piccolo : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    explicit Piccolo(QGraphicsItem *parent = nullptr);
    int health() const { return m_health; }
    void setHealth(int hp) { m_health = hp; emit healthChanged(hp); }


signals:
    void shoot(Piccolo *self);
    void healthChanged(int);
    void comboUpdated(int);

public slots:
    void takeHit(int dmg);

private slots:
    void aiStep();
    void updateAnimation();
    void resetComboSlot();

private:
    void setState(const QString &prefix, int frames);

    int     m_health;
    int     m_animFrame = 0;
    QString m_animPrefix;
    int     m_animMaxFrames = 1;
    QTimer *m_animTimer = nullptr;

    // Combo
    int     m_comboCount = 0;
    QTimer *m_comboTimer = nullptr;
};

