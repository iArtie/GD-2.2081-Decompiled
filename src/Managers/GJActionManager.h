#pragma once

#include "Platform/cocos2d.h"

class GJActionManager : public cocos2d::CCNode {
public:
    static GJActionManager* create();
    bool init();
    
    void runInternalAction(cocos2d::CCAction* action, cocos2d::CCNode* target);
    cocos2d::CCAction* getInternalAction(int id);
    void stopInternalAction(int id);
    void stopAllInternalActions();
    void updateInternalActions(float dt, bool remove);
    
    virtual ~GJActionManager();

private:
    cocos2d::CCDictionary* m_internalActions;
};