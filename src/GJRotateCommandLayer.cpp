#include "GJRotateCommandLayer.h"
#include "EffectGameObject.h"
#include "CCTextInputNode.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "InfoAlertButton.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameToolbox.h"
#include "ConfigureValuePopup.h"

GJRotateCommandLayer* GJRotateCommandLayer::create(EffectGameObject* object, cocos2d::CCArray* objects) {
    auto layer = new GJRotateCommandLayer();
    if (layer && layer->init(object, objects)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJRotateCommandLayer::init(EffectGameObject* object, cocos2d::CCArray* objects) {
    if (!SetupTriggerPopup::init(object, objects, 430.0f, 310.0f, 1)) {
        return false;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    auto titleLabel = cocos2d::CCLabelBMFont::create("Rotate", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(centerX, m_height * 0.5f + centerY - 15.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);
    
    auto okBtn = ButtonSprite::create("OK");
    auto okItem = CCMenuItemSpriteExtra::create(okBtn, this, menu_selector(GJRotateCommandLayer::onClose));
    m_buttonMenu->addChild(okItem);
    
    auto helpTitle = cocos2d::CCString::create("Rotate command help");
    auto helpDesc = cocos2d::CCString::create(
        "<cg>Rotate a group of objects.</c>\n"
        "<cy>Set Center Group ID to rotate around an object.</c>\n"
        "<cl>Only one object can be in the center group.</c>\n"
        "<co>Use disable optim if rotating from a far away center point and the movement is laggy.</c>"
    );
    auto helpBtn = InfoAlertButton::create(helpTitle->getCString(), helpDesc->getCString(), 1.0f);
    m_buttonMenu->addChild(helpBtn);
    helpBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(
        m_width * 0.5f + centerX - 18.0f,
        m_height * 0.5f + centerY - 18.0f
    )));
    
    determineStartValues();
    
    cocos2d::CCPoint moveTimePos(centerX - 90.0f, centerY - 50.0f);
    
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
    m_moveTimeInput->m_maxLabelLength = 5;
    m_moveTimeInput->setAllowedChars("0123456789.");
    m_moveTimeInput->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
    m_moveTimeInput->setLabelPlaceholderScale(0.5f);
    m_moveTimeInput->m_delegate = this;
    m_moveTimeInput->m_numberInput = true;
    m_moveTimeInput->setTag(3);
    
    m_moveTimeSlider = Slider::create(this, menu_selector(GJRotateCommandLayer::sliderChanged), 0.0f, 0.8f);
    m_moveTimeSlider->setBarVisibility(false);
    m_mainLayer->addChild(m_moveTimeSlider);
    m_moveTimeSlider->setPosition(moveTimePos + cocos2d::CCPoint(0.0f, -32.0f));
    
    float sliderValue = m_moveTime / 10.0f;
    if (sliderValue < 0.0f) sliderValue = 0.0f;
    if (sliderValue > 1.0f) sliderValue = 1.0f;
    m_moveTimeSlider->setValue(sliderValue);
    updateDurLabel(false);
    
    auto easingLabel = cocos2d::CCLabelBMFont::create("Easing", "bigFont.fnt");
    easingLabel->setPosition(cocos2d::CCPoint(centerX - 90.0f, centerY + 20.0f));
    m_mainLayer->addChild(easingLabel);
    easingLabel->setScale(0.8f);
    
    m_rotateEasingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_rotateEasingLabel);
    m_rotateEasingLabel->setPosition(easingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_rotateEasingLabel->setScale(0.7f);
    
    auto rateSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    rateSprite->setColor(cocos2d::ccc3(125, 125, 125));
    rateSprite->setOpacity(100);
    rateSprite->setScaleX(0.8f);
    rateSprite->setScaleY(0.4f);
    
    m_rotateEasingRateLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    rateSprite->addChild(m_rotateEasingRateLabel);
    m_rotateEasingRateLabel->setScaleX(0.375f);
    m_rotateEasingRateLabel->setScaleY(0.75f);
    m_rotateEasingRateLabel->setPosition(cocos2d::CCPoint(1.0f, 1.5f));
    
    m_rotateEasingRateButton = CCMenuItemSpriteExtra::create(rateSprite, this, menu_selector(GJRotateCommandLayer::onEasingRate));
    m_buttonMenu->addChild(m_rotateEasingRateButton);
    m_rotateEasingRateButton->setPosition(m_buttonMenu->convertToNodeSpace(
        easingLabel->getPosition() + cocos2d::CCPoint(55.0f, -2.0f)
    ));
    m_rotateEasingRateButton->setSizeMult(1.2f);
    
    updateEasingRateLabel();
    toggleEasingRateVisibility();
    
    auto leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrow->setScale(0.7f);
    auto leftItem = CCMenuItemSpriteExtra::create(leftArrow, this, menu_selector(GJRotateCommandLayer::onEasing));
    m_buttonMenu->addChild(leftItem);
    leftItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_rotateEasingLabel->getPosition() + cocos2d::CCPoint(-100.0f, 0.0f)
    ));
    leftItem->setSizeMult(2.0f);
    leftItem->setTag(0);
    
    auto rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrow->setFlipX(true);
    rightArrow->setScale(0.7f);
    auto rightItem = CCMenuItemSpriteExtra::create(rightArrow, this, menu_selector(GJRotateCommandLayer::onEasing));
    m_buttonMenu->addChild(rightItem);
    rightItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_rotateEasingLabel->getPosition() + cocos2d::CCPoint(100.0f, 0.0f)
    ));
    rightItem->setSizeMult(2.0f);
    rightItem->setTag(1);
    
    updateEasingLabel();
    
    const char* degLabel = "Degrees";
    const char* timesLabel = "Times";
    const char* degPlaceholder = (m_degrees == -99999.0f) ? "Mixed" : "Num";
    const char* timesPlaceholder = (m_times360 == -99999) ? "Mixed" : "Num";
    
    for (int i = 0; i < 2; ++i) {
        float xOffset = (i == 0) ? -105.0f : 105.0f;
        cocos2d::CCPoint pos(centerX + xOffset, centerY + 105.0f);
        SEL_MenuHandler sliderHandler = (i == 0) ? menu_selector(GJRotateCommandLayer::sliderDegreesChanged) : menu_selector(GJRotateCommandLayer::sliderTimesChanged);
        
        auto label = cocos2d::CCLabelBMFont::create((i == 0) ? degLabel : timesLabel, "bigFont.fnt");
        label->setScale(0.7f);
        label->setPosition(pos);
        label->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        m_mainLayer->addChild(label);
        
        auto bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        bg->setOpacity(100);
        bg->setContentSize(cocos2d::CCSize(70.0f, 30.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(pos + cocos2d::CCPoint(45.0f, 0.0f));
        
        auto input = CCTextInputNode::create(60.0f, 40.0f, (i == 0) ? degPlaceholder : timesPlaceholder, "Thonburi");
        m_mainLayer->addChild(input, 20);
        input->setPosition(bg->getPosition());
        input->setMaxLabelScale(0.6f);
        input->setMaxLabelWidth(60.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = 8;
        input->setAllowedChars("0123456789-.");
        input->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
        input->setLabelPlaceholderScale(0.5f);
        input->m_delegate = this;
        input->m_numberInput = true;
        input->setTag(i);
        
        if (i == 0) {
            m_degreesInput = input;
        } else {
            m_timesInput = input;
        }
        
        auto slider = Slider::create(this, sliderHandler, 0.0f, 0.8f);
        slider->setBarVisibility(false);
        m_mainLayer->addChild(slider);
        slider->setPosition(pos + cocos2d::CCPoint(0.0f, -32.0f));
        
        float val = (i == 0) ? m_degrees / 720.0f + 0.5f : m_times360 / 20.0f + 0.5f;
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        slider->setValue(val);
        
        if (i == 0) {
            m_degreesSlider = slider;
        } else {
            m_timesSlider = slider;
        }
        
        if (i == 1) {
            SetupTriggerPopup::createMultiTriggerItemsDefaultHorizontal();
            
            for (int j = 0; j < 2; ++j) {
                cocos2d::CCPoint targetPos;
                const char* labelText;
                int tag;
                SEL_MenuHandler updateHandler;
                
                if (j == 0) {
                    targetPos = cocos2d::CCPoint(centerX + 90.0f, centerY - 58.0f);
                    labelText = "Target\nGroup ID";
                    tag = 2;
                    updateHandler = menu_selector(GJRotateCommandLayer::onUpdateGroupID);
                } else {
                    targetPos = cocos2d::CCPoint(centerX + 170.0f, centerY - 58.0f);
                    labelText = "Center\nGroup ID";
                    tag = 4;
                    updateHandler = menu_selector(GJRotateCommandLayer::onUpdateGroupID2);
                }
                
                auto targetLabel = cocos2d::CCLabelBMFont::create(labelText, "bigFont.fnt");
                targetLabel->setScale(0.5f);
                m_mainLayer->addChild(targetLabel);
                targetLabel->setPosition(targetPos + cocos2d::CCPoint(0.0f, 75.0f));
                
                auto targetBg = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
                targetBg->setOpacity(100);
                targetBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
                m_mainLayer->addChild(targetBg, -1);
                targetBg->setPosition(targetPos);
                
                const char* placeholder = (j == 0) ?
                    ((m_targetGroupID == -99999) ? "Mixed" : "Num") :
                    ((m_centerGroupID == -99999) ? "Mixed" : "Num");
                
                auto inputNode = CCTextInputNode::create(50.0f, 40.0f, placeholder, "Thonburi");
                m_mainLayer->addChild(inputNode, 20);
                inputNode->setPosition(targetPos);
                inputNode->setMaxLabelScale(0.6f);
                inputNode->setMaxLabelWidth(80.0f);
                inputNode->m_textField->setVisible(false);
                inputNode->m_maxLabelLength = 6;
                inputNode->setAllowedChars("0123456789");
                inputNode->setLabelPlaceholderColor(cocos2d::ccc3(170, 170, 240));
                inputNode->setLabelPlaceholderScale(0.5f);
                inputNode->m_delegate = this;
                inputNode->m_numberInput = true;
                inputNode->setTag(tag);
                
                if (j == 0) {
                    m_targetIDInput = inputNode;
                } else {
                    m_centerIDInput = inputNode;
                }
                
                auto upBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
                upBtn->setScale(1.4f);
                upBtn->setRotation(90.0f);
                auto upItem = CCMenuItemSpriteExtra::create(upBtn, this, updateHandler);
                m_buttonMenu->addChild(upItem);
                upItem->setTag(1);
                upItem->setPosition(m_buttonMenu->convertToNodeSpace(targetPos + cocos2d::CCPoint(0.0f, 40.0f)));
                upItem->setSizeMult(2.0f);
                
                auto downBtn = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
                downBtn->setScale(1.4f);
                downBtn->setRotation(90.0f);
                auto downItem = CCMenuItemSpriteExtra::create(downBtn, this, updateHandler);
                m_buttonMenu->addChild(downItem);
                downItem->setTag(2);
                downItem->setPosition(m_buttonMenu->convertToNodeSpace(targetPos + cocos2d::CCPoint(0.0f, -40.0f)));
                downItem->setSizeMult(2.0f);
            }
            
            updateTextInputLabel();
            updateTextInputLabel2();
        }
        
        if (i == 0) {
            // Lock object rotation toggle
            auto lockLabel = cocos2d::CCLabelBMFont::create("Lock object rotation", "bigFont.fnt");
            lockLabel->setScale(0.5f);
            lockLabel->setPosition(pos + cocos2d::CCPoint(60.0f, 48.0f));
            m_mainLayer->addChild(lockLabel);
            
            GameToolbox::createToggleButton(
                "",
                menu_selector(GJRotateCommandLayer::onLockRotation),
                0,
                m_lockObjectRotation,
                m_buttonMenu,
                pos + cocos2d::CCPoint(60.0f, 48.0f),
                this,
                m_mainLayer,
                0.6f,
                0.4f,
                0.0f,
                cocos2d::CCPoint(8.0f, 0.0f),
                "bigFont.fnt",
                false,
                false,
                0
            );
        } else {
            // Follow P1/P2 toggles
            for (int p = 0; p < 2; ++p) {
                const char* followLabel = (p == 0) ? "Follow\nP1" : "Follow\nP2";
                bool initial = (p == 0) ? m_followP1 : m_followP2;
                int tag = (p == 0) ? 1 : 2;
                
                auto followText = cocos2d::CCLabelBMFont::create(followLabel, "bigFont.fnt");
                followText->setScale(0.5f);
                followText->setPosition(pos + cocos2d::CCPoint(60.0f + p * 90.0f, 48.0f));
                m_mainLayer->addChild(followText);
                
                auto toggler = GameToolbox::createToggleButton(
                    "",
                    menu_selector(GJRotateCommandLayer::onFollowRotation),
                    0,
                    initial,
                    m_buttonMenu,
                    pos + cocos2d::CCPoint(60.0f + p * 90.0f, 48.0f),
                    this,
                    m_mainLayer,
                    0.6f,
                    0.4f,
                    0.0f,
                    cocos2d::CCPoint(8.0f, 0.0f),
                    "bigFont.fnt",
                    false,
                    false,
                    0
                );
                toggler->setTag(tag);
                
                if (p == 0) {
                    m_followP1Toggler = toggler;
                } else {
                    m_followP2Toggler = toggler;
                }
            }
        }
    }
    
    updateDegreesLabel();
    updateTimesLabel();
    m_disableTextChanged = false;
    
    return true;
}

void GJRotateCommandLayer::determineStartValues() {
    m_rotateEasingType = static_cast<EasingType>(-99999);
    m_times360 = -99999;
    m_targetGroupID = -99999;
    m_centerGroupID = -99999;
    m_moveTime = -99999.0f;
    m_degrees = -99999.0f;
    m_rotateEasingRate = -99999.0f;
    m_lockObjectRotation = true;
    m_followP1 = false;
    m_followP2 = false;
    
    if (m_gameObject) {
        m_rotateEasingType = m_gameObject->m_easingType;
        m_times360 = m_gameObject->m_times360;
        m_moveTime = m_gameObject->m_duration;
        m_rotateEasingRate = m_gameObject->m_easingRate;
        m_degrees = m_gameObject->m_rotationDegrees;
        m_targetGroupID = m_gameObject->m_targetGroupID;
        m_touchTriggered = m_gameObject->m_isTouchTriggered;
        m_spawnTriggered = m_gameObject->m_isSpawnTriggered;
        m_centerGroupID = m_gameObject->m_centerGroupID;
        m_lockObjectRotation = m_gameObject->m_lockObjectRotation;
        m_multiTriggered = m_gameObject->m_isMultiTriggered;
        m_followP1 = m_gameObject->m_targetPlayer1;
        m_followP2 = m_gameObject->m_targetPlayer2;
        return;
    }
    
    float moveTime = -99999.0f;
    float easingRate = -99999.0f;
    float degrees = -99999.0f;
    int times360 = -99999;
    int targetGroup = -99999;
    int centerGroup = -99999;
    EasingType easingType = static_cast<EasingType>(-99999);
    bool touch = true;
    bool spawn = true;
    bool multi = true;
    bool lockRotation = true;
    bool followP1 = false;
    bool followP2 = false;
    bool moveTimeMixed = false;
    bool easingTypeMixed = false;
    bool easingRateMixed = false;
    bool degreesMixed = false;
    bool times360Mixed = false;
    bool targetGroupMixed = false;
    bool centerGroupMixed = false;
    
    for (int i = 0; i < m_gameObjects->count(); ++i) {
        auto obj = m_gameObjects->objectAtIndex(i);
        
        if (!obj->m_isTouchTriggered) touch = false;
        if (!obj->m_isSpawnTriggered) spawn = false;
        if (!obj->m_isMultiTriggered) multi = false;
        if (!obj->m_lockObjectRotation) lockRotation = false;
        if (!obj->m_targetPlayer1) followP1 = false;
        if (!obj->m_targetPlayer2) followP2 = false;
        
        if (!moveTimeMixed) {
            if (moveTime == -99999.0f) {
                moveTime = obj->m_duration;
            } else if (moveTime != obj->m_duration) {
                moveTime = -99999.0f;
                moveTimeMixed = true;
            }
        }
        
        if (!easingTypeMixed) {
            if (easingType == static_cast<EasingType>(-99999)) {
                easingType = obj->m_easingType;
            } else if (easingType != obj->m_easingType) {
                easingTypeMixed = true;
                easingType = static_cast<EasingType>(-99999);
            }
        }
        
        if (!easingRateMixed) {
            if (easingRate == -99999.0f) {
                easingRate = obj->m_easingRate;
            } else if (easingRate != obj->m_easingRate) {
                easingRate = -99999.0f;
                easingRateMixed = true;
            }
        }
        
        if (!degreesMixed) {
            if (degrees == -99999.0f) {
                degrees = obj->m_rotationDegrees;
            } else if (degrees != obj->m_rotationDegrees) {
                degrees = -99999.0f;
                degreesMixed = true;
            }
        }
        
        if (!times360Mixed) {
            if (times360 == -99999) {
                times360 = obj->m_times360;
            } else if (times360 != obj->m_times360) {
                times360Mixed = true;
                times360 = -99999;
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
        
        if (!centerGroupMixed) {
            if (centerGroup == -99999) {
                centerGroup = obj->m_centerGroupID;
            } else if (centerGroup != obj->m_centerGroupID) {
                centerGroupMixed = true;
                centerGroup = -99999;
            }
        }
    }
    
    m_touchTriggered = touch;
    m_spawnTriggered = spawn;
    m_multiTriggered = multi;
    m_lockObjectRotation = lockRotation;
    m_followP1 = followP1;
    m_followP2 = followP2;
    
    if (moveTime != -99999.0f) m_moveTime = moveTime;
    if (easingType != static_cast<EasingType>(-99999)) m_rotateEasingType = easingType;
    if (easingRate != -99999.0f) m_rotateEasingRate = easingRate;
    if (degrees != -99999.0f) m_degrees = degrees;
    if (times360 != -99999) m_times360 = times360;
    if (targetGroup != -99999) m_targetGroupID = targetGroup;
    if (centerGroup != -99999) m_centerGroupID = centerGroup;
}

void GJRotateCommandLayer::onClose(cocos2d::CCObject* sender) {
    if (m_moveTimeInput) {
        m_moveTimeInput->m_delegate = nullptr;
        m_moveTimeInput->onClickTrackNode(false);
    }
    if (m_targetIDInput) {
        m_targetIDInput->m_delegate = nullptr;
        m_targetIDInput->onClickTrackNode(false);
    }
    if (m_centerIDInput) {
        m_centerIDInput->m_delegate = nullptr;
        m_centerIDInput->onClickTrackNode(false);
    }
    if (m_degreesInput) {
        m_degreesInput->m_delegate = nullptr;
        m_degreesInput->onClickTrackNode(false);
    }
    if (m_timesInput) {
        m_timesInput->m_delegate = nullptr;
        m_timesInput->onClickTrackNode(false);
    }
    SetupTriggerPopup::onClose(sender);
}

void GJRotateCommandLayer::onEasing(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int newType = static_cast<int>(m_rotateEasingType) + (tag == 1 ? 1 : -1);
    if (newType < 0) newType = 0;
    if (newType > 12) newType = 12;
    m_rotateEasingType = static_cast<EasingType>(newType);
    updateMoveCommandEasing();
    updateEasingLabel();
    toggleEasingRateVisibility();
}

void GJRotateCommandLayer::onEasingRate(cocos2d::CCObject* sender) {
    auto popup = ConfigureValuePopup::create(this, m_rotateEasingRate, 0.1f, 4.0f, "Easing Rate", "");
    popup->show();
}

void GJRotateCommandLayer::onFollowRotation(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    if (tag == 1) {
        m_followP1 = !m_followP1;
        if (m_followP1Toggler) m_followP1Toggler->toggle(m_followP1);
    } else {
        m_followP2 = !m_followP2;
        if (m_followP2Toggler) m_followP2Toggler->toggle(m_followP2);
    }
    
    if (m_gameObject) {
        m_gameObject->m_targetPlayer1 = m_followP1;
        m_gameObject->m_targetPlayer2 = m_followP2;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_targetPlayer1 = m_followP1;
            obj->m_targetPlayer2 = m_followP2;
        }
    }
}

void GJRotateCommandLayer::onLockRotation(cocos2d::CCObject* sender) {
    m_lockObjectRotation = !m_lockObjectRotation;
    
    if (m_gameObject) {
        m_gameObject->m_lockObjectRotation = m_lockObjectRotation;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_lockObjectRotation = m_lockObjectRotation;
        }
    }
}

void GJRotateCommandLayer::onUpdateGroupID(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    m_targetGroupID += (tag == 1) ? 1 : -1;
    if (m_targetGroupID < 1) m_targetGroupID = 1;
    if (m_targetGroupID > 999) m_targetGroupID = 999;
    updateTargetGroupID();
    SetupTriggerPopup::updateEditorLabel();
    updateTextInputLabel();
}

void GJRotateCommandLayer::onUpdateGroupID2(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    m_centerGroupID += (tag == 1) ? 1 : -1;
    if (m_centerGroupID < 0) m_centerGroupID = 0;
    if (m_centerGroupID > 999) m_centerGroupID = 999;
    updateTargetGroupID2();
    updateTextInputLabel2();
}

void GJRotateCommandLayer::sliderChanged(cocos2d::CCObject* sender) {
    m_moveTime = SliderThumb::getValue(sender) * 10.0f;
    updateDuration();
    updateDurLabel(true);
}

void GJRotateCommandLayer::sliderDegreesChanged(cocos2d::CCObject* sender) {
    m_degrees = floorf(SliderThumb::getValue(sender) * 720.0f) - 360.0f;
    updateDegreesLabel();
    updateCommandDegrees();
}

void GJRotateCommandLayer::sliderTimesChanged(cocos2d::CCObject* sender) {
    m_times360 = static_cast<int>(floorf(SliderThumb::getValue(sender) * 20.0f) - 10.0f);
    updateTimesLabel();
    updateCommandTimes();
}

void GJRotateCommandLayer::textChanged(CCTextInputNode* node) {
    if (m_disableTextChanged) return;
    
    int tag = node->getTag();
    const char* text = node->getString();
    
    switch (tag) {
        case 0: {
            float val = atof(text);
            m_degrees = truncateFloat(val);
            updateCommandDegrees();
            float clamped = m_degrees / 720.0f + 0.5f;
            if (clamped < 0.0f) clamped = 0.0f;
            if (clamped > 1.0f) clamped = 1.0f;
            m_degreesSlider->setValue(clamped);
            break;
        }
        case 1: {
            int val = atoi(text);
            m_times360 = val;
            updateCommandTimes();
            float clamped = static_cast<float>(m_times360) / 20.0f + 0.5f;
            if (clamped < 0.0f) clamped = 0.0f;
            if (clamped > 1.0f) clamped = 1.0f;
            m_timesSlider->setValue(clamped);
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
        case 4: {
            int val = atoi(text);
            if (val < 0) val = 0;
            m_centerGroupID = val;
            updateTargetGroupID2();
            break;
        }
    }
}

void GJRotateCommandLayer::toggleEasingRateVisibility() {
    bool visible = GameToolbox::isRateEasing(m_rotateEasingType);
    if (!visible && m_gameObjects) {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            if (GameToolbox::isRateEasing(obj->m_easingType)) {
                visible = true;
                break;
            }
        }
    }
    m_rotateEasingRateButton->setVisible(visible);
}

void GJRotateCommandLayer::updateCommandDegrees() {
    if (m_gameObject) {
        m_gameObject->m_rotationDegrees = m_degrees;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_rotationDegrees = m_degrees;
        }
    }
}

void GJRotateCommandLayer::updateCommandTimes() {
    if (m_gameObject) {
        m_gameObject->m_times360 = m_times360;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_times360 = m_times360;
        }
    }
}

