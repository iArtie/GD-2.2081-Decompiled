#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/SelectArtDelegate.h"

class SmartGameObject;
class SelectArtLayer;

class SetupSmartBlockLayer : public FLAlertLayer, public SelectArtDelegate {
public:
    static SetupSmartBlockLayer* create(SmartGameObject* object, cocos2d::CCArray* objects);
    bool init(SmartGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void keyBackClicked() override;
    void onAllowFlipping(cocos2d::CCObject* sender);
    void onAllowRotation(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onCreate(cocos2d::CCObject* sender);
    void onCreateAll(cocos2d::CCObject* sender);
    void onCreateTemplate(cocos2d::CCObject* sender);
    void onDontDelete(cocos2d::CCObject* sender);
    void onIgnoreCorners(cocos2d::CCObject* sender);
    void onNearbyReference(cocos2d::CCObject* sender);
    void onPasteTemplate(cocos2d::CCObject* sender);
    void onReferenceOnly(cocos2d::CCObject* sender);
    void onSelectPremade(cocos2d::CCObject* sender);
    void onSelectTemplate(cocos2d::CCObject* sender);
    void selectArtClosed(SelectArtLayer* layer) override;
    void show() override;

    ~SetupSmartBlockLayer();

protected:
    int m_specialTemplate = -1;
    cocos2d::CCNode* m_scene = nullptr;
    bool m_allowRotation = false;
    bool m_allowFlipX = false;
    bool m_allowFlipY = false;
    bool m_ignoreCorners = false;
    bool m_useNearby = false;
    bool m_dontDelete = false;
    SmartGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_referenceOnly = false;
};