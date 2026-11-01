#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include "Cocos/CCTextInputNode.h"

class EffectGameObject;

class CollisionBlockPopup : public FLAlertLayer, public TextInputDelegate {
public:
    static CollisionBlockPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);

    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    void determineStartValues();
    void keyBackClicked();
    void onClose(cocos2d::CCObject* sender);
    void onDynamicBlock(cocos2d::CCObject* sender);
    void onItemIDArrow(cocos2d::CCObject* sender);
    void onNextItemID(cocos2d::CCObject* sender);
    void show();
    virtual void textChanged(CCTextInputNode* node) override;
    virtual void textInputClosed(CCTextInputNode* node) override;
    virtual void textInputReturn(CCTextInputNode* node) override;
    virtual void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    void updateEditorLabel();
    void updateItemID();
    void updateTextInputLabel();

    ~CollisionBlockPopup();

private:
    int m_ZOrder;
    cocos2d::CCNode* m_scene;
    cocos2d::CCLayer* m_mainLayer;
    EffectGameObject* m_gameObject;
    bool m_dynamicBlock;
    int m_blockID;
    cocos2d::CCArray* m_gameObjects;
    CCTextInputNode* m_blockIDInput;
    bool m_disableDelegate;
    bool m_noElasticity;
    cocos2d::CCMenu* m_buttonMenu;
    int m_joystickConnected;
};