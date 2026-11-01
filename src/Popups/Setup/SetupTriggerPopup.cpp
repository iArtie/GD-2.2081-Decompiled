#include "Popups/Setup/SetupTriggerPopup.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/LevelEditorLayer.h"
#include "UI/InfoAlertButton.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCMenuItemToggler.h"
#include "Cocos/CCTextInputNode.h"
#include "Popups/ConfigureValuePopup.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"
#include "Objects/EffectGameObject.h"
#include "Utils/GJEnums.h"
#include "Platform/cocos2d.h"
#include <string>

void SetupTriggerPopup::addCloseButton(std::string text) {
    ButtonSprite* sprite = ButtonSprite::create(text.c_str(), 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(sprite, nullptr, this, menu_selector(SetupTriggerPopup::onClose));
    m_buttonMenu->addChild(item);
}

void SetupTriggerPopup::addHelpButton(std::string title, std::string desc, float scale) {
    InfoAlertButton* button = InfoAlertButton::create(title, desc, 1.0f);
    button->m_textScale = scale;
    m_buttonMenu->addChild(button);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint worldPos(
        18.0f - (m_width * 0.5f - winSize.width * 0.5f),
        m_height * 0.5f + winSize.height * 0.5f - 18.0f
    );
    button->setPosition(m_buttonMenu->convertToNodeSpace(worldPos));
}

void SetupTriggerPopup::addObjectToGroup(cocos2d::CCObject* object, int group) {
    cocos2d::CCArray* container = getGroupContainer(group);
    container->addObject(object);
}

void SetupTriggerPopup::addObjectToPage(cocos2d::CCObject* object, int page) {
    cocos2d::CCArray* container = getPageContainer(page);
    container->addObject(object);
}

void SetupTriggerPopup::addObjectsToGroup(cocos2d::CCArray* objects, int group) {
    cocos2d::CCArray* container = getGroupContainer(group);
    container->addObjectsFromArray(objects);
}

void SetupTriggerPopup::addObjectsToPage(cocos2d::CCArray* objects, int page) {
    cocos2d::CCArray* container = getPageContainer(page);
    container->addObjectsFromArray(objects);
}

void SetupTriggerPopup::addTitle(std::string title) {
    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(label);
    label->setScale(0.5f);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    label->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f,
        m_height * 0.5f + winSize.height * 0.5f - 14.0f
    ));
}

void SetupTriggerPopup::closeInputNodes() {
    if (m_inputNodes) {
        cocos2d::CCDictElement* element = m_inputNodes->m_pElements;
        while (element) {
            cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(element->m_pObject);
            if (node) {
                node->m_delegate = nullptr;
                node->onClickTrackNode(false);
            }
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
        }
    }
}

SetupTriggerPopup* SetupTriggerPopup::create(float width, float height) {
    SetupTriggerPopup* instance = new SetupTriggerPopup();
    if (instance && instance->init(nullptr, nullptr, false, width, height, 0)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}


void SetupTriggerPopup::createMultiTriggerItems(cocos2d::CCPoint corner, cocos2d::CCPoint p1, cocos2d::CCPoint p2) {
    m_touchToggle = GameToolbox::createToggleButton(
        "Touch\nTrigger",
        menu_selector(SetupTriggerPopup::onTouchTriggered),
        0,
        m_touchTriggered,
        m_buttonMenu,
        corner,
        this,
        m_mainLayer,
        0.7f,
        0.35f,
        110.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt",
        false,
        false,
        nullptr
    );

    m_spawnToggle = GameToolbox::createToggleButton(
        "Spawn\nTrigger",
        menu_selector(SetupTriggerPopup::onSpawnedByTrigger),
        0,
        m_spawnTriggered,
        m_buttonMenu,
        p1,
        this,
        m_mainLayer,
        0.7f,
        0.35f,
        110.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt",
        false,
        false,
        nullptr
    );

    GameToolbox::createToggleButton(
        "Multi\nTrigger",
        menu_selector(SetupTriggerPopup::onMultiTrigger),
        0,
        m_multiTriggered,
        m_buttonMenu,
        p2,
        this,
        m_mainLayer,
        0.7f,
        0.35f,
        110.0f,
        cocos2d::CCPoint(8.0f, 0.0f),
        "bigFont.fnt",
        false,
        false,
        m_multiTriggerContainer
    );

    updateMultiTriggerBtn();
}

cocos2d::CCArray* SetupTriggerPopup::createValueControlAdvanced(
    int property,
    const std::string& label,
    cocos2d::CCPoint offset,
    float maxValue,
    bool limit,
    InputValueType type,
    int decimals,
    int buttonType,
    float sliderMin,
    float sliderMax,
    bool useInput,
    bool enable,
    GJInputStyle style,
    int labelTag,
    int triggerTag)
{
    int decimalLimit = 0;
    if (type != InputValueType::Float) {
        decimalLimit = (decimals >= 4) ? 4 : decimals;
    }

    cocos2d::CCArray* result = cocos2d::CCArray::create();
    cocos2d::CCArray* pageContainer = getPageContainer(buttonType);
    cocos2d::CCArray* groupContainer = getGroupContainer(labelTag);

    if (enable) {
        offset.x -= sliderMin * 70.0f;
    }

    cocos2d::CCPoint labelPos = offset + cocos2d::CCPoint(sliderMin * 45.0f, 0.0f);
    cocos2d::CCPoint inputPos = offset;
    cocos2d::CCPoint anchor(1.0f, 0.5f);

    if (useInput) {
        if (enable) {
            inputPos = offset + cocos2d::CCPoint(sliderMin * 128.0f, 0.0f);
            labelPos = offset;
            anchor = cocos2d::CCPoint(0.0f, 0.5f);
        } else {
            labelPos = offset;
            inputPos = offset + cocos2d::CCPoint(0.0f, sliderMin * 25.0f);
            anchor = cocos2d::CCPoint(0.5f, 0.0f);
        }
    }

    cocos2d::CCLabelBMFont* labelNode = cocos2d::CCLabelBMFont::create(label.c_str(), "bigFont.fnt");
    float scale = enable ? 1.0f : 0.7f;
    labelNode->setScale(scale * sliderMin);
    labelNode->setPosition(labelPos);
    labelNode->setAnchorPoint(anchor);
    m_mainLayer->addChild(labelNode);
    pageContainer->addObject(labelNode);
    if (labelTag > 0) {
        groupContainer->addObject(labelNode);
    }
    result->addObject(labelNode);

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    m_inputLabels->setObject(labelNode, key->getCString());

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    bg->setOpacity(100);
    bg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(bg, -1);
    pageContainer->addObject(bg);
    if (labelTag > 0) {
        groupContainer->addObject(bg);
    }
    result->addObject(bg);
    bg->setPosition(labelPos);
    bg->setAnchorPoint(anchor);

    CCTextInputNode* input = CCTextInputNode::create(sliderMin * 60.0f, sliderMin * 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(input, 20);
    pageContainer->addObject(input);
    if (labelTag > 0) {
        groupContainer->addObject(input);
    }
    result->addObject(input);
    input->setPosition(bg->getPosition());
    input->setMaxLabelScale(sliderMin * 0.6f);
    input->setMaxLabelWidth(sliderMin * 50.0f);
    input->m_textField->setVisible(false);
    input->m_maxLabelLength = static_cast<int>(inputPos.x) + decimals;
    input->m_valueType = type;
    input->m_decimalPlaces = decimalLimit;

    if (type == InputValueType::Int) {
        input->setAllowedChars("0123456789-");
    } else if (type == InputValueType::Uint) {
        input->setAllowedChars("0123456789");
    } else {
        input->setAllowedChars("0123456789-.");
    }

    input->setLabelPlaceholderColor(cocos2d::ccc3(88, 88, 240));
    input->setLabelPlaceholderScale(0.5f);
    input->m_numberInput = true;
    input->m_delegate = this;
    input->setTag(property);
    m_inputNodes->setObject(input, key->getCString());

    if (!useInput) {
        Slider* slider = Slider::create(this, menu_selector(SetupTriggerPopup::triggerSliderChanged));
        slider->m_delegate = this;
        slider->setBarVisibility(false);
        m_mainLayer->addChild(slider);
        pageContainer->addObject(slider);
        if (labelTag > 0) {
            groupContainer->addObject(slider);
        }
        result->addObject(slider);

        if (enable) {
            slider->setPosition(offset + cocos2d::CCPoint(sliderMin * 145.0f, 0.0f));
            slider->setMaxOffset(100.0f);
            slider->m_groove->setScaleX(0.7f);
        } else {
            slider->setPosition(offset + cocos2d::CCPoint(0.0f, sliderMin * -32.0f));
        }

        slider->setTag(property);
        slider->getThumb()->setTag(property);
        m_sliders->setObject(slider, key->getCString());
    }

    setMinSliderValue(sliderMin, property);
    if (sliderMax != 0.0f) {
        setMaxSliderValue(sliderMax, property);
    }

    if (limit) {
        cocos2d::CCPoint localPos = m_buttonMenu->convertToNodeSpace(input->getPosition());

        cocos2d::CCSprite* leftSpr = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        leftSpr->setScale(sliderMin * 1.2f);
        CCMenuItemSpriteExtra* leftBtn = CCMenuItemSpriteExtra::create(leftSpr, nullptr, this,
            menu_selector(SetupTriggerPopup::triggerArrowLeft));
        m_buttonMenu->addChild(leftBtn);
        pageContainer->addObject(leftBtn);
        if (labelTag > 0) {
            groupContainer->addObject(leftBtn);
        }
        result->addObject(leftBtn);
        leftBtn->setTag(property);
        leftBtn->setPosition(localPos + cocos2d::CCPoint(-(sliderMin * 50.0f * 1.2f), 0.0f));
        leftBtn->setSizeMult(2.0f);

        cocos2d::CCSprite* rightSpr = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        rightSpr->setScale(sliderMin * 1.2f);
        CCMenuItemSpriteExtra* rightBtn = CCMenuItemSpriteExtra::create(rightSpr, nullptr, this,
            menu_selector(SetupTriggerPopup::triggerArrowRight));
        m_buttonMenu->addChild(rightBtn);
        pageContainer->addObject(rightBtn);
        if (labelTag > 0) {
            groupContainer->addObject(rightBtn);
        }
        result->addObject(rightBtn);
        rightBtn->setTag(property);
        rightBtn->setPosition(localPos + cocos2d::CCPoint(sliderMin * 50.0f * 1.2f, 0.0f));
        rightBtn->setSizeMult(2.0f);
    }

    if (triggerTag) {
        cocos2d::CCSprite* resetSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
        resetSpr->setScale(sliderMin);
        CCMenuItemSpriteExtra* resetBtn = CCMenuItemSpriteExtra::create(resetSpr, nullptr, this,
            menu_selector(SetupTriggerPopup::onDisableValue));
        resetBtn->setTag(property);
        m_disableButtons->setObject(resetBtn, key->getCString());
        m_buttonMenu->addChild(resetBtn);

        cocos2d::CCPoint resetLocal = m_buttonMenu->convertToNodeSpace(
            offset + cocos2d::CCPoint(sliderMin * 230.0f, 0.0f));
        resetBtn->setPosition(resetLocal);

        addObjectToPage(resetBtn, buttonType);
        if (labelTag > 0) {
            addObjectToGroup(resetBtn, labelTag);
        }
    }

    return result;
}

void SetupTriggerPopup::createMultiTriggerItemsCorner() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint corner(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f),
        20.0f - (m_height * 0.5f - winSize.height * 0.5f)
    );

    cocos2d::CCPoint p1 = corner + cocos2d::CCPoint(0.0f, 30.0f);
    cocos2d::CCPoint p2 = corner;
    cocos2d::CCPoint p3 = corner + cocos2d::CCPoint(90.0f, 0.0f);

    createMultiTriggerItems(p1, p2, p3);
}

void SetupTriggerPopup::createMultiTriggerItemsDefault() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint corner(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f),
        20.0f - (m_height * 0.5f - winSize.height * 0.5f)
    );

    cocos2d::CCPoint p1 = corner + cocos2d::CCPoint(0.0f, 30.0f);
    cocos2d::CCPoint p2 = corner;
    cocos2d::CCPoint p3(
        m_width * 0.5f + winSize.width * 0.5f - 72.0f,
        corner.y
    );

    createMultiTriggerItems(p1, p2, p3);
}

void SetupTriggerPopup::createMultiTriggerItemsDefaultHorizontal() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint corner(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f),
        20.0f - (m_height * 0.5f - winSize.height * 0.5f)
    );

    cocos2d::CCPoint p1 = corner + cocos2d::CCPoint(90.0f, 0.0f);
    cocos2d::CCPoint p2 = corner;
    cocos2d::CCPoint p3(
        m_width * 0.5f + winSize.width * 0.5f - 72.0f,
        corner.y
    );

    createMultiTriggerItems(p1, p2, p3);
}

void SetupTriggerPopup::createMultiTriggerItemsDefaultVertical() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint corner(
        20.0f - (m_width * 0.5f - winSize.width * 0.5f),
        20.0f - (m_height * 0.5f - winSize.height * 0.5f)
    );

    cocos2d::CCPoint p1 = corner + cocos2d::CCPoint(0.0f, 30.0f);
    cocos2d::CCPoint p2 = corner;
    cocos2d::CCPoint p3 = corner + cocos2d::CCPoint(0.0f, 60.0f);

    createMultiTriggerItems(p1, p2, p3);
}

