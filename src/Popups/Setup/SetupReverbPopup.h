#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class SFXTriggerGameObject;
class CCLabelBMFont;

class SetupReverbPopup : public SetupTriggerPopup {
public:
    static SetupReverbPopup* create(SFXTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(SFXTriggerGameObject* object, cocos2d::CCArray* objects);

    void onClose(cocos2d::CCObject* sender);
    void onPlay(cocos2d::CCObject* sender);
    void onReverb(cocos2d::CCObject* sender);
    void updateReverbLabel();

    ~SetupReverbPopup();

protected:
    SFXTriggerGameObject* m_gameObject = nullptr;
    CCLabelBMFont* m_reverbLabel = nullptr;
};