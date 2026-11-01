#include "GJObjectDecoder.h"
#include "GJGameLevel.h"
#include "SongInfoObject.h"
#include "GJChallengeItem.h"
#include "GJRewardItem.h"
#include "GJRewardObject.h"
#include "GJSmartTemplate.h"
#include "GJSmartPrefab.h"
#include "GJLevelList.h"

GJObjectDecoder* GJObjectDecoder::s_sharedDecoder = nullptr;

cocos2d::CCObject* GJObjectDecoder::getDecodedObject(int type, DS_Dictionary* dict) {
    switch (type) {
        case 4:
            return GJGameLevel::createWithCoder(dict);
        case 5:
            return cocos2d::CCObject::createWithCoder(dict);
        case 6:
            return SongInfoObject::createWithCoder(dict);
        case 7:
            return GJChallengeItem::createWithCoder(dict);
        case 8:
            return GJRewardItem::createWithCoder(dict);
        case 9:
            return GJRewardObject::createWithCoder(dict);
        case 10:
            return GJSmartTemplate::createWithCoder(dict);
        case 11:
            return GJSmartPrefab::createWithCoder(dict);
        case 12:
            return GJLevelList::createWithCoder(dict);
        default:
            return nullptr;
    }
}

GJObjectDecoder* GJObjectDecoder::sharedDecoder() {
    if (!s_sharedDecoder) {
        s_sharedDecoder = new GJObjectDecoder();
        s_sharedDecoder->init();
    }
    return s_sharedDecoder;
}

GJObjectDecoder::~GJObjectDecoder() {
}