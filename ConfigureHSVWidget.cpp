#include "ConfigureHSVWidget.h"
#include "HSVWidgetDelegate.h"
#include "GameToolbox.h"
#include "extension/CCScale9Sprite.h"

ConfigureHSVWidget* ConfigureHSVWidget::create(cocos2d::ccHSVValue hsv, bool noBackground, bool addInputs) {
    ConfigureHSVWidget* pRet = new ConfigureHSVWidget();
    if (pRet && pRet->init(hsv, noBackground, addInputs)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool ConfigureHSVWidget::init(cocos2d::ccHSVValue hsv, bool noBackground, bool addInputs) {
    if (!cocos2d::CCNode::init()) {
        return false;
    }

    m_updating = true;
    m_inputs = cocos2d::CCDictionary::create();
    m_inputs->retain();
    m_hsv = hsv;
    m_addInputs = addInputs;
    m_delegate = nullptr;
    m_mixed = false;

    // Background
    if (!noBackground) {
        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02_001.png", cocos2d::CCRect(0, 0, 80, 80));
        bg->setOpacity(75);
        bg->setContentSize(cocos2d::CCSize(270, 160));
        addChild(bg, -1);
    }

    // Hue Label
    m_hueLabel = cocos2d::CCLabelBMFont::create("Hue: 0", "bigFont.fnt");
    addChild(m_hueLabel);
    m_hueLabel->setScale(0.5f);
    m_hueLabel->setPosition(cocos2d::CCPoint(0, 66));

    if (m_addInputs) {
        cocos2d::CCPoint pos = m_hueLabel->getPosition();
        createTextInputNode(pos + cocos2d::CCPoint(-10, 0));
    }

    // Hue Slider
    m_hueSlider = Slider::create(this, menu_selector(ConfigureHSVWidget::sliderChanged));
    m_hueSlider->setBarVisibility(false);
    addChild(m_hueSlider);
    cocos2d::CCPoint huePos = m_hueLabel->getPosition();
    m_hueSlider->setPosition(cocos2d::CCPoint(huePos.x, huePos.y - 25));
    SliderThumb* hueThumb = m_hueSlider->getThumb();
    hueThumb->setTag(1);

    // Saturation Label
    m_saturationLabel = cocos2d::CCLabelBMFont::create("Saturation: 0.00", "bigFont.fnt");
    addChild(m_saturationLabel);
    m_saturationLabel->setScale(0.5f);
    cocos2d::CCPoint satPos = m_hueLabel->getPosition();
    m_saturationLabel->setPosition(cocos2d::CCPoint(0, satPos.y - 50));

    if (m_addInputs) {
        cocos2d::CCPoint pos = m_saturationLabel->getPosition();
        createTextInputNode(pos + cocos2d::CCPoint(22, 0));
    }

    // Saturation Slider
    m_saturationSlider = Slider::create(this, menu_selector(ConfigureHSVWidget::sliderChanged));
    m_saturationSlider->setBarVisibility(false);
    addChild(m_saturationSlider);
    cocos2d::CCPoint satSliderPos = m_saturationLabel->getPosition();
    m_saturationSlider->setPosition(cocos2d::CCPoint(satSliderPos.x, satSliderPos.y - 25));
    SliderThumb* satThumb = m_saturationSlider->getThumb();
    satThumb->setTag(2);

    // Brightness Label
    m_brightnessLabel = cocos2d::CCLabelBMFont::create("Brightness: 0.00", "bigFont.fnt");
    addChild(m_brightnessLabel);
    m_brightnessLabel->setScale(0.5f);
    cocos2d::CCPoint briPos = m_saturationLabel->getPosition();
    m_brightnessLabel->setPosition(cocos2d::CCPoint(0, briPos.y - 50));

    if (m_addInputs) {
        cocos2d::CCPoint pos = m_brightnessLabel->getPosition();
        createTextInputNode(pos + cocos2d::CCPoint(22, 0));
    }

    // Brightness Slider
    m_brightnessSlider = Slider::create(this, menu_selector(ConfigureHSVWidget::sliderChanged));
    m_brightnessSlider->setBarVisibility(false);
    addChild(m_brightnessSlider);
    cocos2d::CCPoint briSliderPos = m_brightnessLabel->getPosition();
    m_brightnessSlider->setPosition(cocos2d::CCPoint(briSliderPos.x, briSliderPos.y - 25));
    SliderThumb* briThumb = m_brightnessSlider->getThumb();
    briThumb->setTag(3);

    // Menu for toggle buttons and reset button
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    addChild(menu);

    // Set touch priority
    if (cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->isUsingForcePrio()) {
        menu->setTouchPriority(cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->getPriority() - 1);
    }

    // Saturation absolute toggle
    cocos2d::CCPoint togglePos = m_saturationLabel->getPosition() + cocos2d::CCPoint(-95, 0);
    GameToolbox::createToggleButton(
        " ",
        menu_selector(ConfigureHSVWidget::onToggleSConst),
        0,
        m_hsv.absoluteSaturation,
        menu,
        togglePos,
        this,
        this,
        0.6f,
        0.4f,
        150.0f,
        cocos2d::CCPoint(0, 1),
        "bigFont.fnt",
        true,
        false,
        false
    );

    // Brightness absolute toggle
    togglePos = m_brightnessLabel->getPosition() + cocos2d::CCPoint(-95, 0);
    GameToolbox::createToggleButton(
        " ",
        menu_selector(ConfigureHSVWidget::onToggleVConst),
        0,
        m_hsv.absoluteBrightness,
        menu,
        togglePos,
        this,
        this,
        0.6f,
        0.4f,
        150.0f,
        cocos2d::CCPoint(0, 1),
        "bigFont.fnt",
        true,
        false,
        false
    );

    // Reset button
    cocos2d::CCSprite* resetSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
    CCMenuItemSpriteExtra* resetBtn = CCMenuItemSpriteExtra::create(
        resetSprite, nullptr, this, menu_selector(ConfigureHSVWidget::onResetHSV));
    menu->addChild(resetBtn);
    cocos2d::CCPoint resetPos = menu->convertToNodeSpace(cocos2d::CCPoint(119, 64));
    resetBtn->setPosition(resetPos);

    // Initialize values
    if (m_hsv.h == -0.001f) {
        onResetHSV(nullptr);
        if (!m_addInputs) {
            m_hueLabel->setString("Hue: Mixed");
            m_saturationLabel->setString("Saturation: Mixed");
            m_brightnessLabel->setString("Brightness: Mixed");
        }
        m_mixed = true;
    } else {
        updateLabels();
    }

    updateSliders();
    m_updating = false;

    return true;
}

void ConfigureHSVWidget::onClose() {
    if (m_inputs) {
        cocos2d::CCDictElement* element = m_inputs->m_pElements;
        while (element) {
            CCTextInputNode* node = (CCTextInputNode*)element->m_pObject;
            if (node) {
                node->setDelegate(nullptr);
                node->setEnabled(false);
            }
            element = (cocos2d::CCDictElement*)element->hh.next;
        }
        m_inputs->removeAllObjects();
    }
}

void ConfigureHSVWidget::onResetHSV(cocos2d::CCObject* sender) {
    m_hsv.h = 0.0f;
    m_hsv.s = 0.0f;
    m_hsv.v = 0.0f;
    m_hsv.absoluteSaturation = false;
    m_hsv.absoluteBrightness = false;

    Slider::setValue(m_hueSlider, 0.5f);
    Slider::setValue(m_saturationSlider, 0.5f);
    Slider::setValue(m_brightnessSlider, 0.5f);

    updateLabels();
    if (m_delegate) {
        m_delegate->hsvChanged(this);
    }
}

void ConfigureHSVWidget::onToggleSConst(cocos2d::CCObject* sender) {
    m_hsv.absoluteSaturation = !m_hsv.absoluteSaturation;
    Slider::setValue(m_saturationSlider, 0.5f);
    m_hsv.s = m_hsv.absoluteSaturation ? 1.0f : 0.0f;
    updateLabels();
    if (m_delegate) {
        m_delegate->hsvChanged(this);
    }
}

void ConfigureHSVWidget::onToggleVConst(cocos2d::CCObject* sender) {
    m_hsv.absoluteBrightness = !m_hsv.absoluteBrightness;
    Slider::setValue(m_brightnessSlider, 0.5f);
    m_hsv.v = m_hsv.absoluteBrightness ? 1.0f : 0.0f;
    updateLabels();
    if (m_delegate) {
        m_delegate->hsvChanged(this);
    }
}

void ConfigureHSVWidget::sliderChanged(cocos2d::CCObject* sender) {
    float value = ((SliderThumb*)sender)->getValue();
    int tag = sender->getTag();

    switch (tag) {
        case 1: // Hue
            m_hsv.h = (value * 360.0f) - 180.0f;
            break;
        case 2: // Saturation
            if (m_hsv.absoluteSaturation) {
                m_hsv.s = (value * 2.0f) - 1.0f;
            } else {
                m_hsv.s = value * 2.0f;
            }
            break;
        case 3: // Brightness
            if (m_hsv.absoluteBrightness) {
                m_hsv.v = (value * 2.0f) - 1.0f;
            } else {
                m_hsv.v = value * 2.0f;
            }
            break;
    }

    updateLabels();
    if (m_delegate) {
        m_delegate->hsvChanged(this);
    }
}

void ConfigureHSVWidget::textChanged(CCTextInputNode* node) {
    if (m_updating) return;

    int tag = node->getTag();
    const char* str = node->getString();

    switch (tag) {
        case 1: { // Hue
            int val = atoi(str);
            if (val < -360) val = -360;
            if (val > 360) val = 360;
            m_hsv.h = (float)val;
            break;
        }
        case 2: { // Saturation
            float val = atof(str);
            m_hsv.s = val;
            break;
        }
        case 3: { // Brightness
            float val = atof(str);
            m_hsv.v = val;
            break;
        }
    }

    updateSliders();
}

void ConfigureHSVWidget::textInputClosed(CCTextInputNode* node) {
    textChanged(node);
}

void ConfigureHSVWidget::textInputOpened(CCTextInputNode* node) {
    // Empty implementation
}

void ConfigureHSVWidget::updateLabels() {
    if (m_addInputs) {
        m_updating = true;

        // Update text input fields
        CCTextInputNode* hueInput = (CCTextInputNode*)m_inputs->objectForKey("1");
        if (hueInput) {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%.0f", m_hsv.h);
            hueInput->setString(str->getCString());
        }

        CCTextInputNode* satInput = (CCTextInputNode*)m_inputs->objectForKey("2");
        if (satInput) {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%.2f", m_hsv.s);
            satInput->setString(str->getCString());
        }

        CCTextInputNode* briInput = (CCTextInputNode*)m_inputs->objectForKey("3");
        if (briInput) {
            cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%.2f", m_hsv.v);
            briInput->setString(str->getCString());
        }

        m_updating = false;
    } else {
        // Update labels with formatted strings
        std::string hueStr = "Hue: ";
        if (m_hsv.h > 0) hueStr += "+";
        hueStr += cocos2d::CCString::createWithFormat("%i", (int)m_hsv.h)->getCString();
        m_hueLabel->setString(hueStr.c_str());

        std::string satStr = "Saturation: ";
        if (m_hsv.absoluteSaturation) {
            if (m_hsv.s > 0) satStr += "+";
        } else {
            satStr += "x";
        }
        satStr += cocos2d::CCString::createWithFormat("%.02f", m_hsv.s)->getCString();
        m_saturationLabel->setString(satStr.c_str());

        std::string briStr = "Brightness: ";
        if (m_hsv.absoluteBrightness) {
            if (m_hsv.v > 0) briStr += "+";
        } else {
            briStr += "x";
        }
        briStr += cocos2d::CCString::createWithFormat("%.02f", m_hsv.v)->getCString();
        m_brightnessLabel->setString(briStr.c_str());
    }

    m_mixed = false;
}

void ConfigureHSVWidget::updateSliders() {
    // Hue: map from [-180, 180] to [0, 1]
    float hueVal = cocos2d::clampf((m_hsv.h + 180.0f) / 360.0f, 0.0f, 1.0f);
    Slider::setValue(m_hueSlider, hueVal);

    // Saturation
    if (m_hsv.absoluteSaturation) {
        float satVal = cocos2d::clampf((m_hsv.s + 1.0f) * 0.5f, 0.0f, 1.0f);
        Slider::setValue(m_saturationSlider, satVal);
    } else {
        float satVal = cocos2d::clampf(m_hsv.s * 0.5f, 0.0f, 1.0f);
        Slider::setValue(m_saturationSlider, satVal);
    }

    // Brightness
    if (m_hsv.absoluteBrightness) {
        float briVal = cocos2d::clampf((m_hsv.v + 1.0f) * 0.5f, 0.0f, 1.0f);
        Slider::setValue(m_brightnessSlider, briVal);
    } else {
        float briVal = cocos2d::clampf(m_hsv.v * 0.5f, 0.0f, 1.0f);
        Slider::setValue(m_brightnessSlider, briVal);
    }
}

ConfigureHSVWidget::~ConfigureHSVWidget() {
    CC_SAFE_RELEASE(m_inputs);
}