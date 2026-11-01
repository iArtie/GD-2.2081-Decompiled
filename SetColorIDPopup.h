#pragma once

#include "cocos2d.h"
#include "SetIDPopup.h"
#include "TextInputDelegate.h"
#include "GJSpecialColorSelectDelegate.h"

class CCTextInputNode;
class GJSpecialColorSelect;

class SetColorIDPopup : public SetIDPopup, public TextInputDelegate, public GJSpecialColorSelectDelegate {
public:
    static SetColorIDPopup* create(int id);
    bool init(int id) override;

    void colorSelectClosed(GJSpecialColorSelect* select, int id) override;
    void onSelectSpecialColor(cocos2d::CCObject* sender);

    ~SetColorIDPopup();

protected:
    int m_value = 0;
    bool m_disableDelegate = false;
    CCTextInputNode* m_inputNode = nullptr;
};