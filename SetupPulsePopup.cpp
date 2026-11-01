#include "SetupPulsePopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCMenuItemToggler.h"
#include "CCTextInputNode.h"
#include "CCLabelBMFont.h"
#include "ConfigureHSVWidget.h"
#include "GJSpecialColorSelect.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"
#include "ColorSelectPopup.h"
#include "CCControlColourPicker.h"

SetupPulsePopup* SetupPulsePopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupPulsePopup* instance = new SetupPulsePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupPulsePopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    m_groupMainOnly = true;
    m_groupSecondaryOnly = true;
    m_pulseExclusive = true;
    m_staticHSV = true;
    m_currentColor = cocos2d::ccColor3B(255, 255, 255);
    m_hsv = cocos2d::ccHSVValue(1.0f, 1.0f, 1.0f, 1.0f, false, false);
    m_targetGroupID = -1;
    m_copyColorID = -1;
    m_fadeInTime = -1.0f;
    m_holdTime = -1.0f;
    m_fadeOutTime = -1.0f;
    m_pulseMode = -1;
    m_pulseTargetType = -1;

    int fadeInConflict = 0, holdConflict = 0, fadeOutConflict = 0;
    int modeConflict = 0, targetTypeConflict = 0;
    int colorConflict = 0, hsvConflict = 0;
    int firstColor = 1, firstHsv = 1;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (!obj->m_pulseMainOnly)
            m_groupMainOnly = false;
        if (!obj->m_pulseDetailOnly)
            m_groupSecondaryOnly = false;
        if (!obj->m_pulseExclusive)
            m_pulseExclusive = false;
        if (!obj->m_legacyHSV)
            m_staticHSV = false;

        if (m_fadeInTime == -1.0f) m_fadeInTime = obj->m_fadeInDuration;
        else if (m_fadeInTime != obj->m_fadeInDuration) m_fadeInTime = -2.0f;

        if (m_holdTime == -1.0f) m_holdTime = obj->m_holdDuration;
        else if (m_holdTime != obj->m_holdDuration) m_holdTime = -2.0f;

        if (m_fadeOutTime == -1.0f) m_fadeOutTime = obj->m_fadeOutDuration;
        else if (m_fadeOutTime != obj->m_fadeOutDuration) m_fadeOutTime = -2.0f;

        if (m_pulseMode == -1) m_pulseMode = obj->m_pulseMode;
        else if (m_pulseMode != obj->m_pulseMode) m_pulseMode = -2;

        if (m_pulseTargetType == -1) m_pulseTargetType = obj->m_pulseTargetType;
        else if (m_pulseTargetType != obj->m_pulseTargetType) m_pulseTargetType = -2;

        if (!colorConflict)
        {
            if (firstColor) { m_currentColor = obj->m_triggerTargetColor; firstColor = 0; }
            else if (m_currentColor.r != obj->m_triggerTargetColor.r ||
                     m_currentColor.g != obj->m_triggerTargetColor.g ||
                     m_currentColor.b != obj->m_triggerTargetColor.b)
            {
                colorConflict = 1;
                m_currentColor = cocos2d::ccColor3B(255, 255, 255);
            }
        }

        if (!hsvConflict)
        {
            if (firstHsv) { m_hsv = obj->m_hsvValue; firstHsv = 0; }
            else if (m_hsv.h != obj->m_hsvValue.h || m_hsv.s != obj->m_hsvValue.s ||
                     m_hsv.v != obj->m_hsvValue.v ||
                     m_hsv.absoluteSaturation != obj->m_hsvValue.absoluteSaturation ||
                     m_hsv.absoluteBrightness != obj->m_hsvValue.absoluteBrightness)
            {
                hsvConflict = 1;
                m_hsv = cocos2d::ccHSVValue(0.0f, 0.0f, 0.0f, 0.0f, false, false);
            }
        }

        if (m_targetGroupID == -1) m_targetGroupID = obj->m_targetGroupID;
        else if (m_targetGroupID != obj->m_targetGroupID) m_targetGroupID = 0;

        if (m_copyColorID == -1) m_copyColorID = obj->m_copyColorID;
        else if (m_copyColorID != obj->m_copyColorID) m_copyColorID = 0;
    }

    if (m_fadeInTime < 0.0f) m_fadeInTime = -0.1f;
    if (m_holdTime < 0.0f) m_holdTime = -0.1f;
    if (m_fadeOutTime < 0.0f) m_fadeOutTime = -0.1f;
    if (m_pulseMode < 0) m_pulseMode = -1;
    if (m_pulseTargetType < 0) m_pulseTargetType = -1;
}

cocos2d::ccColor3B SetupPulsePopup::getColorValue()
{
    return cocos2d::ccColor3B(m_colorPicker->m_rgb.r, m_colorPicker->m_rgb.g, m_colorPicker->m_rgb.b);
}

void SetupPulsePopup::colorSelectClosed(GJSpecialColorSelect* select, int id)
{
    if (id <= 999) return;

    if (select->getTag() == 7)
    {
        m_disableTextDelegate = true;
        m_copyColorID = id;
        m_colorIDInput->setString("0");
        m_colorIDInput->updateLabel(GJSpecialColorSelect::textForColorIdx(id));
        updateCopyColor();
        m_disableTextDelegate = false;
    }
    else if (select->getTag() == 6)
    {
        m_disableTextDelegate = true;
        m_targetGroupID = id;
        m_idInput->setString("0");
        m_idInput->updateLabel(GJSpecialColorSelect::textForColorIdx(id));
        updateTargetID();
        updateEditorLabel();
        m_disableTextDelegate = false;
    }
}