void GJRotateCommandLayer::updateDegreesLabel() {
    m_disableTextChanged = true;
    const char* text = (m_degrees == -99999.0f) ? "" : truncFloatToStr(m_degrees).c_str();
    m_degreesInput->setString(text);
    m_disableTextChanged = false;
}

void GJRotateCommandLayer::updateDurLabel(bool decimals) {
    m_disableTextChanged = true;
    
    const char* text;
    if (m_moveTime == -99999.0f) {
        text = "Mixed";
    } else if (!decimals && m_moveTime == static_cast<int>(m_moveTime)) {
        text = cocos2d::CCString::createWithFormat("%i", static_cast<int>(m_moveTime))->getCString();
    } else {
        text = cocos2d::CCString::createWithFormat("%.02f", m_moveTime)->getCString();
    }
    
    m_moveTimeInput->setString(text);
    m_disableTextChanged = false;
}

void GJRotateCommandLayer::updateDuration() {
    if (m_gameObject) {
        m_gameObject->m_duration = m_moveTime;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_duration = m_moveTime;
        }
    }
}

void GJRotateCommandLayer::updateEasingLabel() {
    const char* text = "Mixed";
    if (m_rotateEasingType != static_cast<EasingType>(-99999)) {
        text = GameToolbox::easeToText(m_rotateEasingType).c_str();
    }
    m_rotateEasingLabel->setString(text);
    m_rotateEasingLabel->limitLabelWidth(150.0f, 0.7f, 0.0f);
}

