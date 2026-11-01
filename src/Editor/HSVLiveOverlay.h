#pragma once

#include "Platform/cocos2d.h"

class GameObject;
class ColorSelectDelegate;
class ConfigureHSVWidget;
class HSVWidgetDelegate;

class HSVLiveOverlay : public FLAlertLayer {
public:
    void closeColorSelect(cocos2d::CCObject* sender);
    static HSVLiveOverlay* create(GameObject* object, cocos2d::CCArray* objects);
    void createHSVWidget(int tab);
    void determineStartValues();
    void hsvChanged(ConfigureHSVWidget* widget);
    bool init(GameObject* object, cocos2d::CCArray* objects) override;
    void keyBackClicked() override;
    void onSelectTab(cocos2d::CCObject* sender);
    void show() override;
    void toggleControls(bool visible);
    virtual ~HSVLiveOverlay();

private:
    ColorSelectDelegate* m_delegate;
    int m_ZOrder;
    cocos2d::CCNode* m_scene;
    GameObject* m_object;
    cocos2d::CCArray* m_objects;
    bool m_unkBool1;
    bool m_unkBool2;
    bool m_unkBool3;
    cocos2d::CCArray* m_unkArray;
    int m_activeTab;
    ConfigureHSVWidget* m_widget;
    cocos2d::CCArray* m_controls;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLayer* m_mainLayer;
};