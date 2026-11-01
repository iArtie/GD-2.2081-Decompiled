#include "Layers/CustomizeObjectLayer.h"
#include "Objects/GameObject.h"
#include "Objects/TextGameObject.h"
#include "GJSpriteColor.h"
#include "Editor/ColorChannelSprite.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/Slider.h"
#include "UI/SliderThumb.h"
#include "Managers/GJEffectManager.h"
#include "Layers/GJColorSetupLayer.h"
#include "Popups/ColorSelectPopup.h"
#include "Popups/HSVWidgetPopup.h"
#include "Popups/CustomizeObjectSettingsPopup.h"
#include "UI/InfoAlertButton.h"
#include "Layers/LevelEditorLayer.h"
#include "Editor/EditorUI.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Editor/GJSpecialColorSelect.h"

void CustomizeObjectLayer::colorSetupClosed(int id)
{
    updateSelected(id);
    m_customColorChannel = id;
    m_disableTextDelegate = true;
    updateCustomColorLabels();
    m_disableTextDelegate = false;
    
    ButtonSprite* btn = getButtonByTag(1008);
    highlightSelected(btn);
    updateColorSprite();
    updateChannelLabel(id);
}

CustomizeObjectLayer* CustomizeObjectLayer::create(GameObject* object, cocos2d::CCArray* objects)
{
    CustomizeObjectLayer* pRet = new CustomizeObjectLayer();
    if (pRet && pRet->init(object, objects))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void CustomizeObjectLayer::determineStartValues()
{
    m_hasBaseColor = false;
    m_hasDetailColor = false;
    m_glowDisabled = true;
    
    if (m_targetObject)
    {
        m_hasBaseColor = (GameObject::getRelativeSpriteColor(m_targetObject, 1) != nullptr);
        m_hasDetailColor = (GameObject::getRelativeSpriteColor(m_targetObject, 2) != nullptr);
        m_glowDisabled = m_targetObject->m_hasNoGlow;
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_targetObjects->objectAtIndex(i);
            if (GameObject::getRelativeSpriteColor(obj, 1))
                m_hasBaseColor = true;
            if (GameObject::getRelativeSpriteColor(obj, 2))
                m_hasDetailColor = true;
            if (!obj->m_hasNoGlow)
                m_glowDisabled = false;
        }
    }
}

int CustomizeObjectLayer::getActiveMode(bool ignoreDefault)
{
    if (m_targetObject)
    {
        GJSpriteColor* color = GameObject::getRelativeSpriteColor(m_targetObject, m_selectedMode);
        if (!color)
            return 0;
            
        int id = color->m_colorID;
        int defaultId = color->m_defaultColorID;
        
        if (id == defaultId || id == 0)
            id = defaultId;
            
        if (ignoreDefault)
            return id;
        else
            return 0;
    }
    else if (m_targetObjects)
    {
        int result = -1;
        bool allDefault = !ignoreDefault;
        
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_targetObjects->objectAtIndex(i);
            GJSpriteColor* color = GameObject::getRelativeSpriteColor(obj, m_selectedMode);
            
            if (color)
            {
                int id = color->m_colorID;
                int defaultId = color->m_defaultColorID;
                
                if (id == defaultId || id == 0)
                    id = defaultId;
                    
                if (allDefault)
                    allDefault = (defaultId == id);
                    
                if (result == -1)
                    result = id;
                else if (id != result)
                {
                    if (allDefault)
                        return 0;
                    return -1;
                }
            }
        }
        
        if (allDefault)
            return 0;
        return result;
    }
    
    return 0;
}

ButtonSprite* CustomizeObjectLayer::getButtonByTag(int tag)
{
    for (unsigned int i = 0; i < m_colorButtons->count(); i++)
    {
        ButtonSprite* btn = (ButtonSprite*)m_colorButtons->objectAtIndex(i);
        if (btn->getTag() == tag)
            return btn;
    }
    return nullptr;
}

void CustomizeObjectLayer::highlightSelected(ButtonSprite* sprite)
{
    for (unsigned int i = 0; i < m_colorButtons->count(); i++)
    {
        ButtonSprite* btn = (ButtonSprite*)m_colorButtons->objectAtIndex(i);
        const char* texture = (btn->getTag() == 1008) ? "GJ_button_05.png" : "GJ_button_04.png";
        ButtonSprite::updateBGImage(btn, texture);
    }
    
    bool isCustom = (sprite && sprite->getTag() == 1008);
    m_arrowDown->setVisible(isCustom);
    m_arrowUp->setVisible(isCustom);
    m_customColorInput->setTouchEnabled(isCustom);
    m_customColorInput->setVisible(isCustom);
    m_customColorInputBG->setVisible(isCustom);
}

