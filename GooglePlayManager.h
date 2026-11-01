#pragma once

#include "cocos2d.h"

class GooglePlayDelegate;

class GooglePlayManager : public cocos2d::CCNode {
public:
    void googlePlaySignedIn();
    bool init() override;
    static GooglePlayManager* sharedState();
    virtual ~GooglePlayManager();

private:
    GooglePlayDelegate* m_delegate1;
    GooglePlayDelegate* m_delegate2;
    
    static GooglePlayManager* s_sharedInstance;
};