void SetupTriggerPopup::createPageButtons(float offset, int arrow) {
    std::string frame;
    int arrowIdx = 0;

    if (arrow < 0) {
        frame = "edit_leftBtn_001.png";
        arrowIdx = 0;
    } else if (arrow == 0) {
        frame = "edit_leftBtn_001.png";
        arrowIdx = 0;
    } else {
        arrowIdx = (arrow > 3) ? 3 : arrow;
        frame = cocos2d::CCString::createWithFormat("GJ_arrow_0%i_001.png", arrowIdx)->getCString();
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    float scale = (arrowIdx != 0) ? 0.7f : 1.0f;

    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
    prevSprite->setScale(scale);
    m_prevButton = CCMenuItemSpriteExtra::create(prevSprite, nullptr, this, menu_selector(SetupTriggerPopup::onPage));
    m_buttonMenu->addChild(m_prevButton);
    m_prevButton->setTag(-1);
    m_prevButton->setPosition(center + cocos2d::CCPoint(-offset, 0.0f));
    m_prevButton->setSizeMult(2.0f);

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame.c_str());
    nextSprite->setFlipX(true);
    nextSprite->setScale(scale);
    m_nextButton = CCMenuItemSpriteExtra::create(nextSprite, nullptr, this, menu_selector(SetupTriggerPopup::onPage));
    m_buttonMenu->addChild(m_nextButton);
    m_nextButton->setTag(1);
    m_nextButton->setPosition(center + cocos2d::CCPoint(offset, 0.0f));
    m_nextButton->setSizeMult(2.0f);
}

void SetupTriggerPopup::determineStartValues() {
    cocos2d::CCArray* objects = getObjects();
    if (!objects || objects->count() == 0) {
        return;
    }

    m_touchTriggered = true;
    m_spawnTriggered = true;
    m_multiTriggered = true;
    m_effectObjects = true;

    for (unsigned int i = 0; i < objects->count(); ++i) {
        GameObject* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (obj->m_objectType != 1) {
            m_effectObjects = false;
            break;
        }
        if (!obj->m_isTouchTriggered) {
            m_touchTriggered = false;
        }
        if (!obj->m_isSpawnTriggered) {
            m_spawnTriggered = false;
        }
        if (!obj->m_isMultiTriggered) {
            m_multiTriggered = false;
        }
    }
}

cocos2d::CCArray* SetupTriggerPopup::getGroupContainer(int group) {
    while (static_cast<unsigned int>(m_groupContainers->count()) <= group) {
        cocos2d::CCArray* container = cocos2d::CCArray::create();
        m_groupContainers->addObject(container);
    }
    return static_cast<cocos2d::CCArray*>(m_groupContainers->objectAtIndex(group));
}

float SetupTriggerPopup::getMaxSliderValue(int property) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCObject* obj = m_maxSliderValues->objectForKey(key->getCString());
    if (obj) {
        return static_cast<cocos2d::CCString*>(obj)->floatValue();
    }
    return 1.0f;
}

float SetupTriggerPopup::getMinSliderValue(int property) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCObject* obj = m_minSliderValues->objectForKey(key->getCString());
    if (obj) {
        return static_cast<cocos2d::CCString*>(obj)->floatValue();
    }
    return 0.0f;
}

cocos2d::CCArray* SetupTriggerPopup::getObjects() {
    cocos2d::CCArray* result = cocos2d::CCArray::create();
    if (m_gameObject) {
        result->addObject(m_gameObject);
    }
    return result;
}

cocos2d::CCArray* SetupTriggerPopup::getPageContainer(int page) {
    while (static_cast<unsigned int>(m_pageContainers->count()) <= page) {
        cocos2d::CCArray* container = cocos2d::CCArray::create();
        m_pageContainers->addObject(container);
    }
    return static_cast<cocos2d::CCArray*>(m_pageContainers->objectAtIndex(page));
}