void GJRotateCommandLayer::updateEasingRateLabel() {
    const char* text = (m_rotateEasingRate == -99999.0f) ? "?" : cocos2d::CCString::createWithFormat("%.02f", m_rotateEasingRate)->getCString();
    m_rotateEasingRateLabel->setString(text);
}

void GJRotateCommandLayer::updateMoveCommandEasing() {
    if (m_gameObject) {
        m_gameObject->m_easingType = m_rotateEasingType;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_easingType = m_rotateEasingType;
        }
    }
}

void GJRotateCommandLayer::updateMoveCommandEasingRate() {
    if (m_gameObject) {
        m_gameObject->m_easingRate = m_rotateEasingRate;
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->m_easingRate = m_rotateEasingRate;
        }
    }
}

void GJRotateCommandLayer::updateTargetGroupID() {
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

void GJRotateCommandLayer::updateTargetGroupID2() {
    if (m_centerGroupID < 0) m_centerGroupID = 0;
    if (m_centerGroupID > 9999) m_centerGroupID = 9999;
    
    if (m_gameObject) {
        m_gameObject->setTargetID2(m_centerGroupID);
    } else {
        for (int i = 0; i < m_gameObjects->count(); ++i) {
            auto obj = m_gameObjects->objectAtIndex(i);
            obj->setTargetID2(m_centerGroupID);
        }
    }
}

void GJRotateCommandLayer::updateTextInputLabel() {
    m_disableTextChanged = true;
    const char* text = (m_targetGroupID == -99999) ? "" : cocos2d::CCString::createWithFormat("%i", m_targetGroupID)->getCString();
    m_targetIDInput->setString(text);
    m_disableTextChanged = false;
}

void GJRotateCommandLayer::updateTextInputLabel2() {
    m_disableTextChanged = true;
    const char* text = (m_centerGroupID == -99999) ? "" : cocos2d::CCString::createWithFormat("%i", m_centerGroupID)->getCString();
    m_centerIDInput->setString(text);
    m_disableTextChanged = false;
}

void GJRotateCommandLayer::updateTimesLabel() {
    m_disableTextChanged = true;
    const char* text = (m_times360 == -99999) ? "" : cocos2d::CCString::createWithFormat("%i", m_times360)->getCString();
    m_timesInput->setString(text);
    m_disableTextChanged = false;
}

void GJRotateCommandLayer::valuePopupClosed(ConfigureValuePopup* popup, float value) {
    float val = value;
    if (val < 0.1f) val = 0.1f;
    if (val > 20.0f) val = 20.0f;
    m_rotateEasingRate = val;
    updateMoveCommandEasingRate();
    updateEasingRateLabel();
}

GJRotateCommandLayer::~GJRotateCommandLayer() {
}