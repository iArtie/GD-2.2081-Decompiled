#pragma once

#include "Popups/SetIDPopup.h"
#include "Platform/cocos2d.h"

class FindObjectPopup : public SetIDPopup {
public:
    static FindObjectPopup* create();
    
    bool init() override;
    void onFindObjectID(cocos2d::CCObject* sender);
    
    virtual ~FindObjectPopup();

private:
    bool m_unknownBool;
};