float SetupTriggerPopup::getTriggerValue(int property, GameObject* object) {
    EffectGameObject* obj = static_cast<EffectGameObject*>(object);
    switch (property) {
        case 10:  return obj->m_duration;
        case 12:  return static_cast<float>(obj->m_isTouchTriggered);
        case 28:  return obj->m_moveOffset.x;
        case 29:  return obj->m_moveOffset.y;
        case 30:  return static_cast<float>(obj->m_easingType);
        case 51:  return static_cast<float>(obj->m_targetGroupID);
        case 55:  return obj->m_shakeStrength;
        case 56:  return static_cast<float>(obj->m_tintGround);
        case 58:  return static_cast<float>(obj->m_lockToPlayerX);
        case 59:  return static_cast<float>(obj->m_lockToPlayerY);
        case 63:  return obj->m_shakeInterval;
        case 65:  return static_cast<float>(obj->m_usesPlayerColor1);
        case 66:  return static_cast<float>(obj->m_usesPlayerColor2);
        case 68:  return obj->m_moveModX;
        case 69:  return static_cast<float>(obj->m_lockToCameraX);
        case 70:  return obj->m_moveModY;
        case 71:  return static_cast<float>(obj->m_centerGroupID);
        case 76:  return static_cast<float>(obj->m_easingRate);
        case 77:  return static_cast<float>(obj->m_lockToCameraY);
        case 78:  return static_cast<float>(obj->m_usesBlending);
        case 80:  return obj->m_fadeInDuration;
        case 85:  return obj->m_holdDuration;
        case 87:  return obj->m_fadeOutDuration;
        case 88:  return static_cast<float>(obj->m_useMoveTarget);
        case 95:  return static_cast<float>(obj->m_isDirectionFollowSnap360);
        case 97:  return obj->m_directionModeDistance;
        case 98:  return static_cast<float>(obj->m_moveTargetMode);
        case 99:  return obj->m_followXMod;
        case 100: return obj->m_smallStep ? 1.0f : 0.0f;
        case 101: return static_cast<float>(obj->m_isDynamicMode);
        case 102: return static_cast<float>(obj->m_isSilent);
        case 106: return obj->m_rotationDegrees;
        case 107: return obj->m_rotationOffset;
        case 110: return obj->m_followYDelay;
        case 122: return static_cast<float>(obj->m_times360);
        case 123: return obj->m_followYMaxSpeed;
        case 126: return obj->m_followYMod;
        case 127: return obj->m_followYSpeed;
        case 133: return obj->m_followYOffset;
        case 138: return static_cast<float>(obj->m_isDynamicBlock);
        case 139: return static_cast<float>(obj->m_targetPlayer1);
        case 141: return obj->m_easingRate;
        case 142: return obj->m_cameraEasingValue;
        case 143: return obj->m_cameraPaddingValue;
        case 144: return obj->m_gravityValue;
        case 148: return obj->m_zoomValue;
        case 149: return static_cast<float>(obj->m_targetPlayer2);
        case 150: return static_cast<float>(obj->m_followCPP);
        case 151: return static_cast<float>(obj->m_legacyHSV);
        case 153: return static_cast<float>(obj->m_pulseMode);
        case 154: return static_cast<float>(obj->m_pulseTargetType);
        case 159: return static_cast<float>(obj->m_pulseMainOnly);
        case 160: return static_cast<float>(obj->m_pulseDetailOnly);
        case 161: return static_cast<float>(obj->m_pulseExclusive);
        case 162: return static_cast<float>(obj->m_copyColorID);
        case 163: return obj->m_hsvValue.h;
        case 165: return obj->m_hsvValue.s;
        case 166: return obj->m_hsvValue.v;
        case 167: return static_cast<float>(obj->m_hsvValue.absoluteSaturation);
        case 169: return static_cast<float>(obj->m_copyOpacity);
        case 171: return static_cast<float>(obj->m_triggerTargetColor.r);
        case 172: return static_cast<float>(obj->m_triggerTargetColor.g);
        case 173: return static_cast<float>(obj->m_triggerTargetColor.b);
        case 175: return static_cast<float>(obj->m_isColorTrigger);
        case 176: return static_cast<float>(obj->m_activateGroup);
        case 177: return static_cast<float>(obj->m_touchHoldMode);
        case 179: return static_cast<float>(obj->m_touchToggleMode);
        case 180: return static_cast<float>(obj->m_touchPlayerMode);
        case 181: return static_cast<float>(obj->m_isDualMode);
        case 182: return static_cast<float>(obj->m_animationID);
        case 183: return static_cast<float>(obj->m_spawnOrder);
        case 184: return static_cast<float>(obj->m_isMultiTriggered);
        case 185: return static_cast<float>(obj->m_previewDisable);
        case 186: return static_cast<float>(obj->m_spawnOrdered);
        case 187: return static_cast<float>(obj->m_triggerOnExit);
        case 188: return static_cast<float>(obj->m_itemID2);
        case 189: return static_cast<float>(obj->m_controlID);
        case 190: return static_cast<float>(obj->m_targetControlID);
        case 191: return static_cast<float>(obj->m_subtractCount);
        case 192: return static_cast<float>(obj->m_collectibleIsPickupItem);
        case 194: return static_cast<float>(obj->m_collectibleIsToggleTrigger);
        case 195: return static_cast<float>(obj->m_collectibleParticleID);
        case 196: return static_cast<float>(obj->m_collectiblePoints);
        case 197: return static_cast<float>(obj->m_hasNoAnimation);
        case 199: return static_cast<float>(obj->m_isTouchTriggered);
        case 200: return static_cast<float>(obj->m_isSpawnTriggered);
        case 201: return static_cast<float>(obj->m_isMultiTriggered);
        case 203: return static_cast<float>(obj->m_rotationTargetID);
        case 204: return obj->m_followYDelay;
        case 205: return obj->m_followYMaxSpeed;
        case 206: return obj->m_followYOffset;
        case 207: return obj->m_cameraEasingValue;
        case 208: return obj->m_cameraPaddingValue;
        case 209: return static_cast<float>(obj->m_cameraIsFreeMode);
        case 212: return static_cast<float>(obj->m_cameraEditCameraSettings);
        case 213: return static_cast<float>(obj->m_cameraDisableGridSnap);
        case 214: return static_cast<float>(obj->m_speedModType);
        case 218: return obj->m_gravityMod;
        case 219: return obj->m_timeWarpTimeMod;
        case 220: return static_cast<float>(obj->m_itemID);
        case 221: return obj->m_spawnXPosition;
        case 222: return static_cast<float>(obj->m_targetModCenterID);
        case 223: return static_cast<float>(obj->m_lockObjectRotation);
        case 224: return static_cast<float>(obj->m_hasUniqueCoin);
        case 225: return static_cast<float>(obj->m_unknownBool);
        case 231: return obj->m_rotationSpeed;
        case 232: return obj->m_rotationAngle;
        case 233: return obj->m_rotationDelta;
        case 234: return obj->m_rotationAnimationSpeed;
        case 235: return static_cast<float>(obj->m_animationRandomizedStart);
        case 236: return static_cast<float>(obj->m_animationShouldUseSpeed);
        case 237: return static_cast<float>(obj->m_disableDelayedLoop);
        case 238: return static_cast<float>(obj->m_disableAnimShine);
        case 239: return static_cast<float>(obj->m_singleFrame);
        case 240: return static_cast<float>(obj->m_usesSpecialAnimation);
        case 241: return obj->m_randomFrameTime;
        case 242: return obj->m_frameTime;
        case 243: return static_cast<float>(obj->m_isNoMultiActivate);
        case 248: return static_cast<float>(obj->m_isMultiActivate);
        case 249: return obj->m_animationSpeed;
        case 252: return obj->m_animationStart;
        case 253: return obj->m_animationOffset;
        case 260: return static_cast<float>(obj->m_animationTriggered);
        case 261: return static_cast<float>(obj->m_animateOnTrigger);
        case 262: return static_cast<float>(obj->m_animateOnlyWhenActive);
        case 263: return static_cast<float>(obj->m_hasCustomAnimation);
        case 264: return static_cast<float>(obj->m_hasCustomRotation);
        case 265: return static_cast<float>(obj->m_disableRotation);
        case 270: return static_cast<float>(obj->m_hasNoGlow);
        case 271: return static_cast<float>(obj->m_cantColorGlow);
        case 275: return static_cast<float>(obj->m_glowColorIsLBG);
        case 276: return static_cast<float>(obj->m_customGlowColor);
        case 278: return static_cast<float>(obj->m_isNoTouch);
        case 280: return static_cast<float>(obj->m_isRingPoweredOn);
        case 281: return static_cast<float>(obj->m_poweredOn);
        case 282: return static_cast<float>(obj->m_isUIObject);
        case 283: return static_cast<float>(obj->m_isSolidColorBlock);
        case 286: return static_cast<float>(obj->m_isInvisibleBlock);
        case 287: return static_cast<float>(obj->m_isInvisible);
        case 288: return static_cast<float>(obj->m_hasNoEffects);
        case 290: return static_cast<float>(obj->m_hasNoParticles);
        case 291: return static_cast<float>(obj->m_particleLocked);
        case 292: return static_cast<float>(obj->m_isColorSpriteBlack);
        case 293: return static_cast<float>(obj->m_isObjectBlack);
        case 298: return static_cast<float>(obj->m_blackChildOpacityLocked);
        case 299: return obj->m_blackChildOpacity;
        case 300: return static_cast<float>(obj->m_editorEnabled);
        case 301: return static_cast<float>(obj->m_isGroupDisabled);
        case 305: return static_cast<float>(obj->m_unk28c);
        case 306: return static_cast<float>(obj->m_activeMainColorID);
        case 307: return static_cast<float>(obj->m_activeDetailColorID);
        case 308: return static_cast<float>(obj->m_baseUsesHSV);
        case 309: return static_cast<float>(obj->m_detailUsesHSV);
        case 310: return obj->m_positionXOffset;
        case 311: return obj->m_positionYOffset;
        case 312: return obj->m_rotationXOffset;
        case 313: return obj->m_rotationYOffset;
        case 314: return obj->m_scaleXOffset;
        case 315: return obj->m_scaleYOffset;
        case 316: return static_cast<float>(obj->m_isFlipX);
        case 317: return static_cast<float>(obj->m_isFlipY);
        case 318: return obj->m_unk2BC;
        case 319: return obj->m_unk2C0;
        case 320: return obj->m_customBoxOffset.x;
        case 321: return obj->m_customBoxOffset.y;
        case 322: return static_cast<float>(obj->m_boxOffsetCalculated);
        case 323: return obj->m_boxOffset.x;
        case 324: return obj->m_boxOffset.y;
        case 325: return static_cast<float>(obj->m_orientedBox);
        case 326: return static_cast<float>(obj->m_shouldUseOuterOb);
        case 327: return static_cast<float>(obj->m_addToNodeContainer);
        case 328: return obj->m_width;
        case 329: return obj->m_height;
        case 330: return static_cast<float>(obj->m_hasParticles);
        case 331: return static_cast<float>(obj->m_particleUseObjectColor);
        case 332: return static_cast<float>(obj->m_hasColorSprite);
        case 333: return obj->m_particleOffset.x;
        case 334: return obj->m_particleOffset.y;
        case 335: return static_cast<float>(obj->m_isParticleSpriteLocked);
        case 336: return static_cast<float>(obj->m_isDirty);
        case 337: return static_cast<float>(obj->m_isObjectPosDirty);
        case 338: return static_cast<float>(obj->m_isUnmodifiedPosDirty);
        case 339: return obj->m_fadeMargin;
        case 340: return static_cast<float>(obj->m_isObjectRectDirty);
        case 341: return static_cast<float>(obj->m_isOrientedBoxDirty);
        case 343: return static_cast<float>(obj->m_colorSpriteLocked);
        case 344: return static_cast<float>(obj->m_unk353);
        case 345: return static_cast<float>(obj->m_canRotateFree);
        case 346: return static_cast<float>(obj->m_isMirroredByScale);
        case 347: return static_cast<float>(obj->m_linkedGroup);
        case 348: return static_cast<float>(obj->m_unk35C);
        case 349: return static_cast<float>(obj->m_colorType);
        case 350: return static_cast<float>(obj->m_childColorType);
        case 351: return static_cast<float>(obj->m_shouldBlendBase);
        case 352: return static_cast<float>(obj->m_shouldBlendDetail);
        case 353: return static_cast<float>(obj->m_hasCustomChild);
        case 354: return static_cast<float>(obj->m_unk367);
        case 355: return static_cast<float>(obj->m_isRotationAligned);
        case 357: return static_cast<float>(obj->m_uniqueID);
        case 358: return static_cast<float>(obj->m_objectType);
        case 359: return static_cast<float>(obj->m_unk390);
        case 360: return obj->m_objectRadius;
        case 361: return obj->m_spriteWidthScale;
        case 362: return obj->m_spriteHeightScale;
        case 363: return obj->m_unmodifiedPositionX;
        case 364: return obj->m_unmodifiedPositionY;
        case 365: return obj->m_positionX;
        case 366: return obj->m_positionY;
        case 367: return obj->m_startPosition.x;
        case 368: return obj->m_startPosition.y;
        case 369: return static_cast<float>(obj->m_parentMode);
        case 370: return static_cast<float>(obj->m_usesAudioScale);
        case 371: return static_cast<float>(obj->m_hasNoAudioScale);
        case 375: return obj->m_startScaleX;
        case 376: return obj->m_startScaleY;
        case 377: return static_cast<float>(obj->m_startFlipX);
        case 378: return static_cast<float>(obj->m_startFlipY);
        case 379: return obj->m_startRotationX;
        case 380: return obj->m_startRotationY;
        case 381: return static_cast<float>(obj->m_unk3ee);
        case 382: return obj->m_customScaleX;
        case 383: return obj->m_customScaleY;
        case 385: return static_cast<float>(obj->m_unk3DE);
        case 386: return static_cast<float>(obj->m_enterType);
        case 387: return static_cast<float>(obj->m_unk3D8);
        case 388: return static_cast<float>(obj->m_exitType);
        case 389: return static_cast<float>(obj->m_enterChannel);
        case 390: return static_cast<float>(obj->m_objectMaterial);
        case 391: return static_cast<float>(obj->m_unk3E8);
        case 392: return static_cast<float>(obj->m_objectID);
        case 393: return static_cast<float>(obj->m_unk3F8);
        case 394: return static_cast<float>(obj->m_intrinsicDontFade);
        case 395: return static_cast<float>(obj->m_ignoreEnter);
        case 396: return static_cast<float>(obj->m_ignoreFade);
        case 397: return static_cast<float>(obj->m_unk3FD);
        case 399: return static_cast<float>(obj->m_customSpriteColor);
        case 400: return static_cast<float>(obj->m_customColorType);
        case 401: return static_cast<float>(obj->m_isDontEnter);
        case 402: return static_cast<float>(obj->m_isDontFade);
        case 403: return static_cast<float>(obj->m_defaultZOrder);
        case 404: return static_cast<float>(obj->m_unk40C);
        case 405: return static_cast<float>(obj->m_colorZLayerRelated);
        case 406: return static_cast<float>(obj->m_customAudioScale);
        case 407: return static_cast<float>(obj->m_property53);
        case 408: return static_cast<float>(obj->m_slopeUphill);
        case 409: return static_cast<float>(obj->m_slopeDirection);
        case 410: return static_cast<float>(obj->m_slopeIsHazard);
        case 411: return static_cast<float>(obj->m_baseColor);
        case 412: return static_cast<float>(obj->m_detailColor);
        case 413: return static_cast<float>(obj->m_baseOrDetailBlending);
        case 414: return static_cast<float>(obj->m_defaultZLayer);
        case 415: return static_cast<float>(obj->m_zFixedZLayer);
        case 416: return static_cast<float>(obj->m_zLayer);
        case 417: return static_cast<float>(obj->m_zOrder);
        case 418: return static_cast<float>(obj->m_wasSelected);
        case 419: return static_cast<float>(obj->m_isSelected);
        case 420: return obj->m_unk460;
        case 421: return obj->m_minAudioScale;
        case 422: return obj->m_maxAudioScale;
        case 423: return obj->m_opacityMod;
        case 424: return obj->m_opacityMod2;
        case 425: return obj->m_unk464.x;
        case 426: return obj->m_unk464.y;
        case 427: return static_cast<float>(obj->m_updateParents);
        case 428: return static_cast<float>(obj->m_updateEditorColor);
        case 431: return static_cast<float>(obj->m_hasGroupParent);
        case 432: return static_cast<float>(obj->m_hasAreaParent);
        case 433: return static_cast<float>(obj->m_groupCount);
        case 434: return static_cast<float>(obj->m_hasGroupParentsString);
        case 436: return static_cast<float>(obj->m_colorGroupCount);
        case 437: return static_cast<float>(obj->m_opacityGroupCount);
        case 438: return static_cast<float>(obj->m_editorLayer);
        case 439: return static_cast<float>(obj->m_editorLayer2);
        case 440: return static_cast<float>(obj->m_enabledGroupsCounter);
        case 441: return static_cast<float>(obj->m_updateCustomContentSize);
        case 443: return static_cast<float>(obj->m_hasContentSize);
        case 444: return obj->m_scaleX;
        case 445: return obj->m_scaleY;
        case 446: return obj->m_lastSize.width;
        case 447: return obj->m_lastSize.height;
        case 448: return obj->m_lastPosition.x;
        case 449: return obj->m_lastPosition.y;
        case 450: return static_cast<float>(obj->m_unk4C0);
        case 451: return static_cast<float>(obj->m_unk4C4);
        case 452: return static_cast<float>(obj->m_unk4C8);
        case 453: return static_cast<float>(obj->m_unk4CC);
        case 454: return static_cast<float>(obj->m_classType);
        case 455: return static_cast<float>(obj->m_isTrigger);
        case 456: return static_cast<float>(obj->m_isSpawnOrderTrigger);
        case 457: return static_cast<float>(obj->m_isColorTrigger);
        case 458: return static_cast<float>(obj->m_dontIgnoreDuration);
        case 459: return static_cast<float>(obj->m_canBeControlled);
        case 460: return static_cast<float>(obj->m_activateTriggerInEditor);
        case 461: return static_cast<float>(obj->m_isHighDetail);
        case 462: return static_cast<float>(obj->m_unk4F8);
        case 463: return static_cast<float>(obj->m_isDecoration);
        case 464: return static_cast<float>(obj->m_isDecoration2);
        case 465: return static_cast<float>(obj->m_unk4fb);
        case 466: return static_cast<float>(obj->m_maybeNotColorable);
        case 467: return static_cast<float>(obj->m_isPassable);
        case 468: return static_cast<float>(obj->m_isHide);
        case 469: return static_cast<float>(obj->m_isNonStickX);
        case 470: return static_cast<float>(obj->m_isNonStickY);
        case 471: return static_cast<float>(obj->m_isIceBlock);
        case 472: return static_cast<float>(obj->m_isGripSlope);
        case 473: return static_cast<float>(obj->m_isScaleStick);
        case 474: return static_cast<float>(obj->m_isExtraSticky);
        case 475: return static_cast<float>(obj->m_isDontBoostY);
        case 476: return static_cast<float>(obj->m_isDontBoostX);
        case 477: return static_cast<float>(obj->m_unk507);
        case 478: return static_cast<float>(obj->m_unk508);
        case 479: return obj->m_unk50C;
        case 480: return obj->m_pixelScaleX;
        case 481: return obj->m_pixelScaleY;
        case 482: return static_cast<float>(obj->m_areaOpacityRelated);
        case 483: return static_cast<float>(obj->m_mainColorKeyIndex);
        case 484: return static_cast<float>(obj->m_frames);
        case 485: return static_cast<float>(obj->m_detailColorKeyIndex);
        case 486: return obj->m_areaOpacityValue;
        case 487: return static_cast<float>(obj->m_areaOpacityIndex);
        case 489: return static_cast<float>(obj->m_unk52C);
        case 491: return static_cast<float>(obj->m_unk530);
        case 492: return static_cast<float>(obj->m_greenDebugDraw);
        case 493: return static_cast<float>(obj->m_state);
        case 494: return static_cast<float>(obj->m_unk548);
        case 497: return static_cast<float>(obj->m_visible);
        case 498: return obj->m_unk540;
        case 499: return static_cast<float>(obj->m_shouldNotHideAnimFreeze);
        case 500: return obj->m_unk544;
        case 501: return static_cast<float>(obj->m_unkAnimationInt);
        case 502: return static_cast<float>(obj->m_maybeAnimationVariableXInt);
        case 503: return static_cast<float>(obj->m_maybeAnimationVariableYInt);
        case 504: return static_cast<float>(obj->m_hasUniqueCoin);
        case 505: return static_cast<float>(obj->m_unknownBool);
        case 506: return obj->m_unk6a8;
        case 508: return static_cast<float>(obj->m_unk698);
        case 510: return obj->m_unk6ac;
        case 512: return static_cast<float>(obj->m_unk6b4);
        case 513: return obj->m_unk6b0;
        case 514: return static_cast<float>(obj->m_unk6f4);
        case 515: return static_cast<float>(obj->m_unk6f5);
        case 516: return obj->m_speedStart.x;
        case 517: return obj->m_speedStart.y;
        case 518: return obj->m_endPosition.x;
        case 519: return obj->m_endPosition.y;
        case 520: return static_cast<float>(obj->m_unk708);
        case 521: return static_cast<float>(obj->m_spawnOrdered);
        case 522: return static_cast<float>(obj->m_previewDisable);
        case 524: return static_cast<float>(obj->m_hasFollower);
        case 525: return static_cast<float>(obj->m_propagateScaleChanges);
        case 526: return static_cast<float>(obj->m_someOtherIndex);
        case 527: return static_cast<float>(obj->m_middleSectionIndex);
        case 528: return static_cast<float>(obj->m_innerSectionIndex);
        case 529: return static_cast<float>(obj->m_hasExtendedCollision);
        case 530: return static_cast<float>(obj->m_isDual);
        case 531: return static_cast<float>(obj->m_centerID);
        case 532: return static_cast<float>(obj->m_rotID);
        case 533: return static_cast<float>(obj->m_propagateFlipChanges);
        case 534: return static_cast<float>(obj->m_controlID);
        case 535: return static_cast<float>(obj->m_targetControlID);
        case 536: return obj->m_moveModX;
        case 537: return obj->m_moveModY;
        case 538: return static_cast<float>(obj->m_speedModType);
        case 539: return obj->m_gravityMod;
        case 540: return obj->m_duration;
        case 541: return obj->m_shakeStrength;
        case 542: return obj->m_shakeInterval;
        case 543: return obj->m_directionModeDistance;
        case 544: return static_cast<float>(obj->m_isSilent);
        case 545: return obj->m_rotationDegrees;
        case 546: return obj->m_rotationOffset;
        case 547: return obj->m_followXMod;
        case 548: return obj->m_followYMod;
        case 549: return obj->m_followYSpeed;
        case 550: return obj->m_followYDelay;
        case 551: return obj->m_followYMaxSpeed;
        case 552: return obj->m_followYOffset;
        case 553: return static_cast<float>(obj->m_followCPP);
        case 554: return static_cast<float>(obj->m_targetPlayer1);
        case 555: return static_cast<float>(obj->m_targetPlayer2);
        case 556: return static_cast<float>(obj->m_ordValue);
        case 557: return static_cast<float>(obj->m_channelValue);
        case 558: return obj->m_unk2A8;
        case 559: return obj->m_rotationYOffset;
        case 560: return obj->m_unk2B0;
        case 561: return obj->m_positionYOffset;
        case 562: return obj->m_rotationXOffset;
        case 563: return obj->m_positionXOffset;
        case 564: return obj->m_scaleXOffset;
        case 565: return obj->m_scaleYOffset;
        case 566: return obj->m_unk2BC;
        case 567: return obj->m_unk2C0;
        case 568: return obj->m_customBoxOffset.x;
        case 569: return obj->m_customBoxOffset.y;
        case 570: return static_cast<float>(obj->m_shouldUseOuterOb);
        case 571: return static_cast<float>(obj->m_boxOffsetCalculated);
        case 572: return obj->m_boxOffset.x;
        case 573: return obj->m_boxOffset.y;
        case 574: return static_cast<float>(obj->m_orientedBox);
        case 575: return obj->m_width;
        case 576: return obj->m_height;
        case 577: return static_cast<float>(obj->m_hasParticles);
        case 578: return static_cast<float>(obj->m_particleUseObjectColor);
        case 579: return static_cast<float>(obj->m_hasColorSprite);
        case 580: return obj->m_particleOffset.x;
        case 581: return obj->m_particleOffset.y;
        case 582: return static_cast<float>(obj->m_isParticleSpriteLocked);
        case 583: return static_cast<float>(obj->m_isDirty);
        case 584: return static_cast<float>(obj->m_isObjectPosDirty);
        case 585: return static_cast<float>(obj->m_isUnmodifiedPosDirty);
        case 586: return obj->m_fadeMargin;
        case 587: return static_cast<float>(obj->m_isObjectRectDirty);
        case 588: return static_cast<float>(obj->m_isOrientedBoxDirty);
        case 589: return static_cast<float>(obj->m_colorSpriteLocked);
        case 590: return static_cast<float>(obj->m_unk353);
        case 591: return static_cast<float>(obj->m_canRotateFree);
        case 592: return static_cast<float>(obj->m_isMirroredByScale);
        case 593: return static_cast<float>(obj->m_linkedGroup);
        case 595: return static_cast<float>(obj->m_unk35C);
        case 596: return static_cast<float>(obj->m_colorType);
        case 597: return static_cast<float>(obj->m_childColorType);
        case 598: return static_cast<float>(obj->m_shouldBlendBase);
        case 599: return static_cast<float>(obj->m_shouldBlendDetail);
        default:  return 0.0f;
    }
}

