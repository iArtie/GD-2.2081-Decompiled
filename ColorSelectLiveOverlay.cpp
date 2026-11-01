#include "ColorSelectLiveOverlay.h"
#include "ColorAction.h"
#include "EffectGameObject.h"
#include "SliderThumb.h"
#include "ButtonSprite.h"
#include "GJSpecialColorSelect.h"
#include "cocos2d.h"

void ColorSelectLiveOverlay::closeColorSelect(cocos2d::CCObject* sender) {
    m_closeButtonPressed = (sender != nullptr);
    this->setKeypadEnabled(false);
    if (m_delegate) {
        m_delegate->colorSelectClosed(this);
    }
    this->removeFromParentAndCleanup(true);
}

void ColorSelectLiveOverlay::colorValueChanged(cocos2d::ccColor3B color) {
    // Empty implementation
}

ColorSelectLiveOverlay* ColorSelectLiveOverlay::createWithActions(ColorAction* baseAction, ColorAction* detailAction) {
    return create(baseAction, detailAction, nullptr);
}

ColorSelectLiveOverlay* ColorSelectLiveOverlay::createWithObject(EffectGameObject* object) {
    return create(nullptr, nullptr, object);
}

void ColorSelectLiveOverlay::determineStartValues() {
    if (m_effectGameObject) {
        m_targetColor = m_effectGameObject->m_triggerTargetColor;
        m_mainColor = m_targetColor;
        m_opacity = m_effectGameObject->m_opacity;
    } else if (m_baseColorAction) {
        m_targetColor = m_baseColorAction->m_fromColor;
        m_mainColor = m_targetColor;
        m_opacity = m_baseColorAction->m_fromOpacity;

        if (m_detailColorAction) {
            m_detailColor = m_detailColorAction->m_fromColor;
        }
    }
}

cocos2d::ccColor3B ColorSelectLiveOverlay::getColorValue() {
    return cocos2d::ccc3(255, 255, 255);
}

void ColorSelectLiveOverlay::keyBackClicked() {
    closeColorSelect(nullptr);
}

void ColorSelectLiveOverlay::onSelectTab(cocos2d::CCObject* sender) {
    bool isDetailTab = (sender->getTag() == 1);
    if (m_detailColorSelected == isDetailTab) {
        return;
    }

    m_detailColorSelected = isDetailTab;
    ColorAction* targetAction = m_detailColorSelected ? m_detailColorAction : m_baseColorAction;

    if (targetAction) {
        m_opacity = targetAction->m_fromOpacity;
        selectColor(targetAction->m_fromColor);
    }

    // Update button sprites
    for (unsigned int i = 0; i < m_12buttons->count(); i++) {
        ButtonSprite* btn = static_cast<ButtonSprite*>(m_12buttons->objectAtIndex(i));
        if (btn->getTag() == sender->getTag()) {
            btn->updateBGImage("GJ_button_01.png");
        } else {
            btn->updateBGImage("GJ_button_04.png");
        }
    }

    cocos2d::ccColor3B* color = m_detailColorSelected ? &m_detailColor : &m_mainColor;
    m_oldColorSprite->setColor(*color);
    updateColorLabel();
}

void ColorSelectLiveOverlay::show() {
    if (m_ZOrder == 0) {
        m_ZOrder = 105;
    }

    if (!m_scene) {
        m_scene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
    }

    m_scene->addChild(this, m_ZOrder);
}

