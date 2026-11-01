#pragma once

#include "cocos2d.h"

class CCIndexPath : public cocos2d::CCObject {
public:
    CCIndexPath();
    CCIndexPath(int row, int section);

    int m_row;
    int m_section;

    ~CCIndexPath();
};