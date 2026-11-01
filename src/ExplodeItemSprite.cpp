#include "ExplodeItemSprite.h"

ExplodeItemSprite* ExplodeItemSprite::create() {
    ExplodeItemSprite* ret = new ExplodeItemSprite();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ExplodeItemSprite::~ExplodeItemSprite() {
    // Base class destructor handles cleanup
}