void CustomizeObjectLayer::hsvPopupClosed(HSVWidgetPopup* popup, cocos2d::ccHSVValue value)
{
    int mode = (popup->getTag() == 1) ? 1 : 2;
    
    if (m_targetObject)
    {
        GJSpriteColor* color = GameObject::getRelativeSpriteColor(m_targetObject, mode);
        if (color)
            color->m_hsv = value;
    }
    else if (m_targetObjects)
    {
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_targetObjects->objectAtIndex(i);
            GJSpriteColor* color = GameObject::getRelativeSpriteColor(obj, mode);
            if (color)
                color->m_hsv = value;
        }
    }
    
    updateHSVButtons();
}

bool CustomizeObjectLayer::init(GameObject* object, cocos2d::CCArray* objects)
{
    if (!FLAlertLayer::init(200))
        return false;
    
    m_targetObject = (TextGameObject*)object;
    m_targetObjects = cocos2d::CCArray::create();
    m_targetObjects->retain();
    
    if (object)
    {
        m_targetObject = (TextGameObject*)object;
    }
    else if (objects)
    {
        for (unsigned int i = 0; i < objects->count(); i++)
        {
            GameObject* obj = (GameObject*)objects->objectAtIndex(i);
            if (GameObject::canChangeCustomColor(obj))
                m_targetObjects->addObject(obj);
        }
    }
    
    m_disableTextDelegate = true;
    m_noElasticity = true;
    m_selectedMode = 1;
    m_customColorChannel = 10;
    m_kerningAmount = 0;
    m_showTextInput = false;
    m_customColorSelected = false;
    m_hasBaseColor = false;
    m_hasDetailColor = false;
    m_glowDisabled = true;
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setContentSize(cocos2d::CCSize(360, 240));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(centerX, centerY));
    
    // Title
    m_titleLabel = cocos2d::CCLabelBMFont::create("Base Color", "bigFont.fnt");
    m_titleLabel->setPosition(cocos2d::CCPoint(centerX, centerY + 120.0f - 25.0f));
    m_mainLayer->addChild(m_titleLabel);
    
    m_colorTabNodes = cocos2d::CCArray::create();
    m_colorTabNodes->retain();
    m_unkArray = cocos2d::CCArray::create();
    m_unkArray->retain();
    m_detailTabNodes = cocos2d::CCArray::create();
    m_detailTabNodes->retain();
    
    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_mainLayer->setPosition(cocos2d::CCPoint(0, -15));
    
    // OK button
    ButtonSprite* okBtn = ButtonSprite::create("OK");
    CCMenuItemSpriteExtra* okMenuItem = CCMenuItemSpriteExtra::create(
        okBtn, nullptr, this, menu_selector(CustomizeObjectLayer::onClose));
    m_buttonMenu->addChild(okMenuItem);
    okMenuItem->setPosition(cocos2d::CCPoint(centerX, centerY - 120.0f + 25.0f));
    
    // Info button
    InfoAlertButton* infoBtn = InfoAlertButton::create(
        "Help",
        "Select <cg>color channel</c> to apply to the selected object(s). Edit the properties of the selected channel in the bottom right.\n<cy>Next Free</c> uses the next unused color channel ID, which goes up to 999 maximum.\n<cl>HSV</c> lets you change the hue, saturation, and brightness of the selected object(s) (and not the channel as a whole).\nThe <cp>Copy and Paste</c> buttons allow you to quickly apply to other objects.",
        1.0f);
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition(cocos2d::CCPoint(centerX + 180.0f - 18.0f, centerY + 120.0f - 18.0f));
    
    determineStartValues();
    
    // Base button
    ButtonSprite* baseSprite = ButtonSprite::create("Base", 0, 1, 0.6f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    m_baseButton = CCMenuItemSpriteExtra::create(baseSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onSelectMode));
    m_baseButton->setTag(1);
    m_buttonMenu->addChild(m_baseButton);
    m_baseButton->setPosition(cocos2d::CCPoint(centerX - 180.0f + 35.0f + 8.0f, centerY + 120.0f + 19.0f - 15.0f));
    
    // Detail button
    ButtonSprite* detailSprite = ButtonSprite::create("Detail", 0, 1, 0.6f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
    m_detailButton = CCMenuItemSpriteExtra::create(detailSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onSelectMode));
    m_detailButton->setTag(2);
    m_buttonMenu->addChild(m_detailButton);
    m_detailButton->setPosition(m_baseButton->getPosition() + cocos2d::CCPoint(95, 0));
    
    if (m_hasDetailColor && m_hasBaseColor)
    {
        // Both colors available
    }
    else if (m_hasDetailColor)
    {
        m_baseButton->setEnabled(false);
        m_baseButton->setVisible(false);
        m_detailButton->setPosition(m_baseButton->getPosition());
    }
    else if (m_hasBaseColor)
    {
        m_detailButton->setEnabled(false);
        m_detailButton->setVisible(false);
    }
    else
    {
        m_detailButton->setEnabled(false);
        m_detailButton->setVisible(false);
    }
    
    // Settings button
    cocos2d::CCSprite* settingsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSprite->setScale(0.6f);
    CCMenuItemSpriteExtra* settingsBtn = CCMenuItemSpriteExtra::create(
        settingsSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onSettings));
    m_buttonMenu->addChild(settingsBtn);
    
    CCMenuItemSpriteExtra* refBtn = m_detailButton->isVisible() ? m_detailButton : m_baseButton;
    settingsBtn->setPosition(refBtn->getPosition() + cocos2d::CCPoint(70, 0));
    
    // Selected color label
    m_selectedColorLabel = cocos2d::CCLabelBMFont::create(" ", "bigFont.fnt");
    m_selectedColorLabel->setScale(0.75f);
    m_selectedColorLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_mainLayer->addChild(m_selectedColorLabel, 10);
    m_selectedColorLabel->setPosition(cocos2d::CCPoint(centerX + 130.0f, centerY - 91.0f));
    m_colorTabNodes->addObject(m_selectedColorLabel);
    
    // HSV buttons
    cocos2d::CCSprite* hsvSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_hsvBtn_001.png");
    m_baseColorHSV = CCMenuItemSpriteExtra::create(hsvSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onHSV));
    m_baseColorHSV->setTag(1);
    m_buttonMenu->addChild(m_baseColorHSV);
    m_baseColorHSV->setPosition(cocos2d::CCPoint(centerX - 180.0f + 25.0f, centerY + 120.0f - 40.0f));
    
    cocos2d::CCSprite* hsvSprite2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_hsvBtn_001.png");
    m_detailColorHSV = CCMenuItemSpriteExtra::create(hsvSprite2, nullptr, this, menu_selector(CustomizeObjectLayer::onHSV));
    m_detailColorHSV->setTag(2);
    m_buttonMenu->addChild(m_detailColorHSV);
    m_detailColorHSV->setPosition(m_baseColorHSV->getPosition());
    
    m_baseColorHSV->setVisible(false);
    m_detailColorHSV->setVisible(false);
    
    m_colorButtons = cocos2d::CCArray::create();
    m_colorButtons->retain();
    
    // Create color buttons: P-Col 1, P-Col 2, Light BG, Default, and 1-10
    const char* labels[] = {"P-Col 1", "P-Col 2", "Light BG", "Default"};
    int tags[] = {1005, 1006, 1007, 0};
    
    for (int i = 0; i < 4; i++)
    {
        ButtonSprite* sprite = ButtonSprite::create(labels[i], 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            sprite, nullptr, this, menu_selector(CustomizeObjectLayer::onSelectColor));
        item->setTag(tags[i]);
        sprite->setTag(tags[i]);
        m_buttonMenu->addChild(item);
        m_colorButtons->addObject(sprite);
        m_colorTabNodes->addObject(item);
    }
    
    // Position the 4 preset buttons
    cocos2d::CCPoint basePos = cocos2d::CCPoint(centerX, centerY + 25.0f);
    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_colorButtons->objectAtIndex(i);
        float x = basePos.x - 135.0f + i * 90.0f;
        node->setPosition(cocos2d::CCPoint(x, basePos.y));
    }
    
    // Create color buttons 1-10
    for (int i = 1; i <= 10; i++)
    {
        ButtonSprite* sprite = ButtonSprite::create(
            cocos2d::CCString::createWithFormat("%i", i)->getCString(),
            0, 1, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            sprite, nullptr, this, menu_selector(CustomizeObjectLayer::onSelectColor));
        sprite->setTag(i);
        item->setTag(i);
        m_buttonMenu->addChild(item);
        m_colorButtons->addObject(sprite);
        m_colorTabNodes->addObject(item);
        
        float x = basePos.x - 130.0f + ((i - 1) % 5) * 65.0f;
        float y = basePos.y - 40.0f * ((int)((i - 1) / 5) + 1);
        item->setPosition(cocos2d::CCPoint(x, y));
        
        if (i == 10)
        {
            m_customColorButton = item;
            m_customColorButtonSprite = sprite;
            sprite->setTag(1008);
            item->setTag(1008);
            ButtonSprite::setString(m_customColorButtonSprite, "+");
        }
    }
    
    // Custom color input
    cocos2d::CCPoint inputPos = cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 30.0f, centerY);
    
    m_customColorInputBG = cocos2d::extension::CCScale9Sprite::create(
        "square02_small.png", cocos2d::CCRect(0, 0, 40, 40));
    m_customColorInputBG->setContentSize(cocos2d::CCSize(50, 30));
    m_mainLayer->addChild(m_customColorInputBG, -1);
    m_customColorInputBG->setPosition(inputPos);
    
    m_customColorInput = CCTextInputNode::create(100.0f, 40.0f, "Num", "Thonburi");
    m_mainLayer->addChild(m_customColorInput, 20);
    m_customColorInput->setPosition(inputPos);
    m_customColorInput->setMaxLabelScale(0.6f);
    m_customColorInput->setMaxLabelWidth(200.0f);
    m_customColorInput->m_textField->setVisible(false);
    m_customColorInput->m_maxLabelLength = 4;
    m_customColorInput->setAllowedChars("0123456789");
    m_customColorInput->setLabelPlaceholderColor(cocos2d::ccColor3B(170, 170, 240));
    m_customColorInput->setLabelPlaceholderScale(0.5f);
    m_customColorInput->m_delegate = this;
    m_customColorInput->setTag(0);
    
    // Arrow buttons
    cocos2d::CCSprite* downSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    downSprite->setScale(1.4f);
    downSprite->setRotation(90.0f);
    m_arrowDown = CCMenuItemSpriteExtra::create(
        downSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onUpdateCustomColor));
    m_arrowDown->setTag(1);
    m_buttonMenu->addChild(m_arrowDown);
    m_arrowDown->setPosition(inputPos + cocos2d::CCPoint(0, 40));
    m_arrowDown->setSizeMult(2.0f);
    
    cocos2d::CCSprite* upSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    upSprite->setScale(1.4f);
    upSprite->setRotation(90.0f);
    m_arrowUp = CCMenuItemSpriteExtra::create(
        upSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onUpdateCustomColor));
    m_arrowUp->setTag(2);
    m_buttonMenu->addChild(m_arrowUp);
    m_arrowUp->setPosition(inputPos + cocos2d::CCPoint(0, -40));
    m_arrowUp->setSizeMult(2.0f);
    
    m_customColorChannel = GameManager::sharedState()->m_currentColorChannel;
    updateCustomColorLabels();
    
    // Color sprite
    m_colorSprite = ColorChannelSprite::create();
    m_colorSpriteButton = CCMenuItemSpriteExtra::create(
        m_colorSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onEditColor));
    m_buttonMenu->addChild(m_colorSpriteButton);
    m_colorSpriteButton->setPosition(cocos2d::CCPoint(centerX + 180.0f - 25.0f, centerY - 120.0f + 12.0f));
    m_colorTabNodes->addObject(m_colorSpriteButton);
    
    // Live edit button
    cocos2d::CCSprite* liveSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    liveSprite->setColor(cocos2d::ccColor3B(170, 170, 170));
    liveSprite->setOpacity(100);
    
    cocos2d::CCLabelBMFont* liveLabel = cocos2d::CCLabelBMFont::create("L", "bigFont.fnt");
    liveLabel->setScale(1.4f);
    liveLabel->setOpacity(100);
    liveLabel->setPosition(cocos2d::CCPoint(2.0f, 2.5f));
    liveSprite->addChild(liveLabel);
    liveSprite->setScale(0.3f);
    
    m_liveSelectButton = CCMenuItemSpriteExtra::create(
        liveSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onLiveEdit));
    m_buttonMenu->addChild(m_liveSelectButton);
    m_liveSelectButton->setPosition(m_colorSpriteButton->getPosition() + cocos2d::CCPoint(18, 22));
    m_liveSelectButton->setSizeMult(2.0f);
    
    // Next Free button
    ButtonSprite* nextSprite = ButtonSprite::create("Next Free", 0, 1, 0.7f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* nextBtn = CCMenuItemSpriteExtra::create(
        nextSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onNextColorChannel));
    m_buttonMenu->addChild(nextBtn);
    nextBtn->setPosition(cocos2d::CCPoint(centerX - 180.0f + 42.0f, centerY - 120.0f + 8.0f));
    m_colorTabNodes->addObject(nextBtn);
    
    // Copy/Paste buttons
    ButtonSprite* copySprite = ButtonSprite::create("Copy", 0, 1, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    ButtonSprite* pasteSprite = ButtonSprite::create("Paste", 0, 1, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    
    CCMenuItemSpriteExtra* copyBtn = CCMenuItemSpriteExtra::create(
        copySprite, nullptr, this, menu_selector(CustomizeObjectLayer::onCopy));
    CCMenuItemSpriteExtra* pasteBtn = CCMenuItemSpriteExtra::create(
        pasteSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onPaste));
    
    cocos2d::CCPoint copyPos = cocos2d::CCPoint(centerX + 210.0f, centerY + 142.0f);
    if (object)
    {
        m_buttonMenu->addChild(copyBtn);
        copyBtn->setPosition(copyPos);
        m_buttonMenu->addChild(pasteBtn);
        pasteBtn->setPosition(copyPos + cocos2d::CCPoint(0, -31));
    }
    else if (LevelEditorLayer::canPasteState(GameManager::sharedState()->m_levelEditorLayer))
    {
        m_buttonMenu->addChild(pasteBtn);
        pasteBtn->setPosition(copyPos);
    }
    
    // Browse button
    ButtonSprite* browseSprite = ButtonSprite::create("Browse", 0, 1, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 25.0f);
    CCMenuItemSpriteExtra* browseBtn = CCMenuItemSpriteExtra::create(
        browseSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onBrowse));
    m_buttonMenu->addChild(browseBtn);
    browseBtn->setPosition(cocos2d::CCPoint(centerX + 210.0f, centerY - 140.0f));
    
    // Text support for object ID 914 or 1615
    if (m_targetObject && (m_targetObject->m_objectID == 914 || m_targetObject->m_objectID == 1615))
    {
        m_customColorSelected = true;
        m_textTabNodes = cocos2d::CCArray::create();
        m_textTabNodes->retain();
        
        // Text button
        ButtonSprite* textSprite = ButtonSprite::create("Text", 0, 1, 0.6f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
        m_textButton = CCMenuItemSpriteExtra::create(
            textSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onSelectMode));
        m_textButton->setTag(3);
        m_buttonMenu->addChild(m_textButton);
        m_textButton->setPosition(cocos2d::CCPoint(centerX + 180.0f - 35.0f - 8.0f, centerY + 120.0f + 19.0f - 15.0f));
        
        // Kerning label
        m_kerningLabel = cocos2d::CCLabelBMFont::create("Kerning: 0", "bigFont.fnt");
        m_mainLayer->addChild(m_kerningLabel);
        m_kerningLabel->setScale(0.6f);
        m_kerningLabel->setPosition(cocos2d::CCPoint(centerX, centerY - 15.0f));
        m_textTabNodes->addObject(m_kerningLabel);
        
        // Kerning slider
        m_kerningSlider = Slider::create(this, menu_selector(CustomizeObjectLayer::sliderChanged), 0, 0.8f);
        m_kerningSlider->setBarVisibility(false);
        m_mainLayer->addChild(m_kerningSlider);
        m_kerningSlider->setPosition(m_kerningLabel->getPosition() + cocos2d::CCPoint(0, -28));
        m_textTabNodes->addObject(m_kerningSlider);
        
        m_kerningAmount = m_targetObject->getTextKerning();
        updateKerningLabel();
        m_kerningSlider->setValue(kerningSliderValueFromValue(m_kerningAmount));
        
        if (m_targetObject->m_objectID == 914)
        {
            // Text input for object 914
            cocos2d::CCPoint textPos = cocos2d::CCPoint(centerX, centerY + 50.0f);
            
            cocos2d::extension::CCScale9Sprite* textBg = cocos2d::extension::CCScale9Sprite::create(
                "square02_small.png", cocos2d::CCRect(0, 0, 40, 40));
            textBg->setOpacity(100);
            textBg->setContentSize(cocos2d::CCSize(240, 30));
            m_mainLayer->addChild(textBg, -1);
            textBg->setPosition(textPos);
            m_textTabNodes->addObject(textBg);
            textBg->setVisible(false);
            
            m_textInput = CCTextInputNode::create(240.0f, 40.0f, "Text", "Thonburi");
            m_mainLayer->addChild(m_textInput, 20);
            m_textInput->setPosition(textPos);
            m_textInput->setMaxLabelScale(0.6f);
            m_textInput->setMaxLabelWidth(240.0f);
            m_textInput->m_textField->setVisible(false);
            m_textInput->m_maxLabelLength = 20;
            m_textInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");
            m_textInput->setLabelPlaceholderColor(cocos2d::ccColor3B(170, 170, 240));
            m_textInput->setLabelPlaceholderScale(0.5f);
            m_textInput->m_delegate = this;
            m_textInput->setTag(1);
            m_textInput->setVisible(false);
            m_textInput->setTouchEnabled(false);
            m_textInput->m_kerningAmount = m_kerningAmount;
            m_textInput->m_filterSwearWords = true;
            m_textInput->setString(m_targetObject->getDisplayText());
            
            // Split button
            ButtonSprite* splitSprite = ButtonSprite::create("Split", 0, 1, 0.6f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
            CCMenuItemSpriteExtra* splitBtn = CCMenuItemSpriteExtra::create(
                splitSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onBreakApart));
            m_buttonMenu->addChild(splitBtn);
            m_textTabNodes->addObject(splitBtn);
            splitBtn->setPosition(cocos2d::CCPoint(centerX + 180.0f - 36.0f, centerY - 120.0f + 10.0f));
            
            // Clear button
            cocos2d::CCSprite* clearSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
            CCMenuItemSpriteExtra* clearBtn = CCMenuItemSpriteExtra::create(
                clearSprite, nullptr, this, menu_selector(CustomizeObjectLayer::onClear));
            m_buttonMenu->addChild(clearBtn);
            m_textTabNodes->addObject(clearBtn);
            clearBtn->setPosition(cocos2d::CCPoint(centerX + 120.0f + 24.0f, centerY + 35.0f));
        }
    }
    
    // Select initial mode
    CCMenuItemSpriteExtra* initialBtn = m_hasBaseColor ? m_baseButton : m_detailButton;
    onSelectMode(initialBtn);
    
    m_disableTextDelegate = false;
    return true;
}

