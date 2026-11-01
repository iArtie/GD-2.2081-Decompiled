#include "CCNodeContainer.h"
#include "cocos2d.h"

CCNodeContainer* CCNodeContainer::create() {
    CCNodeContainer* container = new CCNodeContainer();
    if (container && container->init()) {
        container->autorelease();
        return container;
    }
    delete container;
    return nullptr;
}

void CCNodeContainer::visit() {
    if (this->getChildrenCount() > 0) {
        cocos2d::CCNode::visit();
    }
}

CCNodeContainer::~CCNodeContainer() {
    // Destructor
}