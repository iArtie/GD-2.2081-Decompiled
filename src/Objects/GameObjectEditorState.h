// GameObjectEditorState.h
#pragma once

#include "Platform/cocos2d.h"
#include "Objects/GameObject.h"

class GameObjectEditorState {
public:
    void loadValues(GameObject* obj);

private:
    cocos2d::CCPoint m_position;
    float m_scaleX;
    float m_scaleY;
    float m_rotationX;
    float m_rotationY;
};