#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Layers/FLAlertLayer.h"

class EffectGameObject;
class CCLabelBMFont;
class Slider;

class SetupPortalPopup : public FLAlertLayer {
public:
    static SetupPortalPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onEditCameraSettings(cocos2d::CCObject* sender);
    void onUnboundMode(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void updateCameraEasing();
    void updateCameraEasingLabel();
    void updateCameraPadding();
    void updateCameraPaddingLabel();
    void updateItemVisibility();

    ~SetupPortalPopup();

protected:
    cocos2d::CCArray* m_gameObjects = nullptr;
    EffectGameObject* m_gameObject = nullptr;
    int m_cameraEasing = 0;
    float m_cameraPadding = 0.0f;
    bool m_freeMode = true;
    bool m_cameraSettingsEnabled = true;
    cocos2d::CCArray* m_freeModeObjects = nullptr;
    cocos2d::CCArray* m_cameraSettingsObjects = nullptr;
    CCLabelBMFont* m_cameraEasingLabel = nullptr;
    CCLabelBMFont* m_cameraPaddingLabel = nullptr;
    bool m_disableDelegate = false;
    Slider* m_cameraEasingSlider = nullptr;
    Slider* m_cameraPaddingSlider = nullptr;
};