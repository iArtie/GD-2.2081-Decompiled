#pragma once

#include "cocos2d.h"

class ExplodeItemSprite : public cocos2d::CCSprite {
public:
    static ExplodeItemSprite* create();
    
    virtual ~ExplodeItemSprite();

private:
    int m_unk596;
    int m_unk600;
    int m_unk604;
    int m_unk608;
    int m_unk612;
};