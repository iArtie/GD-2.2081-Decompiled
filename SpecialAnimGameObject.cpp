#include "SpecialAnimGameObject.h"
#include "GJBaseGameLayer.h"

SpecialAnimGameObject* SpecialAnimGameObject::create(const char* frame) {
    SpecialAnimGameObject* returnVal = new SpecialAnimGameObject();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

std::string SpecialAnimGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EnhancedGameObject::getSaveString(layer);
    if (m_disableAnimShine) {
        saveString += ",";
        saveString += std::to_string(127);
        saveString += ",";
        saveString += std::to_string(m_disableAnimShine);
    }
    return saveString;
}

bool SpecialAnimGameObject::init(const char* frame) {
    if (!EnhancedGameObject::init(frame))
        return false;
    m_bUnkBool2 = false;
    return true;
}

void SpecialAnimGameObject::resetObject() {
    m_skipMainColorUpdate = false;
    m_skipSecondaryColorUpdate = false;
    EnhancedGameObject::resetObject();
}

void SpecialAnimGameObject::updateMainColor(const cocos2d::ccColor3B* color) {
    if (!m_skipMainColorUpdate)
        GameObject::updateMainColor(color);
}

void SpecialAnimGameObject::updateSecondaryColor(const cocos2d::ccColor3B* color) {
    if (!m_skipSecondaryColorUpdate)
        GameObject::updateSecondaryColor(color);
}

void SpecialAnimGameObject::updateSyncedAnimation(float totalTime, int frameIndex) {
    EnhancedGameObject::updateSyncedAnimation(totalTime, -1);

    if (m_disableAnimShine)
        return;

    if (m_objectID == 2047) {
        if (m_maybeAnimationVariableXInt == 1) {
            m_skipMainColorUpdate = false;
            m_skipSecondaryColorUpdate = false;
            updateMainColor(&cocos2d::ccc3(255, 255, 255));
            updateSecondaryColor(&cocos2d::ccc3(255, 255, 255));
            m_skipMainColorUpdate = true;
            m_skipSecondaryColorUpdate = true;
            return;
        }
        m_skipMainColorUpdate = false;
        m_skipSecondaryColorUpdate = false;
        return;
    }

    if (m_objectID == 2055) {
        if (m_maybeAnimationVariableXInt == 1) {
            m_skipMainColorUpdate = false;
            updateMainColor(&cocos2d::ccc3(255, 255, 255));
            m_skipMainColorUpdate = true;
        } else {
            m_skipMainColorUpdate = false;
        }
        return;
    }

    if (m_objectID == 2046) {
        if (m_maybeAnimationVariableXInt > 3 || m_maybeAnimationVariableXInt == 1) {
            m_skipSecondaryColorUpdate = false;
            updateSecondaryColor(&cocos2d::ccc3(255, 255, 255));
            m_skipSecondaryColorUpdate = true;
            return;
        }
        m_skipSecondaryColorUpdate = false;
        return;
    }
}

SpecialAnimGameObject::~SpecialAnimGameObject() {
}