#pragma once

#include "Platform/cocos2d.h"

class PointNode;

class HardStreak : public cocos2d::CCDrawNode {
public:
    void addPoint(cocos2d::CCPoint point);
    void clearAboveXPos(float x);
    void clearBehindXPos(float x);
    static HardStreak* create();
    HardStreak* createDuplicate();
    void firstSetup();
    bool init() override;
    double normalizeAngle(double angle);
    void reset();
    void resumeStroke();
    void scheduleAutoUpdate();
    void stopStroke();
    void updateStroke(float dt);
    virtual ~HardStreak();

private:
    cocos2d::CCArray* m_pointArray;
    bool m_drawStreak;
    bool m_isSolid;
    cocos2d::CCPoint m_currentPoint;
    bool m_isFlipped;
    float m_waveSize;
    float m_pulseSize;
};