float SetupTriggerPopup::getTruncatedValue(float value, int decimals) {
    if (decimals <= 0) {
        return static_cast<float>(static_cast<int>(value));
    }
    float multiplier;
    if (decimals == 1) {
        multiplier = 10.0f;
    } else {
        multiplier = powf(10.0f, static_cast<float>(decimals));
    }
    return roundf(value * multiplier) / multiplier;
}

float SetupTriggerPopup::getTruncatedValueByTag(int tag, float value) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
    cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(m_inputNodes->objectForKey(key->getCString()));
    if (node) {
        return getTruncatedValue(value, node->m_decimalPlaces);
    }
    return value;
}

float SetupTriggerPopup::getValue(int tag) {
    if (tag < 0) {
        cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
        cocos2d::CCObject* obj = m_triggerValues->objectForKey(key->getCString());
        if (obj) {
            return static_cast<cocos2d::CCString*>(obj)->floatValue();
        }
        return 0.0f;
    }

    if (m_gameObject) {
        return getTriggerValue(tag, m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0) {
        return 0.0f;
    }

    float result = -99999.0f;
    for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
        GameObject* obj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(i));
        float value = getTriggerValue(tag, obj);
        if (result == -99999.0f) {
            result = value;
        } else if (value != result) {
            return 0.0f;
        }
    }
    return result;
}

void SetupTriggerPopup::goToPage(int page, bool hideAll) {
    if (m_pageContainers->count() == 0 || static_cast<int>(m_pageContainers->count()) < page) {
        return;
    }

    m_page = page;
    m_hideAll = hideAll;

    for (unsigned int i = 0; i < m_pageContainers->count(); ++i) {
        cocos2d::CCArray* container = static_cast<cocos2d::CCArray*>(m_pageContainers->objectAtIndex(i));
        for (unsigned int j = 0; j < container->count(); ++j) {
            cocos2d::CCObject* obj = container->objectAtIndex(j);

            cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", obj->getTag());
            if (!m_disabledEasingButtons->objectForKey(key->getCString())) {
                static_cast<cocos2d::CCNode*>(obj)->setVisible((page == static_cast<int>(i)) && !hideAll);
            }

            cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(
                m_inputNodes->objectForKey(key->getCString())
            );
            if (node) {
                node->onClickTrackNode(false);
            }
        }
    }

    if (m_prevButton) {
        m_prevButton->setVisible(!hideAll);
    }
    if (m_nextButton) {
        m_nextButton->setVisible(!hideAll);
    }

    refreshGroupVisibility();
    pageChanged();
}

void SetupTriggerPopup::hideAll() {
    goToPage(0, true);
}

void SetupTriggerPopup::keyBackClicked() {
    onClose(nullptr);
}

void SetupTriggerPopup::onClose(cocos2d::CCObject* sender) {
    LevelEditorLayer::resetEffectTriggerOptim(
        GameManager::sharedState()->m_levelEditorLayer,
        m_gameObject,
        m_gameObjects
    );
    closeInputNodes();
    updateEditorLabel();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetupTriggerPopup::onCustomButton(cocos2d::CCObject* sender) {
}

void SetupTriggerPopup::onCustomEaseArrow(int property, bool up) {
    int value = static_cast<int>(getValue(property));
    int newValue = up ? value + 1 : value - 1;
    if (newValue < 0) newValue = 0;
    if (newValue > 18) newValue = 18;

    valueChanged(property, static_cast<float>(newValue));
    updateCustomEaseLabel(property, newValue);

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCObject* obj = m_customEasingTags->objectForKey(key->getCString());
    if (obj) {
        toggleCustomEaseRateVisibility(property, static_cast<int>(obj->getTag()));
    }
}

void SetupTriggerPopup::onCustomEaseArrowDown(cocos2d::CCObject* sender) {
    onCustomEaseArrow(sender->getTag(), false);
}

void SetupTriggerPopup::onCustomEaseArrowUp(cocos2d::CCObject* sender) {
    onCustomEaseArrow(sender->getTag(), true);
}

void SetupTriggerPopup::onCustomEaseRate(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    float value = getValue(tag);

    ConfigureValuePopup* popup = ConfigureValuePopup::create(this, value, 0.1f, 4.0f, "Easing Rate", "");
    popup->setTag(tag);
    popup->show();
}

void SetupTriggerPopup::onCustomToggleTriggerValue(cocos2d::CCObject* sender) {
}

void SetupTriggerPopup::onDisableValue(cocos2d::CCObject* sender) {
    updateValue(sender->getTag(), -909190.0f);
}

void SetupTriggerPopup::onEase(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    EasingType newType;
    if (tag == 1) {
        newType = static_cast<EasingType>(static_cast<int>(m_easingType) + 1);
    } else {
        newType = static_cast<EasingType>(static_cast<int>(m_easingType) - 1);
    }
    if (static_cast<int>(newType) < 0) newType = static_cast<EasingType>(0);
    if (static_cast<int>(newType) > static_cast<int>(EasingType::BackOut)) {
        newType = EasingType::BackOut;
    }
    m_easingType = newType;
    valueChanged(30, static_cast<float>(static_cast<int>(newType)));
    updateEaseLabel();
    toggleEaseRateVisibility();
}

void SetupTriggerPopup::onEaseRate(cocos2d::CCObject* sender) {
    ConfigureValuePopup* popup = ConfigureValuePopup::create(this, m_easingRate, 0.1f, 4.0f, "Easing Rate", "");
    popup->setTag(85);
    popup->show();
}

void SetupTriggerPopup::onMultiTrigger(cocos2d::CCObject* sender) {
    m_multiTriggered = !m_multiTriggered;

    if (m_gameObject) {
        m_gameObject->m_isMultiTriggered = m_multiTriggered;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->m_isMultiTriggered = m_multiTriggered;
        }
    }
}

void SetupTriggerPopup::onPage(cocos2d::CCObject* sender) {
    int pageCount = m_pageContainers->count();
    int newPage = sender->getTag() + m_page;

    if (newPage < 0) {
        newPage += pageCount;
    } else if (newPage >= pageCount) {
        newPage -= pageCount;
    }

    m_page = newPage;
    goToPage(newPage, false);
}

void SetupTriggerPopup::onPlusButton(cocos2d::CCObject* sender) {
}

void SetupTriggerPopup::onSpawnedByTrigger(cocos2d::CCObject* sender) {
    bool wasOn = m_spawnTriggered;
    m_spawnTriggered = !m_spawnTriggered;

    if (!wasOn) {
        m_touchTriggered = false;
        updateTouchTriggered();
        if (m_touchToggle) {
            m_touchToggle->toggle(m_touchTriggered);
        }
    }

    updateSpawnedByTrigger();
}

void SetupTriggerPopup::onToggleTriggerValue(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    bool toggled = !static_cast<bool>(sender->getTag());

    m_disableTextDelegate = true;
    updateToggleItem(tag, toggled);
    valueChanged(tag, static_cast<float>(toggled));
}

void SetupTriggerPopup::onTouchTriggered(cocos2d::CCObject* sender) {
    bool wasOn = m_touchTriggered;
    m_touchTriggered = !m_touchTriggered;

    if (!wasOn) {
        m_spawnTriggered = false;
        updateSpawnedByTrigger();
        if (m_spawnToggle) {
            m_spawnToggle->toggle(m_spawnTriggered);
        }
    }

    updateTouchTriggered();
}

void SetupTriggerPopup::pageChanged() {
}

void SetupTriggerPopup::postSetup() {
    updateDefaultTriggerValues();
    m_disableTextDelegate = false;
}

void SetupTriggerPopup::preSetup() {
    m_disableTextDelegate = true;
    determineStartValues();
}

void SetupTriggerPopup::refreshGroupVisibility() {
    if (m_groupContainers->count() == 0) {
        return;
    }

    for (unsigned int i = 0; i < m_groupContainers->count(); ++i) {
        cocos2d::CCArray* container = static_cast<cocos2d::CCArray*>(m_groupContainers->objectAtIndex(i));
        int tag = container->getTag();
        bool empty = container->count() == 0;

        for (unsigned int j = 0; !empty && j < container->count(); ++j) {
            cocos2d::CCNode* obj = static_cast<cocos2d::CCNode*>(container->objectAtIndex(j));
            if (obj->isVisible()) {
                if (tag == 0) {
                    obj->setVisible(false);
                }
            }
            empty = (j + 1 >= container->count());
        }
    }
}

void SetupTriggerPopup::removeObjectFromGroup(cocos2d::CCObject* object, int group) {
    cocos2d::CCArray* container = getGroupContainer(group);
    container->removeObject(object, true);
}

void SetupTriggerPopup::removeObjectFromPage(cocos2d::CCObject* object, int page) {
    cocos2d::CCArray* container = getPageContainer(page);
    container->removeObject(object, true);
}

void SetupTriggerPopup::resetDisabledValues() {
    if (!m_inputNodes) {
        return;
    }

    cocos2d::CCDictElement* element = m_inputNodes->m_pElements;
    while (element) {
        cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(element->m_pObject);
        if (node && getValue(node->getTag()) == -909190.0f) {
            updateValue(node->getTag(), 0.0f);
        }
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void SetupTriggerPopup::setMaxSliderValue(float value, int property) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", value);
    m_maxSliderValues->setObject(valueStr, key->getCString());
}

void SetupTriggerPopup::setMinSliderValue(float value, int property) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", value);
    m_minSliderValues->setObject(valueStr, key->getCString());
}

bool SetupTriggerPopup::shouldLimitValue(int property) {
    auto it = m_shouldLimitValues.find(property);
    if (it != m_shouldLimitValues.end()) {
        return it->second;
    }
    m_shouldLimitValues[property] = false;
    return false;
}

void SetupTriggerPopup::show() {
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;

    if (!m_scene) {
        m_scene = runningScene;
    }

    if (m_ZOrder == 0) {
        m_ZOrder = 105;
    }

    m_scene->addChild(this, m_ZOrder);
}

void SetupTriggerPopup::sliderBegan(Slider* slider) {
    if (!m_disableSliderDelegate) {
        return;
    }

    cocos2d::CCNode* bgNode = m_mainLayer->getChildByTag(1);
    if (bgNode) {
        bgNode->stopActionByTag(2);
        cocos2d::CCFadeTo* fade = cocos2d::CCFadeTo::create(0.15f, 0);
        fade->setTag(1);
        bgNode->runAction(fade);
    }

    stopActionByTag(2);
    cocos2d::CCFadeTo* selfFade = cocos2d::CCFadeTo::create(0.15f, 0);
    selfFade->setTag(1);
    runAction(selfFade);
}

