#include "ColorChannelSprite.h"
#include "ColorAction.h"
#include "GJSpecialColorSelect.h"
#include "cocos2d.h"

ColorChannelSprite* ColorChannelSprite::create() {
    ColorChannelSprite* sprite = new ColorChannelSprite();
    if (sprite && sprite->init()) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

bool ColorChannelSprite::init() {
    return cocos2d::CCSprite::initWithSpriteFrameName("GJ_colorBtn_001.png");
}

void ColorChannelSprite::updateBlending(bool enabled) {
    if (enabled) {
        if (!m_blendingLabel) {
            m_blendingLabel = cocos2d::CCLabelBMFont::create();
            this->addChild(m_blendingLabel);
            m_blendingLabel->setPosition(
                cocos2d::CCPoint(
                    getContentSize().width * 0.5f - 10.0f,
                    getContentSize().height * 0.5f + 12.0f
                )
            );
            m_blendingLabel->setScale(0.5f);
        }
    } else if (!m_blendingLabel) {
        return;
    }

    m_blendingLabel->setVisible(enabled);
}

void ColorChannelSprite::updateCopyLabel(int channelID, bool copyColorLoop) {
    if (channelID != 0) {
        if (!m_copyLabel) {
            m_copyLabel = cocos2d::CCLabelBMFont::create();
            this->addChild(m_copyLabel, 1);
            m_copyLabel->setPosition(
                cocos2d::CCPoint(
                    getContentSize().width * 0.5f,
                    getContentSize().height * 0.5f + 3.0f
                )
            );
            m_copyLabel->setScale(0.5f);
        }

        const char* text;
        if (channelID <= 999) {
            CCString* str = CCString::createWithFormat("%i", channelID);
            text = str->getCString();
        } else {
            text = GJSpecialColorSelect::textForColorIdx(channelID);
        }

        m_copyLabel->setString(text);
        m_copyLabel->limitLabelWidth(28.0f, 0.5f, 0.0f);
        m_copyLabel->setVisible(true);

        if (copyColorLoop) {
            m_copyLabel->setColor(cocos2d::ccc3(255, 150, 100));
        } else {
            m_copyLabel->setColor(cocos2d::ccc3(100, 150, 255));
        }
    } else if (m_copyLabel) {
        m_copyLabel->setVisible(false);
    }
}

void ColorChannelSprite::updateOpacity(float alpha) {
    if (alpha == 1.0f) {
        if (m_opacityLabel) {
            m_opacityLabel->setVisible(false);
        }
        return;
    }

    if (!m_opacityLabel) {
        m_opacityLabel = cocos2d::CCLabelBMFont::create();
        this->addChild(m_opacityLabel);
        m_opacityLabel->setPosition(
            cocos2d::CCPoint(
                getContentSize().width * 0.5f,
                getContentSize().height * 0.5f - 9.5f
            )
        );
        m_opacityLabel->setScale(0.3f);
    }

    CCString* str = CCString::createWithFormat("%.2f", alpha);
    m_opacityLabel->setString(str->getCString());
    m_opacityLabel->setVisible(true);
}

void ColorChannelSprite::updateValues(ColorAction* action) {
    if (action) {
        updateCopyLabel(action->m_copyID, action->m_copyColorLoop);
        updateOpacity(action->m_fromOpacity);
        updateBlending(action->m_blending);

        cocos2d::ccColor3B color = action->m_fromColor;

        // If there's a copy ID and no copy color loop, make it gray
        if (action->m_copyID != 0 && !action->m_copyColorLoop) {
            color.r = 165;
            color.g = 165;
            color.b = 165;
        }

        this->setColor(color);
    } else {
        this->setColor(cocos2d::ccc3(165, 165, 165));
        updateCopyLabel(0, false);
        updateOpacity(1.0f);
        updateBlending(false);
    }
}

ColorChannelSprite::~ColorChannelSprite() {
    // Destructor
}