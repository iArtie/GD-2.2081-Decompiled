#pragma once

#include "Platform/cocos2d.h"

class SearchButton : public cocos2d::CCSprite {
public:
    static SearchButton* create(const char* background, const char* label, float scale, const char* icon);

    bool init(const char* background, const char* label, float scale, const char* icon) override;

    virtual ~SearchButton();

protected:
    cocos2d::CCLabelBMFont* m_label = nullptr;
    cocos2d::CCSprite* m_icon = nullptr;
};