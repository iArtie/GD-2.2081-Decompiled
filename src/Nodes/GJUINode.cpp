#include "Nodes/GJUINode.h"

GJUINode* GJUINode::create(UIButtonConfig* config) {
    auto node = new GJUINode();
    if (node && node->init(config)) {
        node->autorelease();
        return node;
    }
    delete node;
    return nullptr;
}

bool GJUINode::init(UIButtonConfig* config) {
    if (!CCNode::init()) {
        return false;
    }
    
    setContentSize(cocos2d::CCSize(0.0f, 0.0f));
    
    const char* firstFrame = (config->m_oneButton || config->m_split) ? "PBtn_Jump_001.png" : "PBtn_Move_001.png";
    m_firstSprite = cocos2d::CCSprite::createWithSpriteFrameName(firstFrame);
    addChild(m_firstSprite);
    
    m_oneButton = config->m_oneButton;
    
    if (!m_oneButton) {
        const char* secondFrame = config->m_split ? "PBtn_Jump_001.png" : "PBtn_Move_001.png";
        m_secondSprite = cocos2d::CCSprite::createWithSpriteFrameName(secondFrame);
        m_secondSprite->setFlipX(true);
        addChild(m_secondSprite, 1);
    }
    
    m_split = config->m_split;
    updateSize(static_cast<float>(config->m_width), static_cast<float>(config->m_height));
    updateButtonPositions();
    updateDragRadius(config->m_radius);
    updateDeadzone(static_cast<int>(config->m_deadzone));
    toggleModeB(config->m_modeB);
    updateButtonScale(config->m_scale);
    setOpacity(static_cast<unsigned char>(config->m_opacity));
    setPosition(config->m_position);
    m_snap = config->m_snap;
    m_swiping = false;
    m_touchID = -1;
    m_moving = false;
    m_currentButton = 0;
    
    return true;
}

void GJUINode::loadFromConfig(UIButtonConfig* config) {
    updateSize(static_cast<float>(config->m_width), static_cast<float>(config->m_height));
    updateDragRadius(config->m_radius);
    updateDeadzone(static_cast<int>(config->m_deadzone));
    toggleModeB(config->m_modeB);
    m_snap = config->m_snap;
    updateButtonScale(config->m_scale);
    setOpacity(static_cast<unsigned char>(config->m_opacity));
    m_oneButton = config->m_oneButton;
    m_split = config->m_split;
    updateButtonFrames();
    
    if (config->m_player2) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        setPosition(cocos2d::CCPoint(winSize.width - config->m_position.x, config->m_position.y));
    } else {
        setPosition(config->m_position);
    }
}

void GJUINode::saveToConfig(UIButtonConfig* config) {
    config->m_width = static_cast<int>(m_rect.size.width);
    config->m_height = static_cast<int>(m_rect.size.height);
    config->m_scale = getButtonScale();
    config->m_opacity = getOpacity();
    config->m_modeB = m_modeB;
    config->m_radius = m_radius;
    config->m_deadzone = m_deadzone;
    config->m_position = getPosition();
    config->m_split = m_split;
    config->m_snap = m_snap;
}

void GJUINode::updateSize(float width, float height) {
    if (width < 10.0f) width = 10.0f;
    if (height < 10.0f) height = 10.0f;
    
    m_rect = cocos2d::CCRect(
        -width * 0.5f,
        -height * 0.5f,
        width,
        height
    );
}

void GJUINode::updateButtonPositions() {
    if (!m_secondSprite) {
        return;
    }
    
    if (m_modeB) {
        m_firstSprite->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
        return;
    }
    
    float scale = m_firstSprite->getScale();
    float halfWidth = scale * m_firstSprite->getContentSize().width * 0.5f;
    
    m_firstSprite->setPosition(cocos2d::CCPoint(-m_radius - halfWidth, 0.0f));
    m_secondSprite->setPosition(cocos2d::CCPoint(m_radius + halfWidth, 0.0f));
}

void GJUINode::updateButtonScale(float scale) {
    if (scale < 0.1f) scale = 0.1f;
    
    m_firstSprite->setScale(scale);
    if (m_secondSprite) {
        m_secondSprite->setScale(scale);
    }
    
    updateButtonPositions();
}

void GJUINode::updateDragRadius(float radius) {
    m_radius = (radius <= 0.0f) ? 0.0f : radius;
    updateButtonPositions();
}

void GJUINode::updateDeadzone(int deadzone) {
    m_deadzone = static_cast<float>(deadzone);
}

void GJUINode::updateButtonFrames() {
    const char* firstFrame;
    if (m_modeB) {
        firstFrame = "PBtn_Arrow_001.png";
    } else if (m_oneButton || m_split) {
        firstFrame = "PBtn_Jump_001.png";
    } else {
        firstFrame = "PBtn_Move_001.png";
    }
    
    auto cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_firstSprite->setDisplayFrame(cache->spriteFrameByName(firstFrame));
    
    if (m_secondSprite) {
        const char* secondFrame = m_split ? "PBtn_Jump_001.png" : "PBtn_Move_001.png";
        m_secondSprite->setDisplayFrame(cache->spriteFrameByName(secondFrame));
    }
}

