#include "SetupGradientPopup.h"
#include "SetupTriggerPopup.h"
#include "GradientTriggerObject.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCLabelBMFont.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupGradientPopup* SetupGradientPopup::create(GradientTriggerObject* object, cocos2d::CCArray* objects)
{
    SetupGradientPopup* instance = new SetupGradientPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupGradientPopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (!m_gameObjects || m_gameObjects->count() == 0)
        return;

    int blendingMode = -99999;
    int blendingLayer = -99999;

    for (unsigned int i = 0; i < m_gameObjects->count(); i++)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

        if (blendingMode == -99999)
            blendingMode = obj->m_blendingMode;
        else if (blendingMode != obj->m_blendingMode)
            blendingMode = -9991;

        if (blendingLayer == -99999)
            blendingLayer = obj->m_blendingLayer;
        else if (blendingLayer != obj->m_blendingLayer)
            blendingLayer = -9991;
    }

    if (blendingMode != -9991)
        m_blendingMode = blendingMode;
    if (blendingLayer != -9991)
        m_blendingLayer = blendingLayer;
}

bool SetupGradientPopup::init(GradientTriggerObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 420.0f, 290.0f, 1))
        return false;

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Setup Gradient Effect");
    addHelpButton(
        "Help",
        "Creates a <cg>gradient</c> with set colors in a given area.\n"
        "<cy>U, D, L, and R</c> must be set to a group at each side of the area.\n"
        "Use <cl>Vertex Mode</c> to set corners of the area instead.\n"
        "<cp>Blending</c> changes the gradient method.\n"
        "Use <cr>ID</c> to give a gradient a unique identifier that can be used to disable it with another of these triggers.",
        1.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupTriggerPopup::onClose));
    m_buttonMenu->addChild(okItem);

    cocos2d::CCPoint colorBase(winSize.width * 0.5f - 20.0f, winSize.height * 0.5f + 90.0f);

    createValueControlAdvanced(203, " ", cocos2d::CCPoint(0.7f, 0.0f), 0.0f,
        &(colorBase + cocos2d::CCPoint(-135.0f, 0.0f)), Float, 2, 5, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);
    createValueControlAdvanced(204, " ", cocos2d::CCPoint(0.7f, 0.0f), 0.0f,
        &(colorBase + cocos2d::CCPoint(-45.0f, 0.0f)), Float, 2, 5, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);
    createValueControlAdvanced(205, " ", cocos2d::CCPoint(0.7f, 0.0f), 0.0f,
        &(colorBase + cocos2d::CCPoint(45.0f, 0.0f)), Float, 2, 5, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);
    createValueControlAdvanced(206, " ", cocos2d::CCPoint(0.7f, 0.0f), 0.0f,
        &(colorBase + cocos2d::CCPoint(135.0f, 0.0f)), Float, 2, 5, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    m_gradientLabels = cocos2d::CCArray::create();
    m_gradientLabels->retain();

    for (int i = 0; i < 4; i++)
    {
        CCLabelBMFont* label = CCLabelBMFont::create(" ", "goldFont.fnt");
        label->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
        m_mainLayer->addChild(label);
        label->setScale(0.5f);
        label->setPosition(colorBase + cocos2d::CCPoint((i - 1.5f) * 90.0f, 0.0f));
        m_gradientLabels->addObject(label);
    }

    updateGradientLabels(false);

    cocos2d::CCPoint vertexPos(m_width * 0.5f + winSize.width * 0.5f - 72.0f,
                               m_height * 0.5f + winSize.height * 0.5f - 20.0f);
    createToggleValueControl(207, "Vertex\nMode", cocos2d::CCPoint(1.0f, 0.0f), &vertexPos, 0, 0, 0.0f);

    cocos2d::CCPoint disablePos(winSize.width * 0.5f + 145.0f, winSize.height * 0.5f + 40.0f);
    createToggleValueControl(208, "Disable", cocos2d::CCPoint(0.8f, 0.0f), &disablePos, 0, 0, 0.0f);

    cocos2d::CCPoint disableAllPos = disablePos + cocos2d::CCPoint(0.0f, -25.0f);
    createToggleValueControl(508, "Disable\nAll", cocos2d::CCPoint(0.8f, 0.0f), &disableAllPos, 0, 0, 0.0f);

    cocos2d::CCPoint previewPos(m_width * 0.5f + winSize.width * 0.5f - 72.0f,
                                40.0f - (m_height * 0.5f - winSize.height * 0.5f));
    createValueControlAdvanced(456, "Preview\nOpacity", cocos2d::CCPoint(0.65f, 0.0f), 0.0f,
        &previewPos, (InputValueType)0, 1, 5, 0.0f, 0.0f, 0, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint idBase(winSize.width * 0.5f - 20.0f, winSize.height * 0.5f + 60.0f);
    cocos2d::CCPoint idPos = idBase + cocos2d::CCPoint(75.0f, -25.0f);
    createValueControlAdvanced(209, "ID", cocos2d::CCPoint(0.7f, 0.0f), 0.0f,
        &idPos, Float, 2, 3, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint plusPos = idBase + cocos2d::CCPoint(95.0f, 0.0f);
    createPlusButton(209, cocos2d::CCPoint(0.8f, 0.0f), 0.0f, "", &plusPos, 0);

    CCLabelBMFont* blendingTitle = CCLabelBMFont::create("Blending", "bigFont.fnt");
    blendingTitle->setPosition(idBase + cocos2d::CCPoint(-85.0f, 0.0f));
    m_mainLayer->addChild(blendingTitle);
    blendingTitle->setScale(0.6f);

    m_blendingLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_blendingLabel);
    m_blendingLabel->setPosition(blendingTitle->getPosition() + cocos2d::CCPoint(0.0f, -25.0f));
    m_blendingLabel->setScale(0.5f);
    updateBlendingLabel();

    cocos2d::CCSprite* leftArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    leftArrow->setFlipX(true);
    leftArrow->setScale(0.6f);
    CCMenuItemSpriteExtra* leftItem = CCMenuItemSpriteExtra::create(
        leftArrow, nullptr, menu_selector(SetupGradientPopup::onBlending));
    m_buttonMenu->addChild(leftItem);
    leftItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_blendingLabel->getPosition() + cocos2d::CCPoint(75.0f, 0.0f)));
    leftItem->setSizeMult(1.4f);
    leftItem->setTag(1);

    cocos2d::CCSprite* rightArrow = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrow->setScale(0.6f);
    CCMenuItemSpriteExtra* rightItem = CCMenuItemSpriteExtra::create(
        rightArrow, nullptr, menu_selector(SetupGradientPopup::onBlending));
    m_buttonMenu->addChild(rightItem);
    rightItem->setPosition(m_buttonMenu->convertToNodeSpace(
        m_blendingLabel->getPosition() + cocos2d::CCPoint(-75.0f, 0.0f)));
    rightItem->setSizeMult(1.4f);
    rightItem->setTag(0);

    createMultiTriggerItemsCorner();

    m_layerButtons = cocos2d::CCArray::create();
    m_layerButtons->retain();

    cocos2d::CCArray* topRow = cocos2d::CCArray::create();
    cocos2d::CCArray* bottomRow = cocos2d::CCArray::create();

    for (int i = 1; i < 16; i++)
    {
        const char* layerName = SetupShaderEffectPopup::zLayerToString(i);
        ButtonSprite* layerBtn = ButtonSprite::create(
            layerName, 0, 0, 0.3f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f);

        m_layerButtons->addObject(layerBtn);

        CCMenuItemSpriteExtra* layerItem = CCMenuItemSpriteExtra::create(
            layerBtn, nullptr, menu_selector(SetupGradientPopup::onZLayer));
        layerItem->setTag(i);
        layerBtn->setTag(i);
        m_buttonMenu->addChild(layerItem);

        if (i <= 7)
            bottomRow->addObject(layerItem);
        else
            topRow->addObject(layerItem);
    }

    updateZLayerButtons();

    cocos2d::CCPoint row1Pos(winSize.width * 0.5f - 20.0f, winSize.height * 0.5f - 10.0f);
    cocos2d::CCPoint row1Local = m_buttonMenu->convertToNodeSpace(row1Pos);
    GameToolbox::alignItemsHorisontally(bottomRow, 8.0f, row1Local, false);

    cocos2d::CCPoint row2Pos(winSize.width * 0.5f - 20.0f, winSize.height * 0.5f - 50.0f);
    cocos2d::CCPoint row2Local = m_buttonMenu->convertToNodeSpace(row2Pos);
    GameToolbox::alignItemsHorisontally(topRow, 8.0f, row2Local, false);

    updateDefaultTriggerValues();

    m_disableTextDelegate = false;

    return true;
}