void SetupPulsePopup::colorValueChanged(cocos2d::ccColor3B color)
{
    if (!m_currentColorSprite) return;

    m_currentColor = getColorValue();
    m_currentColorSprite->setColor(m_currentColor);
    m_originalColorSprite->setColor(m_originalColor);
    updateColorValue();
    updateColorLabels();
}

bool SetupPulsePopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 450.0f, 310.0f, 1))
        return false;

    m_modesInitializing = true;
    m_colorObjects = cocos2d::CCArray::create();
    m_colorObjects->retain();
    m_hsvObjects = cocos2d::CCArray::create();
    m_hsvObjects->retain();
    m_colorInitializing = true;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Pulse Trigger", "bigFont.fnt");
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.8f);
    titleLabel->setPosition(winSize.width * 0.5f - 65.0f, winSize.height * 0.5f + 138.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupPulsePopup::onClose));
    m_buttonMenu->addChild(okItem);

    cocos2d::CCPoint fadeBase(winSize.width * 0.5f - 29.0f, winSize.height * 0.5f - 44.0f);

    auto createTimingControls = [&](const char* label, cocos2d::CCPoint pos, CCTextInputNode** input,
        Slider** slider, int sliderTag, float time, int textTag)
    {
        cocos2d::CCLabelBMFont* lbl = cocos2d::CCLabelBMFont::create(label, "bigFont.fnt");
        lbl->setScale(0.5f);
        lbl->setPosition(pos + cocos2d::CCPoint(-10.0f, 12.0f));
        lbl->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        m_mainLayer->addChild(lbl, 10);

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
        bg->setColor(cocos2d::ccColor3B(0, 0, 0));
        bg->setOpacity(100);
        bg->setContentSize(cocos2d::CCSize(60.0f, 20.0f));
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(pos + cocos2d::CCPoint(40.0f, 0.0f));

        *input = CCTextInputNode::create(50.0f, 20.0f, time == -0.1f ? "Mixed" : "Num", "Thonburi");
        m_mainLayer->addChild(*input, 20);
        (*input)->setPosition(bg->getPosition());
        (*input)->setMaxLabelScale(0.6f);
        (*input)->setMaxLabelWidth(50.0f);
        (*input)->m_textField->setVisible(false);
        (*input)->m_maxLabelLength = 4;
        (*input)->setAllowedChars("0123456789.");
        (*input)->setLabelPlaceholderColor(cocos2d::ccColor3B(120, 170, 240));
        (*input)->setLabelPlaceholderScale(0.5f);
        (*input)->m_delegate = this;
        (*input)->setTag(textTag);
        (*input)->m_numberInput = true;

        *slider = Slider::create(this, menu_selector(SetupPulsePopup::sliderChanged), 0, 0.8f);
        m_mainLayer->addChild(*slider);
        (*slider)->setBarVisibility(true);
        (*slider)->setPosition(pos + cocos2d::CCPoint(-96.0f, -1.5f));
        (*slider)->getThumb()->setTag(sliderTag);

        float val = 0.0f;
        if (time != -0.1f)
        {
            val = time / 10.0f;
            if (val >= 1.0f) val = 1.0f;
            else if (val <= 0.0f) val = 0.0f;
        }
        (*slider)->setValue(val);
    };

    createTimingControls("Fade In", fadeBase, &m_fadeInInput, &m_fadeInSlider, 1, m_fadeInTime, 8);
    updateFadeInLabel(false);

    cocos2d::CCPoint holdPos = fadeBase + cocos2d::CCPoint(0.0f, -27.0f);
    createTimingControls("Hold", holdPos, &m_holdInput, &m_holdSlider, 2, m_holdTime, 9);
    updateHoldLabel(false);

    cocos2d::CCPoint fadeOutPos = holdPos + cocos2d::CCPoint(0.0f, -27.0f);
    createTimingControls("Fade Out", fadeOutPos, &m_fadeOutInput, &m_fadeOutSlider, 3, m_fadeOutTime, 10);
    updateFadeOutLabel(false);

    cocos2d::CCPoint rightPanel(winSize.width * 0.5f + 145.0f, winSize.height * 0.5f - 60.0f);

    cocos2d::CCLabelBMFont* targetTypeLabel = cocos2d::CCLabelBMFont::create("Target Type", "bigFont.fnt");
    m_mainLayer->addChild(targetTypeLabel);
    targetTypeLabel->setPosition(rightPanel + cocos2d::CCPoint(0.0f, 25.0f));
    targetTypeLabel->setScale(0.5f);

    ButtonSprite* channelBtn = ButtonSprite::create("Channel", 0, 0, 0.45f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
    m_channelButton = CCMenuItemSpriteExtra::create(
        channelBtn, nullptr, menu_selector(SetupPulsePopup::onSelectTargetMode));
    m_buttonMenu->addChild(m_channelButton);
    m_channelButton->setTag(0);

    ButtonSprite* groupBtn = ButtonSprite::create("Group", 0, 0, 0.45f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
    m_groupButton = CCMenuItemSpriteExtra::create(
        groupBtn, nullptr, menu_selector(SetupPulsePopup::onSelectTargetMode));
    m_buttonMenu->addChild(m_groupButton);
    m_groupButton->setTag(1);

    m_channelButton->setPosition(rightPanel + cocos2d::CCPoint(-35.0f, 0.0f));
    m_groupButton->setPosition(rightPanel + cocos2d::CCPoint(35.0f, 0.0f));

    m_idLabel = cocos2d::CCLabelBMFont::create("ID", "bigFont.fnt");
    m_mainLayer->addChild(m_idLabel);
    m_idLabel->setScale(0.5f);
    m_idLabel->setPosition(rightPanel + cocos2d::CCPoint(0.0f, -35.0f));

    cocos2d::CCPoint idInputPos = rightPanel + cocos2d::CCPoint(0.0f, -65.0f);

    cocos2d::extension::CCScale9Sprite* idBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    idBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    idBg->setOpacity(100);
    idBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(idBg, -1);
    idBg->setPosition(idInputPos);

    m_idInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_idInput, 20);
    m_idInput->setPosition(idInputPos);
    m_idInput->setMaxLabelScale(0.6f);
    m_idInput->setMaxLabelWidth(200.0f);
    m_idInput->m_textField->setVisible(false);
    m_idInput->m_maxLabelLength = 6;
    m_idInput->setAllowedChars("0123456789");
    m_idInput->setLabelPlaceholderColor(cocos2d::ccColor3B(120, 170, 240));
    m_idInput->setLabelPlaceholderScale(0.5f);
    m_idInput->m_delegate = this;
    m_idInput->m_numberInput = true;
    m_idInput->setTag(4);

    cocos2d::CCPoint idLocalPos = m_buttonMenu->convertToNodeSpace(idInputPos);

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupPulsePopup::onUpdateCustomColor));
    m_buttonMenu->addChild(leftItem);
    leftItem->setTag(1);
    leftItem->setPosition(idLocalPos + cocos2d::CCPoint(-50.0f, 0.0f));
    leftItem->setSizeMult(2.0f);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow->setScale(1.1f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupPulsePopup::onUpdateCustomColor));
    m_buttonMenu->addChild(rightItem);
    rightItem->setTag(2);
    rightItem->setPosition(idLocalPos + cocos2d::CCPoint(50.0f, 0.0f));
    rightItem->setSizeMult(2.0f);

    cocos2d::CCSprite* plusIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    m_specialColorButton = CCMenuItemSpriteExtra::create(
        plusIcon, nullptr, menu_selector(SetupPulsePopup::onSelectSpecialTargetID));
    m_buttonMenu->addChild(m_specialColorButton);
    m_specialColorButton->setPosition(idLocalPos + cocos2d::CCPoint(50.0f, 30.0f));
    m_specialColorButton->setVisible(false);

    cocos2d::CCPoint modeBase(winSize.width * 0.5f - 65.0f, winSize.height * 0.5f - 60.0f);

    cocos2d::CCLabelBMFont* modeLabel = cocos2d::CCLabelBMFont::create("Pulse Mode", "bigFont.fnt");
    m_mainLayer->addChild(modeLabel);
    modeLabel->setPosition(rightPanel + cocos2d::CCPoint(0.0f, 75.0f));
    modeLabel->setScale(0.5f);

    ButtonSprite* colorBtn = ButtonSprite::create("Color", 0, 0, 0.45f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
    m_colorButton = CCMenuItemSpriteExtra::create(
        colorBtn, nullptr, menu_selector(SetupPulsePopup::onSelectPulseMode));
    m_buttonMenu->addChild(m_colorButton);
    m_colorButton->setTag(0);

    ButtonSprite* hsvBtn = ButtonSprite::create("HSV", 0, 0, 0.45f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
    m_hsvButton = CCMenuItemSpriteExtra::create(
        hsvBtn, nullptr, menu_selector(SetupPulsePopup::onSelectPulseMode));
    m_buttonMenu->addChild(m_hsvButton);
    m_hsvButton->setTag(1);

    m_colorButton->setPosition(rightPanel + cocos2d::CCPoint(-35.0f, 50.0f));
    m_hsvButton->setPosition(rightPanel + cocos2d::CCPoint(35.0f, 50.0f));

    cocos2d::CCPoint toggleBase(winSize.width * 0.5f - 200.0f, winSize.height * 0.5f - 130.0f);

    m_touchToggle = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Touch\nTriggered", menu_selector(SetupTriggerPopup::onTouchTriggered), 0, m_touchTriggered,
        m_buttonMenu, toggleBase, this, m_mainLayer, 0.7f, 0.3f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    m_spawnToggle = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Spawn\nTriggered", menu_selector(SetupTriggerPopup::onSpawnedByTrigger), 0, m_spawnTriggered,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(90.0f, 0.0f), this, m_mainLayer, 0.7f, 0.3f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    GameToolbox::createToggleButton(
        "Multi\nTrigger", menu_selector(SetupTriggerPopup::onMultiTrigger), 0, m_multiTriggered,
        m_buttonMenu, cocos2d::CCPoint(winSize.width * 0.5f + 60.0f, winSize.height * 0.5f - 82.0f),
        this, m_mainLayer, 0.6f, 0.25f, 0.0f, cocos2d::CCPoint(2.0f, -39.0f),
        "bigFont.fnt", true, 0, m_multiTriggerContainer);
    updateMultiTriggerBtn();

    GameToolbox::createToggleButton(
        "Exclusive", menu_selector(SetupPulsePopup::onExclusive), 0, m_pulseExclusive,
        m_buttonMenu, cocos2d::CCPoint(winSize.width * 0.5f + 60.0f, winSize.height * 0.5f - 130.0f),
        this, m_mainLayer, 0.7f, 0.3f, 0.0f, cocos2d::CCPoint(0.0f, -32.0f),
        "bigFont.fnt", true, 0, false);

    cocos2d::CCPoint groupFilterBase(winSize.width * 0.5f + 95.0f, winSize.height * 0.5f + 130.0f);

    m_mainOnlyToggler = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Main\nOnly", menu_selector(SetupPulsePopup::onGroupMainOnly), 0, m_groupMainOnly,
        m_buttonMenu, groupFilterBase, this, m_mainLayer, 0.7f, 0.3f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 1100, false);
    m_mainOnlyLabel = (cocos2d::CCLabelBMFont*)m_mainLayer->getChildByTag(1100);

    m_secondaryOnlyToggler = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Detail\nOnly", menu_selector(SetupPulsePopup::onGroupSecondaryOnly), 0, m_groupSecondaryOnly,
        m_buttonMenu, groupFilterBase + cocos2d::CCPoint(0.0f, -30.0f), this, m_mainLayer, 0.7f, 0.3f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 1200, false);
    m_secondaryOnlyLabel = (cocos2d::CCLabelBMFont*)m_mainLayer->getChildByTag(1200);

    m_staticHSVToggler = (CCMenuItemToggler*)GameToolbox::createToggleButton(
        "Static\nHSV", menu_selector(SetupPulsePopup::onHSVLegacyMode), 0, !m_staticHSV,
        m_buttonMenu, groupFilterBase + cocos2d::CCPoint(68.0f, 0.0f), this, m_mainLayer, 0.7f, 0.3f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 1300, false);
    m_staticHSVLabel = (cocos2d::CCLabelBMFont*)m_mainLayer->getChildByTag(1300);
    m_hsvObjects->addObject(m_staticHSVToggler);
    if (m_staticHSVLabel)
        m_hsvObjects->addObject(m_staticHSVLabel);

    cocos2d::extension::CCScale9Sprite* colorPanel = cocos2d::extension::CCScale9Sprite::create(
        "square02_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    colorPanel->setOpacity(75);
    colorPanel->setContentSize(cocos2d::CCSize(260.0f, 145.0f));
    m_mainLayer->addChild(colorPanel, -1);
    colorPanel->setPosition(winSize.width * 0.5f - 65.0f, winSize.height * 0.5f + 52.0f);
    m_colorObjects->addObject(colorPanel);

    m_colorPicker = cocos2d::extension::CCControlColourPicker::create();
    m_colorPicker->m_delegate = this;
    m_mainLayer->addChild(m_colorPicker, 2);
    m_colorPicker->setPosition(winSize.width * 0.5f - 25.0f, winSize.height * 0.5f + 35.0f);
    m_colorPicker->setScale(0.8f);
    m_colorObjects->addObject(m_colorPicker);

    ButtonSprite* copyBtn = ButtonSprite::create("Copy", 0, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0f);
    ButtonSprite* pasteBtn = ButtonSprite::create("Paste", 0, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0f);
    CCMenuItemSpriteExtra* copyItem = CCMenuItemSpriteExtra::create(
        copyBtn, nullptr, menu_selector(SetupPulsePopup::onCopy));
    CCMenuItemSpriteExtra* pasteItem = CCMenuItemSpriteExtra::create(
        pasteBtn, nullptr, menu_selector(SetupPulsePopup::onPaste));
    m_buttonMenu->addChild(copyItem);
    m_buttonMenu->addChild(pasteItem);

    cocos2d::CCPoint copyPos = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 190.0f, winSize.height * 0.5f + 130.0f));
    copyItem->setPosition(copyPos);
    pasteItem->setPosition(copyPos + cocos2d::CCPoint(0.0f, -40.0f));
    m_colorObjects->addObject(copyItem);
    m_colorObjects->addObject(pasteItem);

    m_currentColorSprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    m_currentColorSprite->setScale(0.8f);
    m_mainLayer->addChild(m_currentColorSprite, 5);
    m_currentColorSprite->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f - 135.0f - 35.0f, winSize.height * 0.5f + 60.0f + 40.0f));
    m_colorObjects->addObject(m_currentColorSprite);

    float colorWidth = m_currentColorSprite->getContentSize().width;

    m_originalColorSprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    m_originalColorSprite->setScaleX(0.8f);
    m_originalColorSprite->setScaleY(0.4f);
    m_mainLayer->addChild(m_originalColorSprite, 5);
    m_originalColorSprite->setPosition(m_currentColorSprite->getPosition() + cocos2d::CCPoint(0.0f, (colorWidth * 0.8f * 0.5f) + (colorWidth * 0.4f * 0.5f)));
    m_colorObjects->addObject(m_originalColorSprite);

    cocos2d::CCPoint colorInputBase(winSize.width * 0.5f - 120.0f, winSize.height * 0.5f + 50.0f);

    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCPoint pos;
        if (i == 3)
            pos = colorInputBase + cocos2d::CCPoint(0.0f, -42.0f);
        else
            pos = colorInputBase + cocos2d::CCPoint(15.0f, (1.5f - i) * 28.0f);

        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        label->setScale(0.5f);
        m_mainLayer->addChild(label);
        label->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        label->setPosition(pos + cocos2d::CCPoint(i == 3 ? -32.0f : -17.0f, 0.0f));
        m_colorObjects->addObject(label);

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
            "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
        bg->setColor(cocos2d::ccColor3B(0, 0, 0));
        bg->setOpacity(100);
        bg->setContentSize(i == 3 ? cocos2d::CCSize(100.0f, 30.0f) : cocos2d::CCSize(50.0f, 30.0f));
        bg->setScale(0.6f);
        m_mainLayer->addChild(bg, -1);
        bg->setPosition(pos);
        m_colorObjects->addObject(bg);

        CCTextInputNode* input = CCTextInputNode::create(i == 3 ? 80.0f : 40.0f, 20.0f, "Num", "Thonburi");
        m_mainLayer->addChild(input, 20);
        input->setPosition(pos);
        input->setMaxLabelScale(0.4f);
        input->setMaxLabelWidth(50.0f);
        input->m_textField->setVisible(false);
        input->m_maxLabelLength = (i == 3) ? 7 : 4;
        input->setAllowedChars((i == 3) ? "0123456789ABCDEFabcdef" : "0123456789");
        input->setLabelPlaceholderColor(cocos2d::ccColor3B(120, 170, 240));
        input->setLabelPlaceholderScale(0.4f);
        input->m_delegate = this;
        input->m_numberInput = (i <= 2);
        m_colorObjects->addObject(input);

        if (i == 2) { m_bInput = input; input->setTag(13); }
        else if (i == 3) { m_hexInput = input; input->setTag(14); }
        else if (i == 1) { m_gInput = input; input->setTag(12); }
        else { m_rInput = input; input->setTag(11); }
    }

    selectColor(m_currentColor);

    m_hsvWidget = ConfigureHSVWidget::create(m_hsv, false, false);
    addChild(m_hsvWidget);
    m_hsvWidget->setPosition(winSize.width * 0.5f - 75.0f, winSize.height * 0.5f + 45.0f);
    m_hsvWidget->setScale(0.85f);
    m_hsvObjects->addObject(m_hsvWidget);

    cocos2d::CCPoint copyColorBase(winSize.width * 0.5f + 142.0f, winSize.height * 0.5f + 76.0f);

    cocos2d::CCLabelBMFont* copyColorLabel = cocos2d::CCLabelBMFont::create("Copy Color", "bigFont.fnt");
    addChild(copyColorLabel);
    copyColorLabel->setScale(0.5f);
    copyColorLabel->setPosition(copyColorBase);
    m_hsvObjects->addObject(copyColorLabel);

    cocos2d::CCPoint copyColorInputPos = copyColorBase + cocos2d::CCPoint(0.0f, -30.0f);

    cocos2d::extension::CCScale9Sprite* copyColorBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
    copyColorBg->setColor(cocos2d::ccColor3B(0, 0, 0));
    copyColorBg->setOpacity(100);
    copyColorBg->setContentSize(cocos2d::CCSize(50.0f, 30.0f));
    m_mainLayer->addChild(copyColorBg, -1);
    copyColorBg->setPosition(copyColorInputPos);
    m_hsvObjects->addObject(copyColorBg);

    m_colorIDInput = CCTextInputNode::create(50.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_colorIDInput, 20);
    m_colorIDInput->setPosition(copyColorInputPos);
    m_colorIDInput->setMaxLabelScale(0.6f);
    m_colorIDInput->setMaxLabelWidth(200.0f);
    m_colorIDInput->m_textField->setVisible(false);
    m_colorIDInput->m_maxLabelLength = 6;
    m_colorIDInput->setAllowedChars("0123456789");
    m_colorIDInput->setLabelPlaceholderColor(cocos2d::ccColor3B(120, 170, 240));
    m_colorIDInput->setLabelPlaceholderScale(0.5f);
    m_colorIDInput->m_delegate = this;
    m_colorIDInput->setTag(5);
    m_colorIDInput->m_numberInput = true;
    m_hsvObjects->addObject(m_colorIDInput);

    cocos2d::CCPoint copyColorLocal = m_buttonMenu->convertToNodeSpace(copyColorInputPos);

    cocos2d::CCSprite* leftArrow2 = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    leftArrow2->setScale(1.1f);
    CCMenuItemSpriteExtra* leftItem2 = CCMenuItemSpriteExtra::create(
        leftArrow2, nullptr, menu_selector(SetupPulsePopup::onUpdateCopyColor));
    m_buttonMenu->addChild(leftItem2);
    leftItem2->setTag(1);
    leftItem2->setPosition(copyColorLocal + cocos2d::CCPoint(-50.0f, 0.0f));
    leftItem2->setSizeMult(2.0f);
    m_hsvObjects->addObject(leftItem2);

    cocos2d::CCSprite* rightArrow2 = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    rightArrow2->setScale(1.1f);
    CCMenuItemSpriteExtra* rightItem2 = CCMenuItemSpriteExtra::create(
        rightArrow2, nullptr, menu_selector(SetupPulsePopup::onUpdateCopyColor));
    m_buttonMenu->addChild(rightItem2);
    rightItem2->setTag(2);
    rightItem2->setPosition(copyColorLocal + cocos2d::CCPoint(50.0f, 0.0f));
    rightItem2->setSizeMult(2.0f);
    m_hsvObjects->addObject(rightItem2);

    cocos2d::CCSprite* plusIcon2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
    CCMenuItemSpriteExtra* plusItem2 = CCMenuItemSpriteExtra::create(
        plusIcon2, nullptr, menu_selector(SetupPulsePopup::onSelectSpecialColor));
    m_buttonMenu->addChild(plusItem2);
    plusItem2->setPosition(m_buttonMenu->convertToNodeSpace(copyColorInputPos + cocos2d::CCPoint(50.0f, 40.0f)));
    m_hsvObjects->addObject(plusItem2);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Applies a temporary <cg>color effect</c> to a group or channel. Set which using <cy>Target Type</c>.\n"
        "If set to group, you can toggle between main and detail colors.\n"
        "<cl>Color Mode</c> tints the target to the selected color.\n"
        "<cp>HSV Mode</c> modifies the base color's hue, saturation, and brightness values.\n"
        "<cr>Exclusive</c> disables all other pulses using the same ID when activated (not visible in editor).",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    CCMenuItemSpriteExtra* modeSelect = (m_pulseMode == 1) ? m_hsvButton : m_colorButton;
    onSelectPulseMode(modeSelect);

    CCMenuItemSpriteExtra* targetSelect = (m_pulseTargetType == 1) ? m_groupButton : m_channelButton;
    onSelectTargetMode(targetSelect);

    updateTextInputLabel();
    updateCopyColorTextInputLabel();

    m_colorInitializing = false;
    m_modesInitializing = false;

    return true;
}

