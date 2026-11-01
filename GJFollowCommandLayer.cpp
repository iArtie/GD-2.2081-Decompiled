// GJFollowCommandLayer.cpp (solo las partes que no están ya completas)
#include "GJFollowCommandLayer.h"
#include "EffectGameObject.h"
#include "CCTextInputNode.h"
#include "Slider.h"
#include "InfoAlertButton.h"

GJFollowCommandLayer* GJFollowCommandLayer::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    GJFollowCommandLayer* instance = new GJFollowCommandLayer();
    if (instance && instance->init(object, objects)) {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJFollowCommandLayer::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 430.0f, 310.0f, true)) {
        return false;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Title label
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create();
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 15.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    // OK button
    cocos2d::CCNode* okButton = ButtonSprite::create("OK");
    CCMenuItemSpriteExtra* okMenuItem = CCMenuItemSpriteExtra::create(okButton, nullptr, this, cocos2d::SEL_MenuHandler(&GJFollowCommandLayer::onClose));
    m_buttonMenu->addChild(okMenuItem);
    
    // Help button
    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Make a group of objects <cg>follow</c> a target object.\n"
        "<cy>Target Group ID</c> is the group of objects that follow the object using <cl>Follow Group ID</c>. Make sure that this group only contains one object or has a group ID parent.",
        1.0f
    );
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(cocos2d::CCPoint(
        18.0f - (m_width * 0.5f - winSize.width * 0.5f),
        m_height * 0.5f + winSize.height * 0.5f - 18.0f
    ));
    
    determineStartValues();
    
    // Move Time input
    cocos2d::CCPoint moveTimePos = cocos2d::CCPoint(winSize.width * 0.5f - 105.0f, winSize.height * 0.5f);
    
    cocos2d::CCLabelBMFont* moveTimeLabel = cocos2d::CCLabelBMFont::create();
    moveTimeLabel->setScale(0.6f);
    moveTimeLabel->setPosition(moveTimePos);
    moveTimeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(moveTimeLabel);
    
    // Move Time input background
    cocos2d::extension::CCScale9Sprite* moveTimeBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    moveTimeBg->setColor(cocos2d::ccc3(255, 255, 255));
    moveTimeBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(moveTimeBg, -1);
    moveTimeBg->setPosition(cocos2d::CCPoint(moveTimePos.x + 45.0f, moveTimePos.y));
    
    // Move Time input field
    const char* moveTimePlaceholder = (m_moveTime == -99999.0f) ? "Mixed" : "Num";
    m_moveTimeInput = CCTextInputNode::create(50.0f, 40.0f, moveTimePlaceholder, "Thonburi");
    m_mainLayer->addChild(m_moveTimeInput, 20);
    m_moveTimeInput->setPosition(moveTimeBg->getPosition());
    m_moveTimeInput->setMaxLabelScale(0.6f);
    m_moveTimeInput->setMaxLabelWidth(200.0f);
    m_moveTimeInput->m_textField->setVisible(false);
    m_moveTimeInput->m_maxLabelLength = 4;
    m_moveTimeInput->setAllowedChars("0123456789.");
    m_moveTimeInput->setLabelPlaceholderColor(cocos2d::ccc3(120, 120, 120));
    m_moveTimeInput->setLabelPlaceholderScale(0.5f);
    m_moveTimeInput->m_delegate = this;
    m_moveTimeInput->m_numberInput = true;
    m_moveTimeInput->setTag(3);
    
    // Move Time slider
    m_moveTimeSlider = Slider::create(this, (cocos2d::SEL_MenuHandler)&GJFollowCommandLayer::sliderChanged, 0, 0.8f);
    m_moveTimeSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_moveTimeSlider);
    m_moveTimeSlider->setPosition(cocos2d::CCPoint(moveTimePos.x, moveTimePos.y - 32.0f));
    
    float sliderValue = m_moveTime / 10.0f;
    if (sliderValue < 0.0f) sliderValue = 0.0f;
    if (sliderValue > 1.0f) sliderValue = 1.0f;
    m_moveTimeSlider->setValue(sliderValue);
    
    updateDurLabel(false);
    
    // X Mod and Y Mod inputs
    for (int i = 0; i < 2; ++i) {
        bool isYMod = (i == 1);
        float xOffset = isYMod ? 105.0f : -105.0f;
        float yOffset = winSize.height * 0.5f + 95.0f;
        cocos2d::CCPoint modPos = cocos2d::CCPoint(winSize.width * 0.5f + xOffset, yOffset);
        
        // Label
        cocos2d::CCLabelBMFont* modLabel = cocos2d::CCLabelBMFont::create();
        modLabel->setScale(0.7f);
        modLabel->setPosition(modPos);
        modLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        m_mainLayer->addChild(modLabel);
        
        // Background
        cocos2d::extension::CCScale9Sprite* modBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        modBg->setColor(cocos2d::ccc3(255, 255, 255));
        modBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
        m_mainLayer->addChild(modBg, -1);
        modBg->setPosition(cocos2d::CCPoint(modPos.x + 45.0f, modPos.y));
        
        // Input field
        float modValue = isYMod ? m_yMod : m_xMod;
        const char* placeholder = (modValue == -99999.0f) ? "Mixed" : "Num";
        CCTextInputNode* modInput = CCTextInputNode::create(60.0f, 40.0f, placeholder, "Thonburi");
        m_mainLayer->addChild(modInput, 20);
        modInput->setPosition(modBg->getPosition());
        modInput->setMaxLabelScale(0.6f);
        modInput->setMaxLabelWidth(200.0f);
        modInput->m_textField->setVisible(false);
        modInput->m_maxLabelLength = 5;
        modInput->setAllowedChars("0123456789.-");
        modInput->setLabelPlaceholderColor(cocos2d::ccc3(120, 120, 120));
        modInput->setLabelPlaceholderScale(0.5f);
        modInput->m_delegate = this;
        modInput->m_numberInput = true;
        modInput->setTag(isYMod ? 1 : 0);
        
        if (isYMod) {
            m_yModInput = modInput;
        } else {
            m_xModInput = modInput;
        }
        
        // Slider
        Slider* modSlider = Slider::create(
            this,
            isYMod ? (cocos2d::SEL_MenuHandler)&GJFollowCommandLayer::sliderYModChanged : (cocos2d::SEL_MenuHandler)&GJFollowCommandLayer::sliderXModChanged,
            0, 0.8f
        );
        modSlider->setBarVisibility(false);
        m_mainLayer->addChild(modSlider);
        modSlider->setPosition(cocos2d::CCPoint(modPos.x, modPos.y - 32.0f));
        
        float modSliderValue = (modValue == -99999.0f) ? 0.5f : modValue * 0.5f + 0.5f;
        modSlider->setValue(modSliderValue);
        
        if (isYMod) {
            m_yModSlider = modSlider;
        } else {
            m_xModSlider = modSlider;
        }
    }
    
    // Multi-trigger items
    createMultiTriggerItemsDefaultVertical();
    
    // Target Group ID and Follow Group ID inputs
    for (int i = 0; i < 2; ++i) {
        bool isFollowID = (i == 1);
        
        cocos2d::CCPoint idPos = cocos2d::CCPoint(
            winSize.width * 0.5f + (isFollowID ? 160.0f : 80.0f),
            winSize.height * 0.5f - 70.0f
        );
        
        const char* labelText = isFollowID ? "Follow\nGroup ID" : "Target\nGroup ID";
        int currentID = isFollowID ? m_followGroupID : m_targetGroupID;
        int tag = isFollowID ? 4 : 2;
        cocos2d::SEL_MenuHandler handler = isFollowID ? (cocos2d::SEL_MenuHandler)&GJFollowCommandLayer::onUpdateGroupID2 : (cocos2d::SEL_MenuHandler)&GJFollowCommandLayer::onUpdateGroupID;
        
        // Label
        cocos2d::CCLabelBMFont* idLabel = cocos2d::CCLabelBMFont::create();
        idLabel->setScale(0.5f);
        m_mainLayer->addChild(idLabel);
        idLabel->setPosition(cocos2d::CCPoint(idPos.x, idPos.y + 75.0f));
        
        // Background
        cocos2d::extension::CCScale9Sprite* idBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        idBg->setColor(cocos2d::ccc3(255, 255, 255));
        idBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
        m_mainLayer->addChild(idBg, -1);
        idBg->setPosition(idPos);
        
        // Input field
        const char* idPlaceholder = (currentID == -99999) ? "Mixed" : "Num";
        CCTextInputNode* idInput = CCTextInputNode::create(50.0f, 40.0f, idPlaceholder, "Thonburi");
        m_mainLayer->addChild(idInput, 20);
        idInput->setPosition(idPos);
        idInput->setMaxLabelScale(0.6f);
        idInput->setMaxLabelWidth(80.0f);
        idInput->m_textField->setVisible(false);
        idInput->m_maxLabelLength = 6;
        idInput->setAllowedChars("0123456789");
        idInput->setLabelPlaceholderColor(cocos2d::ccc3(120, 120, 120));
        idInput->setLabelPlaceholderScale(0.5f);
        idInput->m_delegate = this;
        idInput->m_numberInput = true;
        idInput->setTag(tag);
        
        if (isFollowID) {
            m_followIDInput = idInput;
        } else {
            m_targetIDInput = idInput;
        }
        
        // Left button
        cocos2d::CCSprite* leftBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        leftBtn->setScale(1.4f);
        leftBtn->setRotation(90.0f);
        CCMenuItemSpriteExtra* leftMenuItem = CCMenuItemSpriteExtra::create(leftBtn, nullptr, this, handler);
        m_buttonMenu->addChild(leftMenuItem);
        leftMenuItem->setTag(1);
        leftMenuItem->setPosition(cocos2d::CCPoint(idPos.x, idPos.y + 40.0f));
        leftMenuItem->setSizeMult(2.0f);
        
        // Right button
        cocos2d::CCSprite* rightBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        rightBtn->setScale(1.4f);
        rightBtn->setRotation(90.0f);
        CCMenuItemSpriteExtra* rightMenuItem = CCMenuItemSpriteExtra::create(rightBtn, nullptr, this, handler);
        m_buttonMenu->addChild(rightMenuItem);
        rightMenuItem->setTag(2);
        rightMenuItem->setPosition(cocos2d::CCPoint(idPos.x, idPos.y - 40.0f));
        rightMenuItem->setSizeMult(2.0f);
    }
    
    updateTextInputLabel();
    updateTextInputLabel2();
    updateXModLabel();
    updateYModLabel();
    
    m_disableTextDelegate = false;
    return true;
}

