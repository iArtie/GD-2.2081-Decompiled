#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include <string>
#include <set>

class SetupEventLinkPopup;
class CCMenuItemToggler;

class SelectEventLayer : public SetupTriggerPopup {
public:
    static SelectEventLayer* create(SetupEventLinkPopup* popup, std::set<int>* eventIDs);
    bool init(SetupEventLinkPopup* popup, std::set<int>* eventIDs) override;

    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender) override;
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender) override;

    void addToggle(int id, std::string info);
    cocos2d::CCPoint nextPosition();
    void onInfo(cocos2d::CCObject* sender);

    ~SelectEventLayer();

protected:
    int m_toggleCount = 0;
    bool m_eventsChanged = false;
    std::set<int> m_eventIDs;
    cocos2d::CCDictionary* m_customValueToggles = nullptr;
    SetupEventLinkPopup* m_eventLinkPopup = nullptr;
    int m_maxPage = 0;
    std::map<int, std::string> m_eventInfos;
};