#include "ColorSelectPopup.h"
#include "ColorAction.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "GJEffectManager.h"
#include "GJSpecialColorSelect.h"
#include "ConfigureHSVWidget.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "CCTextInputNode.h"
#include "TextAlertPopup.h"
#include "GameToolbox.h"
#include "cocos2d.h"

void ColorSelectPopup::closeColorSelect(cocos2d::CCObject* sender) {
    if (m_colorInput) {
        m_colorInput->m_delegate = nullptr;
        m_colorInput->onClickTrackNode(false);
    }
    if (m_fadeTimeInput) {
        m_fadeTimeInput->m_delegate = nullptr;
        m_fadeTimeInput->onClickTrackNode(false);
    }
    m_copyColorInput->m_delegate = nullptr;
    m_copyColorInput->onClickTrackNode(false);
    m_rInput->m_delegate = nullptr;
    m_rInput->onClickTrackNode(false);
    m_gInput->m_delegate = nullptr;
    m_gInput->onClickTrackNode(false);
    m_bInput->m_delegate = nullptr;
    m_bInput->onClickTrackNode(false);
    m_hexInput->m_delegate = nullptr;
    m_hexInput->onClickTrackNode(false);

    if (!m_hsvWidget->m_mixed) {
        m_hsv = m_hsvWidget->m_hsv;
        updateHSVValue();
    }

    GameManager::sharedState()->m_levelEditorLayer->resetEffectTriggerOptim(m_gameObject, m_gameObjects);
    this->setKeypadEnabled(false);

    if (m_delegate) {
        m_delegate->colorSelectClosed(this);
    }

    this->removeFromParentAndCleanup(true);
}

void ColorSelectPopup::colorSelectClosed(GJSpecialColorSelect* select, int id) {
    if (id > 999) {
        if (select->getTag() == 6) {
            m_disableTextDelegate = true;
            m_colorID = id;
            m_colorInput->setString("0");
            m_colorInput->updateLabel(GJSpecialColorSelect::textForColorIdx(id));
            updateCustomColorIdx();
            SetupTriggerPopup::updateEditorLabel();
            m_disableTextDelegate = false;
        } else if (select->getTag() == 7) {
            m_disableTextDelegate = true;
            m_copyColorID = id;
            m_copyColorInput->setString("0");
            m_copyColorInput->updateLabel(GJSpecialColorSelect::textForColorIdx(id));
            updateCopyColor();
            m_disableTextDelegate = false;
        }
    }
}

void ColorSelectPopup::colorValueChanged(cocos2d::ccColor3B color) {
    if (m_currentColorSprite) {
        m_currentColor = getColorValue();
        m_currentColorSprite->setColor(m_currentColor);
        m_originalColorSprite->setColor(m_originalColor);
        updateColorValue();
        updateColorLabels();
    }
}

ColorSelectPopup* ColorSelectPopup::create(ColorAction* action) {
    ColorSelectPopup* popup = new ColorSelectPopup();
    if (popup && popup->init(action, nullptr, nullptr, nullptr)) {
        popup->autorelease();
        return popup;
    }
    delete popup;
    return nullptr;
}