void GJFollowCommandLayer::determineStartValues() {
    m_targetGroupID = -99999;
    m_followGroupID = -99999;
    m_moveTime = -99999.0f;
    m_xMod = -99999.0f;
    m_yMod = -99999.0f;
    
    if (m_gameObject) {
        m_targetGroupID = m_gameObject->m_targetGroupID;
        m_touchTriggered = m_gameObject->m_isTouchTriggered;
        m_moveTime = m_gameObject->m_duration;
        m_xMod = m_gameObject->m_followXMod;
        m_yMod = m_gameObject->m_followYMod;
        m_spawnTriggered = m_gameObject->m_isSpawnTriggered;
        m_followGroupID = m_gameObject->m_centerGroupID;
        m_multiTriggered = m_gameObject->m_isMultiTriggered;
        return;
    }
    
    float moveTime = -99999.0f;
    float xMod = -99999.0f;
    float yMod = -99999.0f;
    int targetGroupID = -99999;
    int followGroupID = -99999;
    bool touchTriggered = true;
    bool spawnTriggered = true;
    bool multiTriggered = true;
    int moveTimeCount = 0;
    int xModCount = 0;
    int yModCount = 0;
    int targetGroupIDCount = 0;
    int followGroupIDCount = 0;
    
    for (int i = 0; i < m_gameObjects->count(); ++i) {
        EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
        
        if (!obj->m_isTouchTriggered) touchTriggered = false;
        if (!obj->m_isSpawnTriggered) spawnTriggered = false;
        if (!obj->m_isMultiTriggered) multiTriggered = false;
        
        if (!moveTimeCount) {
            if (moveTime == -99999.0f) {
                moveTime = obj->m_duration;
            } else if (moveTime != obj->m_duration) {
                moveTime = -99999.0f;
                moveTimeCount = 1;
            }
        }
        
        if (!xModCount) {
            if (xMod == -99999.0f) {
                xMod = obj->m_followXMod;
            } else if (xMod != obj->m_followXMod) {
                xMod = -99999.0f;
                xModCount = 1;
            }
        }
        
        if (!yModCount) {
            if (yMod == -99999.0f) {
                yMod = obj->m_followYMod;
            } else if (yMod != obj->m_followYMod) {
                yMod = -99999.0f;
                yModCount = 1;
            }
        }
        
        if (!targetGroupIDCount) {
            if (targetGroupID == -99999) {
                targetGroupID = obj->m_targetGroupID;
            } else if (targetGroupID != obj->m_targetGroupID) {
                targetGroupIDCount = 1;
                targetGroupID = -99999;
            }
        }
        
        if (!followGroupIDCount) {
            if (followGroupID == -99999) {
                followGroupID = obj->m_centerGroupID;
            } else if (followGroupID != obj->m_centerGroupID) {
                followGroupIDCount = 1;
                followGroupID = -99999;
            }
        }
    }
    
    if (moveTime != -99999.0f) m_moveTime = moveTime;
    if (xMod != -99999.0f) m_xMod = xMod;
    if (yMod != -99999.0f) m_yMod = yMod;
    if (targetGroupID != -99999) m_targetGroupID = targetGroupID;
    if (followGroupID != -99999) m_followGroupID = followGroupID;
    
    m_touchTriggered = touchTriggered;
    m_spawnTriggered = spawnTriggered;
    m_multiTriggered = multiTriggered;
}

