#include "Editor/GJScaleControl.h"
#include "Editor/GJScaleControlDelegate.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

GJScaleControl* GJScaleControl::create() {
    auto control = new GJScaleControl();
    if (control && control->init()) {
        control->autorelease();
        return control;
    }
    delete control;
    return nullptr;
}

bool GJScaleControl::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
    setTouchEnabled(true);
    setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    
    m_scaleLocked = false;
    m_touchID = -1;
    m_scaleButtonType = 0;
    m_valueX = 1.0f;
    m_valueY = 1.0f;
    m_changedValueX = 1.0f;
    m_changedValueY = 1.0f;
    m_lowerBound = 0.1f;
    m_upperBound = 2.0f;
    m_scale1Lock = true;
    m_delegate = nullptr;
    
    auto lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("warpLockOffBtn_001.png");
    m_scaleLockButton = CCMenuItemSpriteExtra::create(lockSprite, this, menu_selector(GJScaleControl::onToggleLockScale));
    auto menu = cocos2d::CCMenu::createWithItem(m_scaleLockButton);
    addChild(menu, 3);
    
    m_sliderX = Slider::create(this, menu_selector(GJScaleControl::sliderChanged), 0.0f, "sliderBar.png", "slidergroove_02.png", "GJ_moveBtn.png", "GJ_moveSBtn.png", 1.0f);
    m_sliderX->setBarVisibility(false);
    addChild(m_sliderX, 2);
    m_sliderX->disableTouch();
    m_sliderX->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    
    m_sliderY = Slider::create(this, menu_selector(GJScaleControl::sliderChanged), 0.0f, "sliderBar.png", "slidergroove_02.png", "GJ_moveBtn.png", "GJ_moveSBtn.png", 1.0f);
    m_sliderY->setBarVisibility(false);
    addChild(m_sliderY, 2);
    m_sliderY->disableTouch();
    m_sliderY->setPosition(m_sliderX->getPosition() + cocos2d::CCPoint(0.0f, 60.0f));
    
    m_sliderXY = Slider::create(this, menu_selector(GJScaleControl::sliderChanged), 0.0f, "sliderBar.png", "slidergroove_02.png", "GJ_moveBtn.png", "GJ_moveSBtn.png", 1.0f);
    m_sliderXY->setBarVisibility(false);
    addChild(m_sliderXY, 2);
    m_sliderXY->disableTouch();
    m_sliderXY->setPosition(m_sliderX->getPosition());
    
    m_scaleXLabel = cocos2d::CCLabelBMFont::create("ScaleX 1.00", "bigFont.fnt");
    addChild(m_scaleXLabel);
    m_scaleXLabel->setPosition(m_sliderX->getPosition() + cocos2d::CCPoint(0.0f, 30.0f));
    
    m_scaleYLabel = cocos2d::CCLabelBMFont::create("ScaleY 1.00", "bigFont.fnt");
    addChild(m_scaleYLabel);
    m_scaleYLabel->setPosition(m_sliderY->getPosition() + cocos2d::CCPoint(0.0f, 30.0f));
    
    m_scaleLabel = cocos2d::CCLabelBMFont::create("Scale: 1.00", "bigFont.fnt");
    addChild(m_scaleLabel);
    m_scaleLabel->setPosition(m_sliderXY->getPosition() + cocos2d::CCPoint(0.0f, 30.0f));
    
    return true;
}

bool GJScaleControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != -1) {
        return false;
    }
    
    if (m_sliderX->ccTouchBegan(touch, event)) {
        m_touchID = touch->getID();
        m_valueX = scaleFromValue(m_sliderX->getValue());
        if (m_delegate) {
            m_delegate->scaleChangeBegin();
        }
        m_scaleButtonType = 0;
        return true;
    }
    
    if (m_sliderY->ccTouchBegan(touch, event)) {
        m_touchID = touch->getID();
        m_valueY = scaleFromValue(m_sliderY->getValue());
        if (m_delegate) {
            m_delegate->scaleChangeBegin();
        }
        m_scaleButtonType = 1;
        return true;
    }
    
    if (m_sliderXY->ccTouchBegan(touch, event)) {
        m_touchID = touch->getID();
        if (m_delegate) {
            m_delegate->scaleChangeBegin();
        }
        m_scaleButtonType = 2;
        return true;
    }
    
    return false;
}

void GJScaleControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != touch->getID()) {
        return;
    }
    
    Slider* activeSlider = nullptr;
    if (m_scaleButtonType == 0) {
        activeSlider = m_sliderX;
    } else if (m_scaleButtonType == 1) {
        activeSlider = m_sliderY;
    } else {
        activeSlider = m_sliderXY;
    }
    
    activeSlider->ccTouchMoved(touch, event);
    float val = activeSlider->getValue();
    float scale = scaleFromValue(val);
    
    if (scale > 0.97f && scale < 1.03f && m_scale1Lock) {
        scale = 1.0f;
        activeSlider->setValue(valueFromScale(1.0f));
    }
    
    if (m_scaleButtonType != 2) {
        m_changedValueX = scale;
        m_changedValueY = scale;
        
        if (m_delegate) {
            if (m_scaleButtonType == 0) {
                m_delegate->scaleXChanged(m_scaleLocked, scale);
            } else if (m_scaleButtonType == 1) {
                m_delegate->scaleYChanged(m_scaleLocked, scale);
            } else {
                m_delegate->scaleXYChanged(m_scaleLocked, scale, scale);
            }
        }
        
        if (m_scaleButtonType == 0) {
            if (m_valueY > scale) m_valueY = scale;
            updateLabelX(scale);
            updateLabelXY(m_valueY);
            m_sliderY->setValue(valueFromScale(m_valueY));
        } else if (m_scaleButtonType == 1) {
            m_valueY = m_valueX;
            if (m_valueY < scale) m_valueY = scale;
            updateLabelY(scale);
            updateLabelXY(m_valueY);
            m_sliderX->setValue(valueFromScale(m_valueY));
        }
    } else {
        float scaleY = scale;
        if (m_valueX == m_valueY) {
            if (scale == m_changedValueX) {
                float minScale = scale;
                if (scale > m_changedValueY) minScale = m_changedValueY;
                if (m_lowerBound <= minScale && (scale <= m_upperBound || m_changedValueY <= m_upperBound)) {
                    m_changedValueX = scale;
                    m_changedValueY = scaleY;
                    if (m_delegate) {
                        m_delegate->scaleXYChanged(m_scaleLocked, scale, scaleY);
                    }
                } else {
                    activeSlider->setValue(val);
                }
                return;
            }
        } else {
            if (m_valueX > m_valueY) {
                scaleY = static_cast<float>(static_cast<int>((m_valueY * (scale / m_valueX)) * 100.0f + 0.5f)) / 100.0f;
            } else {
                scale = static_cast<float>(static_cast<int>((m_valueX * (scale / m_valueY)) * 100.0f + 0.5f)) / 100.0f;
                scaleY = scale;
            }
        }
        
        if (m_changedValueX != scale || m_changedValueY != scaleY) {
            float maxScale = (scale > scaleY) ? scale : scaleY;
            if (m_upperBound < maxScale) {
                activeSlider->setValue(val);
                return;
            }
            float minScale = (scale < scaleY) ? scale : scaleY;
            if (m_lowerBound > minScale) {
                activeSlider->setValue(val);
                return;
            }
        }
        
        m_changedValueX = scale;
        m_changedValueY = scaleY;
        if (m_delegate) {
            m_delegate->scaleXYChanged(m_scaleLocked, scale, scaleY);
        }
    }
}

void GJScaleControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != touch->getID()) {
        return;
    }
    
    Slider* activeSlider = nullptr;
    if (m_scaleButtonType == 0) {
        activeSlider = m_sliderX;
        m_valueX = scaleFromValue(activeSlider->getValue());
    } else if (m_scaleButtonType == 1) {
        activeSlider = m_sliderY;
        m_valueY = scaleFromValue(activeSlider->getValue());
    } else {
        activeSlider = m_sliderXY;
    }
    
    activeSlider->ccTouchEnded(touch, event);
    finishTouch();
}

void GJScaleControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void GJScaleControl::finishTouch() {
    if (m_touchID != -1) {
        m_touchID = -1;
        if (m_delegate) {
            m_delegate->scaleChangeEnded();
        }
    }
}

void GJScaleControl::onToggleLockScale(cocos2d::CCObject* sender) {
    m_scaleLocked = !m_scaleLocked;
    
    auto image = m_scaleLockButton->getNormalImage();
    auto cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    const char* frame = m_scaleLocked ? "warpLockOnBtn_001.png" : "warpLockOffBtn_001.png";
    image->setDisplayFrame(cache->spriteFrameByName(frame));
}

float GJScaleControl::scaleFromValue(float value) {
    float scale = m_lowerBound + (m_upperBound - m_lowerBound) * value;
    return cocos2d::CCString::createWithFormat("%.02f", scale)->floatValue();
}

float GJScaleControl::skewFromValue(float value) {
    return m_lowerBound + (m_upperBound - m_lowerBound) * value;
}

float GJScaleControl::valueFromScale(float scale) {
    float val = (scale - m_lowerBound) / (m_upperBound - m_lowerBound);
    if (val > 1.0f) return 1.0f;
    if (val < 0.0f) return 0.0f;
    return val;
}

float GJScaleControl::valueFromSkew(float value) {
    float val = (value - m_lowerBound) / (m_upperBound - m_lowerBound);
    if (val > 1.0f) return 1.0f;
    if (val < 0.0f) return 0.0f;
    return val;
}

void GJScaleControl::sliderChanged(cocos2d::CCObject* sender) {
    SliderThumb::getValue(static_cast<SliderThumb*>(sender));
}

void GJScaleControl::updateLabelX(float value) {
    m_scaleXLabel->setString(cocos2d::CCString::createWithFormat("ScaleX %.02f", value)->getCString());
    m_scaleXLabel->limitLabelWidth(100.0f, 0.0f, 0.0f);
}

void GJScaleControl::updateLabelY(float value) {
    m_scaleYLabel->setString(cocos2d::CCString::createWithFormat("ScaleY: %.02f", value)->getCString());
    m_scaleYLabel->limitLabelWidth(100.0f, 0.0f, 0.0f);
}

void GJScaleControl::updateLabelXY(float value) {
    m_scaleLabel->setString(cocos2d::CCString::createWithFormat("Scale: %.02f", value)->getCString());
    m_scaleLabel->limitLabelWidth(100.0f, 0.0f, 0.0f);
}

GJScaleControl::~GJScaleControl() {
}