#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Delegates/TextInputDelegate.h"

class RandTriggerGameObject;
class CCTextInputNode;

class SetupRandAdvTriggerPopup : public SetupTriggerPopup, public TextInputDelegate {
public:
    static SetupRandAdvTriggerPopup* create(RandTriggerGameObject* object, cocos2d::CCArray* objects);
    bool init(RandTriggerGameObject* object, cocos2d::CCArray* objects);

    void addChance(int id, int chance);
    void callRemoveFromGroup(float dt);
    void onAddChance(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onRemoveFromGroup(cocos2d::CCObject* sender);
    void removeGroupID(int id);
    void textChanged(CCTextInputNode* node);
    void updateGroupIDButtons();

    ~SetupRandAdvTriggerPopup();

protected:
    CCTextInputNode* m_groupIDInput = nullptr;
    CCTextInputNode* m_chanceInput = nullptr;
    bool m_removingGroup = false;
    cocos2d::CCArray* m_groupButtons = nullptr;
    int m_groupToRemove = 0;
    RandTriggerGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_performedAction = false;
};