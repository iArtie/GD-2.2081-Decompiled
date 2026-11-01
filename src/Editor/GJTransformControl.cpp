#include "Editor/GJTransformControl.h"
#include "Editor/GJTransformControlDelegate.h"
#include "GJTransformState.h"
#include "Editor/EditorUI.h"
#include "Managers/GameManager.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJTransformControl* GJTransformControl::create() {
    auto control = new GJTransformControl();
    if (control && control->init()) {
        control->autorelease();
        return control;
    }
    delete control;
    return nullptr;
}

bool GJTransformControl::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
    setTouchEnabled(true);
    setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    
    m_mainNode = cocos2d::CCNode::create();
    addChild(m_mainNode);
    
    m_mainNodeParent = new GJTransformControl();
    m_mainNode->addChild(m_mainNodeParent);
    
    m_touchID = -1;
    m_transformButtonType = 0;
    m_scaleX = 1.0f;
    m_scaleY = 1.0f;
    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_warpLocked = false;
    m_delegate = nullptr;
    m_objects = nullptr;
    
    auto lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("warpLockOffBtn_001.png");
    m_warpLockButton = CCMenuItemSpriteExtra::create(lockSprite, this, menu_selector(GJTransformControl::onToggleLockScale));
    auto menu = cocos2d::CCMenu::createWithItem(m_warpLockButton);
    m_mainNodeParent->addChild(menu);
    
    m_warpSprites = cocos2d::CCArray::create();
    m_warpSprites->retain();
    
    for (int i = 1; i <= 12; ++i) {
        const char* frame = "warpBtn_02_001.png";
        float rotation = 0.0f;
        
        if (i == 10 || i == 11) {
            frame = "warpBtn_03_001.png";
            if (i == 11) {
                rotation = 90.0f;
            }
        } else if (i == 1 || i == 12) {
            frame = "warpBtn_01_001.png";
        }
        
        auto sprite = cocos2d::CCSprite::createWithSpriteFrameName(frame);
        if (i == 1 || i == 12) {
            addChild(sprite, 100);
        } else {
            m_mainNodeParent->addChild(sprite, 100);
        }
        sprite->setTag(i);
        sprite->setRotation(rotation);
        m_warpSprites->addObject(sprite);
    }
    
    auto manager = GameManager::sharedState();
    m_buttonScale = manager->getGameVariable("0169") ? 0.5f : 1.0f;
    scaleButtons(1.0f);
    
    return true;
}

bool GJTransformControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != -1) {
        return false;
    }
    
    cocos2d::CCPoint location = touch->getLocation();
    cocos2d::CCPoint local = m_mainNodeParent->convertToNodeSpace(location);
    local = convertToNodeSpace(local);
    
    int foundTag = 0;
    for (int i = 1; i <= 12; ++i) {
        auto sprite = spriteByTag(i);
        cocos2d::CCRect rect = sprite->boundingBox();
        if (rect.containsPoint(local)) {
            foundTag = i;
            break;
        }
    }
    
    if (foundTag == 0) {
        return false;
    }
    
    m_transformButtonType = foundTag;
    m_touchID = touch->getID();
    
    auto sprite = spriteByTag(m_transformButtonType);
    m_cursorDifference = local - sprite->getPosition();
    
    auto topRight = spriteByTag(7);
    m_topRightPosition = topRight->getPosition();
    
    auto bottomLeft = spriteByTag(8);
    m_bottomLeftPosition = bottomLeft->getPosition();
    
    if (m_delegate) {
        m_delegate->transformChangeBegin();
    }
    
    if (m_transformButtonType == 12) {
        m_rotationX = m_rotationY;
    }
    
    return true;
}

void GJTransformControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != touch->getID()) {
        return;
    }
    
    cocos2d::CCPoint location = touch->getLocation();
    cocos2d::CCPoint local = (m_transformButtonType == 1 || m_transformButtonType == 12) ? 
        convertToNodeSpace(location) : 
        m_mainNodeParent->convertToNodeSpace(location);
    
    auto anchor = spriteByTag(1);
    int type = m_transformButtonType;
    
    switch (type) {
        case 1: {
            anchor->setPosition(local + m_cursorDifference);
            break;
        }
        case 2:
        case 3: {
            auto sprite = spriteByTag(type);
            sprite->setPosition(cocos2d::CCPoint(local.x + m_cursorDifference.x, sprite->getPosition().y));
            float diff = sprite->getPosition().x - anchor->getPosition().x;
            float base = (type == 3) ? m_topRight.x : m_bottomLeft.x;
            m_scaleX = diff / (base - anchor->getPosition().x);
            if (m_delegate) {
                m_delegate->transformScaleXChanged(m_scaleX);
            }
            break;
        }
        case 4:
        case 5: {
            auto sprite = spriteByTag(type);
            sprite->setPosition(cocos2d::CCPoint(sprite->getPosition().x, local.y + m_cursorDifference.y));
            float diff = sprite->getPosition().y - anchor->getPosition().y;
            float base = (type == 4) ? m_topRight.y : m_bottomLeft.y;
            m_scaleY = diff / (base - anchor->getPosition().y);
            if (m_delegate) {
                m_delegate->transformScaleYChanged(m_scaleY);
            }
            break;
        }
        case 6:
        case 7:
        case 8:
        case 9: {
            auto sprite = spriteByTag(type);
            sprite->setPosition(local + m_cursorDifference);
            
            float xDiff = sprite->getPosition().x - anchor->getPosition().x;
            float xBase = ((type - 7) & 0xFFFD) ? m_bottomLeft.x : m_topRight.x;
            xBase -= anchor->getPosition().x;
            
            float yDiff = sprite->getPosition().y - anchor->getPosition().y;
            float yBase = ((type - 6) <= 1) ? m_topRight.y : m_bottomLeft.y;
            yBase -= anchor->getPosition().y;
            
            m_scaleX = xDiff / xBase;
            m_scaleY = yDiff / yBase;
            
            if (m_warpLocked) {
                m_scaleY = (xDiff / xBase) / (m_scaleX / m_scaleY);
            }
            
            if (m_delegate) {
                m_delegate->transformScaleXYChanged(m_scaleX, m_scaleY);
            }
            break;
        }
        case 10: {
            auto sprite = spriteByTag(10);
            float x = local.x + m_cursorDifference.x;
            x = std::max(-100.0f, std::min(100.0f, x));
            sprite->setPosition(cocos2d::CCPoint(x, sprite->getPosition().y));
            if (m_delegate) {
                m_delegate->transformSkewXChanged(x * 0.45f);
            }
            break;
        }
        case 11: {
            auto sprite = spriteByTag(11);
            float y = local.y + m_cursorDifference.y;
            y = std::max(-100.0f, std::min(100.0f, y));
            sprite->setPosition(cocos2d::CCPoint(sprite->getPosition().x, y));
            if (m_delegate) {
                m_delegate->transformSkewYChanged(y * 0.45f);
            }
            break;
        }
        case 12: {
            float angle = atan2f(local.y + m_cursorDifference.y, local.x + m_cursorDifference.x);
            float rotation = -angle * 57.296f;
            float delta = rotation / 100.0f - m_rotation;
            
            if (m_rotationY != delta) {
                m_rotationY = delta;
                m_mainNode->setRotation(delta);
                if (m_delegate) {
                    m_delegate->transformRotationChanged(m_rotationY);
                }
            }
            break;
        }
    }
    
    updateButtons(false, false);
}

void GJTransformControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != touch->getID()) {
        return;
    }
    
    if (m_transformButtonType == 1) {
        if (m_delegate) {
            auto sprite = spriteByTag(1);
            cocos2d::CCPoint worldPos = convertToWorldSpace(sprite->getPosition());
            m_delegate->anchorPointMoved(worldPos);
            sprite->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
        }
        
        auto firstObj = m_objects->objectAtIndex(0);
        cocos2d::CCPoint oldPos = firstObj->getPosition();
        
        updateButtons(false, true);
        m_transformButtonType = 0;
        updateButtons(false, false);
        
        cocos2d::CCPoint newPos = firstObj->getPosition();
        cocos2d::CCPoint delta = oldPos - newPos;
        
        auto editorUI = m_delegate->getUI();
        editorUI->checkDiffAfterTransformAnchor(delta, m_objects);
        
        updateButtons(false, true);
        updateButtons(false, false);
    } else if (m_transformButtonType != 0) {
        m_touchID = -1;
        if (m_delegate) {
            m_delegate->transformChangeEnded();
        }
        
        if (m_transformButtonType == 10 || m_transformButtonType == 11) {
            updateButtons(false, true);
        }
    }
    
    m_transformButtonType = 0;
    updateButtons(false, false);
}

void GJTransformControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void GJTransformControl::updateButtons(bool transform, bool skew) {
    if (!m_delegate) {
        return;
    }
    
    auto manager = GameManager::sharedState();
    auto objectLayer = manager->m_levelEditorLayer->m_objectLayer;
    
    float originalScale = objectLayer->getScale();
    cocos2d::CCPoint originalPos = objectLayer->getPosition();
    
    objectLayer->setScale(1.0f);
    objectLayer->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    
    float minX = 10000000.0f;
    float minY = 10000000.0f;
    float maxX = -10000000.0f;
    float maxY = -10000000.0f;
    
    if (m_objects && m_objects->count() > 0) {
        for (int i = 0; i < m_objects->count(); ++i) {
            auto node = static_cast<cocos2d::CCNode*>(m_objects->objectAtIndex(i));
            cocos2d::CCPoint pos = node->getPosition();
            cocos2d::CCSize size = node->getContentSize();
            
            float halfWidth = size.width * 0.5f;
            float halfHeight = size.height * 0.5f;
            
            cocos2d::CCPoint corners[4] = {
                node->convertToWorldSpace(cocos2d::CCPoint(-halfWidth, halfHeight)),
                node->convertToWorldSpace(cocos2d::CCPoint(halfWidth, halfHeight)),
                node->convertToWorldSpace(cocos2d::CCPoint(-halfWidth, -halfHeight)),
                node->convertToWorldSpace(cocos2d::CCPoint(halfWidth, -halfHeight))
            };
            
            for (int j = 0; j < 4; ++j) {
                minX = std::min(minX, corners[j].x);
                maxX = std::max(maxX, corners[j].x);
                minY = std::min(minY, corners[j].y);
                maxY = std::max(maxY, corners[j].y);
            }
        }
    }
    
    if (skew) {
        auto editorUI = m_delegate->getUI();
        editorUI->transformObjectsActive();
    } else {
        m_delegate->transformRestoreRotation();
    }
    
    cocos2d::CCPoint topRight(maxX, maxY);
    cocos2d::CCPoint bottomLeft(minX, minY);
    
    if (transform) {
        m_topRightOrigin = topRight;
        m_bottomLeftOrigin = bottomLeft;
        m_topRight = topRight;
        m_bottomLeft = bottomLeft;
        m_scaleX = 1.0f;
        m_scaleY = 1.0f;
        m_rotationX = 0.0f;
        m_rotationY = 0.0f;
        m_mainNode->setRotation(0.0f);
    }
    
    float scaleX = m_scaleX;
    float scaleY = m_scaleY;
    
    if (m_topRightOrigin.x < 0.0f) topRight.x = -scaleX;
    else topRight.x = scaleX;
    
    if (m_bottomLeftOrigin.x < 0.0f) bottomLeft.x = -scaleX;
    else bottomLeft.x = scaleX;
    
    if (m_topRightOrigin.y < 0.0f) topRight.y = -scaleY;
    else topRight.y = scaleY;
    
    if (m_bottomLeftOrigin.y < 0.0f) bottomLeft.y = -scaleY;
    else bottomLeft.y = scaleY;
    
    float width = topRight.x - bottomLeft.x;
    float height = topRight.y - bottomLeft.y;
    float centerY = (topRight.y + bottomLeft.y) * 0.5f;
    float centerX = (topRight.x + bottomLeft.x) * 0.5f;
    
    auto rightMid = spriteByTag(3);
    rightMid->setPosition(cocos2d::CCPoint(topRight.x, centerY));
    
    auto leftMid = spriteByTag(2);
    leftMid->setPosition(cocos2d::CCPoint(bottomLeft.x, centerY));
    
    auto topMid = spriteByTag(4);
    topMid->setPosition(cocos2d::CCPoint(centerX, topRight.y));
    
    auto bottomMid = spriteByTag(5);
    bottomMid->setPosition(cocos2d::CCPoint(centerX, bottomLeft.y));
    
    auto topRightCorner = spriteByTag(6);
    topRightCorner->setPosition(cocos2d::CCPoint(topRight.x, topRight.y));
    
    auto topLeftCorner = spriteByTag(7);
    topLeftCorner->setPosition(cocos2d::CCPoint(bottomLeft.x, topRight.y));
    
    auto bottomRightCorner = spriteByTag(8);
    bottomRightCorner->setPosition(cocos2d::CCPoint(topRight.x, bottomLeft.y));
    
    auto bottomLeftCorner = spriteByTag(9);
    bottomLeftCorner->setPosition(cocos2d::CCPoint(bottomLeft.x, bottomLeft.y));
    
    auto skewX = spriteByTag(10);
    skewX->setPosition(cocos2d::CCPoint(transform ? 0.0f : skewX->getPosition().x, topRight.y + 30.0f));
    
    auto skewY = spriteByTag(11);
    skewY->setPosition(cocos2d::CCPoint(topRight.x + 30.0f, transform ? 0.0f : skewY->getPosition().y));
    
    auto lockMenu = m_warpLockButton->getParent();
    lockMenu->setPosition(cocos2d::CCPoint(topRight.x + 40.0f, topRight.y + 40.0f));
    
    m_rotatePosition = cocos2d::CCPoint(topRight.x + 60.0f, (topRight.y + bottomLeft.y) * 0.5f + height * 0.5f);
    calculateRotationOffset();
    
    auto rotateSprite = spriteByTag(12);
    cocos2d::CCPoint worldPos = m_mainNodeParent->convertToWorldSpace(m_rotatePosition);
    rotateSprite->setPosition(convertToNodeSpace(worldPos));
    
    m_topRight = topRight;
    m_bottomLeft = bottomLeft;
    
    objectLayer->setScale(originalScale);
    objectLayer->setPosition(originalPos);
}

