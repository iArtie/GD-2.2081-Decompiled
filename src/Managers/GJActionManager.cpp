#include "Managers/GJActionManager.h"

GJActionManager* GJActionManager::create()
{
    GJActionManager* instance = new GJActionManager();
    if (instance && instance->init())
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool GJActionManager::init()
{
    if (!cocos2d::CCNode::init())
        return false;

    m_internalActions = cocos2d::CCDictionary::create();
    m_internalActions->retain();
    return true;
}

void GJActionManager::runInternalAction(cocos2d::CCAction* action, cocos2d::CCNode* target)
{
    if (!action || !target)
        return;

    m_internalActions->setObject(action, action->m_nTag);
    action->startWithTarget(target);
}

cocos2d::CCAction* GJActionManager::getInternalAction(int id)
{
    return (cocos2d::CCAction*)m_internalActions->objectForKey(id);
}

void GJActionManager::stopInternalAction(int id)
{
    cocos2d::CCObject* obj = m_internalActions->objectForKey(id);
    if (obj)
    {
        // Cleanup action
        m_internalActions->removeObjectForKey(id);
    }
}

void GJActionManager::stopAllInternalActions()
{
    updateInternalActions(0.0f, true);
}

void GJActionManager::updateInternalActions(float dt, bool remove)
{
    cocos2d::CCArray* keys = m_internalActions->allKeys();
    if (!keys || keys->count() == 0)
        return;

    for (int i = keys->count() - 1; i >= 0; i--)
    {
        int key = (int)keys->objectAtIndex(i);
        cocos2d::CCObject* obj = m_internalActions->objectForKey(key);
        if (!obj)
            continue;

        // Check if action is done
        bool isDone = obj->isDone();

        if (isDone || remove)
        {
            m_internalActions->removeObjectForKey(key);
        }
        else
        {
            // Update action with delta time
            obj->step(dt);
        }
    }
}

GJActionManager::~GJActionManager()
{
    if (m_internalActions)
        m_internalActions->release();
}