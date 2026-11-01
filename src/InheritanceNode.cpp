#include "InheritanceNode.h"

InheritanceNode* InheritanceNode::create(int colorID, InheritanceNode* node) {
    auto obj = new InheritanceNode();
    if (obj && obj->init(colorID, node)) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool InheritanceNode::init(int colorID, InheritanceNode* node) {
    m_colorID = colorID;
    m_inheritanceNode = node;
    m_colorAction = 0;
    m_unk050 = true;
    m_unk051 = false;
    return true;
}

InheritanceNode::~InheritanceNode() {
}