void ColorSelectLiveOverlay::sliderChanged(cocos2d::CCObject* sender) {
    float value = SliderThumb::getValue(static_cast<SliderThumb*>(sender));
    int tag = sender->getTag();

    switch (tag) {
        case 1:
            m_hue = value * 360.0;
            break;
        case 2:
            m_saturation = value;
            break;
        case 3:
            m_value = value;
            break;
        case 4:
            m_opacity = value;
            updateOpacity();
            break;
        default:
            break;
    }

    // Update color display
    cocos2d::CCPoint rgb = cocos2d::extension::CCControlUtils::RGBfromHSV(m_hue, m_saturation, m_value);
    m_targetColor.r = static_cast<unsigned char>(rgb.x * 255.0f);
    m_targetColor.g = static_cast<unsigned char>(rgb.y * 255.0f);
    m_targetColor.b = static_cast<unsigned char>(rgb.z * 255.0f);

    m_colorSprite->setColor(m_targetColor);

    // Update saturation preview
    cocos2d::CCPoint satRgb = cocos2d::extension::CCControlUtils::RGBfromHSV(m_hue, 1.0, 1.0);
    m_saturationSprite->setColor(cocos2d::ccc3(
        static_cast<unsigned char>(satRgb.x * 255.0f),
        static_cast<unsigned char>(satRgb.y * 255.0f),
        static_cast<unsigned char>(satRgb.z * 255.0f)
    ));

    // Update value preview
    cocos2d::CCPoint valRgb = cocos2d::extension::CCControlUtils::RGBfromHSV(m_hue, m_saturation, 1.0);
    m_valueSprite->setColor(cocos2d::ccc3(
        static_cast<unsigned char>(valRgb.x * 255.0f),
        static_cast<unsigned char>(valRgb.y * 255.0f),
        static_cast<unsigned char>(valRgb.z * 255.0f)
    ));

    m_opacitySprite->setColor(m_valueSprite->getColor());
    updateColorValue();
}

void ColorSelectLiveOverlay::textChanged(CCTextInputNode* node) {
    // Empty implementation
}

void ColorSelectLiveOverlay::toggleControls(bool visible) {
    for (unsigned int i = 0; i < m_barSprites->count(); i++) {
        cocos2d::CCObject* obj = m_barSprites->objectAtIndex(i);
        obj->setVisible(visible);
    }
}

void ColorSelectLiveOverlay::updateColorLabel() {
    if (!m_detailColorSelected) {
        m_detailColorAction = m_baseColorAction;
    }

    int colorId = 0;
    if (m_detailColorAction) {
        colorId = m_detailColorAction->m_colorID;
    } else if (m_effectGameObject) {
        colorId = m_effectGameObject->m_targetColor;
    }

    if (colorId <= 0) {
        m_colorLabel->setString(" ");
        return;
    }

    std::string text;
    if (colorId <= 999) {
        CCString* str = CCString::createWithFormat("%i", colorId);
        text = str->getCString();
    } else {
        text = GJSpecialColorSelect::textForColorIdx(colorId);
    }

    m_colorLabel->setString(text.c_str());

    // Toggle controls based on color ID
    if (m_detailColorAction) {
        bool shouldShowControls = (colorId <= 1004) ||
                                  (colorId == 1009) ||
                                  (colorId >= 1009 && (colorId <= 1014));
        toggleControls(shouldShowControls);
    }
}

void ColorSelectLiveOverlay::updateColorValue() {
    if (m_updateColor) {
        return;
    }

    if (m_effectGameObject) {
        m_effectGameObject->m_triggerTargetColor = m_targetColor;
    } else {
        ColorAction* targetAction = m_detailColorSelected ? m_detailColorAction : m_baseColorAction;
        if (targetAction) {
            targetAction->m_fromColor = m_targetColor;
        }
    }
}

void ColorSelectLiveOverlay::updateOpacity() {
    if (m_updateColor) {
        return;
    }

    if (m_effectGameObject) {
        m_effectGameObject->m_opacity = m_opacity;
    } else {
        ColorAction* targetAction = m_detailColorSelected ? m_detailColorAction : m_baseColorAction;
        if (targetAction) {
            targetAction->m_fromOpacity = m_opacity;
        }
    }
}

ColorSelectLiveOverlay::~ColorSelectLiveOverlay() {
    // Destructor
}