#include "SetupPortalPopup.h"
#include "EffectGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCLabelBMFont.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupPortalPopup* SetupPortalPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupPortalPopup* instance = new SetupPortalPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupPortalPopup::determineStartValues()
{
    m_cameraEasing = 0;
    m_cameraPadding = 0.0f;

    if (m_gameObject)
    {
        EffectGameObject* obj = (EffectGameObject*)m_gameObject;
        m_cameraEasing = (int)obj->m_cameraEasingValue;
        m_cameraPadding = obj->m_cameraPaddingValue;
        m_freeMode = obj->m_cameraIsFreeMode;
        m_cameraSettingsEnabled = obj->m_cameraEditCameraSettings;
    }
    else if (m_gameObjects && m_gameObjects->count() > 0)
    {
        m_freeMode = true;
        m_cameraSettingsEnabled = true;
        float easingValue = -99999.0f;
        float paddingValue = -99999.0f;
        int easingConflict = 0;
        int paddingConflict = 0;

        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);

            if (!obj->m_cameraIsFreeMode)
                m_freeMode = false;
            if (!obj->m_cameraEditCameraSettings)
                m_cameraSettingsEnabled = false;

            if (!easingConflict)
            {
                if (easingValue == -99999.0f)
                    easingValue = obj->m_cameraEasingValue;
                else if (easingValue != obj->m_cameraEasingValue)
                {
                    easingValue = -99999.0f;
                    easingConflict = 1;
                }
            }

            if (!paddingConflict)
            {
                if (paddingValue == -99999.0f)
                    paddingValue = obj->m_cameraPaddingValue;
                else if (paddingValue != obj->m_cameraPaddingValue)
                {
                    paddingValue = -99999.0f;
                    paddingConflict = 1;
                }
            }
        }

        if (easingValue != -99999.0f)
            m_cameraEasing = (int)easingValue;
        if (paddingValue != -99999.0f)
            m_cameraPadding = paddingValue;
    }

    if (!m_cameraSettingsEnabled)
    {
        m_cameraEasing = 10;
        m_cameraPadding = 0.5f;
    }
}

bool SetupPortalPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!FLAlertLayer::init(200))
        return false;

    if (object)
    {
        m_gameObjects = cocos2d::CCArray::createWithObject(object);
        m_gameObjects->retain();
    }
    else
    {
        m_gameObject = (EffectGameObject*)objects;
        m_gameObject->retain();
    }

    m_noElasticity = true;
    m_disableDelegate = true;

    m_freeModeObjects = cocos2d::CCArray::create();
    m_freeModeObjects->retain();

    m_cameraSettingsObjects = cocos2d::CCArray::create();
    m_cameraSettingsObjects->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(430.0f, 310.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Portal", "bigFont.fnt");
    titleLabel->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 155.0f - 15.0f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->setPosition(winSize.width * 0.5f, winSize.height * 0.5f - 155.0f + 25.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupPortalPopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create("Portal Help", "No time for help.", 1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 215.0f - 18.0f, winSize.height * 0.5f + 155.0f - 18.0f)));

    determineStartValues();

    GameToolbox::createToggleButton(
        "Free Mode", menu_selector(SetupPortalPopup::onUnboundMode), 0, m_freeMode,
        m_buttonMenu, cocos2d::CCPoint(winSize.width * 0.5f - 215.0f + 20.0f, winSize.height * 0.5f + 155.0f - 20.0f),
        this, m_mainLayer, 0.8f, 0.45f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    GameToolbox::createToggleButton(
        "Edit Camera\nSettings", menu_selector(SetupPortalPopup::onEditCameraSettings), 0, m_cameraSettingsEnabled,
        m_buttonMenu, cocos2d::CCPoint(winSize.width * 0.5f - 215.0f + 20.0f, winSize.height * 0.5f + 155.0f - 60.0f),
        this, m_mainLayer, 0.8f, 0.45f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, m_freeModeObjects);

    m_cameraEasingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_cameraEasingLabel);
    m_cameraEasingLabel->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);
    m_cameraEasingLabel->setScale(0.8f);
    m_cameraSettingsObjects->addObject(m_cameraEasingLabel);

    m_cameraEasingSlider = Slider::create(this, menu_selector(SetupPortalPopup::sliderChanged));
    m_mainLayer->addChild(m_cameraEasingSlider);
    m_cameraEasingSlider->setPosition(m_cameraEasingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_cameraEasingSlider->getThumb()->setTag(0);
    m_cameraSettingsObjects->addObject(m_cameraEasingSlider);

    updateCameraEasingLabel();
    m_cameraEasingSlider->setValue((m_cameraEasing - 1.0f) / 39.0f);

    m_cameraPaddingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_cameraPaddingLabel);
    m_cameraPaddingLabel->setPosition(m_cameraEasingLabel->getPosition() + cocos2d::CCPoint(0.0f, -50.0f));
    m_cameraPaddingLabel->setScale(0.8f);
    m_cameraSettingsObjects->addObject(m_cameraPaddingLabel);

    m_cameraPaddingSlider = Slider::create(this, menu_selector(SetupPortalPopup::sliderChanged));
    m_mainLayer->addChild(m_cameraPaddingSlider);
    m_cameraPaddingSlider->setPosition(m_cameraPaddingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_cameraPaddingSlider->getThumb()->setTag(1);
    m_cameraSettingsObjects->addObject(m_cameraPaddingSlider);

    updateCameraPaddingLabel();
    m_cameraPaddingSlider->setValue(m_cameraPadding);

    updateItemVisibility();

    m_disableDelegate = false;

    return true;
}

