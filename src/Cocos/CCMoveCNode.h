#pragma once

#include "Platform/cocos2d.h"

class GroupCommandObject2;

class CCMoveCNode : public cocos2d::CCObject {
public:
    static CCMoveCNode* create();

    bool init();
    void reset();

    ~CCMoveCNode();

protected:
    std::vector<GroupCommandObject2*>* m_groupCommands
    GroupCommandObject2* m_groupObject;
    int m_unk074;
    int m_unk078;
    int m_unk07c;
    int m_unk0b8;
    bool m_unk070;
    bool m_unk071;
    int m_unk0d0;
    float m_unk038;
    float m_unk040;
    float m_unk048;
    float m_unk080;
    float m_unk088;
    float m_staticMoveOffsetX;
    float m_staticMoveOffsetY;
    float m_unk0a0;
    float m_unk0a8;
    float m_unk0b0;
    float m_unk0c0;
    float m_unk058;
    float m_unk060;
    float m_unk068;
    int m_eObjType;

    double m_moveOffsetX;           
    double m_moveOffsetY;                 
    bool   m_moveActionClaimed;       
  
};