#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"

class EffectGameObject;
class Slider;
class CCLabelBMFont;

class SetupCameraModePopup : public SetupTriggerPopup {
public:
    static SetupCameraModePopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void determineStartValues();
    void onEditCameraSettings(cocos2d::CCObject* sender);
    void onUnboundMode(cocos2d::CCObject* sender);
    void sliderChanged(cocos2d::CCObject* sender);
    void updateCameraEasing();
    void updateCameraEasingLabel();
    void updateCameraPadding();
    void updateCameraPaddingLabel();
    void updateItemVisibility();

    ~SetupCameraModePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_freeMode = true;
    bool m_cameraSettingsEnabled = true;
    bool m_cameraModeObjects = true;
    int m_cameraEasing = 10;
    float m_cameraPadding = 0.5f;
    cocos2d::CCArray* m_freeModeObjects = nullptr;
    cocos2d::CCArray* m_cameraSettingsObjects = nullptr;
    cocos2d::CCLabelBMFont* m_cameraEasingLabel = nullptr;
    cocos2d::CCLabelBMFont* m_cameraPaddingLabel = nullptr;
    Slider* m_cameraEasingSlider = nullptr;
    Slider* m_cameraPaddingSlider = nullptr;
};