void GJFollowCommandLayer::onClose(cocos2d::CCObject* sender) {
    m_moveTimeInput->m_delegate = nullptr;
    m_moveTimeInput->onClickTrackNode(nullptr);
    m_targetIDInput->m_delegate = nullptr;
    m_targetIDInput->onClickTrackNode(nullptr);
    m_followIDInput->m_delegate = nullptr;
    m_followIDInput->onClickTrackNode(nullptr);
    m_xModInput->m_delegate = nullptr;
    m_xModInput->onClickTrackNode(nullptr);
    m_yModInput->m_delegate = nullptr;
    m_yModInput->onClickTrackNode(nullptr);
    SetupTriggerPopup::onClose(sender);
}

void GJFollowCommandLayer::onUpdateGroupID(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int newID = (tag == 1) ? m_targetGroupID + 1 : m_targetGroupID - 1;
    
    if (newID < 1) newID = 1;
    if (newID > 9999) newID = 9999;
    
    m_targetGroupID = newID;
    updateTargetGroupID();
    updateEditorLabel();
    updateTextInputLabel();
}

void GJFollowCommandLayer::onUpdateGroupID2(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int newID = (tag == 1) ? m_followGroupID + 1 : m_followGroupID - 1;
    
    if (newID < 0) newID = 0;
    if (newID > 9999) newID = 9999;
    
    m_followGroupID = newID;
    updateTargetGroupID2();
    updateTextInputLabel2();
}

