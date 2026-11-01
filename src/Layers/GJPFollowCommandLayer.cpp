#include "Layers/GJPFollowCommandLayer.h"
#include "Objects/EffectGameObject.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"
#include "UI/InfoAlertButton.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJPFollowCommandLayer* GJPFollowCommandLayer::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    auto layer = new GJPFollowCommandLayer();
    if (layer && layer->init(object, objects)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJPFollowCommandLayer::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 430.0f, 310.0f, 1)) {
        return false;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto titleLabel = cocos2d::CCLabelBMFont::create("Follow Player Y", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, m_height * 0.5f + centerY - 15.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    auto okBtn = ButtonSprite::create("OK");
    auto okItem = CCMenuItemSpriteExtra::create(okBtn, this, menu_selector(GJPFollowCommandLayer::onClose));
    m_buttonMenu->addChild(okItem);
    
    auto helpTitle = cocos2d::CCString::create("Help");
    auto helpDesc = cocos2d::CCString::create(
        "Make a group of objects <cg>follow</c> the player's Y position.\n"
        "<cy>Speed</c> is how fast it snaps to the position. Leaving this at 1.0 makes it instantaneous.\n"
        "<cl>Max Speed</c> limits how fast the movement can be."
    );
    auto helpBtn = InfoAlertButton::create(helpTitle->getCString(), helpDesc->getCString(), 1.0f);
    m_buttonMenu->addChild(helpBtn);
    helpBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(
        18.0f - (m_width * 0.5f - centerX),
        m_height * 0.5f + centerY - 18.0f
    )));
    
    determineStartValues();
    
    cocos2d::CCPoint moveTimePos(centerX, centerY - 60.0f);
    
    auto moveTimeLabel = cocos2d::CCLabelBMFont::create("Move Time", "bigFont.fnt");
    moveTimeLabel->setScale(0.7f);
    moveTimeLabel->setPosition(moveTimePos);
    moveTimeLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(moveTimeLabel);
    
    auto moveTimeBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    moveTimeBg->setOpacity(100);
    moveTimeBg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
    m_mainLayer->addChild(moveTimeBg, -1);
    moveTimeBg->setPosition(moveTimePos + cocos2d::CCPoint(45.0f, 0.0f));
    
    const char* moveTimePlaceholder = (m_moveTime == -99999.0f) ? "Mixed" : "Num";
    m_moveTimeInput = CCTextInputNode::create(50.0f, 40.0f, moveTimePlaceholder, "Thonburi");
    m_mainLayer->addChild(m_moveTimeInput, 20);
    m_moveTimeInput->setPosition(moveTimeBg->getPosition());
    m_moveTimeInput->setMaxLabelScale(0.6f);
    m_moveTimeInput->setMaxLabelWidth(200.0f);
    m_moveTimeInput->m_textField->setVisible(false);
    m_moveTimeInput->m_maxLabelLength = 4;
    m_moveTimeInput->setAllowedChars("0123456789.");
    m_moveTimeInput->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
    m_moveTimeInput->setLabelPlaceholderScale(0.5f);
    m_moveTimeInput->m_delegate = this;
    m_moveTimeInput->m_numberInput = true;
    m_moveTimeInput->setTag(3);
    
    m_moveTimeSlider = Slider::create(this, menu_selector(GJPFollowCommandLayer::sliderChanged), 0.0f, 0.8f);
    m_moveTimeSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_moveTimeSlider);
    m_moveTimeSlider->setPosition(moveTimePos + cocos2d::CCPoint(0.0f, -32.0f));
    
    float sliderValue = m_moveTime / 10.0f;
    if (sliderValue < 0.0f) sliderValue = 0.0f;
    if (sliderValue > 1.0f) sliderValue = 1.0f;
    m_moveTimeSlider->setValue(sliderValue);
    updateDurLabel(false);
    
    const char* xLabel = "Speed";
    const char* yLabel = "Delay";
    const char* xPlaceholder = (m_speed == -99999.0f) ? "Mixed" : "Num";
    const char* yPlaceholder = (m_delay == -99999.0f) ? "Mixed" : "Num";
    
    for (int i = 0; i < 2; ++i) {
        float xOffset = (i == 0) ? -105.0f : 105.0f;
        cocos2d::CCPoint pos(centerX + xOffset, centerY + 95.0f);
        SEL_MenuHandler sliderHandler = (i == 0) ? menu_selector(GJPFollowCommandLayer::sliderXModChanged) : menu_selector(GJPFollowCommandLayer::sliderYModChanged);
        
        auto label = cocos2d::CCLabelBMFont::create((i == 0) ? xLabel : yLabel, "bigFont.fnt");
        label->setScale(0.7f);
        label->setPosition(pos);
        label->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        m_mainLayer->addChild(label);
        
        auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        bg->setOpacity(100);
        bg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(pos + cocos2d::CCPoint(45.0f, 0.0f));
        
        auto input = CCTextInputNode::create(60.0f, 40.0f, (i == 0) ? xPlaceholder : yPlaceholder, "Thonburi");
        m_mainLayer->addChild(input, 20);
        input->setPosition(bg->getPosition());
        input->setMaxLabelScale(0.6f);
        input->setMaxLabelWidth(200.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = 5;
        input->setAllowedChars("0123456789.-");
        input->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
        input->setLabelPlaceholderScale(0.5f);
        input->m_delegate = this;
        input->m_numberInput = true;
        input->setTag(i);
        
        if (i == 0) {
            m_speedInput = input;
        } else {
            m_delayInput = input;
        }
        
        auto slider = Slider::create(this, sliderHandler, 0.0f, 0.8f);
        slider->setBarVisibility(false);
        m_mainLayer->addChild(slider);
        slider->setPosition(pos + cocos2d::CCPoint(0.0f, -32.0f));
        
        float val = (i == 0) ? m_speed : m_delay;
        if (val == -99999.0f) val = 0.5f;
        slider->setValue(val);
        
        if (i == 0) {
            m_speedSlider = slider;
        } else {
            m_delaySlider = slider;
        }
    }
    
    SetupTriggerPopup::createMultiTriggerItemsDefaultVertical();
    
    cocos2d::CCPoint targetPos(centerX + 170.0f, centerY - 80.0f);
    
    auto targetLabel = cocos2d::CCLabelBMFont::create("Target\nGroup ID", "bigFont.fnt");
    targetLabel->setScale(0.5f);
    m_mainLayer->addChild(targetLabel);
    targetLabel->setPosition(targetPos + cocos2d::CCPoint(0.0f, 75.0f));
    
    auto targetBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    targetBg->setOpacity(100);
    targetBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(targetBg, -1);
    targetBg->setPosition(targetPos);
    
    const char* targetPlaceholder = (m_targetGroupID == -99999) ? "Mixed" : "Num";
    m_targetIDInput = CCTextInputNode::create(50.0f, 40.0f, targetPlaceholder, "Thonburi");
    m_mainLayer->addChild(m_targetIDInput, 20);
    m_targetIDInput->setPosition(targetPos);
    m_targetIDInput->setMaxLabelScale(0.6f);
    m_targetIDInput->setMaxLabelWidth(80.0f);
    m_targetIDInput->m_textField->setVisible(false);
    m_targetIDInput->m_maxLabelLength = 6;
    m_targetIDInput->setAllowedChars("0123456789");
    m_targetIDInput->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
    m_targetIDInput->setLabelPlaceholderScale(0.5f);
    m_targetIDInput->m_delegate = this;
    m_targetIDInput->m_numberInput = true;
    m_targetIDInput->setTag(2);
    
    auto upBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    upBtn->setScale(1.4f);
    upBtn->setRotation(90.0f);
    auto upItem = CCMenuItemSpriteExtra::create(upBtn, this, menu_selector(GJPFollowCommandLayer::onUpdateGroupID));
    m_buttonMenu->addChild(upItem);
    upItem->setTag(1);
    upItem->setPosition(m_buttonMenu->convertToNodeSpace(targetPos + cocos2d::CCPoint(0.0f, 40.0f)));
    upItem->setSizeMult(2.0f);
    
    auto downBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    downBtn->setScale(1.4f);
    downBtn->setRotation(90.0f);
    auto downItem = CCMenuItemSpriteExtra::create(downBtn, this, menu_selector(GJPFollowCommandLayer::onUpdateGroupID));
    m_buttonMenu->addChild(downItem);
    downItem->setTag(2);
    downItem->setPosition(m_buttonMenu->convertToNodeSpace(targetPos + cocos2d::CCPoint(0.0f, -40.0f)));
    downItem->setSizeMult(2.0f);
    
    cocos2d::CCPoint offsetPos(centerX - 100.0f, centerY);
    
    auto offsetLabel = cocos2d::CCLabelBMFont::create("Offset\n(player)", "bigFont.fnt");
    offsetLabel->setScale(0.5f);
    m_mainLayer->addChild(offsetLabel);
    offsetLabel->setPosition(offsetPos + cocos2d::CCPoint(0.0f, 30.0f));
    
    auto offsetBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    offsetBg->setOpacity(100);
    offsetBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(offsetBg, -1);
    offsetBg->setPosition(offsetPos);
    
    m_offsetInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_offsetInput, 20);
    m_offsetInput->setPosition(offsetPos);
    m_offsetInput->setMaxLabelScale(0.6f);
    m_offsetInput->setMaxLabelWidth(80.0f);
    m_offsetInput->m_textField->setVisible(false);
    m_offsetInput->m_maxLabelLength = 6;
    m_offsetInput->setAllowedChars("0123456789-");
    m_offsetInput->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
    m_offsetInput->setLabelPlaceholderScale(0.5f);
    m_offsetInput->m_delegate = this;
    m_offsetInput->m_numberInput = true;
    m_offsetInput->setTag(5);
    
    cocos2d::CCPoint maxSpeedPos(centerX + 100.0f, centerY);
    
    auto maxSpeedLabel = cocos2d::CCLabelBMFont::create("Max\nSpeed", "bigFont.fnt");
    maxSpeedLabel->setScale(0.5f);
    m_mainLayer->addChild(maxSpeedLabel);
    maxSpeedLabel->setPosition(maxSpeedPos + cocos2d::CCPoint(0.0f, 30.0f));
    
    auto maxSpeedBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
    maxSpeedBg->setOpacity(100);
    maxSpeedBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(maxSpeedBg, -1);
    maxSpeedBg->setPosition(maxSpeedPos);
    
    m_maxSpeedInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_maxSpeedInput, 20);
    m_maxSpeedInput->setPosition(maxSpeedPos);
    m_maxSpeedInput->setMaxLabelScale(0.6f);
    m_maxSpeedInput->setMaxLabelWidth(80.0f);
    m_maxSpeedInput->m_textField->setVisible(false);
    m_maxSpeedInput->m_maxLabelLength = 3;
    m_maxSpeedInput->setAllowedChars("0123456789.");
    m_maxSpeedInput->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
    m_maxSpeedInput->setLabelPlaceholderScale(0.5f);
    m_maxSpeedInput->m_delegate = this;
    m_maxSpeedInput->m_numberInput = true;
    m_maxSpeedInput->setTag(6);
    
    updateTextInputLabel();
    updateXModLabel();
    updateYModLabel();
    updateOffsetLabel();
    updateMaxSpeedLabel();
    
    m_disableTextDelegate = false;
    
    return true;
}

