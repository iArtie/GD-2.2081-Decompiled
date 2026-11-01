#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include <string>

class EffectGameObject;

class SetupShaderEffectPopup : public SetupTriggerPopup {
public:
    static SetupShaderEffectPopup* create(EffectGameObject* object, cocos2d::CCArray* objects, int id);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects, int id);

    void onResetColors(cocos2d::CCObject* sender);
    void onZLayer(cocos2d::CCObject* sender);

    void setupBulge();
    void setupChromatic();
    void setupChromaticGlitch();
    void setupColorChange();
    void setupGlitch();
    void setupGrayscale();
    void setupHueShift();
    void setupInvertColor();
    void setupLensCircle();
    void setupMotionBlur();
    void setupPinch();
    void setupPixelate();
    void setupRadialBlur();
    void setupSepia();
    void setupShaderTrigger();
    void setupShockLine();
    void setupShockWave();
    void setupSplitScreen();

    void toggleGroup(int group, bool visible) override;
    void updateDefaultTriggerValues() override;
    void updateZLayerButtons();
    void valueDidChange(int tag, float value) override;

    static std::string zLayerToString(int zLayer);

    ~SetupShaderEffectPopup();

protected:
    int m_objectID = 0;
    cocos2d::CCArray* m_zLayerSprites = nullptr;
    int m_zLayerMin = 0;
    int m_zLayerMax = 0;
    bool m_changeMin = false;
};