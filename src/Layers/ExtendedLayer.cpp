#include "Layers/ExtendedLayer.h"
#include "Delegates/BoomScrollLayerDelegate.h"

ExtendedLayer* ExtendedLayer::create() {
    ExtendedLayer* ret = new ExtendedLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ExtendedLayer::setPosition(const cocos2d::CCPoint& position) {
    cocos2d::CCNode::setPosition(position);
    if (m_delegate) {
        m_delegate->scrollLayerMoved(position);
    }
}

ExtendedLayer::~ExtendedLayer() {
    // Base class destructor handles cleanup
}