#pragma once

#include "Platform/cocos2d.h"
#include <string>

class BonusDropdown {
public:
    static BonusDropdown* create(std::string text, int price);
    bool init(std::string text, int price);
    void show();
    ~BonusDropdown();

private:
    float m_yPosition;
    cocos2d::CCLayerColor* m_layerColor;
    cocos2d::CCScene* m_scene;
};