void GJFollowCommandLayer::sliderChanged(cocos2d::CCObject* sender) {
    m_moveTime = SliderThumb::getValue(static_cast<SliderThumb*>(sender)) * 10.0f;
    updateDuration();
    updateDurLabel(true);
}

void GJFollowCommandLayer::sliderXModChanged(cocos2d::CCObject* sender) {
    m_xMod = SliderThumb::getValue(static_cast<SliderThumb*>(sender)) * 2.0f - 1.0f;
    updateXModLabel();
    updateXMod();
}

void GJFollowCommandLayer::sliderYModChanged(cocos2d::CCObject* sender) {
    m_yMod = SliderThumb::getValue(static_cast<SliderThumb*>(sender)) * 2.0f - 1.0f;
    updateYModLabel();
    updateYMod();
}

void GJFollowCommandLayer::textChanged(CCTextInputNode* node) {
    if (m_disableTextDelegate) return;
    
    int tag = node->getTag();
    switch (tag) {
        case 0: { // X Mod
            std::string str = node->getString();
            float value = atof(str.c_str());
            m_xMod = value;
            updateXMod();
            float sliderValue = m_xMod * 0.5f + 0.5f;
            if (sliderValue < 0.0f) sliderValue = 0.0f;
            if (sliderValue > 1.0f) sliderValue = 1.0f;
            m_xModSlider->setValue(sliderValue);
            break;
        }
        case 1: { // Y Mod
            std::string str = node->getString();
            float value = atof(str.c_str());
            m_yMod = value;
            updateYMod();
            float sliderValue = m_yMod * 0.5f + 0.5f;
            if (sliderValue < 0.0f) sliderValue = 0.0f;
            if (sliderValue > 1.0f) sliderValue = 1.0f;
            m_yModSlider->setValue(sliderValue);
            break;
        }
        case 2: { // Target Group ID
            std::string str = node->getString();
            int value = atoi(str.c_str());
            if (value < 1) value = 1;
            m_targetGroupID = value;
            updateTargetGroupID();
            updateEditorLabel();
            break;
        }
        case 3: { // Move Time
            std::string str = node->getString();
            float value = atof(str.c_str());
            m_moveTime = value;
            updateDuration();
            float sliderValue = m_moveTime / 10.0f;
            if (sliderValue < 0.0f) sliderValue = 0.0f;
            if (sliderValue > 1.0f) sliderValue = 1.0f;
            m_moveTimeSlider->setValue(sliderValue);
            break;
        }
        case 4: { // Follow Group ID
            std::string str = node->getString();
            int value = atoi(str.c_str());
            if (value < 0) value = 0;
            m_followGroupID = value;
            updateTargetGroupID2();
            break;
        }
    }
}

