#include "Objects/CheckpointObject.h"
#include "Objects/GameObject.h"
#include "Platform/cocos2d.h"

CheckpointObject* CheckpointObject::create() {
    CheckpointObject* obj = new CheckpointObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

GameObject* CheckpointObject::getObject() {
    return m_physicalCheckpointObject;
}

void CheckpointObject::setObject(GameObject* object) {
    if (m_physicalCheckpointObject == object) {
        return;
    }

    if (object) {
        object->retain();
    }

    if (m_physicalCheckpointObject) {
        m_physicalCheckpointObject->release();
    }

    m_physicalCheckpointObject = object;
}

CheckpointObject::~CheckpointObject() {
    if (m_physicalCheckpointObject) {
        m_physicalCheckpointObject->release();
        m_physicalCheckpointObject = nullptr;
    }
}