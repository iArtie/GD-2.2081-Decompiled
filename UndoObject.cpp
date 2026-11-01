#include "UndoObject.h"
#include "GameObject.h"

UndoObject* UndoObject::create(GameObject* object, UndoCommand command)
{
    UndoObject* undoObject = new UndoObject();

    if (undoObject->init(object, command))
    {
        undoObject->autorelease();
        return undoObject;
    }

    delete undoObject;
    return nullptr;
}

UndoObject* UndoObject::createWithArray(cocos2d::CCArray* array, UndoCommand command)
{
    UndoObject* undoObject = new UndoObject();

    undoObject->setObjects(array);
    undoObject->m_command = command;
    undoObject->autorelease();
    return undoObject;
}

UndoObject* UndoObject::createWithTransformObjects(cocos2d::CCArray* objects, UndoCommand command)
{
    UndoObject* undoObject = new UndoObject();

    if (undoObject->initWithTransformObjects(objects, command))
    {
        undoObject->autorelease();
        return undoObject;
    }

    delete undoObject;
    return nullptr;
}

bool UndoObject::init(GameObject* object, UndoCommand command)
{
    m_objectCopy = object;
    m_command = command;

    if (m_objectCopy)
        m_objectCopy->retain();

    return true;
}

bool UndoObject::initWithTransformObjects(cocos2d::CCArray* objects, UndoCommand command)
{
    m_objects = objects;
    m_command = command;

    if (m_objects)
        m_objects->retain();

    return true;
}

void UndoObject::setObjects(cocos2d::CCArray* objects)
{
    if (m_objects)
        m_objects->release();

    m_objects = objects;

    if (m_objects)
        m_objects->retain();
}

UndoObject::~UndoObject()
{
    if (m_objectCopy)
        m_objectCopy->release();

    if (m_objects)
        m_objects->release();
}