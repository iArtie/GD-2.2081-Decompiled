#pragma once

#include "SetupTriggerPopup.h"
#include "CustomizeObjectLayer.h"

class CustomizeObjectSettingsPopup : public SetupTriggerPopup {
public:
    static CustomizeObjectSettingsPopup* create(GameObject* targetObject, cocos2d::CCArray* targetObjects, CustomizeObjectLayer* customizeLayer);
    bool init(GameObject* targetObject, cocos2d::CCArray* targetObjects, CustomizeObjectLayer* customizeLayer);
    
    void onClose(cocos2d::CCObject* sender) override;
    virtual ~CustomizeObjectSettingsPopup();

protected:
    CustomizeObjectLayer* m_customizeLayer;
    bool m_settingsChanged;
};