void ColorSelectPopup::determineStartValues() {
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject) {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects && m_gameObjects->count() > 0) {
        float fadeTime = -1.0f;
        float opacity = -1.0f;
        m_blending = true;
        m_copyOpacity = true;
        m_backgroundTrigger = true;
        bool firstColor = true;
        m_tintGround = true;
        bool firstHsv = true;
        m_legacyHSV = true;
        m_playerColor = -1;
        bool playerColorMixed = false;
        m_currentColor.r = -1;
        bool colorMixed = false;
        m_currentColor.g = -1;
        m_currentColor.b = -1;
        m_colorID = -1;
        m_copyColorID = -1;
        m_opacity = 1.0f;
        m_hsv.h = 0.0f;
        m_hsv.s = 0.0f;
        m_hsv.v = 0.0f;
        m_hsv.absoluteSaturation = false;
        m_hsv.absoluteBrightness = false;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);

            // Copy opacity
            if (!obj[28].m_copyOpacity) {
                m_copyOpacity = false;
            }

            // Legacy HSV
            if (!obj[28].m_legacyHSV) {
                m_legacyHSV = false;
            }

            // Player color
            if (!playerColorMixed) {
                if (m_playerColor == -1) {
                    if (obj[26].m_usesPlayerColor1) {
                        m_playerColor = 1;
                    } else if (obj[26].m_usesPlayerColor2) {
                        m_playerColor = 2;
                    } else {
                        m_playerColor = 0;
                    }
                } else if ((obj[26].m_usesPlayerColor1 && m_playerColor != 1) ||
                           (obj[26].m_usesPlayerColor2 && m_playerColor != 2)) {
                    m_playerColor = 0;
                    playerColorMixed = true;
                }
            }

            // Background trigger
            if (obj[18].m_objectID == 29) {
                if (m_tintGround) {
                    m_tintGround = obj[26].m_tintGround;
                }
            } else {
                m_backgroundTrigger = false;
            }

            // Blending
            if (m_blending) {
                m_blending = obj[26].m_usesBlending;
            }

            // Color trigger
            if (!m_colorTrigger) {
                m_colorTrigger = (obj[18].m_objectID == 899 || obj[18].m_objectID == 744);
            }

            // New color trigger
            if (!m_newColorTrigger) {
                m_newColorTrigger = (obj[18].m_objectID == 899);
            }

            // Fade time
            if (fadeTime == -1.0f) {
                fadeTime = obj[25].m_fadeTime;
            } else if (fadeTime != obj[25].m_fadeTime) {
                fadeTime = -2.0f;
            }

            // Opacity
            if (opacity == -1.0f) {
                opacity = obj[25].m_opacity;
            } else if (opacity != obj[25].m_opacity) {
                opacity = -2.0f;
            }

            // Current color
            if (!colorMixed) {
                if (firstColor) {
                    m_currentColor = obj[25].m_currentColor;
                    firstColor = false;
                } else if (m_currentColor.r != obj[25].m_currentColor.r ||
                           m_currentColor.g != obj[25].m_currentColor.g ||
                           m_currentColor.b != obj[25].m_currentColor.b) {
                    m_currentColor.r = -1;
                    m_currentColor.g = -1;
                    m_currentColor.b = -1;
                    colorMixed = true;
                }
            }

            // HSV
            if (!playerColorMixed) {
                if (firstHsv) {
                    m_hsv = obj[28].m_hsv;
                    firstHsv = false;
                } else if (m_hsv.h != obj[28].m_hsv.h ||
                           m_hsv.s != obj[28].m_hsv.s ||
                           m_hsv.v != obj[28].m_hsv.v ||
                           m_hsv.absoluteSaturation != obj[28].m_hsv.absoluteSaturation ||
                           m_hsv.absoluteBrightness != obj[28].m_hsv.absoluteBrightness) {
                    m_hsv.h = -0.001f;
                    m_hsv.s = 0.0f;
                    m_hsv.v = 0.0f;
                    m_hsv.absoluteSaturation = false;
                    m_hsv.absoluteBrightness = false;
                    playerColorMixed = true;
                }
            }

            // Color ID
            if (m_colorID == -1) {
                m_colorID = obj[18].m_targetColor;
            } else if (m_colorID != obj[18].m_targetColor) {
                m_colorID = 0;
            }

            // Copy color ID
            int copyId = obj[28].m_copyColorID;
            if (m_copyColorID == -1) {
                m_copyColorID = copyId;
            } else if (m_copyColorID != copyId) {
                m_copyColorID = 0;
            }
        }

        if (fadeTime >= 0.0f) {
            m_fadeTime = fadeTime;
        }
        if (opacity >= 0.0f) {
            m_opacity = opacity;
        }
    } else if (m_colorAction) {
        m_backgroundTrigger = false;
        m_blending = m_colorAction->m_blending;
        m_playerColor = m_colorAction->m_playerColor;
        m_opacity = m_colorAction->m_fromOpacity;
        m_colorID = m_colorAction->m_colorID;
        m_colorTrigger = (m_colorID >= 1 && m_colorID <= 999) || (m_colorID >= 1002 && m_colorID <= 1003);
        m_noChannel = (m_colorID == 0);
        m_currentColor = m_colorAction->m_fromColor;
        m_fadeTime = 0.0f;
        m_copyColorID = m_colorAction->m_copyID;
        m_copyOpacity = m_colorAction->m_copyOpacity;
        m_hsv = m_colorAction->m_copyHSV;
    }

    m_copyID = m_copyColorID;
    m_colorTrigger = true;
    m_showCopyObjects = (m_copyID != 0);
}