void GJFollowCommandLayer::updateDurLabel(bool decimals) {
    m_disableTextDelegate = true;
    
    std::string text;
    if (m_moveTime == -99999.0f) {
        text = "Mixed";
    } else if (!decimals && (m_moveTime - (int)m_moveTime) == 0.0f) {
        text = cocos2d::CCString::createWithFormat("%i", (int)m_moveTime)->getCString();
    } else {
        text = cocos2d::CCString::createWithFormat("%.02f", m_moveTime)->getCString();
    }
    
    m_moveTimeInput->setString(text);
    m_disableTextDelegate = false;
}

void GJFollowCommandLayer::updateDuration() {
    if (m_gameObject) {
        m_gameObject->m_duration = m_moveTime;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->m_duration = m_moveTime;
        }
    }
}

void GJFollowCommandLayer::updateTargetGroupID() {
    if (m_targetGroupID < 0) m_targetGroupID = 0;
    if (m_targetGroupID > 9999) m_targetGroupID = 9999;
    
    if (m_gameObject) {
        m_gameObject->setTargetID(m_targetGroupID);
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->setTargetID(m_targetGroupID);
        }
    }
}

void GJFollowCommandLayer::updateTargetGroupID2() {
    if (m_followGroupID < 0) m_followGroupID = 0;
    if (m_followGroupID > 9999) m_followGroupID = 9999;
    
    if (m_gameObject) {
        m_gameObject->setTargetID2(m_followGroupID);
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->setTargetID2(m_followGroupID);
        }
    }
}

void GJFollowCommandLayer::updateTextInputLabel() {
    m_disableTextDelegate = true;
    
    std::string text;
    if (m_targetGroupID == -99999) {
        text = "";
    } else {
        text = cocos2d::CCString::createWithFormat("%i", m_targetGroupID)->getCString();
    }
    
    m_targetIDInput->setString(text);
    m_disableTextDelegate = false;
}

void GJFollowCommandLayer::updateTextInputLabel2() {
    m_disableTextDelegate = true;
    
    std::string text;
    if (m_followGroupID == -99999) {
        text = "";
    } else {
        text = cocos2d::CCString::createWithFormat("%i", m_followGroupID)->getCString();
    }
    
    m_followIDInput->setString(text);
    m_disableTextDelegate = false;
}

void GJFollowCommandLayer::updateXMod() {
    if (m_gameObject) {
        m_gameObject->m_followXMod = m_xMod;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->m_followXMod = m_xMod;
        }
    }
}

void GJFollowCommandLayer::updateXModLabel() {
    m_disableTextDelegate = true;
    
    std::string text;
    if (m_xMod == -99999.0f) {
        text = "";
    } else {
        text = cocos2d::CCString::createWithFormat("%0.2f", m_xMod)->getCString();
    }
    
    m_xModInput->setString(text);
    m_disableTextDelegate = false;
}

void GJFollowCommandLayer::updateYMod() {
    if (m_gameObject) {
        m_gameObject->m_followYMod = m_yMod;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            EffectGameObject* obj = static_cast<EffectGameObject*>(m_gameObjects->objectAtIndex(i));
            obj->m_followYMod = m_yMod;
        }
    }
}

void GJFollowCommandLayer::updateYModLabel() {
    m_disableTextDelegate = true;
    
    std::string text;
    if (m_yMod == -99999.0f) {
        text = "";
    } else {
        text = cocos2d::CCString::createWithFormat("%0.2f", m_yMod)->getCString();
    }
    
    m_yModInput->setString(text);
    m_disableTextDelegate = false;
}

GJFollowCommandLayer::~GJFollowCommandLayer() {
    // Base class handles cleanup
}