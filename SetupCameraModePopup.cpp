#include "SetupCameraModePopup.h"
#include "SetupTriggerPopup.h"
#include "EffectGameObject.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "InfoAlertButton.h"
#include "Slider.h"
#include "SliderThumb.h"

SetupCameraModePopup* SetupCameraModePopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupCameraModePopup* instance = new SetupCameraModePopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupCameraModePopup::determineStartValues()
{
    SetupTriggerPopup::determineStartValues();

    if (m_gameObject)
    {
        m_gameObjects = cocos2d::CCArray::create();
        m_gameObjects->addObject(m_gameObject);
    }

    if (m_gameObjects && m_gameObjects->count() > 0)
    {
        m_freeMode = true;
        m_cameraSettingsEnabled = true;
        m_cameraModeObjects = true;

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

            if (obj->m_objectID != 2925)
                m_cameraModeObjects = false;
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

bool SetupCameraModePopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 400.0f, 280.0f, 1))
        return false;

    m_freeModeObjects = cocos2d::CCArray::create();
    m_freeModeObjects->retain();

    m_cameraSettingsObjects = cocos2d::CCArray::create();
    m_cameraSettingsObjects->retain();

    determineStartValues();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(titleLabel);
    titleLabel->setScale(0.7f);
    titleLabel->setPosition(winSize.width * 0.5f, m_height * 0.5f + winSize.height * 0.5f - 18.0f);

    ButtonSprite* okButton = ButtonSprite::create("OK", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* okItem = CCMenuItemSpriteExtra::create(
        okButton, nullptr, menu_selector(SetupCameraModePopup::onClose));
    m_buttonMenu->addChild(okItem);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "Toggles <cg>Free Mode</c>. This removes the borders for certain game modes.\n"
        "<cy>Edit Camera Settings</c> allows you to edit camera movement in Free Mode. <cl>Padding</c> is how close the pla"
        "yer has to be to the top or bottom for the camera to follow.\n"
        "<cp>Disable GridSnap</c> removes default snapping to nearest grid space for the camera center.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(18.0f - (m_width * 0.5f - winSize.width * 0.5f),
                         m_height * 0.5f + winSize.height * 0.5f - 18.0f)));

    cocos2d::CCPoint freeModePos(winSize.width * 0.5f - 120.0f, winSize.height * 0.5f + 70.0f);
    GameToolbox::createToggleButton(
        "Free Mode", menu_selector(SetupCameraModePopup::onUnboundMode), 0, m_freeMode,
        m_buttonMenu, freeModePos, this, m_mainLayer, 0.8f, 0.45f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    cocos2d::CCPoint editPos = freeModePos + cocos2d::CCPoint(150.0f, 0.0f);
    GameToolbox::createToggleButton(
        "Edit Camera\nSettings", menu_selector(SetupCameraModePopup::onEditCameraSettings), 0, m_cameraSettingsEnabled,
        m_buttonMenu, editPos, this, m_mainLayer, 0.8f, 0.45f, 0.0f,
        cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, m_freeModeObjects);

    cocos2d::CCPoint gridSnapPos(m_width * 0.5f + winSize.width * 0.5f - 90.0f,
                                 m_height * 0.5f + winSize.height * 0.5f - 20.0f);
    createToggleValueControl(370, "Disable\nGridSnap", cocos2d::CCPoint(1.0f, 0.0f), &gridSnapPos, 0, 0, 0.0f);

    m_cameraEasingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_cameraEasingLabel);
    m_cameraEasingLabel->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 20.0f);
    m_cameraEasingLabel->setScale(0.8f);
    m_cameraSettingsObjects->addObject(m_cameraEasingLabel);

    m_cameraEasingSlider = Slider::create(this, menu_selector(SetupCameraModePopup::sliderChanged));
    m_mainLayer->addChild(m_cameraEasingSlider);
    m_cameraEasingSlider->setPosition(m_cameraEasingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_cameraEasingSlider->getThumb()->setTag(0);
    m_cameraSettingsObjects->addObject(m_cameraEasingSlider);

    updateCameraEasingLabel();
    m_cameraEasingSlider->setValue((m_cameraEasing - 1.0f) / 39.0f);

    m_cameraPaddingLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(m_cameraPaddingLabel);
    m_cameraPaddingLabel->setPosition(m_cameraEasingLabel->getPosition() + cocos2d::CCPoint(0.0f, -60.0f));
    m_cameraPaddingLabel->setScale(0.8f);
    m_cameraSettingsObjects->addObject(m_cameraPaddingLabel);

    m_cameraPaddingSlider = Slider::create(this, menu_selector(SetupCameraModePopup::sliderChanged));
    m_mainLayer->addChild(m_cameraPaddingSlider);
    m_cameraPaddingSlider->setPosition(m_cameraPaddingLabel->getPosition() + cocos2d::CCPoint(0.0f, -30.0f));
    m_cameraPaddingSlider->getThumb()->setTag(1);
    m_cameraSettingsObjects->addObject(m_cameraPaddingSlider);

    updateCameraPaddingLabel();
    m_cameraPaddingSlider->setValue(m_cameraPadding);

    updateItemVisibility();
    updateDefaultTriggerValues();

    if (m_cameraModeObjects)
        createMultiTriggerItemsDefaultVertical();

    m_disableTextDelegate = false;

    return true;
}

void SetupCameraModePopup::onEditCameraSettings(cocos2d::CCObject* sender)
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

void SetupCameraModePopup::onUnboundMode(cocos2d::CCObject* sender)
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

void SetupCameraModePopup::sliderChanged(cocos2d::CCObject* sender)
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

void SetupCameraModePopup::updateCameraEasing()
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

void SetupCameraModePopup::updateCameraEasingLabel()
{
    m_cameraEasingLabel->setString(
        cocos2d::CCString::createWithFormat("Easing: %i", m_cameraEasing)->getCString());
}

void SetupCameraModePopup::updateCameraPadding()
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

void SetupCameraModePopup::updateCameraPaddingLabel()
{
    m_cameraPaddingLabel->setString(
        cocos2d::CCString::createWithFormat("Padding: %.02f", m_cameraPadding)->getCString());
}

void SetupCameraModePopup::updateItemVisibility()
{
    for (unsigned int i = 0; i < m_freeModeObjects->count(); i++)
    {
        cocos2d::CCObject* obj = m_freeModeObjects->objectAtIndex(i);
        ((cocos2d::CCNode*)obj)->setVisible(m_freeMode);
    }

    for (unsigned int i = 0; i < m_cameraSettingsObjects->count(); i++)
    {
        cocos2d::CCObject* obj = m_cameraSettingsObjects->objectAtIndex(i);
        ((cocos2d::CCNode*)obj)->setVisible(m_cameraSettingsEnabled);
    }
}

SetupCameraModePopup::~SetupCameraModePopup()
{
}