void GJPFollowCommandLayer::determineStartValues() {
    m_targetGroupID = -99999;
    m_offset = -99999;
    m_moveTime = -99999.0f;
    m_speed = -99999.0f;
    m_delay = -99999.0f;
    m_maxSpeed = -99999.0f;
    
    if (m_gameObject) {
        m_targetGroupID = m_gameObject->m_targetGroupID;
        m_touchTriggered = m_gameObject->m_isTouchTriggered;
        m_moveTime = m_gameObject->m_duration;
        m_speed = m_gameObject->m_followYSpeed;
        m_delay = m_gameObject->m_followYDelay;
        m_spawnTriggered = m_gameObject->m_isSpawnTriggered;
        m_offset = m_gameObject->m_followYOffset;
        m_maxSpeed = m_gameObject->m_followYMaxSpeed;
        m_multiTriggered = m_gameObject->m_isMultiTriggered;
        return;
    }
    
    float moveTime = -99999.0f;
    float speed = -99999.0f;
    float maxSpeed = -99999.0f;
    float delay = -99999.0f;
    int targetGroup = -99999;
    int offset = -99999;
    bool touch = true;
    bool spawn = true;
    bool multi = true;
    bool moveTimeMixed = false;
    bool speedMixed = false;
    bool maxSpeedMixed = false;
    bool delayMixed = false;
    bool targetGroupMixed = false;
    bool offsetMixed = false;
    
    for (int i = 0; i < m_gameObjects->count(); ++i) {
        auto obj = m_gameObjects->objectAtIndex(i);
        
        if (!obj->m_isTouchTriggered) touch = false;
        if (!obj->m_isSpawnTriggered) spawn = false;
        if (!obj->m_isMultiTriggered) multi = false;
        
        if (!moveTimeMixed) {
            if (moveTime == -99999.0f) {
                moveTime = obj->m_duration;
            } else if (moveTime != obj->m_duration) {
                moveTime = -99999.0f;
                moveTimeMixed = true;
            }
        }
        
        if (!speedMixed) {
            if (speed == -99999.0f) {
                speed = obj->m_followYSpeed;
            } else if (speed != obj->m_followYSpeed) {
                speed = -99999.0f;
                speedMixed = true;
            }
        }
        
        if (!maxSpeedMixed) {
            if (maxSpeed == -99999.0f) {
                maxSpeed = obj->m_followYMaxSpeed;
            } else if (maxSpeed != obj->m_followYMaxSpeed) {
                maxSpeed = -99999.0f;
                maxSpeedMixed = true;
            }
        }
        
        if (!delayMixed) {
            if (delay == -99999.0f) {
                delay = obj->m_followYDelay;
            } else if (delay != obj->m_followYDelay) {
                delay = -99999.0f;
                delayMixed = true;
            }
        }
        
        if (!targetGroupMixed) {
            if (targetGroup == -99999) {
                targetGroup = obj->m_targetGroupID;
            } else if (targetGroup != obj->m_targetGroupID) {
                targetGroupMixed = true;
                targetGroup = -99999;
            }
        }
        
        if (!offsetMixed) {
            if (offset == -99999) {
                offset = obj->m_followYOffset;
            } else if (offset != obj->m_followYOffset) {
                offsetMixed = true;
                offset = -99999;
            }
        }
    }
    
    m_touchTriggered = touch;
    m_spawnTriggered = spawn;
    m_multiTriggered = multi;
    
    if (moveTime != -99999.0f) m_moveTime = moveTime;
    if (speed != -99999.0f) m_speed = speed;
    if (maxSpeed != -99999.0f) m_maxSpeed = maxSpeed;
    if (delay != -99999.0f) m_delay = delay;
    if (targetGroup != -99999) m_targetGroupID = targetGroup;
    if (offset != -99999) m_offset = offset;
}

