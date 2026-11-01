// GameObjectEditorState.h
#pragma once

#include "cocos2d.h"
#include "GameObject.h"

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