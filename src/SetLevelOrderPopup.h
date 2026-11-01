#pragma once

#include "cocos2d.h"
#include "SetIDPopup.h"

class SetLevelOrderPopup : public SetIDPopup {
public:
    static SetLevelOrderPopup* create(int levelID, int order, int amount);
    bool init(int levelID, int order, int amount) override;

    void onOrderButton(cocos2d::CCObject* sender);

    ~SetLevelOrderPopup();

protected:
    int m_levelID = 0;
};