void SetupPulsePopup::onClose(cocos2d::CCObject* sender)
{
    m_idInput->m_delegate = nullptr;
    m_idInput->onClickTrackNode(false);
    m_colorIDInput->m_delegate = nullptr;
    m_colorIDInput->onClickTrackNode(false);
    m_fadeInInput->m_delegate = nullptr;
    m_fadeInInput->onClickTrackNode(false);
    m_holdInput->m_delegate = nullptr;
    m_holdInput->onClickTrackNode(false);
    m_fadeOutInput->m_delegate = nullptr;
    m_fadeOutInput->onClickTrackNode(false);
    m_rInput->m_delegate = nullptr;
    m_rInput->onClickTrackNode(false);
    m_gInput->m_delegate = nullptr;
    m_gInput->onClickTrackNode(false);
    m_bInput->m_delegate = nullptr;
    m_bInput->onClickTrackNode(false);
    m_hexInput->m_delegate = nullptr;
    m_hexInput->onClickTrackNode(false);

    if (!m_hsvWidget->m_mixed)
    {
        m_hsv = m_hsvWidget->m_hsv;
        updateHSVValue();
    }

    SetupTriggerPopup::onClose(sender);
}

void SetupPulsePopup::onCopy(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_copiedColor = getColorValue();
}

