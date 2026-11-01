#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"

class GJUINode;

class UIOptionsLayer : public FLAlertLayer {
public:
    static UIOptionsLayer* create(bool dual);
    bool init(bool dual) override;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void registerWithTouchDispatcher() override;

    GJUINode* getNode(int type);
    float getValue(int tag);
    void onClose(cocos2d::CCObject* sender);
    void onReset(cocos2d::CCObject* sender);
    void onSaveLoad(cocos2d::CCObject* sender);
    void toggleUIGroup(int group);
    void valueDidChange(int tag, float value);

    ~UIOptionsLayer();

protected:
    bool m_dual = false;
    GJUINode* m_uiNode1 = nullptr;
    GJUINode* m_uiNode2 = nullptr;
    GJUINode* m_uiNode3 = nullptr;
    GJUINode* m_uiNode4 = nullptr;
    int m_activeUIGroup = 0;
    cocos2d::CCLabelBMFont* m_nameLabel = nullptr;
};