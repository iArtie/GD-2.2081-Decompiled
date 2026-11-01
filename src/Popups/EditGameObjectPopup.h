#pragma once

#include "Platform/cocos2d.h"
#include "Layers/FLAlertLayer.h"
#include "Delegates/TextInputDelegate.h"
#include "Delegates/SliderDelegate.h"
#include "Editor/ColorSelectDelegate.h"
#include "Editor/HSVWidgetDelegate.h"
#include "Delegates/ConfigureValuePopupDelegate.h"

class EffectGameObject;
class EditGameObjectPopupDelegate;

class EditGameObjectPopup : public FLAlertLayer, public TextInputDelegate, public SliderDelegate, public ColorSelectDelegate, public HSVWidgetDelegate, public ConfigureValuePopupDelegate {
public:
    static EditGameObjectPopup* create(EffectGameObject* object, cocos2d::CCArray* objects, bool platformer);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects, bool platformer);
    
    virtual ~EditGameObjectPopup();

protected:
    EffectGameObject* m_targetObject;
    cocos2d::CCArray* m_targetObjects;
    cocos2d::CCArray* m_unknownArray;
    
    bool m_platformerMode;
    bool m_unknownBool1;
    bool m_unknownBool2;
    bool m_unknownBool3;
    bool m_unknownBool4;
    
    int m_unknownInt1;
    int m_unknownInt2;
    int m_unknownInt3;
    int m_unknownInt4;
    int m_unknownInt5;
    float m_unknownFloat1;
    float m_unknownFloat2;
    
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCMenu* m_buttonMenu;
    cocos2d::CCLabelBMFont* m_titleLabel;
    
    EditGameObjectPopupDelegate* m_delegate;
};