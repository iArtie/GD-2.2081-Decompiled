#include "ButtonPage.h"

ButtonPage::~ButtonPage()
{
    cocos2d::CCLayer::~CCLayer(this);
    operator delete(this);
}