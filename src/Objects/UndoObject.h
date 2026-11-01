#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"

class GameObject;

class UndoObject {
public:
    static UndoObject* create(GameObject* object, UndoCommand command);
    static UndoObject* createWithArray(cocos2d::CCArray* array, UndoCommand command);
    static UndoObject* createWithTransformObjects(cocos2d::CCArray* objects, UndoCommand command);

    bool init(GameObject* object, UndoCommand command);
    bool initWithTransformObjects(cocos2d::CCArray* objects, UndoCommand command);
    void setObjects(cocos2d::CCArray* objects);

    ~UndoObject();

protected:
    GameObject* m_objectCopy = nullptr;
    UndoCommand m_command = UndoCommand::None;
    cocos2d::CCArray* m_objects = nullptr;
};