void GJPFollowCommandLayer::onClose(cocos2d::CCObject* sender) {
    if (m_moveTimeInput) {
        m_moveTimeInput->m_delegate = nullptr;
        m_moveTimeInput->onClickTrackNode(false);
    }
    if (m_targetIDInput) {
        m_targetIDInput->m_delegate = nullptr;
        m_targetIDInput->onClickTrackNode(false);
    }
    if (m_speedInput) {
        m_speedInput->m_delegate = nullptr;
        m_speedInput->onClickTrackNode(false);
    }
    if (m_delayInput) {
        m_delayInput->m_delegate = nullptr;
        m_delayInput->onClickTrackNode(false);
    }
    if (m_offsetInput) {
        m_offsetInput->m_delegate = nullptr;
        m_offsetInput->onClickTrackNode(false);
    }
    if (m_maxSpeedInput) {
        m_maxSpeedInput->m_delegate = nullptr;
        m_maxSpeedInput->onClickTrackNode(false);
    }
    SetupTriggerPopup::onClose(sender);
}

void GJPFollowCommandLayer::onUpdateGroupID(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int delta = (tag == 1) ? 1 : -1;
    m_targetGroupID += delta;
    if (m_targetGroupID < 1) m_targetGroupID = 1;
    if (m_targetGroupID > 9999) m_targetGroupID = 9999;
    updateTargetGroupID();
    SetupTriggerPopup::updateEditorLabel();
    updateTextInputLabel();
}