cocos2d::ccColor3B ColorSelectPopup::getColorValue() {
    return m_colorPicker->m_rgb;
}

cocos2d::ccColor3B ColorSelectPopup::hexToColor(std::string hex) {
    // Convert hex string to uppercase
    for (char& c : hex) {
        c = toupper(c);
    }

    // Parse hex values
    int r = 0, g = 0, b = 0;
    const char* hexStr = hex.c_str();

    if (strlen(hexStr) == 6) {
        char hexByte[3] = {0, 0, 0};
        hexByte[0] = hexStr[0];
        hexByte[1] = hexStr[1];
        r = strtol(hexByte, nullptr, 16);
        hexByte[0] = hexStr[2];
        hexByte[1] = hexStr[3];
        g = strtol(hexByte, nullptr, 16);
        hexByte[0] = hexStr[4];
        hexByte[1] = hexStr[5];
        b = strtol(hexByte, nullptr, 16);
    }

    return cocos2d::ccc3(r, g, b);
}

void ColorSelectPopup::keyBackClicked() {
    closeColorSelect(nullptr);
}

void ColorSelectPopup::onCopy(cocos2d::CCObject* sender) {
    GameManager::sharedState()->m_copiedColor = getColorValue();
}

void ColorSelectPopup::onCopyOpacity(cocos2d::CCObject* sender) {
    m_copyOpacity = !m_copyOpacity;

    if (m_gameObject) {
        m_gameObject->m_copyOpacity = m_copyOpacity;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[28].m_copyOpacity = m_copyOpacity;
        }
    } else if (m_colorAction) {
        m_colorAction->m_copyOpacity = m_copyOpacity;
    }
}

void ColorSelectPopup::onDefault(cocos2d::CCObject* sender) {
    LevelEditorLayer* layer = GameManager::sharedState()->m_levelEditorLayer;
    if (!layer) {
        return;
    }

    if (m_gameObject) {
        ColorAction* action = layer->m_levelSettings->m_effectManager->getColorAction(m_gameObject->m_targetColor);
        if (action) {
            selectColor(action->m_fromColor);
            if (m_blending != action->m_blending) {
                onToggleTintMode(nullptr);
            }
            m_opacity = action->m_fromOpacity;
            updateOpacity();
            updateOpacityLabel();
            if (m_opacitySlider) {
                m_opacitySlider->setValue(m_opacity);
            }
        }
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            ColorAction* action = layer->m_levelSettings->m_effectManager->getColorAction(obj[18].m_targetColor);
            if (action) {
                obj[25].m_currentColor = action->m_fromColor;
                obj[26].m_usesBlending = action->m_blending;
                obj[25].m_opacity = action->m_fromOpacity;
                m_opacity = action->m_fromOpacity;
            }
        }

        TextAlertPopup* alert = TextAlertPopup::create("Defaults Loaded", 0.5f, 0.6f, 150, "");
        this->addChild(alert, 100);
    }
}

