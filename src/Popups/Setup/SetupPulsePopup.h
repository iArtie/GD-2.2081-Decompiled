#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Editor/GJSpecialColorSelectDelegate.h"
#include "Delegates/TextInputDelegate.h"
#include "ColorPickerDelegate.h"

class EffectGameObject;
class GJSpecialColorSelect;
class CCTextInputNode;
class ConfigureHSVWidget;
class Slider;
class CCMenuItemSpriteExtra;
class CCLabelBMFont;
class CCMenuItemToggler;
class CCControlColourPicker;

class SetupPulsePopup : public SetupTriggerPopup, public GJSpecialColorSelectDelegate, public TextInputDelegate, public cocos2d::extension::ColorPickerDelegate {
public:
    static SetupPulsePopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void colorSelectClosed(GJSpecialColorSelect* select, int id) override;
    void colorValueChanged(cocos2d::ccColor3B color) override;

    void determineStartValues();
    cocos2d::ccColor3B getColorValue();

    void onClose(cocos2d::CCObject* sender);
    void onCopy(cocos2d::CCObject* sender);
    void onExclusive(cocos2d::CCObject* sender);
    void onGroupMainOnly(cocos2d::CCObject* sender);
    void onGroupSecondaryOnly(cocos2d::CCObject* sender);
    void onHSVLegacyMode(cocos2d::CCObject* sender);
    void onPaste(cocos2d::CCObject* sender);
    void onSelectPulseMode(cocos2d::CCObject* sender);
    void onSelectSpecialColor(cocos2d::CCObject* sender);
    void onSelectSpecialTargetID(cocos2d::CCObject* sender);
    void onSelectTargetMode(cocos2d::CCObject* sender);
    void onUpdateCopyColor(cocos2d::CCObject* sender);
    void onUpdateCustomColor(cocos2d::CCObject* sender);
    void show() override;
    void sliderChanged(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void textInputOpened(CCTextInputNode* node) override;
    void updateColorLabels();
    void updateColorValue();
    void updateCopyColor();
    void updateCopyColorTextInputLabel();
    void updateFadeInLabel(bool decimals);
    void updateFadeInTime();
    void updateFadeOutLabel(bool decimals);
    void updateFadeOutTime();
    void updateGroupMainOnly();
    void updateGroupSecondaryOnly();
    void updateHSVValue();
    void updateHoldLabel(bool decimals);
    void updateHoldTime();
    void updatePulseMode();
    void updatePulseTargetType();
    void updateTargetID();
    void updateTextInputLabel();

    ~SetupPulsePopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    cocos2d::CCArray* m_gameObjects = nullptr;
    bool m_groupMainOnly = true;
    bool m_groupSecondaryOnly = true;
    bool m_pulseExclusive = true;
    bool m_staticHSV = true;
    cocos2d::ccColor3B m_currentColor = {255, 255, 255};
    cocos2d::ccHSVValue m_hsv;
    int m_targetGroupID = 0;
    int m_copyColorID = 0;
    float m_fadeInTime = 0.0f;
    float m_holdTime = 0.0f;
    int m_pulseMode = 0;
    float m_fadeOutTime = 0.0f;
    int m_pulseTargetType = 0;
    bool m_disableTextDelegate = false;
    CCTextInputNode* m_fadeInInput = nullptr;
    CCTextInputNode* m_holdInput = nullptr;
    CCTextInputNode* m_fadeOutInput = nullptr;
    CCMenuItemToggler* m_secondaryOnlyToggler = nullptr;
    CCMenuItemToggler* m_mainOnlyToggler = nullptr;
    CCTextInputNode* m_idInput = nullptr;
    CCTextInputNode* m_colorIDInput = nullptr;
    CCTextInputNode* m_rInput = nullptr;
    CCTextInputNode* m_gInput = nullptr;
    CCTextInputNode* m_bInput = nullptr;
    CCTextInputNode* m_hexInput = nullptr;
    ConfigureHSVWidget* m_hsvWidget = nullptr;
    bool m_colorInitializing = false;
    cocos2d::extension::CCControlColourPicker* m_colorPicker = nullptr;
    cocos2d::ccColor3B m_originalColor = {255, 255, 255};
    cocos2d::CCSprite* m_currentColorSprite = nullptr;
    cocos2d::CCSprite* m_originalColorSprite = nullptr;
    Slider* m_fadeInSlider = nullptr;
    Slider* m_holdSlider = nullptr;
    Slider* m_fadeOutSlider = nullptr;
    CCMenuItemSpriteExtra* m_colorButton = nullptr;
    CCMenuItemSpriteExtra* m_hsvButton = nullptr;
    cocos2d::CCArray* m_colorObjects = nullptr;
    cocos2d::CCArray* m_hsvObjects = nullptr;
    bool m_modesInitializing = false;
    CCMenuItemSpriteExtra* m_channelButton = nullptr;
    CCMenuItemSpriteExtra* m_groupButton = nullptr;
    CCMenuItemSpriteExtra* m_specialColorButton = nullptr;
    CCLabelBMFont* m_mainOnlyLabel = nullptr;
    CCLabelBMFont* m_secondaryOnlyLabel = nullptr;
    CCLabelBMFont* m_idLabel = nullptr;
    bool m_touchTriggered = false;
    CCMenuItemToggler* m_touchToggle = nullptr;
    bool m_spawnTriggered = false;
    CCMenuItemToggler* m_spawnToggle = nullptr;
    bool m_multiTriggered = false;
    cocos2d::CCNode* m_multiTriggerContainer = nullptr;
    CCMenuItemToggler* m_staticHSVToggler = nullptr;
    CCLabelBMFont* m_staticHSVLabel = nullptr;
};