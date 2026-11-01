#include "CCExtenderNode.h"
#include "cocos2d.h"

void CCExtenderNode::setOpacity(unsigned int opacity) {
    cocos2d::CCArray* children = this->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCObject* obj = children->objectAtIndex(i);
            obj->setOpacity(opacity);
        }
    }
}