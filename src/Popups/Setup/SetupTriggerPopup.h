#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/ConfigureValuePopupDelegate.h"
#include "Delegates/SliderDelegate.h"
#include "Utils/GJEnums.h"
#include <string>
#include <map>

class ButtonSprite;
class CCMenuItemSpriteExtra;
class CCMenuItemToggler;
class CCTextInputNode;
class ConfigureValuePopup;
class GameObject;
class EffectGameObject;
class Slider;
class SliderThumb;
class InfoAlertButton;

class SetupTriggerPopup : public FLAlertLayer,
                          public TextInputDelegate,
                          public ConfigureValuePopupDelegate,
                          public SliderDelegate {
public:
    void addCloseButton(std::string text);
    void addHelpButton(std::string title, std::string desc, float scale);
    void addObjectToGroup(cocos2d::CCObject* object, int group);
    void addObjectToPage(cocos2d::CCObject* object, int page);
    void addObjectsToGroup(cocos2d::CCArray* objects, int group);
    void addObjectsToPage(cocos2d::CCArray* objects, int page);
    void addTitle(std::string title);
    void closeInputNodes();
    static SetupTriggerPopup* create(float width, float height);
    void createMultiTriggerItemsCorner();
    void createMultiTriggerItemsDefault();
    void createMultiTriggerItemsDefaultHorizontal();
    void createMultiTriggerItemsDefaultVertical();
    void createPageButtons(float offset, int arrow);
    void determineStartValues();
    cocos2d::CCArray* getGroupContainer(int group);
    float getMaxSliderValue(int property);
    float getMinSliderValue(int property);
    cocos2d::CCArray* getObjects();
    cocos2d::CCArray* getPageContainer(int page);
    float getTriggerValue(int property, GameObject* object);
    float getTruncatedValue(float value, int decimals);
    float getTruncatedValueByTag(int tag, float value);
    float getValue(int tag);
    void goToPage(int page, bool hideAll);
    void hideAll();
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onCustomButton(cocos2d::CCObject* sender);
    void onCustomEaseArrow(int property, bool up);
    void onCustomEaseArrowDown(cocos2d::CCObject* sender);
    void onCustomEaseArrowUp(cocos2d::CCObject* sender);
    void onCustomEaseRate(cocos2d::CCObject* sender);
    void onCustomToggleTriggerValue(cocos2d::CCObject* sender);
    void onDisableValue(cocos2d::CCObject* sender);
    void onEase(cocos2d::CCObject* sender);
    void onEaseRate(cocos2d::CCObject* sender);
    void onMultiTrigger(cocos2d::CCObject* sender);
    void onPage(cocos2d::CCObject* sender);
    void onPlusButton(cocos2d::CCObject* sender);
    void onSpawnedByTrigger(cocos2d::CCObject* sender);
    void onToggleTriggerValue(cocos2d::CCObject* sender);
    void onTouchTriggered(cocos2d::CCObject* sender);
    void pageChanged();
    void postSetup();
    void preSetup();
    void refreshGroupVisibility();
    void removeObjectFromGroup(cocos2d::CCObject* object, int group);
    void removeObjectFromPage(cocos2d::CCObject* object, int page);
    void resetDisabledValues();
    void setMaxSliderValue(float value, int property);
    void setMinSliderValue(float value, int property);
    bool shouldLimitValue(int property);
    void show() override;
    void sliderBegan(Slider* slider);
    void sliderEnded(Slider* slider);
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputReturn(CCTextInputNode* node) override;
    void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    void toggleBG(bool visible);
    void toggleCustomEaseRateVisibility(int property, int tag);
    void toggleDisableButtons(bool visible);
    void toggleEaseRateVisibility();
    void toggleGroup(int group, bool visible);
    void toggleLimitValue(int property, bool limit);
    void togglePageArrows(bool visible);
    void triggerArrowChanged(int property, bool isRight);
    void triggerArrowLeft(cocos2d::CCObject* sender);
    void triggerArrowRight(cocos2d::CCObject* sender);
    void triggerSliderChanged(cocos2d::CCObject* sender);
    float triggerSliderValueFromValue(int tag, float value);
    float triggerValueFromSliderValue(int tag, float value);
    void updateCustomEaseLabel(int property, int easingType);
    void updateCustomEaseRateLabel(int property, float easingRate);
    void updateCustomToggleTrigger(int tag, bool toggled);
    void updateDefaultTriggerValues();
    void updateEaseLabel();
    void updateEaseRateLabel();
    void updateEditorLabel();
    void updateInputNode(int tag, float value);
    void updateInputNodeLabel(int property, std::string text);
    void updateInputValue(int tag, float* value);
    void updateLabel(int property, std::string text);
    void updateMultiTriggerBtn();
    void updateSlider(int property);
    void updateSlider(int property, float value);
    void updateSpawnedByTrigger();
    void updateToggleItem(int value, bool toggled);
    void updateTouchTriggered();
    void updateValue(int property, float value);
    void updateValueControls(int property, float value);
    void valueChanged(int property, float value);
    void valueDidChange(int tag, float value) override;
    void valuePopupClosed(ConfigureValuePopup* popup, float value) override;
    ~SetupTriggerPopup() override;

    void createValueControlAdvanced(int property, const std::string& label, cocos2d::CCPoint offset, float maxValue, bool limit, InputValueType type, int decimals, int buttonType, float sliderMin, float sliderMax, bool useInput, bool enable, GJInputStyle style, int labelTag, int triggerTag);
    void createMultiTriggerItems(cocos2d::CCPoint corner, cocos2d::CCPoint p1, cocos2d::CCPoint p2);

protected:
    int m_ZOrder = 0;
    cocos2d::CCNode* m_scene = nullptr;
    bool m_disableSliderDelegate = false;
    EffectGameObject* m_gameObject = nullptr;
    bool m_multiTriggered = false;
    cocos2d::CCArray* m_gameObjects = nullptr;
    cocos2d::CCDictionary* m_triggerValues = nullptr;
    cocos2d::CCDictionary* m_valueToggles = nullptr;
    cocos2d::CCDictionary* m_inputNodes = nullptr;
    float m_easingRate = 0.0f;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    cocos2d::CCArray* m_pageContainers = nullptr;
    cocos2d::CCArray* m_groupContainers = nullptr;
    cocos2d::CCDictionary* m_disabledEasingButtons = nullptr;
    int m_page = 0;
    bool m_hideAll = false;
    float m_height = 0.0f;
    float m_width = 0.0f;
    bool m_touchTriggered = false;
    bool m_spawnTriggered = false;
    bool m_effectObjects = false;
    cocos2d::CCArray* m_multiTriggerContainer = nullptr;
    CCMenuItemToggler* m_spawnToggle = nullptr;
    CCMenuItemToggler* m_touchToggle = nullptr;
    cocos2d::CCDictionary* m_disableButtons = nullptr;
    cocos2d::CCDictionary* m_customValueToggles = nullptr;
    cocos2d::CCDictionary* m_inputLabels = nullptr;
    bool m_disableTextDelegate = false;
    cocos2d::CCDictionary* m_minSliderValues = nullptr;
    cocos2d::CCDictionary* m_maxSliderValues = nullptr;
    cocos2d::CCDictionary* m_sliders = nullptr;
    EasingType m_easingType = EasingType::None;
    cocos2d::CCLabelBMFont* m_easingLabel = nullptr;
    float m_easingControlScale = 1.0f;
    cocos2d::CCLabelBMFont* m_easingRateLabel = nullptr;
    CCMenuItemSpriteExtra* m_easingRateButton = nullptr;
    cocos2d::CCDictionary* m_customEasingLabels = nullptr;
    cocos2d::CCDictionary* m_customEasingScales = nullptr;
    cocos2d::CCDictionary* m_customEasingButtons = nullptr;
    std::map<int, bool> m_shouldLimitValues;
    cocos2d::CCDictionary* m_customEasingTags = nullptr;
};