void CustomizeObjectLayer::keyBackClicked()
{
    onClose(nullptr);
}

void CustomizeObjectLayer::onBreakApart(cocos2d::CCObject* sender)
{
    if (m_targetObject)
    {
        onClose(nullptr);
        LevelEditorLayer::breakApartTextObject(GameManager::sharedState()->m_levelEditorLayer, m_targetObject);
    }
}

void CustomizeObjectLayer::onBrowse(cocos2d::CCObject* sender)
{
    GJColorSetupLayer* layer = GJColorSetupLayer::create(
        GameManager::sharedState()->m_levelEditorLayer->m_levelSettings);
    layer->m_delegate = this;
    layer->m_closeOnSelect = true;
    layer->show();
}

void CustomizeObjectLayer::onClear(cocos2d::CCObject* sender)
{
    if (m_targetObject)
    {
        m_disableTextDelegate = true;
        m_targetObject->updateTextObject("");
        m_textInput->setString("");
        m_disableTextDelegate = false;
    }
}

void CustomizeObjectLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_customColorInput)
    {
        m_customColorInput->setDelegate(nullptr);
        m_customColorInput->onClickTrackNode(false);
    }
    
    if (m_textInput)
    {
        m_textInput->onClickTrackNode(false);
        m_textInput->setDelegate(nullptr);
    }
    
    GameManager::sharedState()->m_currentColorChannel = m_customColorChannel;
    EditorUI::updateObjectInfoLabel(GameManager::sharedState()->m_levelEditorLayer->m_editorUI);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void CustomizeObjectLayer::onCopy(cocos2d::CCObject* sender)
{
    LevelEditorLayer::copyObjectState(
        GameManager::sharedState()->m_levelEditorLayer,
        m_targetObject);
}

