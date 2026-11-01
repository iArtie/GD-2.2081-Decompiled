#pragma once

#include "cocos2d.h"

class DS_Dictionary;
class ObjectDecoderDelegate;

class ObjectDecoder : public cocos2d::CCNode {
public:
    static ObjectDecoder* sharedDecoder();

    cocos2d::CCObject* getDecodedObject(int p0, DS_Dictionary* p1);

    virtual ~ObjectDecoder();

    ObjectDecoderDelegate* m_delegate = nullptr;
};