#include "Popups/Setup/SetupSFXPopup.h"
#include "Popups/Setup/SetupAudioTriggerPopup.h"
#include "Objects/SFXTriggerGameObject.h"
#include "UI/CustomSFXWidget.h"
#include "Audio/SFXBrowser.h"
#include "Models/SFXInfoObject.h"
#include "Managers/MusicDownloadManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Managers/GameManager.h"
#include "Layers/LevelEditorLayer.h"
#include "Popups/Setup/SetupReverbPopup.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

SetupSFXPopup* SetupSFXPopup::create(SFXTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupSFXPopup* instance = new SetupSFXPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupSFXPopup::createSFXWidget()
{
    if (m_sfxWidget)
    {
        m_sfxWidget->removeMeAndCleanup();
        m_sfxWidget = nullptr;
    }

    if (m_sfxID <= 0)
        return;

    MusicDownloadManager* manager = MusicDownloadManager::sharedState();
    SFXInfoObject* sfxObject = manager->getSFXObject(m_sfxID);

    m_sfxLength = 0;

    if (!sfxObject || sfxObject->m_folder)
        return;

    m_sfxWidget = CustomSFXWidget::create(sfxObject, this, 0, 1, 1, 0, 0);
    m_mainLayer->addChild(m_sfxWidget, 2);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_sfxWidget->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 70.0f));

    std::string path = MusicDownloadManager::sharedState()->pathForSFX(m_sfxID);
    FMODAudioEngine::sharedEngine()->preloadEffect(path);

    m_sfxLength = FMODAudioEngine::sharedEngine()->lengthForSound(path);

    if (m_sfxLength)
    {
        setMaxSliderValue((float)m_sfxLength, 408);
        setMaxSliderValue((float)m_sfxLength, 410);
        updateSlider(408);
        updateSlider(410);
    }

    updateLength();

    if (m_sfxWidget)
        m_sfxWidget->setVisible(m_page <= 1);
}

int SetupSFXPopup::getActiveSFXID()
{
    return m_sfxID;
}

