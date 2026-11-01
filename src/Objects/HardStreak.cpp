#include "Objects/HardStreak.h"
#include "Nodes/PointNode.h"

HardStreak* HardStreak::create() {
    auto streak = new HardStreak();
    if (streak && streak->init()) {
        streak->autorelease();
        return streak;
    }
    delete streak;
    return nullptr;
}

bool HardStreak::init() {
    if (!CCDrawNode::init()) {
        return false;
    }
    
    m_pointArray = cocos2d::CCArray::create();
    m_pointArray->retain();
    m_drawStreak = false;
    m_isSolid = false;
    m_isFlipped = false;
    m_waveSize = 1.0f;
    m_pulseSize = 1.0f;
    m_currentPoint = cocos2d::CCPoint(0.0f, 0.0f);
    
    firstSetup();
    return true;
}

void HardStreak::firstSetup() {
    addPoint(cocos2d::CCPoint(0.0f, 0.0f));
    m_currentPoint = cocos2d::CCPoint(10.0f, 10.0f);
    updateStroke(0.0f);
    visit();
    reset();
}

HardStreak* HardStreak::createDuplicate() {
    auto duplicate = HardStreak::create();
    
    duplicate->setBlendFunc(getBlendFunc());
    duplicate->m_currentPoint = m_currentPoint;
    duplicate->m_waveSize = m_waveSize;
    duplicate->m_isSolid = m_isSolid;
    duplicate->m_isFlipped = m_isFlipped;
    duplicate->m_pulseSize = m_pulseSize;
    duplicate->setOpacity(getOpacity());
    duplicate->setColor(getColor());
    
    if (m_pointArray) {
        for (int i = 0; i < m_pointArray->count(); ++i) {
            auto node = m_pointArray->objectAtIndex(i);
            cocos2d::CCPoint pos = node->getPosition();
            duplicate->addPoint(pos);
        }
    }
    
    return duplicate;
}

void HardStreak::addPoint(cocos2d::CCPoint point) {
    auto node = PointNode::create(point);
    m_pointArray->addObject(node);
}

void HardStreak::clearAboveXPos(float x) {
    if (m_pointArray->count() <= 1) {
        return;
    }
    
    while (m_pointArray->count() > 1) {
        auto node = m_pointArray->objectAtIndex(1);
        cocos2d::CCPoint pos = node->getPosition();
        if (pos.x <= x) {
            break;
        }
        m_pointArray->removeObjectAtIndex(0);
    }
}

void HardStreak::clearBehindXPos(float x) {
    if (m_pointArray->count() <= 1) {
        return;
    }
    
    while (m_pointArray->count() > 1) {
        auto node = m_pointArray->objectAtIndex(1);
        cocos2d::CCPoint pos = node->getPosition();
        if (pos.x >= x) {
            break;
        }
        m_pointArray->removeObjectAtIndex(0);
    }
}

void HardStreak::reset() {
    clear();
    m_pointArray->removeAllObjects();
}

void HardStreak::resumeStroke() {
    m_drawStreak = true;
    updateStroke(0.0f);
}

void HardStreak::scheduleAutoUpdate() {
    schedule(schedule_selector(HardStreak::updateStroke), 0.0f);
}

void HardStreak::stopStroke() {
    unschedule(schedule_selector(HardStreak::updateStroke));
    m_drawStreak = false;
    reset();
}

void HardStreak::updateStroke(float dt) {
    if (!m_drawStreak || m_pointArray->count() == 0 || getOpacity() == 0) {
        return;
    }
    
    clear();
    
    int passes = m_isSolid ? 1 : 2;
    cocos2d::CCPoint origin = getPosition();
    cocos2d::CCPoint current = m_currentPoint - origin;
    
    for (int pass = 0; pass < passes; ++pass) {
        for (int i = 0; i < m_pointArray->count(); ++i) {
            auto obj1 = m_pointArray->objectAtIndex(i);
            cocos2d::CCPoint p1 = obj1->getPosition();
            
            int nextIdx = i + 1;
            cocos2d::CCPoint p2;
            if (nextIdx < m_pointArray->count()) {
                auto obj2 = m_pointArray->objectAtIndex(nextIdx);
                p2 = obj2->getPosition();
            } else {
                p2 = current;
            }
            
            if (p1.x == p2.x && p1.y == p2.y) {
                continue;
            }
            
            if (m_isFlipped) {
                std::swap(p1, p2);
            }
            
            float waveSize = (pass == 0) ? m_waveSize * 6.0f : m_waveSize * 2.0f;
            float halfWidth = waveSize * m_pulseSize;
            
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float length = sqrtf(dx * dx + dy * dy);
            
            float angle = atan2f(dy, dx);
            float perpX = -sinf(angle);
            float perpY = cosf(angle);
            
            cocos2d::CCPoint corners[4];
            
            if (pass == 1) {
                // Solid pass - draw filled shape
                corners[0] = p1 + cocos2d::CCPoint(-halfWidth * perpX, -halfWidth * perpY);
                corners[1] = p1 + cocos2d::CCPoint(halfWidth * perpX, halfWidth * perpY);
                corners[2] = p2 + cocos2d::CCPoint(halfWidth * perpX, halfWidth * perpY);
                corners[3] = p2 + cocos2d::CCPoint(-halfWidth * perpX, -halfWidth * perpY);
            } else {
                // Wavy pass
                float waveOffset = halfWidth * 0.5f;
                corners[0] = p1 + cocos2d::CCPoint(-halfWidth * perpX, -halfWidth * perpY);
                corners[1] = p1 + cocos2d::CCPoint(halfWidth * perpX, halfWidth * perpY);
                corners[2] = p2 + cocos2d::CCPoint(halfWidth * perpX, halfWidth * perpY);
                corners[3] = p2 + cocos2d::CCPoint(-halfWidth * perpX, -halfWidth * perpY);
                
                // Add wave effect
                float waveAmp = halfWidth * 0.3f;
                corners[0] += cocos2d::CCPoint(waveAmp * perpX, waveAmp * perpY);
                corners[1] -= cocos2d::CCPoint(waveAmp * perpX, waveAmp * perpY);
                corners[2] -= cocos2d::CCPoint(waveAmp * perpX, waveAmp * perpY);
                corners[3] += cocos2d::CCPoint(waveAmp * perpX, waveAmp * perpY);
            }
            
            cocos2d::ccColor4F color = getColor4F();
            color.a = (pass == 0) ? getOpacity() / 255.0f : (getOpacity() / 255.0f) * 0.65f;
            
            drawPolygon(corners, 4, color, 0.0f);
        }
    }
    
    // Restore positions
    for (int i = 0; i < m_pointArray->count(); ++i) {
        auto node = m_pointArray->objectAtIndex(i);
        cocos2d::CCPoint pos = node->getPosition();
        node->setPosition(pos + origin);
    }
    
    m_currentPoint = m_currentPoint + origin;
}

double HardStreak::normalizeAngle(double angle) {
    if (angle > 360.0) return angle - 360.0;
    if (angle < 0.0) return angle + 360.0;
    return angle;
}

HardStreak::~HardStreak() {
    if (m_pointArray) {
        m_pointArray->release();
    }
}