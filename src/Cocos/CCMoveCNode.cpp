#include "Cocos/CCMoveCNode.h"
#include "Objects/GroupCommandObject2.h"
#include "Platform/cocos2d.h"

CCMoveCNode* CCMoveCNode::create() {
    CCMoveCNode* node = new CCMoveCNode();
    if (node && node->init()) {
        node->autorelease();
        return node;
    }
    delete node;
    return nullptr;
}

bool CCMoveCNode::init() {
    m_groupObjects = cocos2d::CCArray::create();
    m_groupObjects->retain();
    m_groupObject = nullptr;
    m_unk074 = 0;
    m_unk078 = 0;
    m_unk07c = 0;
    m_unk0b8 = 0;
    m_unk070 = false;
    m_unk071 = false;
    m_unk0d0 = 0;
    m_unk038 = 0.0f;
    m_unk040 = 0.0f;
    m_unk048 = 0.0f;
    m_unk080 = 1.0f;
    m_unk088 = 1.0f;
    m_staticMoveOffsetY = 0.0f;
    m_staticMoveOffsetX = 0.0f;
    m_unk0a0 = 0.0f;
    m_unk0a8 = 0.0f;
    m_unk0b0 = 0.0f;
    m_unk0c0 = 0.0f;
    m_unk058 = 0.0f;
    m_unk060 = 0.0f;
    m_unk068 = 0.0f;
    m_eObjType = 0;
    return true;
}

void CCMoveCNode::reset() {
    m_groupObjects->removeAllObjects();
    m_groupObject = nullptr;
    m_unk074 = 0;
    m_unk078 = 0;
    m_unk07c = 0;
    m_unk0b8 = 0;
    m_unk070 = false;
    m_unk071 = false;
    m_unk0d0 = 0;
    m_unk038 = 0.0f;
    m_unk040 = 0.0f;
    m_unk048 = 0.0f;
    m_unk080 = 1.0f;
    m_unk088 = 1.0f;
    m_staticMoveOffsetY = 0.0f;
    m_staticMoveOffsetX = 0.0f;
    m_unk0a0 = 0.0f;
    m_unk0a8 = 0.0f;
    m_unk0b0 = 0.0f;
    m_unk0c0 = 0.0f;
    m_unk058 = 0.0f;
    m_unk060 = 0.0f;
    m_unk068 = 0.0f;
    this->setTag(0);
    m_eObjType = 0;
}

CCMoveCNode::~CCMoveCNode() {
    CC_SAFE_RELEASE_NULL(m_groupObjects);
}