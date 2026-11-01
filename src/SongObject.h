#pragma once

#include "cocos2d.h"

class SongObject : public cocos2d::CCObject {
public:
    static SongObject* create(int audioID);
    bool init(int audioID);
    virtual ~SongObject();

    int m_audioID = 0;
};