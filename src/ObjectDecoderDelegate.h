#pragma once

#include "cocos2d.h"

class DS_Dictionary;

class ObjectDecoderDelegate {
public:
    cocos2d::CCObject* getDecodedObject(int p0, DS_Dictionary* p1);
};