void SetupTriggerPopup::sliderEnded(Slider* slider) {
    if (!m_disableSliderDelegate) {
        return;
    }

    cocos2d::CCNode* bgNode = m_mainLayer->getChildByTag(1);
    if (bgNode) {
        bgNode->stopActionByTag(2);
        cocos2d::CCFadeTo* fade = cocos2d::CCFadeTo::create(0.15f, 255);
        fade->setTag(1);
        bgNode->runAction(fade);
    }

    stopActionByTag(2);
    cocos2d::CCFadeTo* selfFade = cocos2d::CCFadeTo::create(0.15f, 150);
    selfFade->setTag(1);
    runAction(selfFade);
}

void SetupTriggerPopup::textChanged(CCTextInputNode* node) {
    if (m_disableTextDelegate) {
        return;
    }

    int tag = node->getTag();
    float value = atof(node->getString().c_str());
    float truncated = getTruncatedValue(value, node->m_decimalPlaces);

    updateInputValue(tag, &truncated);

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
    cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", truncated);
    m_triggerValues->setObject(valueStr, key->getCString());

    valueChanged(tag, truncated);

    float sliderValue = triggerSliderValueFromValue(tag, truncated);
    updateSlider(tag, sliderValue);
}

void SetupTriggerPopup::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void SetupTriggerPopup::textInputReturn(CCTextInputNode* node) {
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0.0f, 0.0f));
    m_mainLayer->runAction(move);
}

void SetupTriggerPopup::textInputShouldOffset(CCTextInputNode* node, float yOffset) {
    m_mainLayer->stopAllActions();
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.2f, cocos2d::CCPoint(0.0f, yOffset));
    m_mainLayer->runAction(move);
}

void SetupTriggerPopup::toggleBG(bool visible) {
    cocos2d::CCNode* bgNode = m_mainLayer->getChildByTag(1);
    if (bgNode) {
        bgNode->setVisible(visible);
    }
}

void SetupTriggerPopup::toggleCustomEaseRateVisibility(int property, int tag) {
    float value = getValue(property);

    if (!GameToolbox::isRateEasing(static_cast<int>(value))) {
        if (!m_gameObjects || m_gameObjects->count() == 0) {
            cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
            cocos2d::CCNode* button = static_cast<cocos2d::CCNode*>(m_customEasingButtons->objectForKey(key->getCString()));
            if (button) {
                button->setVisible(false);
                m_disabledEasingButtons->setObject(button, key->getCString());
            }
            return;
        }

        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            GameObject* obj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(i));
            float triggerValue = getTriggerValue(property, obj);
            if (!GameToolbox::isRateEasing(static_cast<int>(triggerValue))) {
                cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
                cocos2d::CCNode* button = static_cast<cocos2d::CCNode*>(m_customEasingButtons->objectForKey(key->getCString()));
                if (button) {
                    button->setVisible(false);
                    m_disabledEasingButtons->setObject(button, key->getCString());
                }
                return;
            }
        }
    }

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
    cocos2d::CCNode* button = static_cast<cocos2d::CCNode*>(m_customEasingButtons->objectForKey(key->getCString()));
    if (button) {
        button->setVisible(true);
        m_disabledEasingButtons->removeObjectForKey(key->getCString());
    }
}

void SetupTriggerPopup::toggleDisableButtons(bool visible) {
    if (!m_disableButtons) {
        return;
    }

    cocos2d::CCDictElement* element = m_disableButtons->m_pElements;
    while (element) {
        cocos2d::CCNode* button = static_cast<cocos2d::CCNode*>(element->m_pObject);
        if (button) {
            bool shouldEnable = visible;
            int tag = button->getTag();
            cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
            cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(
                m_inputNodes->objectForKey(key->getCString())
            );
            if (node && !node->isVisible()) {
                shouldEnable = false;
            }
            button->setVisible(shouldEnable);
            static_cast<CCMenuItemSpriteExtra*>(button)->setEnabled(shouldEnable);
        }
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void SetupTriggerPopup::toggleEaseRateVisibility() {
    bool showRate = GameToolbox::isRateEasing(static_cast<int>(m_easingType));

    if (!showRate && m_gameObjects && m_gameObjects->count() > 0) {
        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            if (!GameToolbox::isRateEasing(obj->m_easingType)) {
                showRate = true;
                break;
            }
        }
    }

    if (m_easingRateButton) {
        m_easingRateButton->setVisible(showRate);
    }
}

void SetupTriggerPopup::toggleGroup(int group, bool visible) {
    cocos2d::CCArray* container = getGroupContainer(group);
    container->setTag(visible ? 1 : 0);

    for (unsigned int i = 0; i < container->count(); ++i) {
        cocos2d::CCNode* obj = static_cast<cocos2d::CCNode*>(container->objectAtIndex(i));

        cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", obj->getTag());
        if (!m_disabledEasingButtons->objectForKey(key->getCString())) {
            obj->setVisible(visible);
        }

        cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(
            m_inputNodes->objectForKey(key->getCString())
        );
        if (node) {
            node->onClickTrackNode(false);
        }
    }
}

void SetupTriggerPopup::toggleLimitValue(int property, bool limit) {
    m_shouldLimitValues[property] = limit;
}

void SetupTriggerPopup::togglePageArrows(bool visible) {
    if (!m_prevButton) {
        return;
    }

    m_prevButton->setVisible(visible);
    m_prevButton->setEnabled(visible);
    m_nextButton->setVisible(visible);
    m_nextButton->setEnabled(visible);
}

void SetupTriggerPopup::triggerArrowChanged(int property, bool isRight) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCString* obj = static_cast<cocos2d::CCString*>(m_triggerValues->objectForKey(key->getCString()));

    int value = 0;
    if (obj) {
        value = static_cast<int>(obj->floatValue());
    }

    int newValue = isRight ? value + 1 : value - 1;
    float newValueFloat = static_cast<float>(newValue);

    m_disableTextDelegate = true;
    updateValueControls(property, newValueFloat);
    valueChanged(property, newValueFloat);
}

void SetupTriggerPopup::triggerArrowLeft(cocos2d::CCObject* sender) {
    triggerArrowChanged(sender->getTag(), false);
}

void SetupTriggerPopup::triggerArrowRight(cocos2d::CCObject* sender) {
    triggerArrowChanged(sender->getTag(), true);
}

void SetupTriggerPopup::triggerSliderChanged(cocos2d::CCObject* sender) {
    m_disableTextDelegate = true;

    int tag = sender->getTag();
    SliderThumb* thumb = static_cast<SliderThumb*>(sender);
    float value = thumb->getValue();
    float sliderValue = triggerValueFromSliderValue(tag, value);
    float truncated = getTruncatedValueByTag(tag, sliderValue);

    valueChanged(tag, truncated);
    updateInputNode(tag, truncated);
}

float SetupTriggerPopup::triggerSliderValueFromValue(int tag, float value) {
    float minValue = getMinSliderValue(tag);
    float result = (value - minValue) / (getMaxSliderValue(tag) - minValue);

    if (result >= 1.0f) return 1.0f;
    if (result <= 0.0f) return 0.0f;
    return result;
}

float SetupTriggerPopup::triggerValueFromSliderValue(int tag, float value) {
    float minValue = getMinSliderValue(tag);
    return minValue + (getMaxSliderValue(tag) - minValue) * value;
}

void SetupTriggerPopup::updateCustomEaseLabel(int property, int easingType) {
    std::string text = "Mixed";

    if (easingType != -99999) {
        text = GameToolbox::easeToText(easingType);
    }

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCLabelBMFont* label = static_cast<cocos2d::CCLabelBMFont*>(
        m_customEasingLabels->objectForKey(key->getCString())
    );
    if (!label) {
        return;
    }

    label->setString(text.c_str());

    cocos2d::CCObject* scaleObj = m_customEasingScales->objectForKey(key->getCString());
    float scale = 1.0f;
    if (scaleObj) {
        scale = static_cast<cocos2d::CCString*>(scaleObj)->floatValue();
    }

    label->limitLabelWidth(scale * 150.0f, scale * 0.7f, 0.0f);
}

void SetupTriggerPopup::updateCustomEaseRateLabel(int property, float easingRate) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCLabelBMFont* label = static_cast<cocos2d::CCLabelBMFont*>(
        m_customEasingLabels->objectForKey(key->getCString())
    );
    if (!label) {
        return;
    }

    std::string text = cocos2d::CCString::createWithFormat("%.02f", easingRate)->getCString();
    label->setString(text.c_str());
}

void SetupTriggerPopup::updateCustomToggleTrigger(int tag, bool toggled) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
    CCMenuItemToggler* toggler = static_cast<CCMenuItemToggler*>(
        m_customValueToggles->objectForKey(key->getCString())
    );
    if (toggler) {
        toggler->toggle(toggled);
    }
}

void SetupTriggerPopup::updateDefaultTriggerValues() {
    if (m_gameObject) {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0) {
        cocos2d::CCArray* inputKeys = m_inputNodes->allKeys();
        if (inputKeys) {
            for (unsigned int i = 0; i < inputKeys->count(); ++i) {
                cocos2d::CCString* key = static_cast<cocos2d::CCString*>(inputKeys->objectAtIndex(i));
                int tag = key->intValue();
                float value = getValue(tag);
                updateValueControls(tag, value);
            }
        }

        cocos2d::CCArray* toggleKeys = m_valueToggles->allKeys();
        if (toggleKeys) {
            for (unsigned int i = 0; i < toggleKeys->count(); ++i) {
                cocos2d::CCString* key = static_cast<cocos2d::CCString*>(toggleKeys->objectAtIndex(i));
                int tag = key->intValue();

                if (getValue(tag) == 0.0f) {
                    cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", 0.0f);
                    m_triggerValues->setObject(valueStr, key->getCString());
                } else {
                    updateToggleItem(tag, true);
                }
            }
        }
        return;
    }

    cocos2d::CCArray* inputKeys = m_inputNodes->allKeys();
    if (inputKeys) {
        for (unsigned int i = 0; i < inputKeys->count(); ++i) {
            cocos2d::CCString* key = static_cast<cocos2d::CCString*>(inputKeys->objectAtIndex(i));
            int tag = key->intValue();

            float value = -99999.0f;
            if (m_gameObjects->count() > 0) {
                GameObject* firstObj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(0));
                float firstValue = getTriggerValue(tag, firstObj);
                bool allSame = true;

                for (unsigned int j = 1; j < m_gameObjects->count(); ++j) {
                    GameObject* obj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(j));
                    float objValue = getTriggerValue(tag, obj);
                    if (firstValue != objValue) {
                        allSame = false;
                        break;
                    }
                }

                if (allSame && firstValue != -99998.0f) {
                    value = firstValue;
                } else {
                    value = -99999.0f;
                }
            }

            if (value != -99999.0f) {
                updateValueControls(tag, value);
            }
        }
    }

    cocos2d::CCArray* toggleKeys = m_valueToggles->allKeys();
    if (toggleKeys) {
        for (unsigned int i = 0; i < toggleKeys->count(); ++i) {
            cocos2d::CCString* key = static_cast<cocos2d::CCString*>(toggleKeys->objectAtIndex(i));
            int tag = key->intValue();

            bool shouldToggle = true;
            if (m_gameObjects && m_gameObjects->count() > 0) {
                for (unsigned int j = 0; j < m_gameObjects->count(); ++j) {
                    GameObject* obj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(j));
                    if (getTriggerValue(tag, obj) == 0.0f) {
                        cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", 0.0f);
                        m_triggerValues->setObject(valueStr, key->getCString());
                        shouldToggle = false;
                        break;
                    }
                }
            }

            if (shouldToggle) {
                updateToggleItem(tag, true);
            }
        }
    }
}

void SetupTriggerPopup::updateEaseLabel() {
    std::string text = "Mixed";
    if (m_easingType != static_cast<EasingType>(-99999)) {
        text = GameToolbox::easeToText(static_cast<int>(m_easingType));
    }

    if (m_easingLabel) {
        m_easingLabel->setString(text.c_str());
        m_easingLabel->limitLabelWidth(m_easingControlScale * 150.0f, m_easingControlScale * 0.7f, 0.0f);
    }
}

void SetupTriggerPopup::updateEaseRateLabel() {
    if (!m_easingRateLabel) {
        return;
    }

    std::string text = cocos2d::CCString::createWithFormat("%.02f", m_easingRate)->getCString();
    if (m_easingRate == -99999.0f) {
        text = "";
    }
    m_easingRateLabel->setString(text.c_str());
}

void SetupTriggerPopup::updateEditorLabel() {
    if (m_gameObject) {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            GameObject* obj = static_cast<GameObject*>(m_gameObjects->objectAtIndex(i));
            LevelEditorLayer::updateObjectLabel(obj);
        }
    }
}

void SetupTriggerPopup::updateInputNode(int tag, float value) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", tag);
    cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(
        m_inputNodes->objectForKey(key->getCString())
    );
    if (!node) {
        return;
    }

    std::string text;
    if (value == -909190.0f) {
        text = "NA";
    } else {
        int decimals = node->m_decimalPlaces;
        switch (decimals) {
            case 0:
                text = cocos2d::CCString::createWithFormat("%i", static_cast<int>(value))->getCString();
                break;
            case 1:
                text = cocos2d::CCString::createWithFormat("%.01f", value)->getCString();
                break;
            case 3:
                text = cocos2d::CCString::createWithFormat("%.03f", value)->getCString();
                break;
            case 4:
                text = cocos2d::CCString::createWithFormat("%.04f", value)->getCString();
                break;
            default:
                text = cocos2d::CCString::createWithFormat("%.02f", value)->getCString();
                break;
        }
    }

    node->setString(text);
}

