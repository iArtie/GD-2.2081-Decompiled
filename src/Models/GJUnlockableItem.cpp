#include "Models/GJUnlockableItem.h"

GJUnlockableItem* GJUnlockableItem::create() {
    auto item = new GJUnlockableItem();
    if (item && item->init()) {
        item->autorelease();
        return item;
    }
    delete item;
    return nullptr;
}

bool GJUnlockableItem::init() {
    return true;
}

GJUnlockableItem::~GJUnlockableItem() {
}