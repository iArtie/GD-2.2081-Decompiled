#pragma once

#include "Platform/cocos2d.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class SetTargetIDLayer : public SetupTriggerPopup {
public:
    bool init(EffectGameObject* object, cocos2d::CCArray* objects, int minID, int maxID);

    void valueDidChange(int tag, float value) override;

    ~SetTargetIDLayer();

protected:
    int m_minimumID = 0;
    int m_maximumID = 0;
};