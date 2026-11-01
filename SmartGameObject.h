#pragma once

#include "cocos2d.h"
#include "GameObject.h"
#include <string>

class GJBaseGameLayer;

class SmartGameObject : public GameObject {
public:
    static SmartGameObject* create(const char* frame);
    bool init(const char* frame) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    void updateSmartFrame();
    virtual ~SmartGameObject();

protected:
    std::string m_smartFrame;
    std::string m_baseFrame;
    bool m_bUnkBool2 = false;
    int m_classType = 0;
    int m_objectID = -1;
    bool m_referenceOnly = false;
};