void GJPFollowCommandLayer::sliderChanged(cocos2d::CCObject* sender) {
    m_moveTime = SliderThumb::getValue(sender) * 10.0f;
    updateDuration();
    updateDurLabel(true);
}

void GJPFollowCommandLayer::sliderXModChanged(cocos2d::CCObject* sender) {
    m_speed = SliderThumb::getValue(sender);
    updateXModLabel();
    updateXMod();
}

void GJPFollowCommandLayer::sliderYModChanged(cocos2d::CCObject* sender) {
    m_delay = SliderThumb::getValue(sender);
    updateYModLabel();
    updateYMod();
}

void GJPFollowCommandLayer::textChanged(CCTextInputNode* node) {
    if (m_disableTextDelegate) return;
    
    int tag = node->getTag();
    const char* text = node->getString();
    
    switch (tag) {
        case 0: {
            float val = atof(text);
            m_speed = val;
            updateXMod();
            float clamped = m_speed;
            if (clamped < 0.0f) clamped = 0.0f;
            if (clamped > 1.0f) clamped = 1.0f;
            m_speedSlider->setValue(clamped);
            break;
        }
        case 1: {
            float val = atof(text);
            m_delay = val;
            updateYMod();
            float clamped = m_delay;
            if (clamped < 0.0f) clamped = 0.0f;
            if (clamped > 1.0f) clamped = 1.0f;
            m_delaySlider->setValue(clamped);
            break;
        }
        case 2: {
            int val = atoi(text);
            if (val < 1) val = 1;
            m_targetGroupID = val;
            updateTargetGroupID();
            SetupTriggerPopup::updateEditorLabel();
            break;
        }
        case 3: {
            float val = atof(text);
            m_moveTime = val;
            updateDuration();
            float clamped = m_moveTime / 10.0f;
            if (clamped < 0.0f) clamped = 0.0f;
            if (clamped > 1.0f) clamped = 1.0f;
            m_moveTimeSlider->setValue(clamped);
            break;
        }
        case 5: {
            int val = atoi(text);
            m_offset = val * 3;
            updatePlayerOffset();
            break;
        }
        case 6: {
            float val = atof(text);
            m_maxSpeed = val;
            updateMaxSpeed();
            break;
        }
    }
}

