#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include <set>

class EventLinkTrigger;
class EffectGameObject;

class SetupEventLinkPopup : public SetupTriggerPopup {
public:
    static SetupEventLinkPopup* create(EventLinkTrigger* object, cocos2d::CCArray* objects);
    bool init(EventLinkTrigger* object, cocos2d::CCArray* objects);

    void onSelectEvent(cocos2d::CCObject* sender);
    void updateEventIDs(std::set<int>* eventIDs);

    ~SetupEventLinkPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
};