bool SetupSFXPopup::init(SFXTriggerGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupAudioTriggerPopup::init(object, objects, 380.0f, 280.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("SFX Trigger");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Lets you start a specified <cg>sound effect</c> (browser top right).\n"
        "<cy>Unique ID</c> is used to refer to a specific sound effect in the level while <cl>SFXGroup</c> refers to multip"
        "le.\n"
        "<cp>Start, End, FadeIn, and FadeOut</c> are in milliseconds.\n"
        "<cr>Ignore Volume Test</c> does not ignore sound effects regardless of whether any volume setting is 0.\n"
        "Page 4 lets you change the volume depending on proximity.",
        1.0f);

    createMultiTriggerItemsDefault();

    m_disableSliderDelegate = true;

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint rowBase(winSize.width * 0.5f, winSize.height * 0.5f + 10.0f);

    createValueControlAdvanced(
        404, "Speed:", cocos2d::CCPoint(0.65f, -12.0f), 12.0f,
        &(rowBase + cocos2d::CCPoint(-91.0f, 0.0f)), (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    createValueControlAdvanced(
        405, "Pitch:", cocos2d::CCPoint(0.65f, -12.0f), 12.0f,
        &(rowBase + cocos2d::CCPoint(91.0f, 0.0f)), (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    createValueControlAdvanced(
        406, "Volume:", cocos2d::CCPoint(0.65f, 0.0f), 2.0f,
        &(rowBase + cocos2d::CCPoint(-91.0f, -39.0f)), (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 2);

    toggleLimitValue(404, true);
    toggleLimitValue(405, true);
    toggleLimitValue(406, true);

    createToggleValueControl(407, "Reverb", cocos2d::CCPoint(1.0f, 0.0f),
        &(rowBase + cocos2d::CCPoint(21.0f, -39.0f)), 0, 0, 0.0f);

    createToggleValueControl(412, "FFT", cocos2d::CCPoint(1.0f, 0.0f),
        &(rowBase + cocos2d::CCPoint(127.0f, -39.0f)), 0, 0, 0.0f);

    createToggleValueControl(413, "Loop", cocos2d::CCPoint(1.0f, 0.0f),
        &(rowBase + cocos2d::CCPoint(21.0f, -78.0f)), 0, 0, 0.0f);

    createToggleValueControl(433, "Pre\nload", cocos2d::CCPoint(1.0f, 0.0f),
        &(rowBase + cocos2d::CCPoint(127.0f, -78.0f)), 0, 0, 0.0f);

    createPlusButton(2, cocos2d::CCPoint(0.6f, 0.0f), 0.0f, "GJ_plus3Btn_001.png",
        &(rowBase + cocos2d::CCPoint(94.0f, -39.0f)), 0);

    addTimeControls(1, 10.0f);

    createValueControlAdvanced(
        416, "Unique ID:", cocos2d::CCPoint(0.65f, 0.0f), 100.0f,
        &(center + cocos2d::CCPoint(-50.0f, 60.0f)), Float, 3, 10, 0.0f, 0.0f, 1, 2, GoldLabel, 0, 2);

    createToggleValueControl(415, "IsUnique", cocos2d::CCPoint(1.0f, 0.0f),
        &(center + cocos2d::CCPoint(50.0f, 60.0f)), 0, 2, 0.0f);

    createValueControlAdvanced(
        455, "SFXGroup:", cocos2d::CCPoint(0.65f, 0.0f), 1000.0f,
        &(center + cocos2d::CCPoint(-50.0f, 0.0f)), Float, 2, 6, 0.0f, 0.0f, 1, 2, GoldLabel, 0, 2);

    createToggleValueControl(420, "Override", cocos2d::CCPoint(1.0f, 0.0f),
        &(center + cocos2d::CCPoint(50.0f, 10.0f)), 0, 2, 0.0f);

    createToggleValueControl(489, "Ignore\nVolume\nTest", cocos2d::CCPoint(1.0f, 0.0f),
        &(center + cocos2d::CCPoint(50.0f, -40.0f)), 0, 2, 0.0f);

    createValueControlAdvanced(
        434, "MinInterval:", cocos2d::CCPoint(0.65f, 0.0f), 1.0f,
        &(center + cocos2d::CCPoint(-50.0f, -40.0f)), (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 2, GoldLabel, 0, 2);

    createPlusButton(0, cocos2d::CCPoint(0.8f, 0.0f), 0.0f, "",
        &(center + cocos2d::CCPoint(10.0f, 83.0f)), 2);
    createPlusButton(1, cocos2d::CCPoint(0.8f, 0.0f), 0.0f, "",
        &(center + cocos2d::CCPoint(10.0f, 23.0f)), 2);

    addProxVolControls(3);

    createValueControlAdvanced(
        596, "Speed +-", cocos2d::CCPoint(0.65f, 0.0f), 12.0f,
        &(rowBase + cocos2d::CCPoint(-91.0f, 0.0f)), (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, 4, GoldLabel, 1, 2);

    createValueControlAdvanced(
        597, "Pitch +-", cocos2d::CCPoint(0.65f, 0.0f), 12.0f,
        &(rowBase + cocos2d::CCPoint(91.0f, 0.0f)), (InputValueType)0, 2, 6, 0.0f, 0.0f, 0, 4, GoldLabel, 1, 2);

    createValueControlAdvanced(
        598, "Volume +-", cocos2d::CCPoint(0.65f, 0.0f), 1.0f,
        &(rowBase + cocos2d::CCPoint(-91.0f, -39.0f)), (InputValueType)0, 1, 6, 0.0f, 0.0f, 0, 4, GoldLabel, 1, 2);

    toggleLimitValue(596, true);
    toggleLimitValue(597, true);
    toggleLimitValue(598, true);

    createToggleValueControl(599, "Pitch\nSteps", cocos2d::CCPoint(1.0f, 0.0f),
        &(rowBase + cocos2d::CCPoint(21.0f, -39.0f)), 0, 4, 0.0f);

    cocos2d::CCSprite* browseIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
    browseIcon->setScale(0.55f);
    CCMenuItemSpriteExtra* browseItem = CCMenuItemSpriteExtra::create(
        browseIcon, nullptr, menu_selector(SetupSFXPopup::onBrowseSFX));
    m_buttonMenu->addChild(browseItem);
    browseItem->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(m_width * 0.5f - 22.0f + winSize.width * 0.5f,
                         m_height * 0.5f - 22.0f + winSize.height * 0.5f)));

    m_sfxID = (int)getValue(392);
    createSFXWidget();

    createPageButtons(m_width * 0.5f + 15.0f, 1);
    goToPage((int)getValue(427), false);

    postSetup();

    return true;
}

void SetupSFXPopup::onBrowseSFX(cocos2d::CCObject* sender)
{
    if (m_sfxWidget)
    {
        m_sfxWidget->removeMeAndCleanup();
        m_sfxWidget = nullptr;
    }

    SFXBrowser* browser = SFXBrowser::create(m_sfxID);
    browser->m_delegate = this;
    browser->show();
}

void SetupSFXPopup::onClose(cocos2d::CCObject* sender)
{
    FMODAudioEngine::sharedEngine()->stopAllEffects();

    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            SFXTriggerGameObject* obj = (SFXTriggerGameObject*)m_gameObjects->objectAtIndex(i);

            MusicDownloadManager::sharedState()->getSFXObject(obj->m_sfxID);
            std::string path = MusicDownloadManager::sharedState()->pathForSFX(obj->m_sfxID);
            FMODAudioEngine::sharedEngine()->preloadEffect(path);

            int length = FMODAudioEngine::sharedEngine()->lengthForSound(path);
            int startOffset = obj->m_startOffset;
            if (!startOffset || obj->m_endOffset > startOffset)
                startOffset = length;

            float pitch = obj->m_pitch ? FMODAudioEngine::sharedEngine()->pitchForIdx(obj->m_pitch) : 1.0f;
            obj->m_duration = (float)((int)((startOffset - obj->m_endOffset) / pitch)) / 1000.0f;
        }
    }

    SetupTriggerPopup::onClose(sender);
}

void SetupSFXPopup::onPlusButton(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag == 1)
    {
        std::unordered_set<int> usedGroups;
        int nextGroup = GameManager::sharedState()->m_levelEditorLayer->getNextFreeSFXGroupID(&usedGroups);
        updateValue(455, (float)nextGroup);
    }
    else if (tag == 2)
    {
        SetupReverbPopup* reverbPopup = SetupReverbPopup::create((SFXTriggerGameObject*)m_gameObject, m_gameObjects);
        reverbPopup->show();
    }
    else if (tag == 0)
    {
        std::unordered_set<int> usedIDs;
        int nextID = GameManager::sharedState()->m_levelEditorLayer->getNextFreeSFXID(&usedIDs);
        updateValue(416, (float)nextID);
    }
}

bool SetupSFXPopup::overridePlaySFX(SFXInfoObject* object)
{
    if (m_gameObject)
    {
        if (m_gameObject->m_reverbEnabled)
            FMODAudioEngine::sharedEngine()->updateReverb(m_gameObject->m_reverbPreset, 0);

        GameManager::sharedState()->playSFXTrigger(m_gameObject);
    }
    return true;
}

void SetupSFXPopup::pageChanged()
{
    updateValue(427, (float)m_page);

    if (m_sfxWidget)
        m_sfxWidget->setVisible(m_page == 4 || m_page <= 1);
}

void SetupSFXPopup::sfxBrowserClosed(SFXBrowser* browser)
{
    updateValue(392, (float)browser->m_sfxID);
}

void SetupSFXPopup::sfxObjectSelected(SFXInfoObject* object)
{
}

void SetupSFXPopup::updateLength()
{
    if (m_sfxWidget)
    {
        float pitch = FMODAudioEngine::sharedEngine()->pitchForIdx((int)getValue(404));
        m_sfxWidget->updateLengthMod(pitch);
    }
}

void SetupSFXPopup::valueDidChange(int tag, float value)
{
    SetupAudioTriggerPopup::valueDidChange(tag, value);

    cocos2d::CCArray* objects = getObjects();
    if (objects && objects->count() > 0)
    {
        for (unsigned int i = 0; i < objects->count(); i++)
        {
            cocos2d::CCObject* obj = objects->objectAtIndex(i);
            // update object
        }
    }

    if (tag == 392)
    {
        m_sfxID = (int)value;
        createSFXWidget();
    }
    else if (tag == 404)
    {
        updateLength();
    }
}

SetupSFXPopup::~SetupSFXPopup()
{
}