void CustomizeObjectLayer::onEditColor(cocos2d::CCObject* sender)
{
    int channel = getActiveMode(true);
    ColorAction* action = GJEffectManager::getColorAction(
        GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager,
        channel);
    ColorSelectPopup* popup = ColorSelectPopup::create(action);
    popup->m_delegate = this;
    popup->show();
}

void CustomizeObjectLayer::onHSV(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    cocos2d::ccHSVValue hsv;
    std::string title;
    
    if (tag == 1)
    {
        title = "Base HSV";
        if (!m_hasBaseColor)
            title = "Detail HSV";
    }
    else
    {
        hsv = getHSV();
        title = "Detail HSV";
    }
    
    HSVWidgetPopup* popup = HSVWidgetPopup::create(hsv, this, title);
    popup->setTag(tag);
    popup->show();
}

void CustomizeObjectLayer::onLiveEdit(cocos2d::CCObject* sender)
{
    int mode = getActiveMode(true);
    onClose(nullptr);
    EditorUI::showLiveColorSelectForModeSpecial(
        GameManager::sharedState()->m_levelEditorLayer->m_editorUI,
        mode);
}

void CustomizeObjectLayer::onNextColorChannel(cocos2d::CCObject* sender)
{
    int channel = LevelEditorLayer::getNextColorChannel(
        GameManager::sharedState()->m_levelEditorLayer);
    m_customColorChannel = channel;
    m_disableTextDelegate = true;
    updateCustomColorLabels();
    m_disableTextDelegate = false;
    
    updateSelected(channel);
    ButtonSprite* btn = getButtonByTag(1008);
    highlightSelected(btn);
    updateColorSprite();
    updateChannelLabel(channel);
}