void GJUINode::toggleModeB(bool modeB) {
    if (!m_secondSprite) {
        return;
    }
    
    m_modeB = modeB;
    m_firstSprite->setVisible(!modeB);
    m_secondSprite->setVisible(!modeB);
    updateButtonFrames();
}

void GJUINode::toggleHighlight(int button, bool highlight) {
    if (m_modeB) {
        if (m_firstSprite) {
            m_firstSprite->setColor(highlight ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(100, 100, 100));
        }
        return;
    }
    
    auto sprite = (button == 3) ? m_firstSprite : m_secondSprite;
    if (sprite) {
        sprite->setColor(highlight ? cocos2d::ccc3(255, 255, 255) : cocos2d::ccc3(100, 100, 100));
    }
}

void GJUINode::highlightButton(int button) {
    if (m_modeB) {
        return;
    }
    
    toggleHighlight(button, true);
    toggleHighlight((button == 2) ? 3 : 2, false);
}

void GJUINode::setOpacity(unsigned char opacity) {
    m_firstSprite->setOpacity(opacity);
    if (m_secondSprite) {
        m_secondSprite->setOpacity(opacity);
    }
}

uint8_t GJUINode::getOpacity() {
    return m_firstSprite->getOpacity();
}

float GJUINode::getButtonScale() {
    return m_firstSprite->getScale();
}

int GJUINode::touchTest(cocos2d::CCPoint position) {
    if (!m_rect.containsPoint(position)) {
        return 0;
    }
    
    if (m_oneButton) {
        return 1;
    }
    
    if (m_modeB && (m_snap || !m_swiping)) {
        updateRangePos(position);
        return 5;
    }
    
    return activeTouchTest(position);
}

int GJUINode::activeTouchTest(cocos2d::CCPoint position) {
    if (m_modeB) {
        return activeRangeTouchTest(position);
    }
    
    if (fabsf(position.x) < m_deadzone) {
        return 5;
    }
    
    return (position.x < 0.0f) ? 2 : 3;
}

void GJUINode::updateRangePos(cocos2d::CCPoint position) {
    m_firstSprite->setPosition(position + cocos2d::CCPoint(0.0f, 40.0f));
    m_firstSprite->setVisible(true);
    m_swiping = true;
}

void GJUINode::touchEnded() {
    m_touchID = -1;
    toggleHighlight(2, false);
    
    if (m_modeB) {
        if (m_snap) {
            m_firstSprite->setVisible(false);
        }
        m_currentButton = 0;
    } else {
        toggleHighlight(3, false);
        m_currentButton = 0;
    }
}

void GJUINode::resetState() {
    m_touchID = -1;
    m_moving = false;
    m_swiping = false;
    toggleHighlight(2, false);
    toggleHighlight(3, false);
}

void GJUINode::updateHeight(float height) {
    updateSize(m_rect.size.width, height);
}

void GJUINode::updateWidth(float width) {
    updateSize(width, m_rect.size.height);
}

void GJUINode::draw() {
    if (!m_drawLines) {
        return;
    }
    
    glLineWidth(1.0f);
    
    int tag = getTag();
    int r, g, b;
    
    if (tag == 1) {
        r = 255; g = 0; b = 0;
    } else if (tag == 2) {
        r = 0; g = 255; b = 0;
    } else {
        r = 0; g = 0; b = 255;
    }
    
    cocos2d::ccDrawColor4B(r, g, b, 255);
    cocos2d::ccDrawRect(m_rect.origin, m_rect.origin + m_rect.size);
    
    if (m_modeB) {
        if (m_radius > 0.0f) {
            cocos2d::ccDrawColor4B(255, 150, 0, 255);
            cocos2d::ccDrawCircle(cocos2d::CCPoint(0.0f, 0.0f), m_radius, 360.0f, 0, false);
        }
        
        if (m_deadzone > 0.0f && m_deadzone < m_radius) {
            cocos2d::ccDrawColor4B(200, 0, 0, 255);
            cocos2d::ccDrawCircle(cocos2d::CCPoint(0.0f, 0.0f), m_deadzone, 360.0f, 0, false);
        }
    } else if (m_secondSprite) {
        if (m_deadzone <= 0.0f) {
            cocos2d::ccDrawColor4B(
                static_cast<unsigned char>(r * 0.3f),
                static_cast<unsigned char>(g * 0.3f),
                static_cast<unsigned char>(b * 0.3f),
                255
            );
            cocos2d::ccDrawLine(
                cocos2d::CCPoint(0.0f, m_rect.origin.y),
                cocos2d::CCPoint(0.0f, m_rect.origin.y + m_rect.size.height)
            );
        } else {
            cocos2d::ccDrawColor4B(200, 0, 0, 255);
            cocos2d::ccDrawRect(
                cocos2d::CCPoint(-m_deadzone, m_rect.origin.y),
                cocos2d::CCPoint(m_deadzone, m_rect.origin.y + m_rect.size.height)
            );
        }
    }
}

GJUINode::~GJUINode() {
}