void SetupPulsePopup::onExclusive(cocos2d::CCObject* sender)
{
    m_pulseExclusive = !m_pulseExclusive;

    if (m_gameObject)
    {
        m_gameObject->m_pulseExclusive = m_pulseExclusive;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_pulseExclusive = m_pulseExclusive;
        }
    }
}

void SetupPulsePopup::onGroupMainOnly(cocos2d::CCObject* sender)
{
    m_groupMainOnly = !m_groupMainOnly;

    if (m_groupMainOnly && m_groupSecondaryOnly)
    {
        m_groupSecondaryOnly = false;
        updateGroupSecondaryOnly();
        m_secondaryOnlyToggler->toggle(false);
        updateGroupMainOnly();
    }
    else
    {
        updateGroupMainOnly();
    }
}

void SetupPulsePopup::onGroupSecondaryOnly(cocos2d::CCObject* sender)
{
    m_groupSecondaryOnly = !m_groupSecondaryOnly;

    if (m_groupSecondaryOnly && m_groupMainOnly)
    {
        m_groupMainOnly = false;
        updateGroupMainOnly();
        m_mainOnlyToggler->toggle(false);
        updateGroupSecondaryOnly();
    }
    else
    {
        updateGroupSecondaryOnly();
    }
}

void SetupPulsePopup::onHSVLegacyMode(cocos2d::CCObject* sender)
{
    m_staticHSV = !m_staticHSV;

    if (m_gameObject)
    {
        m_gameObject->m_legacyHSV = m_staticHSV;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_legacyHSV = m_staticHSV;
        }
    }
}

