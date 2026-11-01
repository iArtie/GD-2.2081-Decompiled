#include "TeleportPortalObject.h"
#include "GJBaseGameLayer.h"
#include "GameManager.h"

int TeleportPortalObject::addToGroup(int id) {
    if (!GameManager::sharedState()->m_editorEnabled || m_objectID != 749) {
        int result = GameObject::addToGroup(id);
        if (m_orangePortal)
            m_orangePortal->addToGroup(id);
        return result;
    }
    return 0;
}

TeleportPortalObject* TeleportPortalObject::create(const char* frame, bool trigger) {
    TeleportPortalObject* returnVal = new TeleportPortalObject();
    if (returnVal->init(frame, trigger)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

std::string TeleportPortalObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);

    if (m_orangePortal) {
        saveString += ",";
        saveString += std::to_string(54);
        saveString += ",";
        saveString += std::to_string(m_orangePortal->getStartPos().y - getStartPos().y);
    }
    if (m_teleportEase) {
        saveString += ",";
        saveString += std::to_string(55);
        saveString += ",";
        saveString += std::to_string(m_teleportEase);
    }
    if (m_targetGroupID) {
        saveString += ",";
        saveString += std::to_string(51);
        saveString += ",";
        saveString += std::to_string(m_targetGroupID);
    }
    if (m_staticForceEnabled) {
        saveString += ",";
        saveString += std::to_string(345);
        saveString += ",";
        saveString += std::to_string(m_staticForceEnabled);
    }
    if (m_staticForce != 0.0) {
        saveString += ",";
        saveString += std::to_string(346);
        saveString += ",";
        saveString += std::to_string(m_staticForce);
    }
    if (m_redirectForceEnabled) {
        saveString += ",";
        saveString += std::to_string(347);
        saveString += ",";
        saveString += std::to_string(m_redirectForceEnabled);
    }
    if (m_redirectForceMod != 0.0) {
        saveString += ",";
        saveString += std::to_string(350);
        saveString += ",";
        saveString += std::to_string(m_redirectForceMod);
    }
    if (m_redirectForceMin != 0.0) {
        saveString += ",";
        saveString += std::to_string(348);
        saveString += ",";
        saveString += std::to_string(m_redirectForceMin);
    }
    if (m_redirectForceMax != 0.0) {
        saveString += ",";
        saveString += std::to_string(349);
        saveString += ",";
        saveString += std::to_string(m_redirectForceMax);
    }
    if (m_saveOffset) {
        saveString += ",";
        saveString += std::to_string(351);
        saveString += ",";
        saveString += std::to_string(m_saveOffset);
    }
    if (m_ignoreX) {
        saveString += ",";
        saveString += std::to_string(352);
        saveString += ",";
        saveString += std::to_string(m_ignoreX);
    }
    if (m_ignoreY) {
        saveString += ",";
        saveString += std::to_string(353);
        saveString += ",";
        saveString += std::to_string(m_ignoreY);
    }
    if (m_gravityMode) {
        saveString += ",";
        saveString += std::to_string(354);
        saveString += ",";
        saveString += std::to_string(m_gravityMode);
    }
    if (m_staticForceAdditive) {
        saveString += ",";
        saveString += std::to_string(443);
        saveString += ",";
        saveString += std::to_string(m_staticForceAdditive);
    }
    if (m_instantCamera) {
        saveString += ",";
        saveString += std::to_string(464);
        saveString += ",";
        saveString += std::to_string(m_instantCamera);
    }
    if (m_snapGround) {
        saveString += ",";
        saveString += std::to_string(510);
        saveString += ",";
        saveString += std::to_string(m_snapGround);
    }
    if (m_redirectDash) {
        saveString += ",";
        saveString += std::to_string(591);
        saveString += ",";
        saveString += std::to_string(m_redirectDash);
    }
    return saveString;
}

float TeleportPortalObject::getTeleportXOff(cocos2d::CCNode* parent) {
    cocos2d::CCNode* node = parent;
    if (!parent) {
        node = getParent();
        if (!node)
            return -10.0f;
    }
    cocos2d::CCPoint point3(-10.0f, 0.0f);
    cocos2d::CCNode::convertToWorldSpace(&point3);
    cocos2d::CCPoint point = cocos2d::CCNode::convertToNodeSpace(node, &point3);
    cocos2d::CCNode::convertToWorldSpace(&cocos2d::CCPoint(0.0f, 0.0f));
    cocos2d::CCPoint point2 = cocos2d::CCNode::convertToNodeSpace(node, &point3);
    return -fabsf(point.x - point2.x);
}

bool TeleportPortalObject::init(const char* frame, bool trigger) {
    if (!EffectGameObject::init(frame))
        return false;
    if (!trigger)
        m_isTouchTriggered = true;
    return true;
}

void TeleportPortalObject::removeFromGroup(int id) {
    if (!GameManager::sharedState()->m_editorEnabled || m_objectID != 749) {
        GameObject::removeFromGroup(id);
        if (m_orangePortal)
            m_orangePortal->removeFromGroup(id);
    }
}

void TeleportPortalObject::setPosition(const cocos2d::CCPoint* position) {
    cocos2d::CCPoint point(*position);
    if (m_isYellowPortal)
        point.x = getPosition().x;
    GameObject::setPosition(&point);
    if (m_orangePortal) {
        float teleportXOff = getTeleportXOff(nullptr);
        cocos2d::CCPoint newPos(point.x + teleportXOff, m_orangePortal->getPosition().y);
        m_orangePortal->setPosition(&newPos);
    }
}

void TeleportPortalObject::setRotation(float rotation) {
    if (!GameManager::sharedState()->m_editorEnabled || m_objectID != 749) {
        GameObject::setRotation(rotation);
        if (m_orangePortal)
            m_orangePortal->setRotation(rotation);
    }
}

void TeleportPortalObject::setStartPos(cocos2d::CCPoint position) {
    cocos2d::CCPoint point(position);
    if (m_isYellowPortal)
        point.x = getStartPos().x;
    GameObject::setStartPos(point);
    if (m_orangePortal) {
        float teleportXOff = getTeleportXOff(nullptr);
        cocos2d::CCPoint newPos(point.x + teleportXOff, m_orangePortal->getPosition().y);
        m_orangePortal->setStartPosOverride(newPos);
    }
}

void TeleportPortalObject::setStartPosOverride(cocos2d::CCPoint position) {
    GameObject::setStartPos(position);
}

TeleportPortalObject::~TeleportPortalObject() {
}