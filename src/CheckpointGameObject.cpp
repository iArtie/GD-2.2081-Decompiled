#include "CheckpointGameObject.h"
#include "GJBaseGameLayer.h"
#include "GameManager.h"
#include "cocos2d.h"

CheckpointGameObject* CheckpointGameObject::create() {
    CheckpointGameObject* obj = new CheckpointGameObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

std::string CheckpointGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EffectGameObject::getSaveString(layer);
    
    if (m_targetGroupID != 0) {
        result += ",51," + std::to_string(m_targetGroupID);
    }
    
    if (m_centerGroupID != 0) {
        result += ",71," + std::to_string(m_centerGroupID);
    }
    
    if (m_respawnID != 0) {
        result += ",448," + std::to_string(m_respawnID);
    }
    
    if (m_targetPlayer1) {
        result += ",138,1";
    }
    
    return result;
}

bool CheckpointGameObject::init() {
    if (!EffectGameObject::init("checkpoint_d_01_001.png")) {
        return false;
    }
    
    m_objectType = GameObjectType::Modifier;
    m_hasCustomAnimation = true;
    
    return true;
}

void CheckpointGameObject::resetCheckpoint() {
    m_checkpointActivated = false;
    updateCheckpointSpriteVisibility();
}

void CheckpointGameObject::resetObject() {
    EffectGameObject::resetObject();
    
    if (m_editorEnabled || GameManager::sharedState()->getGameVariable("0146")) {
        resetCheckpoint();
    }
}

void CheckpointGameObject::restoreObject() {
    m_activatedByPlayer1 = false;
    m_activatedByPlayer2 = false;
    resetCheckpoint();
    _realColor = cocos2d::ccc3(255, 255, 255);
}

void CheckpointGameObject::setupCustomSprites(std::string frameName) {
    m_bDontDraw = true;
    m_colorZLayerRelated = false;
    
    // Deactivated sprites (parts 1-3)
    cocos2d::CCSprite* part1 = GameObject::addCustomChild(this, "checkpoint_d_01_001.png", cocos2d::CCPoint(0, 0));
    part1->setTag(1125);
    
    cocos2d::CCSprite* part2 = GameObject::addCustomChild(this, "checkpoint_d_02_001.png", cocos2d::CCPoint(0, 0));
    part2->setTag(1126);
    
    cocos2d::CCSprite* part3 = GameObject::addCustomChild(this, "checkpoint_d_03_001.png", cocos2d::CCPoint(0, 0));
    part3->setTag(1127);
    
    // Activated sprite
    cocos2d::CCSprite* activated = GameObject::addCustomChild(this, "checkpoint_01_001.png", cocos2d::CCPoint(0, 0));
    activated->setTag(1128);
    activated->setVisible(false);
    
    // Color sprites
    GameObject::addCustomColorChild(this, "checkpoint_d_01_001.png")->m_bDontDraw = true;
    
    cocos2d::CCSprite* colorPart1 = GameObject::addInternalCustomColorChild(
        this, "checkpoint_d_01_color_001.png", cocos2d::CCPoint(0, 0)
    );
    colorPart1->setTag(1125);
    
    cocos2d::CCSprite* colorPart2 = GameObject::addInternalCustomColorChild(
        this, "checkpoint_d_02_color_001.png", cocos2d::CCPoint(0, 0)
    );
    colorPart2->setTag(1126);
    
    cocos2d::CCSprite* colorPart3 = GameObject::addInternalCustomColorChild(
        this, "checkpoint_d_03_color_001.png", cocos2d::CCPoint(0, 0)
    );
    colorPart3->setTag(1127);
    
    cocos2d::CCSprite* colorActivated = GameObject::addInternalCustomColorChild(
        this, "checkpoint_01_color_001.png", cocos2d::CCPoint(0, 0)
    );
    colorActivated->setTag(1128);
    colorActivated->setVisible(false);
}

