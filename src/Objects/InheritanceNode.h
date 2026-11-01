#pragma once

#include "Platform/cocos2d.h"

class InheritanceNode : public cocos2d::CCObject {
public:
    static InheritanceNode* create(int colorID, InheritanceNode* node);
    bool init(int colorID, InheritanceNode* node) override;
    virtual ~InheritanceNode();

    int m_colorID;
    InheritanceNode* m_inheritanceNode;
    int m_colorAction;
    bool m_unk050;
    bool m_unk051;
};