void SetupPulsePopup::onPaste(cocos2d::CCObject* sender)
{
    selectColor(GameManager::sharedState()->m_copiedColor);
}

void SetupPulsePopup::onSelectPulseMode(cocos2d::CCObject* sender)
{
    int tag = -1;
    bool showHsv = false;
    bool showColor = false;

    if (sender)
    {
        tag = sender->getTag();
        ButtonSprite* colorBtn = (ButtonSprite*)m_colorButton->getNormalImage();
        ButtonSprite* hsvBtn = (ButtonSprite*)m_hsvButton->getNormalImage();

        if (tag == 1)
        {
            showHsv = true;
            colorBtn->updateBGImage("GJ_button_04.png");
            hsvBtn->updateBGImage("GJ_button_02.png");
        }
        else
        {
            showColor = true;
            colorBtn->updateBGImage("GJ_button_02.png");
            hsvBtn->updateBGImage("GJ_button_04.png");
        }
    }
    else
    {
        ((ButtonSprite*)m_colorButton->getNormalImage())->updateBGImage("GJ_button_04.png");
        ((ButtonSprite*)m_hsvButton->getNormalImage())->updateBGImage("GJ_button_04.png");
    }

    for (unsigned int i = 0; i < m_colorObjects->count(); i++)
        ((cocos2d::CCNode*)m_colorObjects->objectAtIndex(i))->setVisible(showColor);

    for (unsigned int i = 0; i < m_hsvObjects->count(); i++)
        ((cocos2d::CCNode*)m_hsvObjects->objectAtIndex(i))->setVisible(showHsv);

    m_colorIDInput->setTouchEnabled(showHsv);
    m_rInput->setTouchEnabled(showColor);
    m_gInput->setTouchEnabled(showColor);
    m_bInput->setTouchEnabled(showColor);
    m_hexInput->setTouchEnabled(showColor);

    m_pulseMode = tag;
    if (!m_modesInitializing)
        updatePulseMode();
}