void CustomizeObjectLayer::onPaste(cocos2d::CCObject* sender)
{
    LevelEditorLayer::pasteColorState(
        GameManager::sharedState()->m_levelEditorLayer,
        m_targetObject,
        m_targetObjects);
    updateCurrentSelection();
    updateColorSprite();
    updateChannelLabel(getActiveMode(true));
    updateHSVButtons();
}

void CustomizeObjectLayer::onSelectColor(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    m_customColorChannel = tag;
    updateSelected(tag);
    ButtonSprite* sprite = (ButtonSprite*)sender->getParent();
    highlightSelected(sprite);
    updateColorSprite();
    updateChannelLabel(getActiveMode(true));
}

void CustomizeObjectLayer::onSelectMode(cocos2d::CCObject* sender)
{
    int mode = sender->getTag();
    m_selectedMode = mode;
    
    if (mode == 1)
        m_titleLabel->setString("Base Color");
    else if (mode == 2)
        m_titleLabel->setString("Detail Color");
    else
        m_titleLabel->setString("Insert Text");
    
    ButtonSprite* baseSprite = (ButtonSprite*)m_baseButton->getNormalImage();
    ButtonSprite::updateBGImage(baseSprite, (m_selectedMode == 1) ? "GJ_button_03.png" : "GJ_button_04.png");
    
    ButtonSprite* detailSprite = (ButtonSprite*)m_detailButton->getNormalImage();
    ButtonSprite::updateBGImage(detailSprite, (m_selectedMode == 2) ? "GJ_button_03.png" : "GJ_button_04.png");
    
    if (m_textButton)
    {
        ButtonSprite* textSprite = (ButtonSprite*)m_textButton->getNormalImage();
        ButtonSprite::updateBGImage(textSprite, (m_selectedMode == 3) ? "GJ_button_03.png" : "GJ_button_04.png");
    }
    
    toggleVisible();
    
    m_baseColorHSV->setVisible(m_selectedMode == 1);
    m_baseColorHSV->setEnabled(m_selectedMode == 1);
    m_detailColorHSV->setVisible(m_selectedMode == 2);
    m_detailColorHSV->setEnabled(m_selectedMode == 2);
    m_liveSelectButton->setVisible(m_selectedMode != 3);
    m_liveSelectButton->setEnabled(m_selectedMode != 3);
    
    updateHSVButtons();
    updateColorSprite();
    updateChannelLabel(getActiveMode(true));
}

