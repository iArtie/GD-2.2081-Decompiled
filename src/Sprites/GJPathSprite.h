#pragma once

#include "Platform/cocos2d.h"

class GJPathSprite : public cocos2d::CCSprite {
public:
    void addRankLabel(int path);
    void addShardSprite();
    void changeToLockedArt();
    static GJPathSprite* create(int path);
    bool init(int path);
    void updateState();
    virtual ~GJPathSprite();

private:
    int m_pathNumber;
};