#pragma once

#include "Platform/cocos2d.h"

class DS_Dictionary;

class SpriteDescription : public cocos2d::CCObject {
public:
    static SpriteDescription* createDescription(DS_Dictionary* dict);
    bool initDescription(DS_Dictionary* dict);
    virtual ~SpriteDescription();

protected:
    cocos2d::CCPoint m_position;
    cocos2d::CCPoint m_flipped;
    cocos2d::CCPoint m_scale;
    float m_rotation = 0.0f;
    int m_zValue = 0;
    int m_tag = 0;
    int m_usesCustomTag = 0;
    cocos2d::CCSpriteFrame* m_texture = nullptr;
};