void CustomizeObjectLayer::onSettings(cocos2d::CCObject* sender)
{
    CustomizeObjectSettingsPopup* popup = CustomizeObjectSettingsPopup::create(
        m_targetObject, m_targetObjects, this);
    popup->show();
}

void CustomizeObjectLayer::onUpdateCustomColor(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();
    m_disableTextDelegate = true;
    
    if (tag == 1)
        m_customColorChannel++;
    else
        m_customColorChannel--;
    
    if (m_customColorChannel < 10)
        m_customColorChannel = 10;
    if (m_customColorChannel > 999)
        m_customColorChannel = 999;
    
    updateCustomColorLabels();
    updateSelected(m_customColorChannel);
    m_disableTextDelegate = false;
    updateColorSprite();
    updateChannelLabel(m_customColorChannel);
}

void CustomizeObjectLayer::recreateLayer()
{
    if (m_targetObject) m_targetObject->retain();
    if (m_targetObjects) m_targetObjects->retain();
    
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    onClose(nullptr);
    
    CustomizeObjectLayer* layer = CustomizeObjectLayer::create(m_targetObject, m_targetObjects);
    layer->show();
    
    if (m_targetObject) m_targetObject->release();
    if (m_targetObjects) m_targetObjects->release();
}

void CustomizeObjectLayer::sliderChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();
    m_kerningAmount = kerningValueFromSliderValue(value);
    m_targetObject->updateTextKerning(m_kerningAmount);
    updateKerningLabel();
}