void SetupPulsePopup::onSelectSpecialColor(cocos2d::CCObject* sender)
{
    GJSpecialColorSelect* select = GJSpecialColorSelect::create(m_copyColorID, this, ColorSelectType::Pulse);
    select->setTag(7);
    select->show();
}

void SetupPulsePopup::onSelectSpecialTargetID(cocos2d::CCObject* sender)
{
    GJSpecialColorSelect* select = GJSpecialColorSelect::create(m_targetGroupID, this, ColorSelectType::Pulse);
    select->setTag(6);
    select->show();
}

void SetupPulsePopup::onSelectTargetMode(cocos2d::CCObject* sender)
{
    int tag = -1;
    bool showGroup = false;
    bool showChannel = false;
    const char* labelText = "ID";

    if (sender)
    {
        tag = sender->getTag();

        ButtonSprite* channelBtn = (ButtonSprite*)m_channelButton->getNormalImage();
        ButtonSprite* groupBtn = (ButtonSprite*)m_groupButton->getNormalImage();

        if (tag == 1)
        {
            showGroup = true;
            labelText = "Group ID";
            groupBtn->updateBGImage("GJ_button_02.png");
            channelBtn->updateBGImage("GJ_button_04.png");
        }
        else
        {
            showChannel = true;
            labelText = "Channel ID";
            channelBtn->updateBGImage("GJ_button_02.png");
            groupBtn->updateBGImage("GJ_button_04.png");
        }
    }
    else
    {
        ((ButtonSprite*)m_channelButton->getNormalImage())->updateBGImage("GJ_button_04.png");
        ((ButtonSprite*)m_groupButton->getNormalImage())->updateBGImage("GJ_button_04.png");
    }

    m_specialColorButton->setVisible(showChannel);
    m_specialColorButton->setEnabled(showChannel);

    m_mainOnlyToggler->setVisible(showGroup);
    m_mainOnlyToggler->setEnabled(showGroup);
    if (m_mainOnlyLabel) m_mainOnlyLabel->setVisible(showGroup);

    m_secondaryOnlyToggler->setVisible(showGroup);
    m_secondaryOnlyToggler->setEnabled(showGroup);
    if (m_secondaryOnlyLabel) m_secondaryOnlyLabel->setVisible(showGroup);

    m_idLabel->setString(labelText);

    m_pulseTargetType = tag;

    if (!m_modesInitializing)
    {
        m_targetGroupID = 0;
        updateTargetID();
        updateTextInputLabel();
        updatePulseTargetType();
    }
}

