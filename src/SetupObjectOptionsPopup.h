#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "FLAlertLayer.h"

class GameObject;
class SetGroupIDLayer;

class SetupObjectOptionsPopup : public FLAlertLayer {
public:
    void determineStartValues();
    void keyBackClicked() override;

    void onAlwaysHide(cocos2d::CCObject* sender);
    void onApplyScaleStick(cocos2d::CCObject* sender);
    void onCenterEffect(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onDisableGlow(cocos2d::CCObject* sender);
    void onDisableObject(cocos2d::CCObject* sender);
    void onDontBoost(cocos2d::CCObject* sender);
    void onDontBoostX(cocos2d::CCObject* sender);
    void onDontEnter(cocos2d::CCObject* sender);
    void onDontFade(cocos2d::CCObject* sender);
    void onExtendedCollision(cocos2d::CCObject* sender);
    void onExtraSticky(cocos2d::CCObject* sender);
    void onGripSlope(cocos2d::CCObject* sender);
    void onHideEffects(cocos2d::CCObject* sender);
    void onHighDetail(cocos2d::CCObject* sender);
    void onIceBlock(cocos2d::CCObject* sender);
    void onNoAudioScale(cocos2d::CCObject* sender);
    void onNoParticle(cocos2d::CCObject* sender);
    void onNonStickX(cocos2d::CCObject* sender);
    void onNonStickY(cocos2d::CCObject* sender);
    void onPassable(cocos2d::CCObject* sender);
    void onReverse(cocos2d::CCObject* sender);
    void onSinglePlayerTouch(cocos2d::CCObject* sender);
    void onToggleAreaParent(cocos2d::CCObject* sender);
    void onToggleGroupParent(cocos2d::CCObject* sender);

    void show() override;

    ~SetupObjectOptionsPopup();

protected:
    cocos2d::CCNode* m_scene = nullptr;
    GameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_effectObject = false;
    bool m_iceBlock = true;
    bool m_dontFade = true;
    bool m_dontEnter = true;
    bool m_noEffects = true;
    bool m_noParticle = true;
    bool m_groupParent = true;
    bool m_areaParent = true;
    bool m_highDetail = true;
    bool m_noTouch = false;
    bool m_passable = true;
    bool m_hide = true;
    bool m_nonStickX = true;
    bool m_nonStickY = true;
    bool m_extraSticky = true;
    bool m_dontBoostY = true;
    bool m_dontBoostX = true;
    bool m_scaleStick = true;
    bool m_gripSlope = true;
    bool m_noGlow = true;
    bool m_reverse = true;
    bool m_singlePlayerTouch = true;
    bool m_centerEffect = true;
    bool m_extendedCollision = true;
    bool m_noAudioScale = true;
    SetGroupIDLayer* m_groupIDLayer = nullptr;
};