void GJTransformControl::calculateRotationOffset() {
    m_rotation = -atan2f(m_rotatePosition.y, m_rotatePosition.x) * 57.296f / 100.0f;
}

void GJTransformControl::applyRotation(float rotation) {
    if (m_rotationY == rotation) {
        updateButtons(false, false);
        return;
    }
    
    m_rotationY = rotation;
    m_mainNode->setRotation(rotation);
    
    if (m_delegate) {
        m_delegate->transformRotationChanged(rotation);
    }
    
    updateButtons(false, false);
}

void GJTransformControl::finishTouch() {
    if (m_touchID != -1) {
        m_touchID = -1;
        if (m_delegate) {
            m_delegate->transformChangeEnded();
        }
    }
}

cocos2d::CCSprite* GJTransformControl::spriteByTag(int tag) {
    return static_cast<cocos2d::CCSprite*>(m_warpSprites->objectAtIndex(tag - 1));
}

void GJTransformControl::onToggleLockScale(cocos2d::CCObject* sender) {
    m_warpLocked = !m_warpLocked;
    
    auto image = m_warpLockButton->getNormalImage();
    auto cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    const char* frame = m_warpLocked ? "warpLockOnBtn_001.png" : "warpLockOffBtn_001.png";
    image->setDisplayFrame(cache->spriteFrameByName(frame));
}

