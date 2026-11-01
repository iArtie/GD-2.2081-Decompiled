#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"

class SequenceTriggerGameObject;
class EffectGameObject;
class CCMenuItemSpriteExtra;

class SetupSequenceTriggerPopup : public SetupTriggerPopup {
public:
    static SetupSequenceTriggerPopup* create(SequenceTriggerGameObject* object);
    bool init(SequenceTriggerGameObject* object);

    void onAddChance(cocos2d::CCObject* sender);
    void onChangeOrder(cocos2d::CCObject* sender);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void onDeleteSelected(cocos2d::CCObject* sender);
    void onSelect(cocos2d::CCObject* sender);
    void updateGroupIDButtons();

    ~SetupSequenceTriggerPopup();

protected:
    cocos2d::CCDictionary* m_customValueToggles = nullptr;
    CCMenuItemSpriteExtra* m_selectedButton = nullptr;
    cocos2d::CCArray* m_groupButtons = nullptr;
    EffectGameObject* m_gameObject = nullptr;
};