void GJPFollowCommandLayer::updateDurLabel(bool decimals) {
    m_disableTextDelegate = true;
    
    const char* text;
    if (m_moveTime == -99999.0f) {
        text = "Mixed";
    } else if (!decimals && m_moveTime == static_cast<int>(m_moveTime)) {
        text = cocos2d::CCString::createWithFormat("%i", static_cast<int>(m_moveTime))->getCString();
    } else {
        text = cocos2d::CCString::createWithFormat("%.02f", m_moveTime)->getCString();
    }
    
    m_moveTimeInput->setString(text);
    m_disableTextDelegate = false;
}

void GJPFollowCommandLayer::updateDuration() {
    if (m_gameObject) {
        m_gameObject->m_duration = m_moveTime;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_moveTime;
        }
    }
}

void GJPFollowCommandLayer::updateMaxSpeed() {
    if (m_gameObject) {
        m_gameObject->m_followYMaxSpeed = m_maxSpeed;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_followYMaxSpeed = m_maxSpeed;
        }
    }
}

void GJPFollowCommandLayer::updateMaxSpeedLabel() {
    m_disableTextDelegate = true;
    
    const char* text = (m_maxSpeed == -99999.0f) ? "" : cocos2d::CCString::createWithFormat("%0.1f", m_maxSpeed)->getCString();
    m_maxSpeedInput->setString(text);
    m_disableTextDelegate = false;
}

void GJPFollowCommandLayer::updateOffsetLabel() {
    m_disableTextDelegate = true;
    
    const char* text = (m_offset == -99999) ? "" : cocos2d::CCString::createWithFormat("%i", m_offset / 3)->getCString();
    m_offsetInput->setString(text);
    m_disableTextDelegate = false;
}

void GJPFollowCommandLayer::updatePlayerOffset() {
    if (m_gameObject) {
        m_gameObject->m_followYOffset = m_offset;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_followYOffset = m_offset;
        }
    }
}

void GJPFollowCommandLayer::updateTargetGroupID() {
    if (m_targetGroupID < 0) m_targetGroupID = 0;
    if (m_targetGroupID > 9999) m_targetGroupID = 9999;
    
    if (m_gameObject) {
        m_gameObject->setTargetID(m_targetGroupID);
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->setTargetID(m_targetGroupID);
        }
    }
}

void GJPFollowCommandLayer::updateTextInputLabel() {
    m_disableTextDelegate = true;
    
    const char* text = (m_targetGroupID == -99999) ? "" : cocos2d::CCString::createWithFormat("%i", m_targetGroupID)->getCString();
    m_targetIDInput->setString(text);
    m_disableTextDelegate = false;
}

void GJPFollowCommandLayer::updateXMod() {
    if (m_gameObject) {
        m_gameObject->m_followYSpeed = m_speed;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_followYSpeed = m_speed;
        }
    }
}

void GJPFollowCommandLayer::updateXModLabel() {
    m_disableTextDelegate = true;
    
    const char* text = (m_speed == -99999.0f) ? "" : cocos2d::CCString::createWithFormat("%0.3f", m_speed)->getCString();
    m_speedInput->setString(text);
    m_disableTextDelegate = false;
}

void GJPFollowCommandLayer::updateYMod() {
    if (m_gameObject) {
        m_gameObject->m_followYDelay = m_delay;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_followYDelay = m_delay;
        }
    }
}

void GJPFollowCommandLayer::updateYModLabel() {
    m_disableTextDelegate = true;
    
    const char* text = (m_delay == -99999.0f) ? "" : cocos2d::CCString::createWithFormat("%0.2f", m_delay)->getCString();
    m_delayInput->setString(text);
    m_disableTextDelegate = false;
}

GJPFollowCommandLayer::~GJPFollowCommandLayer() {
}