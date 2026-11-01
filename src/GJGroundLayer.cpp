// GJGroundLayer.cpp
#include "GJGroundLayer.h"
#include "GameManager.h"

GJGroundLayer* GJGroundLayer::create(int groundID, int lineType) {
    GJGroundLayer* instance = new GJGroundLayer();
    if (instance && instance->init(groundID, lineType)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void GJGroundLayer::createLine(int lineType) {
    if (m_lineSprite) {
        int oldType = m_lineType;
        m_lineSprite->removeFromParent();
        m_lineSprite = nullptr;
        lineType = oldType;
    }
    
    m_lineType = lineType;
    if (lineType > 3) {
        m_lineType = 3;
    } else if (lineType <= 0) {
        lineType = 1;
        m_lineType = 1;
    }
    
    float yPos = (m_lineType == 1) ? 0.5f : 0.2f;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    int frameID = (m_lineType == 1) ? 1 : 2;
    std::string frameName = cocos2d::CCString::createWithFormat("floorLine_%02d_001.png", frameID)->getCString();
    
    m_lineSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
    addChild(m_lineSprite, 5);
    m_lineSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, yPos));
    m_lineSprite->setAnchorPoint(cocos2d::CCPoint(0.5f, 1.0f));
    m_lineSprite->setTag(2);
    
    if (m_blendLine) {
        m_lineSprite->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
    }
    
    if (m_lineType > 1) {
        m_lineSprite->setScaleX((winSize.width + 10.0f) / m_lineSprite->getContentSize().width);
        if (m_lineType == 3) {
            m_lineSprite->setScaleY(2.0f);
        }
    }
}

void GJGroundLayer::deactivateGround() {
    stopAllActions();
    m_showGround = false;
}

void GJGroundLayer::draw() {
    // Empty implementation
}

void GJGroundLayer::fadeInFinished() {
    m_showGround = true;
}

void GJGroundLayer::fadeInGround(float duration) {
    stopActionByTag(3);
    cocos2d::CCAction* action = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(duration),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJGroundLayer::fadeInFinished)),
        nullptr
    );
    action->setTag(3);
    runAction(action);
}

void GJGroundLayer::fadeOutGround(float duration) {
    m_showGround = false;
}

float GJGroundLayer::getGroundY() {
    return 0.0f;
}

void GJGroundLayer::hideShadows() {
    cocos2d::CCNode* node1 = getChildByTag(0);
    cocos2d::CCNode* node2 = getChildByTag(1);
    if (node1) node1->setVisible(false);
    if (node2) node2->setVisible(false);
}

bool GJGroundLayer::init(int groundID, int lineType) {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }
    
    setContentSize(cocos2d::CCSizeZero);
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    GameManager* manager = GameManager::sharedState();
    std::string groundTexture = manager->getGTexture(groundID);
    m_blendLine = true;
    
    createLine(lineType);
    
    // Shadow sprites
    cocos2d::CCSprite* shadow1 = cocos2d::CCSprite::createWithSpriteFrameName("groundSquareShadow_001.png");
    shadow1->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    shadow1->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() - 1.0f, 0.0f));
    addChild(shadow1, 6);
    shadow1->setTag(0);
    
    cocos2d::CCSprite* shadow2 = cocos2d::CCSprite::createWithSpriteFrameName("groundSquareShadow_001.png");
    shadow2->setAnchorPoint(cocos2d::CCPoint(1.0f, 1.0f));
    shadow2->setPosition(cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() + 1.0f, 0.0f));
    addChild(shadow2, 6);
    shadow2->setFlipX(true);
    shadow2->setTag(1);
    
    shadow1->setOpacity(100);
    shadow2->setOpacity(100);
    shadow1->setScaleX(0.7f);
    shadow2->setScaleX(0.7f);
    shadow1->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
    shadow2->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
    
    m_showGround = false;
    
    cocos2d::ccColor3B color = cocos2d::ccc3(0, 102, 255);
    
    // Ground 1
    cocos2d::CCSpriteBatchNode* batch1 = cocos2d::CCSpriteBatchNode::create(groundTexture, 5);
    addChild(batch1, 2);
    
    float textureHeight = batch1->getTexture()->getContentSize().height;
    float textureWidth = batch1->getTexture()->getContentSize().width;
    m_textureWidth = textureWidth;
    m_ground1Offset = 128.0f - textureHeight;
    
    int count = static_cast<int>(ceilf(winSize.width / textureWidth) + 1.0f);
    
    m_ground1Sprite = cocos2d::CCSprite::createWithTexture(batch1->getTexture());
    m_ground1Sprite->setDontDraw(true);
    m_ground1Sprite->setPosition(cocos2d::CCPointZero);
    m_ground1Sprite->setColor(color);
    batch1->addChild(m_ground1Sprite);
    batch1->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
    
    loadGroundSprites(count, true);
    updateGround01Color(color);
    
    // Ground 2 (for groundID > 7)
    if (groundID > 7) {
        std::string frameName = cocos2d::CCString::createWithFormat("groundSquare_%02d_2_001.png", groundID)->getCString();
        cocos2d::CCSpriteBatchNode* batch2 = cocos2d::CCSpriteBatchNode::create(frameName, 5);
        addChild(batch2, 3);
        
        m_ground2Sprite = cocos2d::CCSprite::createWithTexture(batch2->getTexture());
        m_ground2Sprite->setDontDraw(true);
        m_ground2Sprite->setPosition(cocos2d::CCPointZero);
        m_ground2Sprite->setColor(color);
        batch2->addChild(m_ground2Sprite);
        
        loadGroundSprites(count, false);
        updateGround01Color(color);
    }
    
    return true;
}