void GJTransformControl::scaleButtons(float scale) {
    if (!m_warpSprites) {
        return;
    }
    
    float buttonScale = scale * 0.7f * m_buttonScale;
    for (int i = 0; i < m_warpSprites->count(); ++i) {
        auto sprite = m_warpSprites->objectAtIndex(i);
        sprite->setScale(buttonScale);
    }
    
    auto normalImage = m_warpLockButton->getNormalImage();
    normalImage->setScale(scale);
}

void GJTransformControl::loadFromState(GJTransformState* state) {
    m_rotationX = state->m_transformRotationX;
    m_rotationY = state->m_transformRotationY;
    m_scaleX = state->m_transformScaleX;
    m_scaleY = state->m_transformScaleY;
    
    m_mainNode->setRotation(m_rotationY);
    setPosition(state->m_transformPosition);
    
    auto anchor = spriteByTag(1);
    anchor->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    
    auto skewX = spriteByTag(10);
    skewX->setPosition(state->m_transformSkewX);
    
    auto skewY = spriteByTag(11);
    skewY->setPosition(state->m_transformSkewY);
}

void GJTransformControl::saveToState(GJTransformState* state) {
    state->m_transformRotationX = m_rotationX;
    state->m_transformRotationY = m_rotationY;
    state->m_transformScaleX = m_scaleX;
    state->m_transformScaleY = m_scaleY;
    state->m_transformPosition = getPosition();
    
    auto skewX = spriteByTag(10);
    state->m_transformSkewX = skewX->getPosition();
    
    auto skewY = spriteByTag(11);
    state->m_transformSkewY = skewY->getPosition();
}

void GJTransformControl::refreshControl() {
    if (m_delegate) {
        auto anchor = spriteByTag(1);
        cocos2d::CCPoint worldPos = convertToWorldSpace(anchor->getPosition());
        m_delegate->anchorPointMoved(worldPos);
        anchor->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
        
        auto firstObj = m_objects->objectAtIndex(0);
        cocos2d::CCPoint oldPos = firstObj->getPosition();
        
        updateButtons(false, true);
        updateButtons(false, false);
        
        cocos2d::CCPoint newPos = firstObj->getPosition();
        cocos2d::CCPoint delta = oldPos - newPos;
        
        auto editorUI = m_delegate->getUI();
        editorUI->checkDiffAfterTransformAnchor(delta, m_objects);
        
        updateButtons(false, true);
    }
    
    updateButtons(false, false);
}

void GJTransformControl::updateAnchorSprite(cocos2d::CCPoint position) {
    auto anchor = spriteByTag(1);
    anchor->setPosition(position);
    if (m_delegate) {
        m_delegate->updateTransformControl();
    }
}

void GJTransformControl::updateMinMaxPositions() {
    if (m_transformButtonType == 10) {
        auto topRight = spriteByTag(7);
        auto bottomLeft = spriteByTag(8);
        float scale = m_topRightPosition.x / m_topRight.x;
        m_topRight.x = topRight->getPosition().x / scale;
        m_bottomLeft.x = bottomLeft->getPosition().x / scale;
    } else if (m_transformButtonType == 11) {
        auto topRight = spriteByTag(7);
        auto bottomLeft = spriteByTag(8);
        float scale = m_topRightPosition.y / m_topRight.y;
        m_topRight.y = topRight->getPosition().y / scale;
        m_bottomLeft.y = bottomLeft->getPosition().y / scale;
    }
}

void GJTransformControl::logCurrentZeroPos() {
}

GJTransformControl::~GJTransformControl() {
    if (m_warpSprites) {
        m_warpSprites->release();
    }
}