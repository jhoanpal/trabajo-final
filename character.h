#pragma once
#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QSet>

class Character : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    explicit Character(QGraphicsItem *parent = nullptr);

    int health() const;
    bool blocking() const;
    void setBlocking(bool b);
    int ki() const;
    void consumeKi(int amount);
    void jump();
    void takeHit(int dmg);
    void punch();
    void fireKi(QGraphicsScene *scene);
    void kamehameha(QGraphicsScene *scene);
    void setState(const QString &prefix, int maxFrames);
    void updateAnimation();
    int comboCount() const;
    void resetCombo();
    void applyPhysics();

    QSet<int> m_recentKeys;
    bool m_sprinting = false;

signals:
    void healthChanged(int);
    void kiChanged(int);
    void comboUpdated(int);

private slots:
    void resetComboSlot();

private:
    int m_health;
    int m_ki;
    bool m_blocking = false;
    bool m_onGround = true;
    int m_velocityY = 0;

    int m_comboCount = 0;
    QTimer *m_comboTimer;
    QTimer *m_animTimer;
    QTimer *m_kiRegenTimer;

    QString m_animPrefix;
    int m_animFrame = 0;
    int m_animMaxFrames = 1;

    const int GRAVITY = 1;
    const int GROUND_Y = 300;
};