void SetupGradientPopup::onBlending(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag == 1)
        m_blendingMode += 1;
    else
        m_blendingMode -= 1;

    if (m_blendingMode < 0)
        m_blendingMode = 0;
    if (m_blendingMode > 3)
        m_blendingMode = 3;

    updateBlending();
    updateBlendingLabel();
}

void SetupGradientPopup::onPlusButton(cocos2d::CCObject* sender)
{
    if (sender->getTag() == 209)
    {
        std::unordered_set<int> usedIDs;
        int nextID = GameManager::sharedState()->m_levelEditorLayer->getNextFreeGradientID(&usedIDs);
        updateValue(209, (float)nextID);
    }
}

void SetupGradientPopup::onZLayer(cocos2d::CCObject* sender)
{
    m_blendingLayer = sender->getTag();

    if (m_gameObject)
    {
        m_gameObject->m_blendingLayer = m_blendingLayer;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_blendingLayer = m_blendingLayer;
        }
    }

    updateZLayerButtons();
}

void SetupGradientPopup::sliderChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();
    sender->getTag();
}

void SetupGradientPopup::updateBlending()
{
    if (m_gameObject)
    {
        m_gameObject->m_blendingMode = m_blendingMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_blendingMode = m_blendingMode;
        }
    }
}

