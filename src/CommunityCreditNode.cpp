#include "CommunityCreditNode.h"
#include "cocos2d.h"

CommunityCreditNode* CommunityCreditNode::create(int unlockType, int iconID, int unknown, std::string author) {
    CommunityCreditNode* node = new CommunityCreditNode();
    if (node && node->init(unlockType, iconID, unknown, author)) {
        node->autorelease();
        return node;
    }
    delete node;
    return nullptr;
}

bool CommunityCreditNode::init(int unlockType, int iconID, int unknown, std::string author) {
    if (!cocos2d::CCNode::init()) {
        return false;
    }

    m_unlockType = unlockType;
    m_iconID = iconID;
    m_unknown = unknown;
    m_author = author;

    return true;
}

CommunityCreditNode::~CommunityCreditNode() {
    // Destructor
}