void CheckpointGameObject::triggerActivated(float xPosition) {
    if (m_checkpointActivated) {
        return;
    }
    
    m_checkpointActivated = true;
    
    // Animate deactivated sprites
    cocos2d::CCNode* activatedNode = getChildByTag(1128);
    activatedNode->setScaleX(1.4f);
    activatedNode->setScaleY(1.1f);
    
    cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.5f, 1.0f);
    cocos2d::CCEaseElasticOut* ease = cocos2d::CCEaseElasticOut::create(scaleTo, 0.3f);
    activatedNode->runAction(ease);
    
    // Color sprites
    cocos2d::CCNode* colorNode = m_colorSprite->getChildByTag(1128);
    colorNode->setScaleX(1.4f);
    colorNode->setScaleY(1.1f);
    
    cocos2d::CCScaleTo* colorScale = cocos2d::CCScaleTo::create(0.5f, 1.0f);
    cocos2d::CCEaseElasticOut* colorEase = cocos2d::CCEaseElasticOut::create(colorScale, 0.3f);
    colorNode->runAction(colorEase);
    
    updateCheckpointSpriteVisibility();
    
    // Shine effect
    cocos2d::CCSprite* shine = cocos2d::CCSprite::createWithSpriteFrameName("checkpoint_01_shine_001.png");
    cocos2d::CCNode* target = getChildByTag(1128);
    target->addChild(shine, 1);
    
    cocos2d::CCSize targetSize = target->getContentSize();
    shine->setPosition(cocos2d::CCPoint(targetSize.width * 0.5f, targetSize.height * 0.5f));
    
    cocos2d::CCDelayTime* delay = cocos2d::CCDelayTime::create(0.1f);
    cocos2d::CCFadeOut* fadeOut = cocos2d::CCFadeOut::create(0.5f);
    cocos2d::CCCallFunc* removeFunc = cocos2d::CCCallFunc::create(shine, callfunc_selector(cocos2d::CCNode::removeFromParentAndCleanup));
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(delay, fadeOut, removeFunc, nullptr);
    shine->runAction(seq);
    
    EffectGameObject::triggerActivated(xPosition);
}

void CheckpointGameObject::updateCheckpointSpriteVisibility() {
    // Main sprites
    getChildByTag(1125)->setVisible(!m_checkpointActivated);
    getChildByTag(1126)->setVisible(!m_checkpointActivated);
    getChildByTag(1127)->setVisible(!m_checkpointActivated);
    getChildByTag(1128)->setVisible(m_checkpointActivated);
    
    // Color sprites
    m_colorSprite->getChildByTag(1125)->setVisible(!m_checkpointActivated);
    m_colorSprite->getChildByTag(1126)->setVisible(!m_checkpointActivated);
    m_colorSprite->getChildByTag(1127)->setVisible(!m_checkpointActivated);
    m_colorSprite->getChildByTag(1128)->setVisible(m_checkpointActivated);
    
    if (m_checkpointActivated) {
        setObjectColor(cocos2d::ccc3(0, 0, 0));
        m_isColorSpriteBlack = true;
    } else {
        m_isColorSpriteBlack = false;
    }
}

void CheckpointGameObject::updateSyncedAnimation(float totalTime, int frameIndex) {
    cocos2d::CCPoint center = cocos2d::CCPoint(
        getContentSize().width * 0.5f,
        getContentSize().height * 0.5f
    );
    
    if (m_checkpointActivated) {
        // Activated animation - floating
        float hover = getHoverValue(totalTime, 3.0f, 5.0f, 2.0f);
        
        cocos2d::CCNode* activatedNode = getChildByTag(1128);
        activatedNode->setPosition(center + cocos2d::CCPoint(0, hover));
        
        cocos2d::CCNode* colorActivated = m_colorSprite->getChildByTag(1128);
        colorActivated->setPosition(activatedNode->getPosition());
    } else {
        // Deactivated animation - floating with offsets
        float hover1 = getHoverValue(totalTime, 4.5f, 4.5f, 1.8f);
        float hover2 = getHoverValue(totalTime, 3.3f, 3.3f, 1.8f);
        float hover3 = getHoverValue(totalTime, 3.0f, 3.0f, 1.8f);
        
        cocos2d::CCNode* part1 = getChildByTag(1125);
        part1->setPosition(center + cocos2d::CCPoint(-1.0f, hover1));
        
        cocos2d::CCNode* part2 = getChildByTag(1126);
        part2->setPosition(center + cocos2d::CCPoint(0.5f, hover2));
        
        cocos2d::CCNode* part3 = getChildByTag(1127);
        part3->setPosition(center + cocos2d::CCPoint(0.5f, hover3 - 1.0f));
        
        // Sync color sprites with main sprites
        m_colorSprite->getChildByTag(1125)->setPosition(part1->getPosition());
        m_colorSprite->getChildByTag(1126)->setPosition(part2->getPosition());
        m_colorSprite->getChildByTag(1127)->setPosition(part3->getPosition());
    }
}

CheckpointGameObject::~CheckpointGameObject() {
    // Destructor
}