void CustomizeObjectLayer::textChanged(CCTextInputNode* node)
{
    if (m_disableTextDelegate)
        return;
    
    int tag = node->getTag();
    
    if (tag == 0) // Custom color input
    {
        const char* str = node->getString();
        int value = atoi(str);
        m_customColorChannel = value;
        ButtonSprite::setString(m_customColorButtonSprite, 
            cocos2d::CCString::createWithFormat("%i", m_customColorChannel)->getCString());
        updateSelected(m_customColorChannel);
        updateColorSprite();
        updateChannelLabel(m_customColorChannel);
    }
    else if (tag == 1) // Text input
    {
        const char* str = node->getString();
        m_targetObject->updateTextObject(str);
    }
}

void CustomizeObjectLayer::textInputClosed(CCTextInputNode* node)
{
    textChanged(node);
}

void CustomizeObjectLayer::textInputOpened(CCTextInputNode* node)
{
    if (node->getTag() == 1)
    {
        const char* str = node->getString();
        if (strcmp(str, "A") == 0)
        {
            onClear(nullptr);
            node->setString(" ");
        }
    }
}

void CustomizeObjectLayer::toggleVisible()
{
    m_showTextInput = (m_selectedMode == 3);
    
    for (unsigned int i = 0; i < m_colorTabNodes->count(); i++)
    {
        cocos2d::CCObject* obj = m_colorTabNodes->objectAtIndex(i);
        obj->setVisible(!m_showTextInput);
    }
    
    for (unsigned int i = 0; i < m_detailTabNodes->count(); i++)
    {
        cocos2d::CCObject* obj = m_detailTabNodes->objectAtIndex(i);
        obj->setVisible(!m_showTextInput);
    }
    
    if (m_customColorSelected && m_textTabNodes)
    {
        for (unsigned int i = 0; i < m_textTabNodes->count(); i++)
        {
            cocos2d::CCObject* obj = m_textTabNodes->objectAtIndex(i);
            obj->setVisible(m_showTextInput);
        }
        
        if (m_textInput)
        {
            m_textInput->setTouchEnabled(m_showTextInput);
            m_textInput->setVisible(m_showTextInput);
        }
    }
    
    updateCurrentSelection();
}

void CustomizeObjectLayer::updateChannelLabel(int channel)
{
    if (channel > 999)
    {
        m_selectedColorLabel->setString(GJSpecialColorSelect::textForColorIdx(channel));
    }
    else if (channel <= 0)
    {
        m_selectedColorLabel->setString(" ");
    }
    else
    {
        m_selectedColorLabel->setString(
            cocos2d::CCString::createWithFormat("%i", channel)->getCString());
    }
}

