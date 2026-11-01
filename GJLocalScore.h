#pragma once

#include "cocos2d.h"

class GJLocalScore : public cocos2d::CCObject {
public:
    static GJLocalScore* create(int points, int type);
    bool init(int points, int type);
    virtual ~GJLocalScore();

    int m_points;
    int m_type;
};