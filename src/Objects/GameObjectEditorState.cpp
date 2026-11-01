// GameObjectEditorState.cpp
#include "Objects/GameObjectEditorState.h"

void GameObjectEditorState::loadValues(GameObject* obj) {
    m_position = obj->getPosition();
    m_scaleX = obj->m_scaleX / obj->m_pixelScaleX;
    m_scaleY = obj->m_scaleY / obj->m_pixelScaleY;
    m_rotationX = obj->getRotationX();
    m_rotationY = obj->getRotationY();
}