void SetupPulsePopup::onUpdateCopyColor(cocos2d::CCObject* sender)
{
    if (m_copyColorID > 999)
        m_copyColorID = 0;

    int newValue = (sender->getTag() == 1) ? m_copyColorID - 1 : m_copyColorID + 1;
    if (newValue < 0) newValue = 0;
    if (newValue > 999) newValue = 999;

    m_copyColorID = newValue;
    updateCopyColor();
    updateCopyColorTextInputLabel();
}

void SetupPulsePopup::onUpdateCustomColor(cocos2d::CCObject* sender)
{
    if (m_targetGroupID > 999 && m_pulseTargetType != 1)
        m_targetGroupID = 0;

    int newValue = (sender->getTag() == 1) ? m_targetGroupID - 1 : m_targetGroupID + 1;
    int maxValue = (m_pulseTargetType == 1) ? 9999 : 999;

    if (newValue < 0) newValue = 0;
    if (newValue > maxValue) newValue = maxValue;

    m_targetGroupID = newValue;
    updateTargetID();
    updateEditorLabel();
    updateTextInputLabel();
}

void SetupPulsePopup::show()
{
    m_colorInitializing = true;
    m_originalColor = getColorValue();
    colorValueChanged(m_originalColor);
    m_colorInitializing = false;
    SetupTriggerPopup::show();
}

void SetupPulsePopup::sliderChanged(cocos2d::CCObject* sender)
{
    float value = cocos2d::CCString::createWithFormat(
        "%.01f", ((SliderThumb*)sender)->getValue() * 10.0f)->floatValue();

    switch (sender->getTag())
    {
        case 1: m_fadeInTime = value; updateFadeInTime(); updateFadeInLabel(true); break;
        case 2: m_holdTime = value; updateHoldTime(); updateHoldLabel(true); break;
        case 3: m_fadeOutTime = value; updateFadeOutTime(); updateFadeOutLabel(true); break;
    }
}

void SetupPulsePopup::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate) return;

    switch (node->getTag())
    {
        case 4:
        {
            m_targetGroupID = atoi(node->getString().c_str());
            if (m_targetGroupID <= 0) m_targetGroupID = 1;
            int maxValue = (m_pulseTargetType == 1) ? 9999 : 999;
            if (m_targetGroupID > maxValue) m_targetGroupID = maxValue;
            updateTargetID();
            updateEditorLabel();
            break;
        }
        case 5:
        {
            m_copyColorID = atoi(node->getString().c_str());
            if (m_copyColorID < 0) m_copyColorID = 0;
            if (m_copyColorID > 999) m_copyColorID = 999;
            updateCopyColor();
            break;
        }
        case 8:
        {
            m_fadeInTime = atof(node->getString().c_str());
            if (m_fadeInTime <= 0.0f) m_fadeInTime = 0.0f;
            updateFadeInTime();
            float v = m_fadeInTime / 10.0f;
            if (v >= 1.0f) v = 1.0f;
            else if (v <= 0.0f) v = 0.0f;
            m_fadeInSlider->setValue(v);
            break;
        }
        case 9:
        {
            m_holdTime = atof(node->getString().c_str());
            if (m_holdTime <= 0.0f) m_holdTime = 0.0f;
            updateHoldTime();
            float v = m_holdTime / 10.0f;
            if (v >= 1.0f) v = 1.0f;
            else if (v <= 0.0f) v = 0.0f;
            m_holdSlider->setValue(v);
            break;
        }
        case 10:
        {
            m_fadeOutTime = atof(node->getString().c_str());
            if (m_fadeOutTime <= 0.0f) m_fadeOutTime = 0.0f;
            updateFadeOutTime();
            float v = m_fadeOutTime / 10.0f;
            if (v >= 1.0f) v = 1.0f;
            else if (v <= 0.0f) v = 0.0f;
            m_fadeOutSlider->setValue(v);
            break;
        }
        case 11:
        case 12:
        case 13:
        {
            int value = atoi(node->getString().c_str());
            if (value < 0) value = 0;
            if (value > 255) value = 255;

            switch (node->getTag())
            {
                case 11: m_currentColor.r = value; break;
                case 12: m_currentColor.g = value; break;
                case 13: m_currentColor.b = value; break;
            }
            selectColor(m_currentColor);
            break;
        }
        case 14:
        {
            std::string hex = m_hexInput->getString();
            if (hex.length() != 6) return;
            m_currentColor = ColorSelectPopup::hexToColor(hex);
            selectColor(m_currentColor);
            break;
        }
    }
}

