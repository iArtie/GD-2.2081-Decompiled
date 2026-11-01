#pragma once

#include "cocos2d.h"
#include "SetupTriggerPopup.h"

// Forward declarations
class EffectGameObject;
class InfoAlertButton;
class CCMenuItemSpriteExtra;

class EditTriggersPopup : public SetupTriggerPopup {
public:
    static EditTriggersPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    
    virtual ~EditTriggersPopup();

private:

    void determineStartValues();  

    cocos2d::CCMenu* m_buttonMenu;      
    InfoAlertButton* m_infoButton;      
    int m_joystickConnected;           
    float m_width;                     
    float m_height;                       
    EffectGameObject* m_targetObject;   
    cocos2d::CCArray* m_targetObjects;  
};