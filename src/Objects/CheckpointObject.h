#pragma once

#include "Platform/cocos2d.h"

class GameObject;

class CheckpointObject : public cocos2d::CCNode {
public:
    static CheckpointObject* create();

    GameObject* getObject();
    void setObject(GameObject* object);

    ~CheckpointObject();

private:
    GameObject* m_physicalCheckpointObject;
};