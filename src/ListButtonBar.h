#pragma once

#include "cocos2d.h"

class BoomScrollLayer;
class ListButtonBarDelegate;

class ListButtonBar : public cocos2d::CCNode {
public:
    int getPage();
    void goToPage(int page);
    void onLeft(cocos2d::CCObject* sender);
    void onRight(cocos2d::CCObject* sender);
    ~ListButtonBar();

protected:
    BoomScrollLayer* m_scrollLayer = nullptr;
    bool m_useMoveAnimation = false;
    ListButtonBarDelegate* m_delegate = nullptr;
};