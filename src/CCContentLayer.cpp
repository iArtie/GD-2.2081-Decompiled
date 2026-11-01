#include "CCContentLayer.h"
#include "TableViewCell.h"
#include "cocos2d.h"

CCContentLayer* CCContentLayer::create(const cocos2d::ccColor4B* color, float width, float height) {
    CCContentLayer* layer = new CCContentLayer();
    if (layer && layer->initWithColor(*color, width, height)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

void CCContentLayer::setPosition(const cocos2d::CCPoint* position) {
    cocos2d::CCNode::setPosition(*position);

    cocos2d::CCArray* children = this->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            TableViewCell* cell = static_cast<TableViewCell*>(children->objectAtIndex(i));
            if (cell) {
                cell->updateVisibility();
            }
        }
    }
}

CCContentLayer::~CCContentLayer() {
    // Destructor
}