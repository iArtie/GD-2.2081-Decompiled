#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Popups/Setup/SetupAudioTriggerPopup.h"
#include "Delegates/SFXBrowserDelegate.h"
#include "Delegates/CustomSFXDelegate.h"

class SFXTriggerGameObject;
class CustomSFXWidget;
class SFXBrowser;
class SFXInfoObject;

class SetupSFXPopup : public SetupAudioTriggerPopup, public SFXBrowserDelegate, public CustomSFXDelegate {
public:
    static SetupSFXPopup* create(SFXTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(SFXTriggerGameObject* object, cocos2d::CCArray* objects);

    void createSFXWidget();
    int getActiveSFXID();

    void onBrowseSFX(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onPlusButton(cocos2d::CCObject* sender);
    bool overridePlaySFX(SFXInfoObject* object) override;
    void pageChanged() override;
    void sfxBrowserClosed(SFXBrowser* browser) override;
    void sfxObjectSelected(SFXInfoObject* object) override;
    void updateLength();
    void valueDidChange(int tag, float value) override;

    ~SetupSFXPopup();

protected:
    int m_sfxID = 0;
    int m_page = 0;
    CustomSFXWidget* m_sfxWidget = nullptr;
    SFXTriggerGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    unsigned int m_sfxLength = 0;
    bool m_disableSliderDelegate = false;
};