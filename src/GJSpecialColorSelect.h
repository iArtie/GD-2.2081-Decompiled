#pragma once

#include "cocos2d.h"

class ButtonSprite;
class GJSpecialColorSelectDelegate;

class GJSpecialColorSelect : public cocos2d::CCLayer {
public:
    ButtonSprite* getButtonByTag(int tag);
    void highlightSelected(ButtonSprite* sprite);
    void keyBackClicked() override;
    void onClose(cocos2d::CCObject* sender);
    void onSelectColor(cocos2d::CCObject* sender);
    static const char* textForColorIdx(int index);
    virtual ~GJSpecialColorSelect();

private:
    GJSpecialColorSelectDelegate* m_delegate;
    int m_colorID;
    cocos2d::CCArray* m_buttonSprites;
};