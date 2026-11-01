#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/ConfigureValuePopupDelegate.h"
#include <string>

class GameObject;
class CCTextInputNode;
class ConfigureValuePopup;

class SetGroupIDLayer : public FLAlertLayer, public TextInputDelegate, public ConfigureValuePopupDelegate {
public:
    static SetGroupIDLayer* create(GameObject* object, cocos2d::CCArray* objects);
    bool init(GameObject* obj, cocos2d::CCArray* objs) override;

    void keyBackClicked() override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void valuePopupClosed(ConfigureValuePopup* popup, float value) override;

    void addGroupID(int id);
    void callRemoveFromGroup(float dt);
    void determineStartValues();
    void onAddGroup(cocos2d::CCObject* sender);
    void onAddGroupParent(cocos2d::CCObject* sender);
    void onAnim(cocos2d::CCObject* sender);
    void onArrow(int tag, int increment);
    void onArrowLeft(cocos2d::CCObject* sender);
    void onArrowRight(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender) override;
    void onCopy(cocos2d::CCObject* sender);
    void onExtra(cocos2d::CCObject* sender);
    void onExtra2(cocos2d::CCObject* sender);
    void onGroupIDOptions(cocos2d::CCObject* sender);
    void onNextFreeEditorLayer1(cocos2d::CCObject* sender);
    void onNextFreeEditorLayer2(cocos2d::CCObject* sender);
    void onNextFreeOrderChannel(cocos2d::CCObject* sender);
    void onNextGroupID1(cocos2d::CCObject* sender);
    void onPaste(cocos2d::CCObject* sender);
    void onRemoveFromGroup(cocos2d::CCObject* sender);
    void onSmoothEase(cocos2d::CCObject* sender);
    void onToggleGuide(cocos2d::CCObject* sender);
    void onToggleSelectedOrder(cocos2d::CCObject* sender);
    void onToggleTraceIn(cocos2d::CCObject* sender);
    void onToggleTraceOut(cocos2d::CCObject* sender);
    void onZLayer(cocos2d::CCObject* sender);
    void onZLayerShift(cocos2d::CCObject* sender);
    void removeGroupID(int id);
    void updateEditorLabel();
    void updateEditorLabel2();
    void updateEditorLayerID();
    void updateEditorLayerID2();
    void updateEditorOrder();
    void updateEditorOrderLabel();
    void updateGroupIDButtons();
    void updateGroupIDLabel();
    void updateOrderChannel();
    void updateOrderChannelLabel();
    void updateZLayerButtons();
    void updateZOrder();
    void updateZOrderLabel();

    ~SetGroupIDLayer();

protected:
    GameObject* m_targetObject = nullptr;
    cocos2d::CCArray* m_targetObjects = nullptr;
    CCTextInputNode* m_groupIDInput = nullptr;
    int m_groupIDValue = 0;
    CCTextInputNode* m_editorLayerInput = nullptr;
    CCTextInputNode* m_editorLayer2Input = nullptr;
    CCTextInputNode* m_zOrderInput = nullptr;
    CCTextInputNode* m_orderInput = nullptr;
    CCTextInputNode* m_channelInput = nullptr;
    bool m_addedGroup = false;
    bool m_reverseChanged = false;
    bool m_channelOrderEdited = false;
    bool m_channelUpdated = false;
    bool m_removeGroupsLock = false;
    cocos2d::CCArray* m_groupIDObjects = nullptr;
    int m_groupToRemove = 0;
    int m_orderValue = 0;
    int m_channelValue = 0;
    bool m_hasTargetObjects = false;
    bool m_unk2ff = false;
    ZLayer m_zLayerValue = ZLayer::Default;
    bool m_showChannelOrder = false;
    int m_editorLayerValue = 0;
    int m_editorLayer2Value = 0;
    int m_zOrderValue = 0;
    cocos2d::CCDictionary* m_parentGroups = nullptr;
    bool m_editorLayerEdited = false;
    cocos2d::CCArray* m_zLayerButtons = nullptr;
};