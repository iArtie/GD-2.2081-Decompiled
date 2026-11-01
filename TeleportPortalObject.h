#pragma once

#include "cocos2d.h"
#include "RingObject.h"
#include <string>

class GJBaseGameLayer;

class TeleportPortalObject : public RingObject {
public:
    static TeleportPortalObject* create(const char* frame, bool trigger);
    bool init(const char* frame, bool trigger) override;
    int addToGroup(int id) override;
    void removeFromGroup(int id) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    float getTeleportXOff(cocos2d::CCNode* parent);
    void setPosition(const cocos2d::CCPoint* position) override;
    void setRotation(float rotation) override;
    void setStartPos(cocos2d::CCPoint position) override;
    void setStartPosOverride(cocos2d::CCPoint position);
    virtual ~TeleportPortalObject();

protected:
    TeleportPortalObject* m_orangePortal = nullptr;
    bool m_isTouchTriggered = false;
    bool m_isYellowPortal = false;
    bool m_teleportEase = false;
    int m_targetGroupID = 0;
    bool m_staticForceEnabled = false;
    double m_staticForce = 0.0;
    bool m_redirectForceEnabled = false;
    double m_redirectForceMod = 0.0;
    double m_redirectForceMin = 0.0;
    double m_redirectForceMax = 0.0;
    bool m_saveOffset = false;
    bool m_ignoreX = false;
    bool m_ignoreY = false;
    bool m_gravityMode = false;
    bool m_staticForceAdditive = false;
    bool m_instantCamera = false;
    bool m_snapGround = false;
    bool m_redirectDash = false;
};