void SetupPulsePopup::textInputOpened(CCTextInputNode* node)
{
}

void SetupPulsePopup::updateColorLabels()
{
    m_disableTextDelegate = true;

    m_rInput->setString(cocos2d::CCString::createWithFormat("%i", m_currentColor.r)->getCString());
    m_gInput->setString(cocos2d::CCString::createWithFormat("%i", m_currentColor.g)->getCString());
    m_bInput->setString(cocos2d::CCString::createWithFormat("%i", m_currentColor.b)->getCString());
    m_hexInput->setString(ColorSelectPopup::colorToHex(m_currentColor).c_str());

    m_disableTextDelegate = false;
}

void SetupPulsePopup::updateColorValue()
{
    if (m_colorInitializing) return;

    if (m_gameObject)
    {
        m_gameObject->m_triggerTargetColor = m_currentColor;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_triggerTargetColor = m_currentColor;
        }
    }
}

void SetupPulsePopup::updateCopyColor()
{
    if (m_gameObject)
    {
        m_gameObject->m_copyColorID = m_copyColorID;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_copyColorID = m_copyColorID;
        }
    }
}

void SetupPulsePopup::updateCopyColorTextInputLabel()
{
    m_disableTextDelegate = true;

    std::string text = cocos2d::CCString::createWithFormat("%i", m_copyColorID)->getCString();
    if (m_copyColorID > 999) text = "0";
    m_colorIDInput->setString(text);

    if (m_copyColorID > 999)
        m_colorIDInput->updateLabel(GJSpecialColorSelect::textForColorIdx(m_copyColorID));

    m_disableTextDelegate = false;
}

void SetupPulsePopup::updateFadeInLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_fadeInTime == -0.1f)
        text = "Mixed";
    else if (!decimals && m_fadeInTime - (float)(int)m_fadeInTime == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_fadeInTime)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_fadeInTime)->getCString();

    m_fadeInInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupPulsePopup::updateFadeInTime()
{
    if (m_gameObject)
    {
        m_gameObject->m_fadeInDuration = m_fadeInTime;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_fadeInDuration = m_fadeInTime;
        }
    }
}

void SetupPulsePopup::updateFadeOutLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_fadeOutTime == -0.1f)
        text = "Mixed";
    else if (!decimals && m_fadeOutTime - (float)(int)m_fadeOutTime == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_fadeOutTime)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_fadeOutTime)->getCString();

    m_fadeOutInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupPulsePopup::updateFadeOutTime()
{
    if (m_gameObject)
    {
        m_gameObject->m_fadeOutDuration = m_fadeOutTime;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_fadeOutDuration = m_fadeOutTime;
        }
    }
}

void SetupPulsePopup::updateGroupMainOnly()
{
    if (m_gameObject)
    {
        m_gameObject->m_pulseMainOnly = m_groupMainOnly;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_pulseMainOnly = m_groupMainOnly;
        }
    }
}

void SetupPulsePopup::updateGroupSecondaryOnly()
{
    if (m_gameObject)
    {
        m_gameObject->m_pulseDetailOnly = m_groupSecondaryOnly;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_pulseDetailOnly = m_groupSecondaryOnly;
        }
    }
}

void SetupPulsePopup::updateHSVValue()
{
    if (m_gameObject)
    {
        m_gameObject->m_hsvValue = m_hsv;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_hsvValue = m_hsv;
        }
    }
}

void SetupPulsePopup::updateHoldLabel(bool decimals)
{
    m_disableTextDelegate = true;

    std::string text;
    if (m_holdTime == -0.1f)
        text = "Mixed";
    else if (!decimals && m_holdTime - (float)(int)m_holdTime == 0.0f)
        text = cocos2d::CCString::createWithFormat("%i", (int)m_holdTime)->getCString();
    else
        text = cocos2d::CCString::createWithFormat("%.02f", m_holdTime)->getCString();

    m_holdInput->setString(text);
    m_disableTextDelegate = false;
}

void SetupPulsePopup::updateHoldTime()
{
    if (m_gameObject)
    {
        m_gameObject->m_holdDuration = m_holdTime;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_holdDuration = m_holdTime;
        }
    }
}

void SetupPulsePopup::updatePulseMode()
{
    if (m_gameObject)
    {
        m_gameObject->m_pulseMode = m_pulseMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_pulseMode = m_pulseMode;
        }
    }
}

void SetupPulsePopup::updatePulseTargetType()
{
    if (m_gameObject)
    {
        m_gameObject->m_pulseTargetType = m_pulseTargetType;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_pulseTargetType = m_pulseTargetType;
        }
    }
}

void SetupPulsePopup::updateTargetID()
{
    if (m_gameObject)
    {
        m_gameObject->setTargetID(m_targetGroupID);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->setTargetID(m_targetGroupID);
        }
    }
}

void SetupPulsePopup::updateTextInputLabel()
{
    m_disableTextDelegate = true;

    std::string text = cocos2d::CCString::createWithFormat("%i", m_targetGroupID)->getCString();
    if (m_targetGroupID > 999 && m_pulseTargetType != 1) text = "0";
    m_idInput->setString(text);

    if (m_targetGroupID > 999 && m_pulseTargetType != 1)
        m_idInput->updateLabel(GJSpecialColorSelect::textForColorIdx(m_targetGroupID));

    m_disableTextDelegate = false;
}

SetupPulsePopup::~SetupPulsePopup()
{
}