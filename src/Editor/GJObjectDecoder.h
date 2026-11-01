#pragma once

#include "CCNode.h"
#include "Platform/cocos2d.h"

class DS_Dictionary;
class GJGameLevel;
class SongInfoObject;
class GJChallengeItem;
class GJRewardItem;
class GJRewardObject;
class GJSmartTemplate;
class GJSmartPrefab;
class GJLevelList;

class GJObjectDecoder : public cocos2d::CCNode {
public:
    cocos2d::CCObject* getDecodedObject(int type, DS_Dictionary* dict);
    static GJObjectDecoder* sharedDecoder();
    virtual ~GJObjectDecoder();

private:
    static GJObjectDecoder* s_sharedDecoder;
};