#include "GJMGLayer.h"
#include "GameManager.h"

GJMGLayer* GJMGLayer::create(int index) {
    auto layer = new GJMGLayer();
    if (layer && layer->init(index)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

void GJMGLayer::deactivateGround() {
    stopAllActions();
    m_showGround = false;
}

float GJMGLayer::defaultYOffsetForBG2(int index) {
    static float offsets[] = {0.0f, 0.0f, 0.0f};
    unsigned int idx = index - 1;
    if (idx <= 2) {
        return offsets[idx];
    }
    return 0.0f;
}

void GJMGLayer::draw() {
}

bool GJMGLayer::init(int index) {
    if (!CCLayer::init()) {
        return false;
    }
    
    setContentSize(cocos2d::CCSize(1024.0f, 768.0f));
    
    auto director = cocos2d::CCDirector::sharedDirector();
    director->getWinSize();
    
    m_ground2Offset = GJMGLayer::defaultYOffsetForBG2(index);
    
    auto manager = GameManager::sharedState();
    std::string mgTexture = manager->getMGTexture(index);
    
    m_showGround = false;
    
    m_mg1BatchNode = cocos2d::CCSpriteBatchNode::create(mgTexture.c_str(), 5);
    addChild(m_mg1BatchNode, 2);
    
    auto texture = m_mg1BatchNode->getTexture();
    auto contentSize = texture->getContentSize();
    m_textureWidth = contentSize.width;
    
    float scale = contentSize.height / m_textureWidth;
    int spriteCount = static_cast<int>(ceilf(scale) + 1.0f);
    
    m_ground1Sprite = cocos2d::CCSprite::createWithTexture(texture);
    m_ground1Sprite->m_bDontDraw = true;
    m_ground1Sprite->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    m_ground1Sprite->setColor(cocos2d::ccc3(0, 102, 255));
    m_mg1BatchNode->addChild(m_ground1Sprite);
    
    loadGroundSprites(spriteCount, true);
    updateGroundColor(cocos2d::ccc3(0, 102, 255), true);
    
    std::string fgTexture = cocos2d::CCString::createWithFormat("fg_%02d_2_001.png", index)->getCString();
    m_mg2BatchNode = cocos2d::CCSpriteBatchNode::create(fgTexture.c_str(), 5);
    addChild(m_mg2BatchNode, 3);
    
    auto mg1Texture = m_mg1BatchNode->getTexture();
    auto mg1Size = mg1Texture->getContentSize();
    float mg1Height = mg1Size.height;
    
    auto mg2Texture = m_mg2BatchNode->getTexture();
    auto mg2Size = mg2Texture->getContentSize();
    float mg2Height = mg2Size.height;
    
    float yOffset = mg1Height - mg2Height;
    
    m_ground2Sprite = cocos2d::CCSprite::createWithTexture(mg2Texture);
    m_ground2Sprite->m_bDontDraw = true;
    m_ground2Sprite->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    m_ground2Sprite->setColor(cocos2d::ccc3(0, 102, 255));
    m_mg2BatchNode->addChild(m_ground2Sprite);
    
    m_mg2BatchNode->setPosition(cocos2d::CCPoint(0.0f, yOffset));
    
    loadGroundSprites(spriteCount, false);
    updateGroundColor(cocos2d::ccc3(0, 102, 255), false);
    
    return true;
}

void GJMGLayer::loadGroundSprites(int count, bool ground1) {
    if (!ground1) {
        m_ground1Sprite = m_ground2Sprite;
    }
    
    if (!m_ground1Sprite) {
        return;
    }
    
    auto children = m_ground1Sprite->getChildren();
    auto array = cocos2d::CCArray::create();
    if (children) {
        array->addObjectsFromArray(children);
    }
    
    if (array->count() == static_cast<unsigned int>(count)) {
        return;
    }
    
    if (count > static_cast<int>(array->count())) {
        int currentCount = array->count();
        while (currentCount < count) {
            auto texture = m_ground1Sprite->getTexture();
            auto sprite = cocos2d::CCSprite::createWithTexture(texture);
            m_ground1Sprite->addChild(sprite);
            
            float spriteWidth = sprite->getContentSize().width;
            sprite->setPosition(cocos2d::CCPoint(currentCount * spriteWidth, 0.0f));
            sprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
            sprite->setOpacity(255);
            ++currentCount;
        }
    } else {
        int removeCount = array->count() - count;
        while (removeCount > 0 && array->count() > 0) {
            auto object = array->lastObject();
            m_ground1Sprite->removeChild(static_cast<cocos2d::CCNode*>(object), true);
            array->removeLastObject();
            --removeCount;
        }
    }
}

float GJMGLayer::scaleGround(float scale) {
    m_groundScale = scale;
    setScaleX(scale);
    setScaleY((getScaleY() < 0.0f) ? -scale : scale);
    updateGroundWidth(true);
    return 0.0f;
}

void GJMGLayer::showGround() {
    m_showGround = true;
}

void GJMGLayer::toggleVisible01(bool visible) {
    if (m_showGround1 != visible) {
        m_showGround2 = false;
        m_showGround1 = visible;
        setVisible(m_showGround2);
    }
}

void GJMGLayer::toggleVisible02(bool visible) {
    if (m_showGround2 != visible) {
        m_showGround2 = visible;
        setVisible(visible && m_showGround1);
    }
}

void GJMGLayer::updateGroundOpacity(unsigned char opacity, bool ground1) {
    if (!ground1) {
        m_ground1Sprite = m_ground2Sprite;
    }
    
    if (!m_ground1Sprite) {
        return;
    }
    
    auto children = m_ground1Sprite->getChildren();
    if (children) {
        for (int i = 0; i < children->count(); ++i) {
            auto child = children->objectAtIndex(i);
            child->setOpacity(opacity);
        }
    }
}

void GJMGLayer::updateGroundWidth(bool useThis) {
    auto parent = getParent();
    if (!parent) {
        return;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    float scaleX = getScaleX();
    bool isRotated = m_cameraRotated;
    float groundWidth = (winSize.width / scaleX) + 10.0f;
    m_groundWidth = groundWidth;
    
    float textureScale = groundWidth / m_textureWidth;
    int spriteCount = static_cast<int>(ceilf(textureScale) + 1.0f);
    
    if (!isRotated) {
        ++spriteCount;
    }
    
    loadGroundSprites(spriteCount, true);
    loadGroundSprites(spriteCount, false);
}

void GJMGLayer::updateMG01Blend(bool blend) {
    if (m_blendMG1 != blend) {
        m_blendMG1 = blend;
        m_mg1BatchNode->setBlendFunc(blend ? cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA} : cocos2d::ccBlendFunc{GL_ONE, GL_ZERO});
    }
}

void GJMGLayer::updateMG02Blend(bool blend) {
    if (m_blendMG2 != blend) {
        m_blendMG2 = blend;
        m_mg2BatchNode->setBlendFunc(blend ? cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA} : cocos2d::ccBlendFunc{GL_ONE, GL_ZERO});
    }
}

GJMGLayer::~GJMGLayer() {
}