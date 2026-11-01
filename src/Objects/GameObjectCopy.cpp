// GameObjectCopy.cpp
#include "Objects/GameObjectCopy.h"

GameObjectCopy* GameObjectCopy::create(GameObject* object) {
    GameObjectCopy* copy = new GameObjectCopy();
    if (copy && copy->init(object)) {
        copy->autorelease();
        return copy;
    }
    if (copy) delete copy;
    return nullptr;
}

bool GameObjectCopy::init(GameObject* object) {
    m_object = object;
    m_object->retain();
    m_position = m_object->getPosition();
    m_rotationX = m_object->getRotationX();
    m_rotationY = m_object->getRotationY();
    m_isFlipX = m_object->isFlipX();
    m_isFlipY = m_object->isFlipY();
    m_customScaleX = m_object->m_scaleX;
    m_customScaleY = m_object->m_scaleY;
    return true;
}

void GameObjectCopy::resetObject() {
    m_object->setPosition(m_position);
    
    if (m_rotationX == m_rotationY) {
        m_object->setRotation(m_rotationX);
    } else {
        m_object->setRotationX(m_rotationX);
        m_object->setRotationY(m_rotationY);
    }
    
    m_object->setFlipX(m_isFlipX);
    m_object->setFlipY(m_isFlipY);
    m_object->updateCustomScaleX(m_customScaleX);
    m_object->updateCustomScaleY(m_customScaleY);
    m_object->updateStartValues();
}

GameObjectCopy::~GameObjectCopy() {
    if (m_object) {
        m_object->release();
    }
}