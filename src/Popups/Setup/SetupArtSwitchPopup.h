#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/SelectArtDelegate.h"

class ArtTriggerGameObject;
class SelectArtLayer;

class SetupArtSwitchPopup : public SetupTriggerPopup, public SelectArtDelegate {
public:
    static SetupArtSwitchPopup* create(ArtTriggerGameObject* object, cocos2d::CCArray* objects, int id);
    bool init(ArtTriggerGameObject* object, cocos2d::CCArray* objects, int id);

    void onArt(cocos2d::CCObject* sender);
    void selectArtClosed(SelectArtLayer* layer);

    ~SetupArtSwitchPopup();

protected:
    cocos2d::CCSprite* m_artSprite = nullptr;
    int m_artType = 0;
};