void ColorSelectPopup::onHSVLegacyMode(cocos2d::CCObject* sender) {
    m_legacyHSV = !m_legacyHSV;

    if (m_gameObject) {
        m_gameObject->m_legacyHSV = m_legacyHSV;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[28].m_legacyHSV = m_legacyHSV;
        }
    }
}

void ColorSelectPopup::onPaste(cocos2d::CCObject* sender) {
    selectColor(GameManager::sharedState()->m_copiedColor);
}

void ColorSelectPopup::onPlayerColor1(cocos2d::CCObject* sender) {
    m_playerColor2Toggle->toggle(false);
    m_playerColor = 1;

    if (m_gameObject) {
        m_gameObject->m_usesPlayerColor1 = true;
        m_gameObject->m_usesPlayerColor2 = false;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[26].m_usesPlayerColor1 = true;
            obj[26].m_usesPlayerColor2 = false;
        }
    } else if (m_colorAction) {
        m_colorAction->m_playerColor = 1;
    }
}

void ColorSelectPopup::onPlayerColor2(cocos2d::CCObject* sender) {
    m_playerColor1Toggle->toggle(false);
    m_playerColor = 2;

    if (m_gameObject) {
        m_gameObject->m_usesPlayerColor1 = false;
        m_gameObject->m_usesPlayerColor2 = true;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[26].m_usesPlayerColor1 = false;
            obj[26].m_usesPlayerColor2 = true;
        }
    } else if (m_colorAction) {
        m_colorAction->m_playerColor = 2;
    }
}

void ColorSelectPopup::onSelectSpecialColor(cocos2d::CCObject* sender) {
    GJSpecialColorSelect* select = GJSpecialColorSelect::create(m_copyColorID, this, true);
    select->setTag(sender->getTag());
    select->show();
}

void ColorSelectPopup::onTintGround(cocos2d::CCObject* sender) {
    m_tintGround = !m_tintGround;

    if (m_gameObject) {
        m_gameObject->m_tintGround = m_tintGround;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[26].m_tintGround = m_tintGround;
        }
    }
}

void ColorSelectPopup::onToggleHSVMode(cocos2d::CCObject* sender) {
    m_copyColorID = 0;
    m_showCopyObjects = !m_showCopyObjects;
    updateCopyColorTextInputLabel();
    updateCopyColor();
    updateHSVMode();
}

void ColorSelectPopup::onToggleTintMode(cocos2d::CCObject* sender) {
    m_blending = !m_blending;

    if (m_gameObject) {
        m_gameObject->m_usesBlending = m_blending;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[26].m_usesBlending = m_blending;
        }
    } else if (m_colorAction) {
        m_colorAction->m_blending = m_blending;
    }
}

void ColorSelectPopup::onUpdateCopyColor(cocos2d::CCObject* sender) {
    if (m_copyColorID > 999) {
        m_copyColorID = 0;
    }

    if (sender->getTag() == 1) {
        m_copyColorID--;
    } else {
        m_copyColorID++;
    }

    if (m_copyColorID < 0) m_copyColorID = 0;
    if (m_copyColorID > 999) m_copyColorID = 999;

    updateCopyColor();
    updateCopyColorTextInputLabel();
}

void ColorSelectPopup::onUpdateCustomColor(cocos2d::CCObject* sender) {
    if (m_colorID > 999) {
        m_colorID = 0;
    }

    if (sender->getTag() == 1) {
        m_colorID++;
    } else {
        m_colorID--;
    }

    if (m_colorID < 0) m_colorID = 0;
    if (m_colorID > 999) m_colorID = 999;

    updateCustomColorIdx();
    SetupTriggerPopup::updateEditorLabel();
    updateTextInputLabel();
}

void ColorSelectPopup::show() {
    m_initializing = true;
    m_originalColor = getColorValue();
    colorValueChanged(m_originalColor);
    m_initializing = false;
    SetupTriggerPopup::show();
}

