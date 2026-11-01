#include "KeyframeGameObject.h"
#include "GJBaseGameLayer.h"
#include "EditorUI.h"
#include "EffectGameObject.h"

KeyframeGameObject* KeyframeGameObject::create() {
    auto obj = new KeyframeGameObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool KeyframeGameObject::init() {
    if (!EffectGameObject::init("keyframeIcon_001.png")) {
        return false;
    }
    
    m_shadowObjects = cocos2d::CCArray::create();
    m_shadowObjects->retain();
    m_previewSprite = nullptr;
    m_previewArt = false;
    m_referenceOnly = false;
    m_targetGroupID = 0;
    m_keyframeGroup = 0;
    m_keyframeIndex = 0;
    m_easingType = 0;
    m_easingRate = 0.0f;
    m_duration = 0.0f;
    m_centerGroupID = 0;
    m_proximity = 0;
    m_spawnDelay = 0.0f;
    m_curve = 0;
    m_timeMode = 0;
    m_closeLoop = 0;
    m_autoLayer = 0;
    m_direction = 0;
    m_revolutions = 0;
    m_lineOpacity = 0.0f;
    m_width = 0.0f;
    m_height = 0.0f;
    m_customBoxOffset = cocos2d::CCPoint(0.0f, 0.0f);
    m_updateCustomContentSize = false;
    m_isObjectRectDirty = false;
    
    return true;
}

void KeyframeGameObject::setOpacity(unsigned char opacity) {
    unsigned char finalOpacity = m_referenceOnly ? (opacity * 0.5f) : opacity;
    EffectGameObject::setOpacity(finalOpacity);
    
    if (m_shadowObjects) {
        for (int i = 0; i < m_shadowObjects->count(); ++i) {
            auto obj = m_shadowObjects->objectAtIndex(i);
            obj->setOpacity(finalOpacity);
        }
    }
}

void KeyframeGameObject::updateShadowObjects(GJBaseGameLayer* layer, EditorUI* ui) {
    if (m_previewSprite) {
        m_previewSprite->removeMeAndCleanup();
        m_previewSprite = nullptr;
    }
    
    m_shadowObjects->removeAllObjects();
    
    if (!m_previewArt) {
        m_bDontDraw = false;
        return;
    }
    
    m_bDontDraw = true;
    
    auto group = layer->getGroup(m_targetGroupID);
    auto groupParent = layer->getGroupParent(m_targetGroupID);
    
    if (group && group->count() > 0 && groupParent) {
        ui->copyObjects(group, false, false);
        
        std::string objectString = ui->getObjectString();
        m_previewSprite = ui->spriteFromObjectString(
            objectString,
            false,
            true,
            false,
            m_shadowObjects,
            group,
            groupParent
        );
        
        addChild(m_previewSprite);
        setContentSize(cocos2d::CCSize(0.0f, 0.0f));
        m_updateCustomContentSize = true;
        
        m_width = m_previewSprite->getContentSize().width;
        m_height = m_previewSprite->getContentSize().height;
        m_previewSprite->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
        
        cocos2d::CCPoint groupCenter = ui->getGroupCenter(group, true);
        cocos2d::CCPoint parentPos = groupParent->getPosition();
        m_customBoxOffset = groupCenter - parentPos;
        m_isObjectRectDirty = true;
        
        setOpacity(getOpacity());
    }
}

std::string KeyframeGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EffectGameObject::getSaveString(layer);
    
    char buffer[512];
    int pos = 0;
    
    auto appendInt = [&](int value, int key) {
        if (value) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i,%i", key, value);
        }
    };
    
    auto appendFloat = [&](float value, int key) {
        if (value != 0.0f) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i,%f", key, value);
        }
    };
    
    appendInt(m_targetGroupID, 51);
    appendInt(m_keyframeGroup, 373);
    appendInt(m_keyframeIndex, 374);
    appendInt(m_referenceOnly, 375);
    appendInt(m_easingType, 30);
    appendFloat(m_easingRate, 85);
    appendFloat(m_duration, 10);
    appendInt(m_centerGroupID, 71);
    appendInt(m_proximity, 377);
    appendFloat(m_spawnDelay, 557);
    appendInt(m_curve, 378);
    appendInt(m_timeMode, 379);
    appendInt(m_closeLoop, 376);
    appendInt(m_previewArt, 380);
    appendInt(m_autoLayer, 459);
    appendInt(m_direction, 536);
    appendInt(m_revolutions, 537);
    appendFloat(m_lineOpacity, 524);
    
    result += buffer;
    
    return result;
}

KeyframeGameObject::~KeyframeGameObject() {
    if (m_shadowObjects) {
        m_shadowObjects->release();
    }
}