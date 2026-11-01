#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/SelectSettingDelegate.h"

class AudioLineGuideGameObject;
class SelectSettingLayer;

class SetupAudioLineGuidePopup : public SetupTriggerPopup, public SelectSettingDelegate {
public:
    static SetupAudioLineGuidePopup* create(AudioLineGuideGameObject* object, cocos2d::CCArray* objects);
    bool init(AudioLineGuideGameObject* object, cocos2d::CCArray* objects);

    void onSpeed(cocos2d::CCObject* sender);
    void selectSettingClosed(SelectSettingLayer* layer);

    ~SetupAudioLineGuidePopup();

protected:
    cocos2d::CCSprite* m_speedSprite = nullptr;
};