void ColorSelectPopup::sliderChanged(cocos2d::CCObject* sender) {
    float value = SliderThumb::getValue(static_cast<SliderThumb*>(sender));

    if (sender->getTag() == 1) {
        m_fadeTime = value * 10.0f;
        updateDuration();
        updateDurLabel();
    } else if (sender->getTag() == 2) {
        m_opacity = value;
        if (m_opacity < 0.02f) m_opacity = 0.0f;
        if (m_opacity > 0.98f) m_opacity = 1.0f;
        updateOpacity();
        updateOpacityLabel();
    }
}

void ColorSelectPopup::textChanged(CCTextInputNode* node) {
    if (m_disableTextDelegate) {
        return;
    }

    int tag = node->getTag();

    switch (tag) {
        case 3: { // Color ID
            std::string text = node->getString();
            m_colorID = atoi(text.c_str());
            if (m_colorID < 0) m_colorID = 0;
            if (m_colorID > 999) m_colorID = 999;
            updateCustomColorIdx();
            SetupTriggerPopup::updateEditorLabel();
            break;
        }
        case 4: { // Copy Color ID
            std::string text = node->getString();
            m_copyColorID = atoi(text.c_str());
            if (m_copyColorID < 0) m_copyColorID = 0;
            if (m_copyColorID > 999) m_copyColorID = 999;
            updateCopyColor();
            break;
        }
        case 5: { // Fade Time
            std::string text = node->getString();
            m_fadeTime = atof(text.c_str());
            updateDuration();
            float sliderValue = m_fadeTime / 10.0f;
            if (sliderValue < 0.0f) sliderValue = 0.0f;
            if (sliderValue > 1.0f) sliderValue = 1.0f;
            m_fadeTimeSlider->setValue(sliderValue);
            break;
        }
        case 8: { // R
            std::string text = node->getString();
            int val = atoi(text.c_str());
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            m_currentColor.r = val;
            selectColor(m_currentColor);
            break;
        }
        case 9: { // G
            std::string text = node->getString();
            int val = atoi(text.c_str());
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            m_currentColor.g = val;
            selectColor(m_currentColor);
            break;
        }
        case 10: { // B
            std::string text = node->getString();
            int val = atoi(text.c_str());
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            m_currentColor.b = val;
            selectColor(m_currentColor);
            break;
        }
        case 11: { // Hex
            std::string text = node->getString();
            if (text.length() == 6) {
                m_currentColor = hexToColor(text);
                selectColor(m_currentColor);
            }
            break;
        }
        default:
            break;
    }
}

void ColorSelectPopup::updateColorLabels() {
    m_disableTextDelegate = true;

    m_rInput->setString(std::to_string(m_currentColor.r));
    m_gInput->setString(std::to_string(m_currentColor.g));
    m_bInput->setString(std::to_string(m_currentColor.b));

    // Update hex
    char hexStr[7];
    sprintf(hexStr, "%02X%02X%02X", m_currentColor.r, m_currentColor.g, m_currentColor.b);
    m_hexInput->setString(hexStr);

    m_disableTextDelegate = false;
}

void ColorSelectPopup::updateColorValue() {
    if (m_initializing) {
        return;
    }

    if (m_gameObject) {
        m_gameObject->m_triggerTargetColor = m_currentColor;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[25].m_currentColor = m_currentColor;
        }
    } else if (m_colorAction) {
        m_colorAction->m_fromColor = m_currentColor;
    }
}

void ColorSelectPopup::updateCopyColor() {
    if (m_gameObject) {
        if (m_copyColorID == m_gameObject->getTargetColorIndex()) {
            m_copyColorID = 0;
        }
        m_gameObject->m_copyColorID = m_copyColorID;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            int copyId = m_copyColorID;
            if (copyId == obj->getTargetColorIndex()) {
                copyId = 0;
            }
            obj->m_copyColorID = copyId;
        }
    } else if (m_colorAction) {
        int copyId = m_copyColorID;
        if (copyId == m_colorAction->m_colorID) {
            copyId = 0;
        }
        m_colorAction->m_copyID = copyId;
    }
}