void SetupTriggerPopup::updateInputNodeLabel(int property, std::string text) {
    m_disableTextDelegate = true;

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCTextInputNode* node = static_cast<cocos2d::CCTextInputNode*>(
        m_inputNodes->objectForKey(key->getCString())
    );
    if (node) {
        node->setString(text);
    }

    m_disableTextDelegate = false;
}

void SetupTriggerPopup::updateInputValue(int tag, float* value) {
}

void SetupTriggerPopup::updateLabel(int property, std::string text) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCLabelBMFont* label = static_cast<cocos2d::CCLabelBMFont*>(
        m_inputLabels->objectForKey(key->getCString())
    );
    if (label) {
        label->setString(text.c_str());
    }
}

void SetupTriggerPopup::updateMultiTriggerBtn() {
    if (!m_multiTriggerContainer) {
        return;
    }

    for (unsigned int i = 0; i < m_multiTriggerContainer->count(); ++i) {
        CCMenuItemToggler* toggler = static_cast<CCMenuItemToggler*>(
            m_multiTriggerContainer->objectAtIndex(i)
        );
        toggler->toggle(m_multiTriggered);
    }
}

void SetupTriggerPopup::updateSlider(int property) {
    float value = getValue(property);
    float sliderValue = triggerSliderValueFromValue(property, value);
    updateSlider(property, sliderValue);
}

void SetupTriggerPopup::updateSlider(int property, float value) {
    if (!m_inputNodes) {
        return;
    }

    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCObject* obj = m_inputNodes->objectForKey(key->getCString());
    if (!obj) {
        return;
    }

    Slider* slider = static_cast<Slider*>(obj);
    slider->setValue(value);
}

void SetupTriggerPopup::updateSpawnedByTrigger() {
    if (m_gameObject) {
        m_gameObject->m_isSpawnTriggered = m_spawnTriggered;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->m_isSpawnTriggered = m_spawnTriggered;
        }
    }
    updateMultiTriggerBtn();
}

void SetupTriggerPopup::updateToggleItem(int value, bool toggled) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", value);
    cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", static_cast<float>(toggled));
    m_triggerValues->setObject(valueStr, key->getCString());

    CCMenuItemToggler* toggler = static_cast<CCMenuItemToggler*>(
        m_valueToggles->objectForKey(key->getCString())
    );
    if (toggler) {
        toggler->toggle(toggled);
    }
}

void SetupTriggerPopup::updateTouchTriggered() {
    if (m_gameObject) {
        m_gameObject->m_isTouchTriggered = m_touchTriggered;
    } else if (m_gameObjects) {
        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->m_isTouchTriggered = m_touchTriggered;
        }
    }
    updateMultiTriggerBtn();
}

void SetupTriggerPopup::updateValue(int property, float value) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    if (m_valueToggles->objectForKey(key->getCString())) {
        updateToggleItem(property, value != 0.0f);
    } else {
        updateValueControls(property, value);
    }
    valueChanged(property, value);
}

void SetupTriggerPopup::updateValueControls(int property, float value) {
    cocos2d::CCString* key = cocos2d::CCString::createWithFormat("%i", property);
    cocos2d::CCString* valueStr = cocos2d::CCString::createWithFormat("%f", value);
    m_triggerValues->setObject(valueStr, key->getCString());

    updateInputNode(property, value);

    float sliderValue = triggerSliderValueFromValue(property, value);
    updateSlider(property, sliderValue);
}

