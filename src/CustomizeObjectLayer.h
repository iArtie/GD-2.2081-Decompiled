#pragma once

#include "cocos2d.h"
#include "FLAlertLayer.h"
#include "TextInputDelegate.h"
#include "HSVWidgetDelegate.h"
#include "ColorSelectDelegate.h"
#include "ColorSetupDelegate.h"
#include "SliderDelegate.h"
#include "extension/CCScale9Sprite.h"

class GameObject;
class TextGameObject;
class GJSpriteColor;
class ColorChannelSprite;
class ButtonSprite;
class CCTextInputNode;
class Slider;

class CustomizeObjectLayer : public FLAlertLayer, public TextInputDelegate, public HSVWidgetDelegate, public ColorSelectDelegate, public ColorSetupDelegate, public SliderDelegate {
public:
    static CustomizeObjectLayer* create(GameObject* object, cocos2d::CCArray* objects);
    bool init(GameObject* object, cocos2d::CCArray* objects);
    
    void colorSetupClosed(int id) override;
    void hsvPopupClosed(HSVWidgetPopup* popup, cocos2d::ccHSVValue value) override;
    
    void keyBackClicked() override;
    void sliderChanged(cocos2d::CCObject* sender) override;
    void textChanged(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textInputOpened(CCTextInputNode* node) override;
    
    void determineStartValues();
    int getActiveMode(bool ignoreDefault);
    ButtonSprite* getButtonByTag(int tag);
    void highlightSelected(ButtonSprite* sprite);
    void onBreakApart(cocos2d::CCObject* sender);
    void onBrowse(cocos2d::CCObject* sender);
    void onClear(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void onCopy(cocos2d::CCObject* sender);
    void onEditColor(cocos2d::CCObject* sender);
    void onHSV(cocos2d::CCObject* sender);
    void onLiveEdit(cocos2d::CCObject* sender);
    void onNextColorChannel(cocos2d::CCObject* sender);
    void onPaste(cocos2d::CCObject* sender);
    void onSelectColor(cocos2d::CCObject* sender);
    void onSelectMode(cocos2d::CCObject* sender);
    void onSettings(cocos2d::CCObject* sender);
    void onUpdateCustomColor(cocos2d::CCObject* sender);
    void recreateLayer();
    void toggleVisible();
    void updateChannelLabel(int channel);
    void updateColorSprite();
    void updateCurrentSelection();
    void updateCustomColorLabels();
    void updateHSVButtons();
    void updateKerningLabel();
    void updateSelected(int id);
    
    virtual ~CustomizeObjectLayer();

protected:
    TextGameObject* m_targetObject;
    cocos2d::CCArray* m_targetObjects;
    
    int m_selectedMode;
    bool m_hasBaseColor;
    bool m_hasDetailColor;
    bool m_glowDisabled;
    bool m_disableTextDelegate;
    bool m_noElasticity;
    bool m_showTextInput;
    bool m_customColorSelected;
    
    int m_customColorChannel;
    int m_kerningAmount;
    
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_titleLabel;
    cocos2d::CCLabelBMFont* m_selectedColorLabel;
    cocos2d::CCLabelBMFont* m_kerningLabel;
    
    CCMenuItemSpriteExtra* m_baseButton;
    CCMenuItemSpriteExtra* m_detailButton;
    CCMenuItemSpriteExtra* m_textButton;
    CCMenuItemSpriteExtra* m_baseColorHSV;
    CCMenuItemSpriteExtra* m_detailColorHSV;
    CCMenuItemSpriteExtra* m_colorSpriteButton;
    CCMenuItemSpriteExtra* m_liveSelectButton;
    CCMenuItemSpriteExtra* m_arrowDown;
    CCMenuItemSpriteExtra* m_arrowUp;
    CCMenuItemSpriteExtra* m_customColorButton;
    
    ButtonSprite* m_customColorButtonSprite;
    cocos2d::CCArray* m_colorButtons;
    cocos2d::CCArray* m_colorTabNodes;
    cocos2d::CCArray* m_detailTabNodes;
    cocos2d::CCArray* m_textTabNodes;
    cocos2d::CCArray* m_unkArray;
    
    ColorChannelSprite* m_colorSprite;
    cocos2d::extension::CCScale9Sprite* m_customColorInputBG;
    CCTextInputNode* m_customColorInput;
    CCTextInputNode* m_textInput;
    Slider* m_kerningSlider;
};