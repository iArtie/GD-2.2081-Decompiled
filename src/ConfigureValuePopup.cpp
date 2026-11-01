#include "ConfigureValuePopup.h"
#include "ConfigureValuePopupDelegate.h"
#include "CCMenuItemSpriteExtra.h"
#include "GameToolbox.h"

ConfigureValuePopup* ConfigureValuePopup::create(float value, float min, float max,
                                                   const std::string& title, const std::string& description,
                                                   int type, bool showSlider, bool showArrows) {
    ConfigureValuePopup* pRet = new ConfigureValuePopup();
    if (pRet && pRet->init(value, min, max, title, description, type, showSlider, showArrows)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool ConfigureValuePopup::init(float value, float min, float max,
                                const std::string& title, const std::string& description,
                                int type, bool showSlider, bool showArrows) {
    // FLAlertLayer::init with size (probably 150)
    if (!FLAlertLayer::init(150)) {
        return false;
    }

    m_value = value;
    m_minimum = min;
    m_maximum = max;
    m_type = type;
    m_enableDelegate = false;
    m_disableTextDelegate = false;
    m_delegate = nullptr;
    m_input = nullptr;
    m_slider = nullptr;
    m_buttonMenu = nullptr;

    // Create button menu
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    // Title label
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
    titleLabel->setScale(0.7f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(0, 90));

    // Description label (if provided)
    if (!description.empty()) {
        cocos2d::CCLabelBMFont* descLabel = cocos2d::CCLabelBMFont::create(description.c_str(), "bigFont.fnt");
        descLabel->setScale(0.5f);
        descLabel->setColor(cocos2d::ccColor3B(150, 150, 150));
        m_mainLayer->addChild(descLabel);
        descLabel->setPosition(cocos2d::CCPoint(0, 60));
    }

    // Text input
    m_input = CCTextInputNode::create(100, 30, " ", "bigFont.fnt");
    m_input->setDelegate(this);
    m_input->setTag(1);
    m_input->setMaxLabelWidth(80);
    m_mainLayer->addChild(m_input);
    m_input->setPosition(cocos2d::CCPoint(0, 30));
    updateTextInputLabel();

    // Slider (if requested)
    if (showSlider) {
        m_slider = Slider::create(this, menu_selector(ConfigureValuePopup::sliderChanged));
        m_slider->setBarVisibility(false);
        m_mainLayer->addChild(m_slider);
        m_slider->setPosition(cocos2d::CCPoint(0, -10));
        
        // Set initial slider position
        float sliderValue = (m_value - m_minimum) / (m_maximum - m_minimum);
        Slider::setValue(m_slider, cocos2d::clampf(sliderValue, 0.0f, 1.0f));
    }

    // Arrow buttons (if requested)
    if (showArrows) {
        // Left arrow
        cocos2d::CCSprite* leftSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        leftSprite->setFlipX(true);
        CCMenuItemSpriteExtra* leftBtn = CCMenuItemSpriteExtra::create(
            leftSprite, nullptr, this, menu_selector(ConfigureValuePopup::onArrow));
        leftBtn->setTag(0);
        leftBtn->setSizeMult(0.8f);
        m_buttonMenu->addChild(leftBtn);
        leftBtn->setPosition(cocos2d::CCPoint(-85, 30));

        // Right arrow
        cocos2d::CCSprite* rightSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        CCMenuItemSpriteExtra* rightBtn = CCMenuItemSpriteExtra::create(
            rightSprite, nullptr, this, menu_selector(ConfigureValuePopup::onArrow));
        rightBtn->setTag(1);
        rightBtn->setSizeMult(0.8f);
        m_buttonMenu->addChild(rightBtn);
        rightBtn->setPosition(cocos2d::CCPoint(85, 30));
    }

    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, this, menu_selector(ConfigureValuePopup::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeBtn);
    closeBtn->setPosition(cocos2d::CCPoint(120, 90));

    return true;
}

void ConfigureValuePopup::addInfoAlertButton(const std::string& title, const std::string& description) {
    InfoAlertButton* infoBtn = InfoAlertButton::create(title, description, 0.8f);
    infoBtn->setTextScale(0.9f);
    m_buttonMenu->addChild(infoBtn);
    
    cocos2d::CCPoint btnPos = cocos2d::CCPoint(130, 100);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(btnPos));
}

void ConfigureValuePopup::keyBackClicked() {
    onClose(nullptr);
}

void ConfigureValuePopup::onArrow(cocos2d::CCObject* sender) {
    m_enableDelegate = true;
    int tag = sender->getTag();
    
    float newValue = m_value;
    if (tag == 1) {
        newValue = m_value + 1.0f;
    } else {
        newValue = m_value - 1.0f;
    }
    
    // Clamp to range
    if (newValue < m_minimum) newValue = m_minimum;
    if (newValue > m_maximum) newValue = m_maximum;
    
    m_value = newValue;
    updateTextInputLabel();
    
    // Update slider if it exists
    if (m_slider) {
        float sliderValue = (m_value - m_minimum) / (m_maximum - m_minimum);
        Slider::setValue(m_slider, cocos2d::clampf(sliderValue, 0.0f, 1.0f));
    }
}

void ConfigureValuePopup::onClose(cocos2d::CCObject* sender) {
    // Disable text input
    if (m_input) {
        m_input->setDelegate(nullptr);
        m_input->onClickTrackNode(false);
    }
    
    // Notify delegate
    if (m_enableDelegate && m_delegate) {
        m_delegate->valuePopupClosed(this, m_value);
    }
    
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ConfigureValuePopup::sliderChanged(cocos2d::CCObject* sender) {
    m_enableDelegate = true;
    float sliderValue = ((SliderThumb*)sender)->getValue();
    m_value = m_minimum + sliderValue * (m_maximum - m_minimum);
    updateTextInputLabel();
}

void ConfigureValuePopup::textChanged(CCTextInputNode* node) {
    if (m_disableTextDelegate) return;
    
    m_enableDelegate = true;
    const char* text = node->getString();
    
    if (m_type == 1) { // Integer type
        m_value = (float)atoi(text);
    } else { // Float type
        float val = atof(text);
        // Round to 2 decimal places
        m_value = val;
    }
    
    // Clamp to range
    if (m_value < m_minimum) m_value = m_minimum;
    if (m_value > m_maximum) m_value = m_maximum;
    
    // Update slider
    if (m_slider) {
        float sliderValue = (m_value - m_minimum) / (m_maximum - m_minimum);
        if (sliderValue < 0.0f) sliderValue = 0.0f;
        if (sliderValue > 1.0f) sliderValue = 1.0f;
        Slider::setValue(m_slider, sliderValue);
    }
}

void ConfigureValuePopup::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void ConfigureValuePopup::updateTextInputLabel() {
    if (!m_input) return;
    
    m_disableTextDelegate = true;
    
    std::string text;
    if (m_type == 1) { // Integer
        text = cocos2d::CCString::createWithFormat("%i", (int)m_value)->getCString();
    } else { // Float
        text = cocos2d::CCString::createWithFormat("%.02f", m_value)->getCString();
    }
    
    m_input->setString(text);
    m_disableTextDelegate = false;
}

void ConfigureValuePopup::setValue(float value) {
    m_value = value;
    updateTextInputLabel();
    
    if (m_slider) {
        float sliderValue = (m_value - m_minimum) / (m_maximum - m_minimum);
        Slider::setValue(m_slider, cocos2d::clampf(sliderValue, 0.0f, 1.0f));
    }
}

ConfigureValuePopup::~ConfigureValuePopup() {
    // Clean up
}