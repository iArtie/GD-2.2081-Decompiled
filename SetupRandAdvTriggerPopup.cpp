#include "SetupRandAdvTriggerPopup.h"
#include "SetupTriggerPopup.h"
#include "RandTriggerGameObject.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCTextInputNode.h"

SetupRandAdvTriggerPopup* SetupRandAdvTriggerPopup::create(RandTriggerGameObject* object, cocos2d::CCArray* objects)
{
    SetupRandAdvTriggerPopup* instance = new SetupRandAdvTriggerPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupRandAdvTriggerPopup::addChance(int id, int chance)
{
    m_performedAction = true;

    if (m_gameObject)
    {
        m_gameObject->addChanceToObject(id, chance);
        updateGroupIDButtons();
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            RandTriggerGameObject* obj = (RandTriggerGameObject*)m_gameObjects->objectAtIndex(i);
            obj->addChanceToObject(id, chance);
        }
        updateGroupIDButtons();
    }
}

void SetupRandAdvTriggerPopup::callRemoveFromGroup(float dt)
{
    unschedule(schedule_selector(SetupRandAdvTriggerPopup::callRemoveFromGroup));
    m_removingGroup = false;
    removeGroupID(m_groupToRemove);
    m_groupToRemove = 0;
}

void SetupRandAdvTriggerPopup::onAddChance(cocos2d::CCObject* sender)
{
    int groupID = atoi(m_groupIDInput->getString().c_str());
    int chance = atoi(m_chanceInput->getString().c_str());

    if (chance > 0 && groupID > 0)
    {
        addChance(groupID, chance);
        updateGroupIDButtons();
    }
}

void SetupRandAdvTriggerPopup::onClose(cocos2d::CCObject* sender)
{
    m_groupIDInput->m_delegate = nullptr;
    m_groupIDInput->onClickTrackNode(false);
    m_chanceInput->m_delegate = nullptr;
    m_chanceInput->onClickTrackNode(false);
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupRandAdvTriggerPopup::onRemoveFromGroup(cocos2d::CCObject* sender)
{
    if (m_removingGroup)
        return;

    for (unsigned int i = 0; i < m_groupButtons->count(); i++)
    {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_groupButtons->objectAtIndex(i);
        node->setVisible(false);
    }

    m_removingGroup = true;
    m_groupToRemove = sender->getTag();

    cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
        schedule_selector(SetupRandAdvTriggerPopup::callRemoveFromGroup), this, 0.0f, false);
}

void SetupRandAdvTriggerPopup::removeGroupID(int id)
{
    m_performedAction = true;

    if (m_gameObject)
    {
        m_gameObject->removeGroupIDFromObject(id);
        updateGroupIDButtons();
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            RandTriggerGameObject* obj = (RandTriggerGameObject*)m_gameObjects->objectAtIndex(i);
            obj->removeGroupIDFromObject(id);
        }
        updateGroupIDButtons();
    }
}

void SetupRandAdvTriggerPopup::textChanged(CCTextInputNode* node)
{
}

void SetupRandAdvTriggerPopup::updateGroupIDButtons()
{
    for (unsigned int i = 0; i < m_groupButtons->count(); i++)
    {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_groupButtons->objectAtIndex(i);
        node->removeFromParentAndCleanup(true);
    }
    m_groupButtons->removeAllObjects();

    std::vector<ChanceObject*> chanceObjects;

    if (m_gameObject)
    {
        for (auto it = m_gameObject->m_chanceObjects.begin(); it != m_gameObject->m_chanceObjects.end(); ++it)
            chanceObjects.push_back(*it);
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            RandTriggerGameObject* obj = (RandTriggerGameObject*)m_gameObjects->objectAtIndex(i);
            for (auto it = obj->m_chanceObjects.begin(); it != obj->m_chanceObjects.end(); ++it)
                chanceObjects.push_back(*it);
        }
    }

    std::sort(chanceObjects.begin(), chanceObjects.end(), chanceObjCompare);

    int totalChance = 0;
    for (size_t i = 0; i < chanceObjects.size(); i++)
        totalChance += chanceObjects[i]->m_chance;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float startX = -((4.5f * 40.0f) - winSize.width * 0.5f);
    float startY = winSize.height * 0.5f + 10.0f;
    cocos2d::CCPoint local = m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(startX, startY));

    int columns = (chanceObjects.size() > 10) ? 10 : 1;
    float colWidth = (chanceObjects.size() > 10) ? 40.0f : 66.0f;
    float rowHeight = (chanceObjects.size() > 10) ? 38.0f : 66.0f;

    for (size_t i = 0; i < chanceObjects.size() && i < 20; i++)
    {
        ChanceObject* chance = chanceObjects[i];
        int groupID = chance->m_groupID;
        int chanceValue = chance->m_chance;

        float percentage = (totalChance > 0) ? roundf((float)chanceValue / (float)totalChance * 100.0f) : 0.0f;

        std::string text;
        if (i == 0)
        {
            if (chanceValue <= 0)
                text = cocos2d::CCString::createWithFormat("%i - ?", groupID)->getCString();
            else
                text = cocos2d::CCString::createWithFormat("%i - %i", groupID, chanceValue)->getCString();
        }
        else
        {
            if (chanceValue <= 0)
                text = cocos2d::CCString::createWithFormat("%i - ?\n?%%", groupID)->getCString();
            else
                text = cocos2d::CCString::createWithFormat("%i - %i\n%i%%", groupID, chanceValue, (int)percentage)->getCString();
        }

        const char* bgImage = "GJ_button_04.png";
        if (!m_gameObject && groupID >= (int)m_gameObjects->count())
            bgImage = "GJ_button_05.png";

        ButtonSprite* button = ButtonSprite::create(
            text.c_str(), 0, 0, 0.35f, true, "bigFont.fnt", bgImage, 30.0f);

        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            button, nullptr, menu_selector(SetupRandAdvTriggerPopup::onRemoveFromGroup));
        item->setTag(groupID);
        m_buttonMenu->addChild(item);

        item->setPosition(local + cocos2d::CCPoint(
            colWidth * (float)((int)i % columns),
            -rowHeight * (float)((int)i / columns)));

        m_groupButtons->addObject(item);
    }
}

SetupRandAdvTriggerPopup::~SetupRandAdvTriggerPopup()
{
}