void GJGroundLayer::updateGround01Color(cocos2d::ccColor3B color) {
    cocos2d::CCArray* children = m_ground1Sprite->getChildren();
    if (!children) {
        return;
    }
    
    for (int i = 0; i < children->count(); ++i) {
        cocos2d::CCObject* obj = children->objectAtIndex(i);
        obj->setColor(color);
    }
}

void GJGroundLayer::updateGround02Color(cocos2d::ccColor3B color) {
    if (!m_ground2Sprite) {
        return;
    }
    
    cocos2d::CCArray* children = m_ground2Sprite->getChildren();
    if (!children) {
        return;
    }
    
    for (int i = 0; i < children->count(); ++i) {
        cocos2d::CCObject* obj = children->objectAtIndex(i);
        obj->setColor(color);
    }
}

void GJGroundLayer::loadGroundSprites(int count, bool ground1) {
    if (ground1) {
        m_ground2Sprite = m_ground1Sprite;
    }
    
    if (!m_ground2Sprite) {
        return;
    }
    
    cocos2d::CCArray* children = cocos2d::CCArray::create();
    if (m_ground2Sprite->getChildren()) {
        children->addObjectsFromArray(m_ground2Sprite->getChildren());
    }
    
    int currentCount = children->count();
    
    if (currentCount < count) {
        for (int i = currentCount; i < count; ++i) {
            cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithTexture(m_ground2Sprite->getTexture());
            m_ground2Sprite->addChild(sprite);
            
            float xPos = i * m_textureWidth;
            float yPos = ground1 ? -m_ground1Offset : 0.0f;
            sprite->setPosition(cocos2d::CCPoint(xPos, yPos));
            sprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
            sprite->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        }
    } else if (currentCount > count) {
        int removeCount = currentCount - count;
        for (int i = 0; i < removeCount; ++i) {
            if (children->count() == 0) break;
            cocos2d::CCObject* obj = children->lastObject();
            m_ground2Sprite->removeChild(static_cast<cocos2d::CCNode*>(obj), true);
            children->removeLastObject();
        }
    }
}

void GJGroundLayer::positionGround(float y) {
    setPosition(cocos2d::CCPoint(0.0f, y));
}

float GJGroundLayer::scaleGround(float scale) {
    setScaleX(1.0f);
    if (getScaleY() < 0.0f) {
        setScaleY(-scale);
    } else {
        setScaleY(scale);
    }
    updateGroundWidth(true);
    updateShadows();
    return m_groundWidth - m_groundWidth;
}

void GJGroundLayer::showGround() {
    m_showGround = true;
}

void GJGroundLayer::toggleVisible01(bool visible) {
    if (m_showGround1 != visible) {
        m_showGround2 = false;
        m_showGround1 = visible;
        setVisible(visible && m_showGround2);
    }
}

void GJGroundLayer::toggleVisible02(bool visible) {
    if (m_showGround2 != visible) {
        m_showGround2 = visible;
        setVisible(visible && m_showGround1);
    }
}

void GJGroundLayer::updateGroundWidth(bool useThis) {
    if (!getParent()) {
        return;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    float scale;
    if (useThis) {
        scale = getScaleX();
    } else {
        scale = getParent()->getScaleX();
    }
    
    bool notRotated = !m_cameraRotated;
    float groundWidth = winSize.width / scale + 10.0f;
    m_groundWidth = groundWidth;
    
    int count = static_cast<int>(ceilf(groundWidth / m_textureWidth) + 1.0f);
    if (!notRotated) {
        ++count;
    }
    
    loadGroundSprites(count, true);
    loadGroundSprites(count, false);
    
    if (m_lineType <= 1) {
        if (useThis) {
            m_lineSprite->setScaleX(1.0f / scale);
        }
    } else {
        m_lineSprite->setScaleX((winSize.width + 10.0f) / scale / m_lineSprite->getContentSize().width);
    }
    
    float xPos = m_groundWidth * 0.5f - 5.0f;
    cocos2d::CCPoint linePos = m_lineSprite->getPosition();
    m_lineSprite->setPosition(cocos2d::CCPoint(xPos, linePos.y));
}

void GJGroundLayer::updateGroundPos(cocos2d::CCPoint pos) {
    m_ground1Sprite->setPosition(pos);
    if (m_ground2Sprite) {
        m_ground2Sprite->setPosition(pos);
    }
}

void GJGroundLayer::updateShadows() {
    float scale = getScaleX();
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    cocos2d::CCNode* shadow = getChildByTag(1);
    if (shadow) {
        shadow->setPosition(cocos2d::CCPoint(winSize.width / scale + 1.0f, 0.0f));
    }
}

void GJGroundLayer::updateShadowXPos(float leftX, float rightX) {
    getScaleX();
    
    cocos2d::CCNode* shadow1 = getChildByTag(0);
    if (shadow1) {
        shadow1->setPosition(cocos2d::CCPoint(leftX - 1.0f, 0.0f));
    }
    
    cocos2d::CCNode* shadow2 = getChildByTag(1);
    if (shadow2) {
        shadow2->setPosition(cocos2d::CCPoint(rightX + 1.0f, 0.0f));
    }
}

void GJGroundLayer::updateShadows() {
    float scale = getScaleX();
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    cocos2d::CCNode* shadow2 = getChildByTag(1);
    if (shadow2) {
        shadow2->setPosition(cocos2d::CCPoint(winSize.width / scale + 1.0f, 0.0f));
    }
}

void GJGroundLayer::updateLineBlend(bool blend) {
    if (m_blendLine != blend) {
        m_blendLine = blend;
        if (blend) {
            m_lineSprite->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE});
        } else {
            m_lineSprite->setBlendFunc(cocos2d::BlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        }
    }
}

GJGroundLayer::~GJGroundLayer() {
    // Base class handles cleanup
}