void SetupTriggerPopup::valueChanged(int property, float value) {
    if (m_gameObject) {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects && m_gameObjects->count() > 0) {
        for (unsigned int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));

            switch (property) {
                case 10:
                    obj->m_duration = value;
                    break;
                case 12:
                    obj->m_isTouchTriggered = (value != 0.0f);
                    break;
                case 28:
                    obj->m_moveOffset.x = value;
                    break;
                case 29:
                    obj->m_moveOffset.y = value;
                    break;
                case 30:
                    obj->m_easingType = static_cast<int>(value);
                    break;
                case 51:
                    obj->m_targetGroupID = static_cast<int>(value);
                    break;
                case 55:
                    obj->m_shakeStrength = value;
                    break;
                case 56:
                    obj->m_tintGround = (value != 0.0f);
                    break;
                case 58:
                    obj->m_lockToPlayerX = (value != 0.0f);
                    break;
                case 59:
                    obj->m_lockToPlayerY = (value != 0.0f);
                    break;
                case 63:
                    obj->m_shakeInterval = value;
                    break;
                case 65:
                    obj->m_usesPlayerColor1 = (value != 0.0f);
                    break;
                case 66:
                    obj->m_usesPlayerColor2 = (value != 0.0f);
                    break;
                case 68:
                    obj->m_moveModX = value;
                    break;
                case 69:
                    obj->m_lockToCameraX = (value != 0.0f);
                    break;
                case 70:
                    obj->m_moveModY = value;
                    break;
                case 71:
                    obj->m_centerGroupID = static_cast<int>(value);
                    break;
                case 76:
                    obj->m_easingRate = value;
                    break;
                case 77:
                    obj->m_lockToCameraY = (value != 0.0f);
                    break;
                case 78:
                    obj->m_usesBlending = (value != 0.0f);
                    break;
                case 80:
                    obj->m_fadeInDuration = value;
                    break;
                case 85:
                    obj->m_holdDuration = value;
                    break;
                case 87:
                    obj->m_fadeOutDuration = value;
                    break;
                case 88:
                    obj->m_useMoveTarget = (value != 0.0f);
                    break;
                case 95:
                    obj->m_isDirectionFollowSnap360 = (value != 0.0f);
                    break;
                case 97:
                    obj->m_directionModeDistance = value;
                    break;
                case 98:
                    obj->m_moveTargetMode = static_cast<int>(value);
                    break;
                case 99:
                    obj->m_followXMod = value;
                    break;
                case 100:
                    obj->m_smallStep = (value != 0.0f);
                    break;
                case 101:
                    obj->m_isDynamicMode = (value != 0.0f);
                    break;
                case 102:
                    obj->m_isSilent = (value != 0.0f);
                    break;
                case 106:
                    obj->m_rotationDegrees = value;
                    break;
                case 107:
                    obj->m_rotationOffset = value;
                    break;
                case 110:
                    obj->m_followYDelay = value;
                    break;
                case 122:
                    obj->m_times360 = static_cast<int>(value);
                    break;
                case 123:
                    obj->m_followYMaxSpeed = value;
                    break;
                case 126:
                    obj->m_followYMod = value;
                    break;
                case 127:
                    obj->m_followYSpeed = value;
                    break;
                case 133:
                    obj->m_followYOffset = value;
                    break;
                case 138:
                    obj->m_isDynamicBlock = (value != 0.0f);
                    break;
                case 139:
                    obj->m_targetPlayer1 = static_cast<int>(value);
                    break;
                case 141:
                    obj->m_easingRate = value;
                    break;
                case 142:
                    obj->m_cameraEasingValue = value;
                    break;
                case 143:
                    obj->m_cameraPaddingValue = value;
                    break;
                case 144:
                    obj->m_gravityValue = value;
                    break;
                case 148:
                    obj->m_zoomValue = value;
                    break;
                case 149:
                    obj->m_targetPlayer2 = static_cast<int>(value);
                    break;
                case 150:
                    obj->m_followCPP = static_cast<int>(value);
                    break;
                case 151:
                    obj->m_legacyHSV = (value != 0.0f);
                    break;
                case 153:
                    obj->m_pulseMode = static_cast<int>(value);
                    break;
                case 154:
                    obj->m_pulseTargetType = static_cast<int>(value);
                    break;
                case 159:
                    obj->m_pulseMainOnly = (value != 0.0f);
                    break;
                case 160:
                    obj->m_pulseDetailOnly = (value != 0.0f);
                    break;
                case 161:
                    obj->m_pulseExclusive = (value != 0.0f);
                    break;
                case 162:
                    obj->m_copyColorID = static_cast<int>(value);
                    break;
                case 163:
                    obj->m_hsvValue.h = value;
                    break;
                case 165:
                    obj->m_hsvValue.s = value;
                    break;
                case 166:
                    obj->m_hsvValue.v = value;
                    break;
                case 167:
                    obj->m_hsvValue.absoluteSaturation = (value != 0.0f);
                    break;
                case 169:
                    obj->m_copyOpacity = (value != 0.0f);
                    break;
                case 171:
                    obj->m_triggerTargetColor.r = static_cast<unsigned char>(value);
                    break;
                case 172:
                    obj->m_triggerTargetColor.g = static_cast<unsigned char>(value);
                    break;
                case 173:
                    obj->m_triggerTargetColor.b = static_cast<unsigned char>(value);
                    break;
                case 175:
                    obj->m_isColorTrigger = (value != 0.0f);
                    break;
                case 176:
                    obj->m_activateGroup = static_cast<int>(value);
                    break;
                case 177:
                    obj->m_touchHoldMode = static_cast<int>(value);
                    break;
                case 179:
                    obj->m_touchToggleMode = static_cast<int>(value);
                    break;
                case 180:
                    obj->m_touchPlayerMode = static_cast<int>(value);
                    break;
                case 181:
                    obj->m_isDualMode = (value != 0.0f);
                    break;
                case 182:
                    obj->m_animationID = static_cast<int>(value);
                    break;
                case 183:
                    obj->m_spawnOrder = static_cast<int>(value);
                    break;
                case 184:
                    obj->m_isMultiTriggered = (value != 0.0f);
                    break;
                case 185:
                    obj->m_previewDisable = (value != 0.0f);
                    break;
                case 186:
                    obj->m_spawnOrdered = (value != 0.0f);
                    break;
                case 187:
                    obj->m_triggerOnExit = (value != 0.0f);
                    break;
                case 188:
                    obj->m_itemID2 = static_cast<int>(value);
                    break;
                case 189:
                    obj->m_controlID = static_cast<int>(value);
                    break;
                case 190:
                    obj->m_targetControlID = static_cast<int>(value);
                    break;
                case 191:
                    obj->m_subtractCount = (value != 0.0f);
                    break;
                case 192:
                    obj->m_collectibleIsPickupItem = (value != 0.0f);
                    break;
                case 194:
                    obj->m_collectibleIsToggleTrigger = (value != 0.0f);
                    break;
                case 195:
                    obj->m_collectibleParticleID = static_cast<int>(value);
                    break;
                case 196:
                    obj->m_collectiblePoints = static_cast<int>(value);
                    break;
                case 197:
                    obj->m_hasNoAnimation = (value != 0.0f);
                    break;
                case 199:
                    obj->m_isTouchTriggered = (value != 0.0f);
                    break;
                case 200:
                    obj->m_isSpawnTriggered = (value != 0.0f);
                    break;
                case 201:
                    obj->m_isMultiTriggered = (value != 0.0f);
                    break;
                case 203:
                    obj->m_rotationTargetID = static_cast<int>(value);
                    break;
                case 204:
                    obj->m_followYDelay = value;
                    break;
                case 205:
                    obj->m_followYMaxSpeed = value;
                    break;
                case 206:
                    obj->m_followYOffset = value;
                    break;
                case 207:
                    obj->m_cameraEasingValue = value;
                    break;
                case 208:
                    obj->m_cameraPaddingValue = value;
                    break;
                case 209:
                    obj->m_cameraIsFreeMode = (value != 0.0f);
                    break;
                case 212:
                    obj->m_cameraEditCameraSettings = (value != 0.0f);
                    break;
                case 213:
                    obj->m_cameraDisableGridSnap = (value != 0.0f);
                    break;
                case 214:
                    obj->m_speedModType = static_cast<int>(value);
                    break;
                case 218:
                    obj->m_gravityMod = value;
                    break;
                case 219:
                    obj->m_timeWarpTimeMod = value;
                    break;
                case 220:
                    obj->m_itemID = static_cast<int>(value);
                    break;
                case 221:
                    obj->m_spawnXPosition = value;
                    break;
                case 222:
                    obj->m_targetModCenterID = static_cast<int>(value);
                    break;
                case 223:
                    obj->m_lockObjectRotation = (value != 0.0f);
                    break;
                case 224:
                    obj->m_hasUniqueCoin = (value != 0.0f);
                    break;
                case 225:
                    obj->m_unknownBool = (value != 0.0f);
                    break;
                case 231:
                    obj->m_rotationSpeed = value;
                    break;
                case 232:
                    obj->m_rotationAngle = value;
                    break;
                case 233:
                    obj->m_rotationDelta = value;
                    break;
                case 234:
                    obj->m_rotationAnimationSpeed = value;
                    break;
                case 235:
                    obj->m_animationRandomizedStart = (value != 0.0f);
                    break;
                case 236:
                    obj->m_animationShouldUseSpeed = (value != 0.0f);
                    break;
                case 237:
                    obj->m_disableDelayedLoop = (value != 0.0f);
                    break;
                case 238:
                    obj->m_disableAnimShine = (value != 0.0f);
                    break;
                case 239:
                    obj->m_singleFrame = (value != 0.0f);
                    break;
                case 240:
                    obj->m_usesSpecialAnimation = (value != 0.0f);
                    break;
                case 241:
                    obj->m_randomFrameTime = value;
                    break;
                case 242:
                    obj->m_frameTime = value;
                    break;
                case 243:
                    obj->m_isNoMultiActivate = (value != 0.0f);
                    break;
                case 248:
                    obj->m_isMultiActivate = (value != 0.0f);
                    break;
                case 249:
                    obj->m_animationSpeed = value;
                    break;
                case 252:
                    obj->m_animationStart = value;
                    break;
                case 253:
                    obj->m_animationOffset = value;
                    break;
                case 260:
                    obj->m_animationTriggered = (value != 0.0f);
                    break;
                case 261:
                    obj->m_animateOnTrigger = (value != 0.0f);
                    break;
                case 262:
                    obj->m_animateOnlyWhenActive = (value != 0.0f);
                    break;
                case 263:
                    obj->m_hasCustomAnimation = (value != 0.0f);
                    break;
                case 264:
                    obj->m_hasCustomRotation = (value != 0.0f);
                    break;
                case 265:
                    obj->m_disableRotation = (value != 0.0f);
                    break;
                case 270:
                    obj->m_hasNoGlow = (value != 0.0f);
                    break;
                case 271:
                    obj->m_cantColorGlow = (value != 0.0f);
                    break;
                case 275:
                    obj->m_glowColorIsLBG = (value != 0.0f);
                    break;
                case 276:
                    obj->m_customGlowColor = static_cast<int>(value);
                    break;
                case 278:
                    obj->m_isNoTouch = (value != 0.0f);
                    break;
                case 280:
                    obj->m_isRingPoweredOn = (value != 0.0f);
                    break;
                case 281:
                    obj->m_poweredOn = (value != 0.0f);
                    break;
                case 282:
                    obj->m_isUIObject = (value != 0.0f);
                    break;
                case 283:
                    obj->m_isSolidColorBlock = (value != 0.0f);
                    break;
                case 286:
                    obj->m_isInvisibleBlock = (value != 0.0f);
                    break;
                case 287:
                    obj->m_isInvisible = (value != 0.0f);
                    break;
                case 288:
                    obj->m_hasNoEffects = (value != 0.0f);
                    break;
                case 290:
                    obj->m_hasNoParticles = (value != 0.0f);
                    break;
                case 291:
                    obj->m_particleLocked = (value != 0.0f);
                    break;
                case 292:
                    obj->m_isColorSpriteBlack = (value != 0.0f);
                    break;
                case 293:
                    obj->m_isObjectBlack = (value != 0.0f);
                    break;
                case 298:
                    obj->m_blackChildOpacityLocked = (value != 0.0f);
                    break;
                case 299:
                    obj->m_blackChildOpacity = value;
                    break;
                case 300:
                    obj->m_editorEnabled = (value != 0.0f);
                    break;
                case 301:
                    obj->m_isGroupDisabled = (value != 0.0f);
                    break;
                case 305:
                    obj->m_unk28c = (value != 0.0f);
                    break;
                case 306:
                    obj->m_activeMainColorID = static_cast<int>(value);
                    break;
                case 307:
                    obj->m_activeDetailColorID = static_cast<int>(value);
                    break;
                case 308:
                    obj->m_baseUsesHSV = (value != 0.0f);
                    break;
                case 309:
                    obj->m_detailUsesHSV = (value != 0.0f);
                    break;
                case 310:
                    obj->m_positionXOffset = value;
                    break;
                case 311:
                    obj->m_positionYOffset = value;
                    break;
                case 312:
                    obj->m_rotationXOffset = value;
                    break;
                case 313:
                    obj->m_rotationYOffset = value;
                    break;
                case 314:
                    obj->m_scaleXOffset = value;
                    break;
                case 315:
                    obj->m_scaleYOffset = value;
                    break;
                case 316:
                    obj->m_isFlipX = (value != 0.0f);
                    break;
                case 317:
                    obj->m_isFlipY = (value != 0.0f);
                    break;
                case 318:
                    obj->m_unk2BC = value;
                    break;
                case 319:
                    obj->m_unk2C0 = value;
                    break;
                case 320:
                    obj->m_customBoxOffset.x = value;
                    break;
                case 321:
                    obj->m_customBoxOffset.y = value;
                    break;
                case 322:
                    obj->m_boxOffsetCalculated = (value != 0.0f);
                    break;
                case 323:
                    obj->m_boxOffset.x = value;
                    break;
                case 324:
                    obj->m_boxOffset.y = value;
                    break;
                case 325:
                    obj->m_orientedBox = (value != 0.0f);
                    break;
                case 326:
                    obj->m_shouldUseOuterOb = (value != 0.0f);
                    break;
                case 327:
                    obj->m_addToNodeContainer = (value != 0.0f);
                    break;
                case 328:
                    obj->m_width = value;
                    break;
                case 329:
                    obj->m_height = value;
                    break;
                case 330:
                    obj->m_hasParticles = (value != 0.0f);
                    break;
                case 331:
                    obj->m_particleUseObjectColor = (value != 0.0f);
                    break;
                case 332:
                    obj->m_hasColorSprite = (value != 0.0f);
                    break;
                case 333:
                    obj->m_particleOffset.x = value;
                    break;
                case 334:
                    obj->m_particleOffset.y = value;
                    break;
                case 335:
                    obj->m_isParticleSpriteLocked = (value != 0.0f);
                    break;
                case 336:
                    obj->m_isDirty = (value != 0.0f);
                    break;
                case 337:
                    obj->m_isObjectPosDirty = (value != 0.0f);
                    break;
                case 338:
                    obj->m_isUnmodifiedPosDirty = (value != 0.0f);
                    break;
                case 339:
                    obj->m_fadeMargin = value;
                    break;
                case 340:
                    obj->m_isObjectRectDirty = (value != 0.0f);
                    break;
                case 341:
                    obj->m_isOrientedBoxDirty = (value != 0.0f);
                    break;
                case 343:
                    obj->m_colorSpriteLocked = (value != 0.0f);
                    break;
                case 344:
                    obj->m_unk353 = (value != 0.0f);
                    break;
                case 345:
                    obj->m_canRotateFree = (value != 0.0f);
                    break;
                case 346:
                    obj->m_isMirroredByScale = (value != 0.0f);
                    break;
                case 347:
                    obj->m_linkedGroup = static_cast<int>(value);
                    break;
                case 348:
                    obj->m_unk35C = static_cast<int>(value);
                    break;
                case 349:
                    obj->m_colorType = static_cast<int>(value);
                    break;
                case 350:
                    obj->m_childColorType = static_cast<int>(value);
                    break;
                case 351:
                    obj->m_shouldBlendBase = (value != 0.0f);
                    break;
                case 352:
                    obj->m_shouldBlendDetail = (value != 0.0f);
                    break;
                case 353:
                    obj->m_hasCustomChild = (value != 0.0f);
                    break;
                case 354:
                    obj->m_unk367 = (value != 0.0f);
                    break;
                case 355:
                    obj->m_isRotationAligned = (value != 0.0f);
                    break;
                case 357:
                    obj->m_uniqueID = static_cast<int>(value);
                    break;
                case 358:
                    obj->m_objectType = static_cast<int>(value);
                    break;
                case 359:
                    obj->m_unk390 = static_cast<int>(value);
                    break;
                case 360:
                    obj->m_objectRadius = value;
                    break;
                case 361:
                    obj->m_spriteWidthScale = value;
                    break;
                case 362:
                    obj->m_spriteHeightScale = value;
                    break;
                case 363:
                    obj->m_unmodifiedPositionX = value;
                    break;
                case 364:
                    obj->m_unmodifiedPositionY = value;
                    break;
                case 365:
                    obj->m_positionX = value;
                    break;
                case 366:
                    obj->m_positionY = value;
                    break;
                case 367:
                    obj->m_startPosition.x = value;
                    break;
                case 368:
                    obj->m_startPosition.y = value;
                    break;
                case 369:
                    obj->m_parentMode = static_cast<int>(value);
                    break;
                case 370:
                    obj->m_usesAudioScale = (value != 0.0f);
                    break;
                case 371:
                    obj->m_hasNoAudioScale = (value != 0.0f);
                    break;
                case 375:
                    obj->m_startScaleX = value;
                    break;
                case 376:
                    obj->m_startScaleY = value;
                    break;
                case 377:
                    obj->m_startFlipX = (value != 0.0f);
                    break;
                case 378:
                    obj->m_startFlipY = (value != 0.0f);
                    break;
                case 379:
                    obj->m_startRotationX = value;
                    break;
                case 380:
                    obj->m_startRotationY = value;
                    break;
                case 381:
                    obj->m_unk3ee = (value != 0.0f);
                    break;
                case 382:
                    obj->m_customScaleX = value;
                    break;
                case 383:
                    obj->m_customScaleY = value;
                    break;
                case 385:
                    obj->m_unk3DE = (value != 0.0f);
                    break;
                case 386:
                    obj->m_enterType = static_cast<int>(value);
                    break;
                case 387:
                    obj->m_unk3D8 = static_cast<int>(value);
                    break;
                case 388:
                    obj->m_exitType = static_cast<int>(value);
                    break;
                case 389:
                    obj->m_enterChannel = static_cast<int>(value);
                    break;
                case 390:
                    obj->m_objectMaterial = static_cast<int>(value);
                    break;
                case 391:
                    obj->m_unk3E8 = static_cast<int>(value);
                    break;
                case 392:
                    obj->m_objectID = static_cast<int>(value);
                    break;
                case 393:
                    obj->m_unk3F8 = static_cast<int>(value);
                    break;
                case 394:
                    obj->m_intrinsicDontFade = (value != 0.0f);
                    break;
                case 395:
                    obj->m_ignoreEnter = (value != 0.0f);
                    break;
                case 396:
                    obj->m_ignoreFade = (value != 0.0f);
                    break;
                case 397:
                    obj->m_unk3FD = (value != 0.0f);
                    break;
                case 399:
                    obj->m_customSpriteColor = static_cast<int>(value);
                    break;
                case 400:
                    obj->m_customColorType = static_cast<int>(value);
                    break;
                case 401:
                    obj->m_isDontEnter = (value != 0.0f);
                    break;
                case 402:
                    obj->m_isDontFade = (value != 0.0f);
                    break;
                case 403:
                    obj->m_defaultZOrder = static_cast<int>(value);
                    break;
                case 404:
                    obj->m_unk40C = static_cast<int>(value);
                    break;
                case 405:
                    obj->m_colorZLayerRelated = static_cast<int>(value);
                    break;
                case 406:
                    obj->m_customAudioScale = static_cast<int>(value);
                    break;
                case 407:
                    obj->m_property53 = (value != 0.0f);
                    break;
                case 408:
                    obj->m_slopeUphill = (value != 0.0f);
                    break;
                case 409:
                    obj->m_slopeDirection = static_cast<int>(value);
                    break;
                case 410:
                    obj->m_slopeIsHazard = (value != 0.0f);
                    break;
                case 411:
                    obj->m_baseColor = static_cast<int>(value);
                    break;
                case 412:
                    obj->m_detailColor = static_cast<int>(value);
                    break;
                case 413:
                    obj->m_baseOrDetailBlending = static_cast<int>(value);
                    break;
                case 414:
                    obj->m_defaultZLayer = static_cast<ZLayer>(static_cast<int>(value));
                    break;
                case 415:
                    obj->m_zFixedZLayer = static_cast<int>(value);
                    break;
                case 416:
                    obj->m_zLayer = static_cast<int>(value);
                    break;
                case 417:
                    obj->m_zOrder = static_cast<int>(value);
                    break;
                case 418:
                    obj->m_wasSelected = (value != 0.0f);
                    break;
                case 419:
                    obj->m_isSelected = (value != 0.0f);
                    break;
                case 420:
                    obj->m_unk460 = value;
                    break;
                case 421:
                    obj->m_minAudioScale = value;
                    break;
                case 422:
                    obj->m_maxAudioScale = value;
                    break;
                case 423:
                    obj->m_opacityMod = value;
                    break;
                case 424:
                    obj->m_opacityMod2 = value;
                    break;
                case 425:
                    obj->m_unk464.x = value;
                    break;
                case 426:
                    obj->m_unk464.y = value;
                    break;
                case 427:
                    obj->m_updateParents = (value != 0.0f);
                    break;
                case 428:
                    obj->m_updateEditorColor = (value != 0.0f);
                    break;
                case 431:
                    obj->m_hasGroupParent = (value != 0.0f);
                    break;
                case 432:
                    obj->m_hasAreaParent = (value != 0.0f);
                    break;
                case 433:
                    obj->m_groupCount = static_cast<int>(value);
                    break;
                case 434:
                    obj->m_hasGroupParentsString = (value != 0.0f);
                    break;
                case 436:
                    obj->m_colorGroupCount = static_cast<int>(value);
                    break;
                case 437:
                    obj->m_opacityGroupCount = static_cast<int>(value);
                    break;
                case 438:
                    obj->m_editorLayer = static_cast<int>(value);
                    break;
                case 439:
                    obj->m_editorLayer2 = static_cast<int>(value);
                    break;
                case 440:
                    obj->m_enabledGroupsCounter = static_cast<int>(value);
                    break;
                case 441:
                    obj->m_updateCustomContentSize = (value != 0.0f);
                    break;
                case 443:
                    obj->m_hasContentSize = (value != 0.0f);
                    break;
                case 444:
                    obj->m_scaleX = value;
                    break;
                case 445:
                    obj->m_scaleY = value;
                    break;
                case 446:
                    obj->m_lastSize.width = value;
                    break;
                case 447:
                    obj->m_lastSize.height = value;
                    break;
                case 448:
                    obj->m_lastPosition.x = value;
                    break;
                case 449:
                    obj->m_lastPosition.y = value;
                    break;
                case 450:
                    obj->m_unk4C0 = static_cast<int>(value);
                    break;
                case 451:
                    obj->m_unk4C4 = static_cast<int>(value);
                    break;
                case 452:
                    obj->m_unk4C8 = static_cast<int>(value);
                    break;
                case 453:
                    obj->m_unk4CC = static_cast<int>(value);
                    break;
                case 454:
                    obj->m_classType = static_cast<int>(value);
                    break;
                case 455:
                    obj->m_isTrigger = (value != 0.0f);
                    break;
                case 456:
                    obj->m_isSpawnOrderTrigger = (value != 0.0f);
                    break;
                case 457:
                    obj->m_isColorTrigger = (value != 0.0f);
                    break;
                case 458:
                    obj->m_dontIgnoreDuration = (value != 0.0f);
                    break;
                case 459:
                    obj->m_canBeControlled = (value != 0.0f);
                    break;
                case 460:
                    obj->m_activateTriggerInEditor = (value != 0.0f);
                    break;
                case 461:
                    obj->m_isHighDetail = (value != 0.0f);
                    break;
                case 462:
                    obj->m_unk4F8 = static_cast<int>(value);
                    break;
                case 463:
                    obj->m_isDecoration = (value != 0.0f);
                    break;
                case 464:
                    obj->m_isDecoration2 = (value != 0.0f);
                    break;
                case 465:
                    obj->m_unk4fb = (value != 0.0f);
                    break;
                case 466:
                    obj->m_maybeNotColorable = (value != 0.0f);
                    break;
                case 467:
                    obj->m_isPassable = (value != 0.0f);
                    break;
                case 468:
                    obj->m_isHide = (value != 0.0f);
                    break;
                case 469:
                    obj->m_isNonStickX = (value != 0.0f);
                    break;
                case 470:
                    obj->m_isNonStickY = (value != 0.0f);
                    break;
                case 471:
                    obj->m_isIceBlock = (value != 0.0f);
                    break;
                case 472:
                    obj->m_isGripSlope = (value != 0.0f);
                    break;
                case 473:
                    obj->m_isScaleStick = (value != 0.0f);
                    break;
                case 474:
                    obj->m_isExtraSticky = (value != 0.0f);
                    break;
                case 475:
                    obj->m_isDontBoostY = (value != 0.0f);
                    break;
                case 476:
                    obj->m_isDontBoostX = (value != 0.0f);
                    break;
                case 477:
                    obj->m_unk507 = (value != 0.0f);
                    break;
                case 478:
                    obj->m_unk508 = static_cast<int>(value);
                    break;
                case 479:
                    obj->m_unk50C = value;
                    break;
                case 480:
                    obj->m_pixelScaleX = value;
                    break;
                case 481:
                    obj->m_pixelScaleY = value;
                    break;
                case 482:
                    obj->m_areaOpacityRelated = static_cast<int>(value);
                    break;
                case 483:
                    obj->m_mainColorKeyIndex = static_cast<int>(value);
                    break;
                case 484:
                    obj->m_frames = static_cast<int>(value);
                    break;
                case 485:
                    obj->m_detailColorKeyIndex = static_cast<int>(value);
                    break;
                case 486:
                    obj->m_areaOpacityValue = value;
                    break;
                case 487:
                    obj->m_areaOpacityIndex = static_cast<int>(value);
                    break;
                case 489:
                    obj->m_unk52C = static_cast<int>(value);
                    break;
                case 491:
                    obj->m_unk530 = static_cast<int>(value);
                    break;
                case 492:
                    obj->m_greenDebugDraw = (value != 0.0f);
                    break;
                case 493:
                    obj->m_state = static_cast<int>(value);
                    break;
                case 494:
                    obj->m_unk548 = static_cast<int>(value);
                    break;
                case 497:
                    obj->m_visible = (value != 0.0f);
                    break;
                case 498:
                    obj->m_unk540 = value;
                    break;
                case 499:
                    obj->m_shouldNotHideAnimFreeze = (value != 0.0f);
                    break;
                case 500:
                    obj->m_unk544 = value;
                    break;
                case 501:
                    obj->m_unkAnimationInt = static_cast<int>(value);
                    break;
                case 502:
                    obj->m_maybeAnimationVariableXInt = static_cast<int>(value);
                    break;
                case 503:
                    obj->m_maybeAnimationVariableYInt = static_cast<int>(value);
                    break;
                case 504:
                    obj->m_hasUniqueCoin = (value != 0.0f);
                    break;
                case 505:
                    obj->m_unknownBool = (value != 0.0f);
                    break;
                case 506:
                    obj->m_unk6a8 = value;
                    break;
                case 508:
                    obj->m_unk698 = static_cast<int>(value);
                    break;
                case 510:
                    obj->m_unk6ac = value;
                    break;
                case 512:
                    obj->m_unk6b4 = static_cast<int>(value);
                    break;
                case 513:
                    obj->m_unk6b0 = value;
                    break;
                case 514:
                    obj->m_unk6f4 = static_cast<int>(value);
                    break;
                case 515:
                    obj->m_unk6f5 = (value != 0.0f);
                    break;
                case 516:
                    obj->m_speedStart.x = value;
                    break;
                case 517:
                    obj->m_speedStart.y = value;
                    break;
                case 518:
                    obj->m_endPosition.x = value;
                    break;
                case 519:
                    obj->m_endPosition.y = value;
                    break;
                case 520:
                    obj->m_unk708 = static_cast<int>(value);
                    break;
                case 521:
                    obj->m_spawnOrdered = (value != 0.0f);
                    break;
                case 522:
                    obj->m_previewDisable = (value != 0.0f);
                    break;
                case 524:
                    obj->m_hasFollower = (value != 0.0f);
                    break;
                case 525:
                    obj->m_propagateScaleChanges = (value != 0.0f);
                    break;
                case 526:
                    obj->m_someOtherIndex = static_cast<int>(value);
                    break;
                case 527:
                    obj->m_middleSectionIndex = static_cast<int>(value);
                    break;
                case 528:
                    obj->m_innerSectionIndex = static_cast<int>(value);
                    break;
                case 529:
                    obj->m_hasExtendedCollision = (value != 0.0f);
                    break;
                case 530:
                    obj->m_isDual = (value != 0.0f);
                    break;
                case 531:
                    obj->m_centerID = static_cast<int>(value);
                    break;
                case 532:
                    obj->m_rotID = static_cast<int>(value);
                    break;
                case 533:
                    obj->m_propagateFlipChanges = (value != 0.0f);
                    break;
                case 534:
                    obj->m_controlID = static_cast<int>(value);
                    break;
                case 535:
                    obj->m_targetControlID = static_cast<int>(value);
                    break;
                case 536:
                    obj->m_moveModX = value;
                    break;
                case 537:
                    obj->m_moveModY = value;
                    break;
                case 538:
                    obj->m_speedModType = static_cast<int>(value);
                    break;
                case 539:
                    obj->m_gravityMod = value;
                    break;
                case 540:
                    obj->m_duration = value;
                    break;
                case 541:
                    obj->m_shakeStrength = value;
                    break;
                case 542:
                    obj->m_shakeInterval = value;
                    break;
                case 543:
                    obj->m_directionModeDistance = value;
                    break;
                case 544:
                    obj->m_isSilent = (value != 0.0f);
                    break;
                case 545:
                    obj->m_rotationDegrees = value;
                    break;
                case 546:
                    obj->m_rotationOffset = value;
                    break;
                case 547:
                    obj->m_followXMod = value;
                    break;
                case 548:
                    obj->m_followYMod = value;
                    break;
                case 549:
                    obj->m_followYSpeed = value;
                    break;
                case 550:
                    obj->m_followYDelay = value;
                    break;
                case 551:
                    obj->m_followYMaxSpeed = value;
                    break;
                case 552:
                    obj->m_followYOffset = value;
                    break;
                case 553:
                    obj->m_followCPP = static_cast<int>(value);
                    break;
                case 554:
                    obj->m_targetPlayer1 = static_cast<int>(value);
                    break;
                case 555:
                    obj->m_targetPlayer2 = static_cast<int>(value);
                    break;
                case 556:
                    obj->m_ordValue = static_cast<int>(value);
                    break;
                case 557:
                    obj->m_channelValue = static_cast<int>(value);
                    break;
                case 558:
                    obj->m_unk2A8 = value;
                    break;
                case 559:
                    obj->m_rotationYOffset = value;
                    break;
                case 560:
                    obj->m_unk2B0 = value;
                    break;
                case 561:
                    obj->m_positionYOffset = value;
                    break;
                case 562:
                    obj->m_rotationXOffset = value;
                    break;
                case 563:
                    obj->m_positionXOffset = value;
                    break;
                case 564:
                    obj->m_scaleXOffset = value;
                    break;
                case 565:
                    obj->m_scaleYOffset = value;
                    break;
                case 566:
                    obj->m_unk2BC = value;
                    break;
                case 567:
                    obj->m_unk2C0 = value;
                    break;
                case 568:
                    obj->m_customBoxOffset.x = value;
                    break;
                case 569:
                    obj->m_customBoxOffset.y = value;
                    break;
                case 570:
                    obj->m_shouldUseOuterOb = (value != 0.0f);
                    break;
                case 571:
                    obj->m_boxOffsetCalculated = (value != 0.0f);
                    break;
                case 572:
                    obj->m_boxOffset.x = value;
                    break;
                case 573:
                    obj->m_boxOffset.y = value;
                    break;
                case 574:
                    obj->m_orientedBox = (value != 0.0f);
                    break;
                case 575:
                    obj->m_width = value;
                    break;
                case 576:
                    obj->m_height = value;
                    break;
                case 577:
                    obj->m_hasParticles = (value != 0.0f);
                    break;
                case 578:
                    obj->m_particleUseObjectColor = (value != 0.0f);
                    break;
                case 579:
                    obj->m_hasColorSprite = (value != 0.0f);
                    break;
                case 580:
                    obj->m_particleOffset.x = value;
                    break;
                case 581:
                    obj->m_particleOffset.y = value;
                    break;
                case 582:
                    obj->m_isParticleSpriteLocked = (value != 0.0f);
                    break;
                case 583:
                    obj->m_isDirty = (value != 0.0f);
                    break;
                case 584:
                    obj->m_isObjectPosDirty = (value != 0.0f);
                    break;
                case 585:
                    obj->m_isUnmodifiedPosDirty = (value != 0.0f);
                    break;
                case 586:
                    obj->m_fadeMargin = value;
                    break;
                case 587:
                    obj->m_isObjectRectDirty = (value != 0.0f);
                    break;
                case 588:
                    obj->m_isOrientedBoxDirty = (value != 0.0f);
                    break;
                case 589:
                    obj->m_colorSpriteLocked = (value != 0.0f);
                    break;
                case 590:
                    obj->m_unk353 = (value != 0.0f);
                    break;
                case 591:
                    obj->m_canRotateFree = (value != 0.0f);
                    break;
                case 592:
                    obj->m_isMirroredByScale = (value != 0.0f);
                    break;
                case 593:
                    obj->m_linkedGroup = static_cast<int>(value);
                    break;
                case 595:
                    obj->m_unk35C = static_cast<int>(value);
                    break;
                case 596:
                    obj->m_colorType = static_cast<int>(value);
                    break;
                case 597:
                    obj->m_childColorType = static_cast<int>(value);
                    break;
                case 598:
                    obj->m_shouldBlendBase = (value != 0.0f);
                    break;
                case 599:
                    obj->m_shouldBlendDetail = (value != 0.0f);
                    break;
                default:
                    break;
            }
        }
    }

    if (shouldLimitValue(property)) {
        float minValue = getMinSliderValue(property);
        float maxValue = getMaxSliderValue(property);

        if ((minValue != 0.0f || m_minSliderValues->objectForKey(cocos2d::CCString::createWithFormat("%i", property)->getCString())) && value < minValue) {
            updateValue(property, minValue);
            return;
        }
        if ((maxValue != 1.0f || m_maxSliderValues->objectForKey(cocos2d::CCString::createWithFormat("%i", property)->getCString())) && value > maxValue) {
            updateValue(property, maxValue);
            return;
        }
    }

    valueDidChange(property, value);
}

void SetupTriggerPopup::valueDidChange(int tag, float value) {
}

void SetupTriggerPopup::valuePopupClosed(ConfigureValuePopup* popup, float value) {
    std::string text = cocos2d::CCString::createWithFormat("%.02f", value)->getCString();
    float clampedValue = atof(text.c_str());

    float bound = 20.0f;
    if (clampedValue >= 20.0f) {
        clampedValue = bound;
    } else {
        bound = 0.1f;
        if (clampedValue <= 0.1f) {
            clampedValue = bound;
        }
    }

    int tag = popup->getTag();
    if (tag == 85) {
        m_easingRate = clampedValue;
        valueChanged(85, clampedValue);
        updateEaseRateLabel();
    } else {
        valueChanged(tag, clampedValue);
        updateCustomEaseRateLabel(tag, clampedValue);
    }
}

SetupTriggerPopup::~SetupTriggerPopup() {
}