void SetupPortalPopup::keyBackClicked()
{
    onClose(nullptr);
}

void SetupPortalPopup::onClose(cocos2d::CCObject* sender)
{
    LevelEditorLayer* editorLayer = GameManager::sharedState()->m_levelEditorLayer;
    editorLayer->resetEffectTriggerOptim((GameObject*)m_gameObjects, m_gameObject);
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetupPortalPopup::onEditCameraSettings(cocos2d::CCObject* sender)
{
    m_cameraSettingsEnabled = !m_cameraSettingsEnabled;

    if (m_gameObject)
    {
        m_gameObject->m_cameraEditCameraSettings = m_cameraSettingsEnabled;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_cameraEditCameraSettings = m_cameraSettingsEnabled;
        }
    }

    if (m_cameraSettingsEnabled)
    {
        updateCameraEasing();
        updateCameraPadding();
    }

    updateItemVisibility();
}

void SetupPortalPopup::onUnboundMode(cocos2d::CCObject* sender)
{
    m_freeMode = !m_freeMode;

    if (m_gameObject)
    {
        m_gameObject->m_cameraIsFreeMode = m_freeMode;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_cameraIsFreeMode = m_freeMode;
        }
    }

    updateItemVisibility();
}

void SetupPortalPopup::sliderChanged(cocos2d::CCObject* sender)
{
    float value = ((SliderThumb*)sender)->getValue();

    if (sender->getTag() == 0)
    {
        m_cameraEasing = (int)(roundf(value * 39.0f) + 1.0f);
        updateCameraEasing();
        updateCameraEasingLabel();
    }
    else if (sender->getTag() == 1)
    {
        m_cameraPadding = value;
        updateCameraPadding();
        updateCameraPaddingLabel();
    }
}

void SetupPortalPopup::updateCameraEasing()
{
    if (m_gameObject)
    {
        m_gameObject->m_cameraEasingValue = (float)m_cameraEasing;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_cameraEasingValue = (float)m_cameraEasing;
        }
    }
}

void SetupPortalPopup::updateCameraEasingLabel()
{
    m_cameraEasingLabel->setString(
        cocos2d::CCString::createWithFormat("Easing: %i", m_cameraEasing)->getCString());
}

void SetupPortalPopup::updateCameraPadding()
{
    if (m_gameObject)
    {
        m_gameObject->m_cameraPaddingValue = m_cameraPadding;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_cameraPaddingValue = m_cameraPadding;
        }
    }
}

void SetupPortalPopup::updateCameraPaddingLabel()
{
    m_cameraPaddingLabel->setString(
        cocos2d::CCString::createWithFormat("Padding: %.01f", m_cameraPadding)->getCString());
}

void SetupPortalPopup::updateItemVisibility()
{
    for (unsigned int i = 0; i < m_freeModeObjects->count(); i++)
    {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_freeModeObjects->objectAtIndex(i);
        node->setVisible(m_freeMode);
    }

    for (unsigned int i = 0; i < m_cameraSettingsObjects->count(); i++)
    {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_cameraSettingsObjects->objectAtIndex(i);
        node->setVisible(m_cameraSettingsEnabled);
    }
}

SetupPortalPopup::~SetupPortalPopup()
{
}