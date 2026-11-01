#include "SetupShaderEffectPopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"
#include "GameToolbox.h"

SetupShaderEffectPopup* SetupShaderEffectPopup::create(EffectGameObject* object, cocos2d::CCArray* objects, int id)
{
    SetupShaderEffectPopup* instance = new SetupShaderEffectPopup();
    if (instance->init(object, objects, id))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupShaderEffectPopup::onResetColors(cocos2d::CCObject* sender)
{
    updateValue(176, 1.0f);
    updateValue(191, 1.0f);
    updateValue(175, 1.0f);
    updateValue(179, 0.0f);
    updateValue(180, 0.0f);
    updateValue(189, 0.0f);
}

void SetupShaderEffectPopup::onZLayer(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag > m_zLayerMax)
    {
        m_zLayerMax = tag;
        m_changeMin = true;
        valueChanged(197, (float)tag);
        updateZLayerButtons();
        return;
    }

    if (tag >= m_zLayerMin)
    {
        if (tag == m_zLayerMin || tag == m_zLayerMax)
        {
            m_zLayerMin = tag;
            m_zLayerMax = tag;
            m_changeMin = false;
            valueChanged(197, (float)tag);
            valueChanged(196, (float)m_zLayerMin);
            updateZLayerButtons();
            return;
        }

        if (m_changeMin)
        {
            m_zLayerMin = tag;
            m_changeMin = false;
            valueChanged(196, (float)tag);
            updateZLayerButtons();
            return;
        }

        m_zLayerMax = tag;
        m_changeMin = true;
        valueChanged(197, (float)tag);
        updateZLayerButtons();
        return;
    }

    m_zLayerMin = tag;
    m_changeMin = false;
    valueChanged(196, (float)tag);
    updateZLayerButtons();
}

bool SetupShaderEffectPopup::init(EffectGameObject* object, cocos2d::CCArray* objects, int id)
{
    if (!SetupTriggerPopup::init(object, objects, 0, 440.0f, 310.0f, 1))
        return false;

    m_objectID = id;

    preSetup();

    addCloseButton("OK");
    createMultiTriggerItemsDefault();
    createPageButtons(m_width * 0.5f + 15.0f, 1);

    switch (m_objectID)
    {
        case 2903: setupBulge(); break;
        case 2904: setupChromatic(); break;
        case 2905: setupChromaticGlitch(); break;
        case 2907: setupColorChange(); break;
        case 2909: setupGlitch(); break;
        case 2910: setupGrayscale(); break;
        case 2911: setupHueShift(); break;
        case 2912: setupInvertColor(); break;
        case 2913: setupLensCircle(); break;
        case 2914: setupMotionBlur(); break;
        case 2915: setupPinch(); break;
        case 2916: setupPixelate(); break;
        case 2917: setupRadialBlur(); break;
        case 2919: setupSepia(); break;
        case 2920: setupShaderTrigger(); break;
        case 2921: setupShockLine(); break;
        case 2922: setupShockWave(); break;
        case 2924: setupSplitScreen(); break;
        default: break;
    }

    goToPage(0, false);
    postSetup();

    return true;
}

void SetupShaderEffectPopup::toggleGroup(int group, bool visible)
{
    SetupTriggerPopup::toggleGroup(group, visible);

    float disableAll = getValue(513);
    toggleDisableButtons(disableAll != 0.0f);
}

void SetupShaderEffectPopup::updateDefaultTriggerValues()
{
    SetupTriggerPopup::updateDefaultTriggerValues();

    CCTextInputNode* idInput = (CCTextInputNode*)m_inputNodes->objectForKey("id");
    if (!idInput)
        return;

    if (getValue(138) != 0.0f)
    {
        idInput->setString("P1");
    }
    else if (getValue(200) != 0.0f)
    {
        idInput->setString("P2");
    }
    else if (getValue(201) != 0.0f)
    {
        idInput->setString("C");
    }
}

void SetupShaderEffectPopup::updateZLayerButtons()
{
    for (unsigned int i = 0; i < m_zLayerSprites->count(); i++)
    {
        ButtonSprite* button = (ButtonSprite*)m_zLayerSprites->objectAtIndex(i);
        int tag = button->getTag();

        if (tag == m_zLayerMin || tag == m_zLayerMax)
            button->updateBGImage("GJ_button_02.png");
        else if (tag > m_zLayerMin && tag < m_zLayerMax)
            button->updateBGImage("GJ_button_01.png");
        else
            button->updateBGImage("GJ_button_04.png");
    }
}

void SetupShaderEffectPopup::valueDidChange(int tag, float value)
{
    if (tag == 513)
    {
        bool enabled = value != 0.0f;
        toggleDisableButtons(enabled);
        if (!enabled)
            resetDisabledValues();
        toggleGroup(103, enabled);
        return;
    }

    if (m_objectID == 2905 || m_objectID == 2914 ||
        (m_objectID >= 2916 && m_objectID <= 2917))
    {
        if (tag == 188)
        {
            bool useTarget = value != 0.0f;
            toggleGroup(100, useTarget);
            toggleGroup(101, !useTarget);
            return;
        }
        if (tag == 184)
        {
            toggleGroup(102, value != 0.0f);
            return;
        }
    }

    if ((m_objectID == 2905 || (m_objectID >= 2913 && m_objectID <= 2917)) &&
        (tag == 200 || tag == 201))
    {
        valueChanged(51, 0.0f);
        std::string text = "0";

        if (value != 0.0f)
        {
            if (tag == 138)
            {
                text = "P1";
            }
            else
            {
                updateValue(138, 0.0f);
                if (tag == 200)
                {
                    text = "P2";
                    updateValue(201, 0.0f);
                }
                else
                {
                    updateValue(200, 0.0f);
                    if (tag == 201)
                        text = "C";
                    else
                        updateValue(201, 0.0f);
                }
            }
        }

        CCTextInputNode* idInput = (CCTextInputNode*)m_inputNodes->objectForKey("id");
        if (idInput)
            idInput->setString(text);
    }
}

std::string SetupShaderEffectPopup::zLayerToString(int zLayer)
{
    switch (zLayer)
    {
        case 1: return "BG";
        case 2: return "MG";
        case 3: return "B5";
        case 4: return "B4";
        case 5: return "B3";
        case 6: return "B2";
        case 7: return "B1";
        case 8: return "P";
        case 9: return "T1";
        case 10: return "T2";
        case 11: return "T3";
        case 12: return "T4";
        case 13: return "G";
        case 14: return "UI";
        case 15: return "Max";
        default: return "";
    }
}

SetupShaderEffectPopup::~SetupShaderEffectPopup()
{
}