void ColorSelectPopup::updateCopyColorTextInputLabel() {
    m_disableTextDelegate = true;

    if (m_copyColorID > 999) {
        m_copyColorInput->setString("0");
        m_copyColorInput->updateLabel(GJSpecialColorSelect::textForColorIdx(m_copyColorID));
    } else {
        m_copyColorInput->setString(std::to_string(m_copyColorID));
    }

    m_disableTextDelegate = false;
}

void ColorSelectPopup::updateCustomColorIdx() {
    if (m_colorID <= 0) {
        m_colorID = 1;
    }

    if (m_gameObject) {
        m_gameObject->m_targetColor = m_colorID;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[18].m_targetColor = m_colorID;
        }
    }
}

void ColorSelectPopup::updateDurLabel() {
    m_disableTextDelegate = true;

    if (m_fadeTime == -99999.0f) {
        m_fadeTimeInput->setString("Mixed");
    } else {
        CCString* str;
        if (m_fadeTime == floorf(m_fadeTime)) {
            str = CCString::createWithFormat("%i", (int)m_fadeTime);
        } else {
            str = CCString::createWithFormat("%.02f", m_fadeTime);
        }
        m_fadeTimeInput->setString(str->getCString());
    }

    m_disableTextDelegate = false;
}

void ColorSelectPopup::updateDuration() {
    if (m_gameObject) {
        m_gameObject->m_duration = m_fadeTime;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[25].m_fadeTime = m_fadeTime;
        }
    }
}

void ColorSelectPopup::updateHSVMode() {
    for (unsigned int i = 0; i < m_colorObjects->count(); i++) {
        cocos2d::CCObject* obj = m_colorObjects->objectAtIndex(i);
        obj->setVisible(!m_showCopyObjects);
    }

    for (unsigned int i = 0; i < m_copyObjects->count(); i++) {
        cocos2d::CCObject* obj = m_copyObjects->objectAtIndex(i);
        obj->setVisible(m_showCopyObjects);
    }

    m_copyColorInput->setTouchEnabled(m_showCopyObjects);
    m_rInput->setTouchEnabled(!m_showCopyObjects);
    m_gInput->setTouchEnabled(!m_showCopyObjects);
    m_bInput->setTouchEnabled(!m_showCopyObjects);
    m_hexInput->setTouchEnabled(!m_showCopyObjects);
}

void ColorSelectPopup::updateHSVValue() {
    if (m_gameObject) {
        m_gameObject->m_hsvValue = m_hsv;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[28].m_hsv = m_hsv;
        }
    } else if (m_colorAction) {
        m_colorAction->m_copyHSV = m_hsv;
    }
}

void ColorSelectPopup::updateOpacity() {
    if (m_gameObject) {
        m_gameObject->m_opacity = m_opacity;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++) {
            cocos2d::CCObject* obj = m_gameObjects->objectAtIndex(i);
            obj[25].m_opacity = m_opacity;
        }
    } else if (m_colorAction) {
        m_colorAction->m_fromOpacity = m_opacity;
    }
}

void ColorSelectPopup::updateOpacityLabel() {
    if (m_opacityLabel) {
        CCString* str = CCString::createWithFormat("Opacity: %.02f", m_opacity);
        m_opacityLabel->setString(str->getCString());
    }
}

void ColorSelectPopup::updateTextInputLabel() {
    m_disableTextDelegate = true;

    if (m_colorID > 999) {
        m_colorInput->setString("0");
        m_colorInput->updateLabel(GJSpecialColorSelect::textForColorIdx(m_colorID));
    } else {
        m_colorInput->setString(std::to_string(m_colorID));
    }

    m_disableTextDelegate = false;
}

ColorSelectPopup::~ColorSelectPopup() {
    // Destructor
}