void SetupGradientPopup::updateBlendingLabel()
{
    const char* text;
    switch (m_blendingMode)
    {
        case -99999: text = "Mixed"; break;
        case 1: text = "Additive"; break;
        case 2: text = "Multiply"; break;
        case 3: text = "Invert"; break;
        default: text = "Normal"; break;
    }
    m_blendingLabel->setString(text);
    m_blendingLabel->limitLabelWidth(150.0f, 0.7f, 0.0f);
}

void SetupGradientPopup::updateGradientLabels(bool vertex)
{
    for (unsigned int i = 0; i < m_gradientLabels->count(); i++)
    {
        CCLabelBMFont* label = (CCLabelBMFont*)m_gradientLabels->objectAtIndex(i);

        switch (i)
        {
            case 0: label->setString(vertex ? "BL:" : "U:"); break;
            case 1: label->setString(vertex ? "BR:" : "D:"); break;
            case 2: label->setString(vertex ? "TL:" : "L:"); break;
            case 3: label->setString(vertex ? "TR:" : "R:"); break;
        }
    }
}

void SetupGradientPopup::updateToggleItem(int value, bool toggled)
{
    SetupTriggerPopup::updateToggleItem(value, toggled);
    if (value == 207)
        updateGradientLabels(toggled);
}

void SetupGradientPopup::updateZLayerButtons()
{
    for (unsigned int i = 0; i < m_layerButtons->count(); i++)
    {
        ButtonSprite* button = (ButtonSprite*)m_layerButtons->objectAtIndex(i);
        if (button->getTag() == m_blendingLayer)
            button->updateBGImage("GJ_button_02.png");
        else
            button->updateBGImage("GJ_button_04.png");
    }
}

void SetupGradientPopup::valueDidChange(int tag, float value)
{
    if (tag == 209)
    {
        if (value < 0.0f)
            updateValue(209, 0.0f);
        else if (value > 999.0f)
            updateValue(209, 999.0f);
        else
            updateEditorLabel();
    }
}

SetupGradientPopup::~SetupGradientPopup()
{
}