void CustomizeObjectLayer::updateColorSprite()
{
    bool visible = false;
    
    if (m_selectedMode != 3)
    {
        int channel = getActiveMode(true);
        if (channel > 0)
        {
            GameManager* gm = GameManager::sharedState();
            if (GJEffectManager::colorExists(gm->m_levelEditorLayer->m_levelSettings->m_effectManager, channel))
            {
                ColorAction* action = GJEffectManager::getColorAction(
                    gm->m_levelEditorLayer->m_levelSettings->m_effectManager, channel);
                ColorChannelSprite::updateValues(m_colorSprite, action);
                visible = true;
            }
        }
    }
    
    m_colorSpriteButton->setVisible(visible);
}

void CustomizeObjectLayer::updateCurrentSelection()
{
    int channel = getActiveMode(false);
    
    if (channel >= 10 && channel <= 999)
    {
        m_customColorChannel = channel;
        m_disableTextDelegate = true;
        updateCustomColorLabels();
        m_disableTextDelegate = false;
        channel = 1008;
    }
    
    ButtonSprite* btn = getButtonByTag(channel);
    highlightSelected(btn);
}

void CustomizeObjectLayer::updateCustomColorLabels()
{
    ButtonSprite::setString(m_customColorButtonSprite,
        cocos2d::CCString::createWithFormat("%i", m_customColorChannel)->getCString());
    m_customColorInput->setString(
        cocos2d::CCString::createWithFormat("%i", m_customColorChannel)->getCString());
}

void CustomizeObjectLayer::updateHSVButtons()
{
    m_selectedMode = 1;
    cocos2d::ccHSVValue baseHSV = getHSV();
    m_selectedMode = 2;
    cocos2d::ccHSVValue detailHSV = getHSV();
    
    bool baseDefault = (baseHSV.h == 0.0f && baseHSV.s == 0.0f && baseHSV.v == 0.0f && !baseHSV.absoluteSaturation && !baseHSV.absoluteBrightness);
    bool detailDefault = (detailHSV.h == 0.0f && detailHSV.s == 0.0f && detailHSV.v == 0.0f && !detailHSV.absoluteSaturation && !detailHSV.absoluteBrightness);
    
    cocos2d::CCNode* baseNode = m_baseColorHSV->getNormalImage();
    baseNode->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
            baseDefault ? "GJ_hsvBtn_001.png" : "GJ_hsv2Btn_001.png"));
    
    cocos2d::CCNode* detailNode = m_detailColorHSV->getNormalImage();
    detailNode->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
            detailDefault ? "GJ_hsvBtn_001.png" : "GJ_hsv2Btn_001.png"));
}

void CustomizeObjectLayer::updateKerningLabel()
{
    if (!m_kerningLabel)
        return;
    
    std::string text = "Kerning: " + GameToolbox::intToString(m_kerningAmount);
    m_kerningLabel->m_nExtraKerning = m_kerningAmount;
    m_kerningLabel->setString(text.c_str());
    
    if (m_textInput)
    {
        m_textInput->m_kerningAmount = m_kerningAmount;
        m_textInput->setString(m_textInput->getString());
    }
}

void CustomizeObjectLayer::updateSelected(int id)
{
    bool isDefault = (id == 0);
    
    if (m_targetObject)
    {
        GJSpriteColor* color = GameObject::getRelativeSpriteColor(m_targetObject, m_selectedMode);
        if (color)
        {
            if (isDefault)
            {
                int defaultId = color->m_defaultColorID;
                if (defaultId > 1100)
                    defaultId = 1101;
                else if (defaultId < 0)
                    defaultId = 0;
                color->m_defaultColorID = defaultId;
                color->m_colorID = 0;
            }
            else
            {
                int channel = id;
                if (channel > 1100)
                    channel = 1101;
                else if (channel < 0)
                    channel = 0;
                color->m_colorID = channel;
            }
            m_targetObject->m_updateParents = true;
        }
    }
    else if (m_targetObjects)
    {
        int channel = id;
        if (channel > 1100)
            channel = 1101;
        else if (channel < 0)
            channel = 0;
            
        for (unsigned int i = 0; i < m_targetObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)m_targetObjects->objectAtIndex(i);
            GJSpriteColor* color = GameObject::getRelativeSpriteColor(obj, m_selectedMode);
            if (color)
            {
                if (isDefault)
                {
                    int defaultId = color->m_defaultColorID;
                    if (defaultId > 1100)
                        defaultId = 1101;
                    else if (defaultId < 0)
                        defaultId = 0;
                    color->m_defaultColorID = defaultId;
                    color->m_colorID = 0;
                }
                else
                {
                    color->m_colorID = channel;
                }
                obj->m_updateParents = true;
            }
        }
    }
}

CustomizeObjectLayer::~CustomizeObjectLayer()
{
    CC_SAFE_RELEASE(m_targetObjects);
    CC_SAFE_RELEASE(m_colorTabNodes);
    CC_SAFE_RELEASE(m_unkArray);
    CC_SAFE_RELEASE(m_detailTabNodes);
    CC_SAFE_RELEASE(m_textTabNodes);
    CC_SAFE_RELEASE(m_colorButtons);
}