#include "Popups/Setup/SetupSpawnPopup.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Objects/EffectGameObject.h"
#include "Managers/GameManager.h"
#include "Layers/LevelEditorLayer.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"

SetupSpawnPopup* SetupSpawnPopup::create(EffectGameObject* object, cocos2d::CCArray* objects)
{
    SetupSpawnPopup* instance = new SetupSpawnPopup();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupSpawnPopup::init(EffectGameObject* object, cocos2d::CCArray* objects)
{
    if (!SetupTriggerPopup::init(object, objects, 380.0f, 280.0f, 1))
        return false;

    preSetup();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    addTitle("Spawn Group");
    addCloseButton("OK");
    addHelpButton(
        "Help",
        "Activates other triggers simultaneously regardless of X position. These triggers need to use <cg>Group ID</c> and "
        "have <cy>Spawn Trigger</c> enabled.\n"
        "<cl>Preview Disable</c> disables activation while not playtesting in editor.\n"
        "<cp>Spawn Ordered</c> spawns the triggers in order from left to right.\n"
        "Use remapping (page 2) to copy a Spawn setup to another group.",
        1.0f);

    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint groupIDPos = center + cocos2d::CCPoint(-50.0f, 50.0f);
    createValueControlAdvanced(
        51, "Group ID", cocos2d::CCPoint(0.9f, 0.0f), 100.0f, &groupIDPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 0, GoldLabel, 0, 2);

    cocos2d::CCPoint delayPos = center + cocos2d::CCPoint(-85.0f, -35.0f);
    createValueControlAdvanced(
        63, "Delay:", cocos2d::CCPoint(0.7f, 0.0f), 1.0f, &delayPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 5);

    cocos2d::CCPoint delayVarPos = center + cocos2d::CCPoint(80.0f, -35.0f);
    createValueControlAdvanced(
        556, "+-", cocos2d::CCPoint(0.7f, 0.0f), 1.0f, &delayVarPos,
        (InputValueType)0, 1, 10, 0.0f, 0.0f, 0, 0, GoldLabel, 1, 5);

    cocos2d::CCPoint previewPos = center + cocos2d::CCPoint(50.0f, 85.0f);
    createToggleValueControl(102, "Preview\nDisable", cocos2d::CCPoint(1.0f, 0.0f), &previewPos, 0, 0, 0.0f);

    cocos2d::CCPoint orderedPos = center + cocos2d::CCPoint(50.0f, 50.0f);
    createToggleValueControl(441, "Spawn\nOrdered", cocos2d::CCPoint(1.0f, 0.0f), &orderedPos, 0, 0, 0.0f);

    cocos2d::CCPoint resetPos = center + cocos2d::CCPoint(50.0f, 15.0f);
    createToggleValueControl(581, "Reset\nRemap", cocos2d::CCPoint(1.0f, 0.0f), &resetPos, 0, 0, 0.0f);

    m_remapButtons = cocos2d::CCArray::create();
    m_remapButtons->retain();

    cocos2d::CCPoint remapBase(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint originalIDPos = remapBase + cocos2d::CCPoint(-100.0f, 65.0f);
    createValueControlAdvanced(
        -1, "OriginalID:", cocos2d::CCPoint(0.7f, 0.0f), 100.0f, &originalIDPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 2);

    cocos2d::CCPoint newIDPos = remapBase + cocos2d::CCPoint(20.0f, 65.0f);
    createValueControlAdvanced(
        -2, "NewID:", cocos2d::CCPoint(0.7f, 0.0f), 100.0f, &newIDPos,
        Float, 3, 6, 0.0f, 0.0f, 1, 1, GoldLabel, 0, 2);

    cocos2d::CCSprite* addIcon = cocos2d::CCSprite::createWithSpriteFrameName("edit_addCBtn_001.png");
    ButtonSprite* addBtn = ButtonSprite::create(addIcon, 0, 0, 1.0f, "GJ_button_01.png");
    CCMenuItemSpriteExtra* addItem = CCMenuItemSpriteExtra::create(
        addBtn, nullptr, menu_selector(SetupSpawnPopup::onAddRemap));
    addItem->setScale(0.6f);
    addItem->m_baseScale = 0.6f;
    m_buttonMenu->addChild(addItem);
    addItem->setPosition(m_buttonMenu->convertToNodeSpace(remapBase + cocos2d::CCPoint(120.0f, 65.0f)));
    addObjectToPage(addItem, 1);

    cocos2d::CCSprite* delIcon = cocos2d::CCSprite::createWithSpriteFrameName("edit_delCBtn_001.png");
    ButtonSprite* delBtn = ButtonSprite::create(delIcon, 0, 0, 1.0f, "GJ_button_06.png");
    CCMenuItemSpriteExtra* delItem = CCMenuItemSpriteExtra::create(
        delBtn, nullptr, menu_selector(SetupSpawnPopup::onDeleteRemap));
    delItem->setScale(0.6f);
    delItem->m_baseScale = 0.6f;
    m_buttonMenu->addChild(delItem);
    delItem->setPosition(m_buttonMenu->convertToNodeSpace(remapBase + cocos2d::CCPoint(150.0f, 65.0f)));
    addObjectToPage(delItem, 1);

    if (!GameManager::sharedState()->m_levelEditorLayer->m_copiedRemaps.empty())
    {
        ButtonSprite* pasteBtn = ButtonSprite::create("Paste", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_04.png", 30.0f);
        pasteBtn->setScale(0.8f);
        CCMenuItemSpriteExtra* pasteItem = CCMenuItemSpriteExtra::create(
            pasteBtn, nullptr, menu_selector(SetupSpawnPopup::onPasteRemap));
        m_buttonMenu->addChild(pasteItem);
        pasteItem->setPosition(m_buttonMenu->convertToNodeSpace(
            cocos2d::CCPoint(m_width * 0.5f + winSize.width * 0.5f - 25.0f,
                             m_height * 0.5f + winSize.height * 0.5f - 20.0f)));
        addObjectToPage(pasteItem, 1);
    }

    cocos2d::extension::CCScale9Sprite* remapBg = cocos2d::extension::CCScale9Sprite::create(
        "square02_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    remapBg->setOpacity(50);
    remapBg->setContentSize(cocos2d::CCSize(360.0f, 90.0f));
    m_mainLayer->addChild(remapBg, -1);
    remapBg->setPosition(remapBase);
    addObjectToPage(remapBg, 1);

    updateRemapButtons(0.0f);

    createMultiTriggerItemsDefault();
    createPageButtons(m_width * 0.5f + 15.0f, 1);
    goToPage(0, false);

    postSetup();

    return true;
}

void SetupSpawnPopup::onAddRemap(cocos2d::CCObject* sender)
{
    if (m_isBusy)
        return;

    int originalID = (int)getValue(-1);
    int newID = (int)getValue(-2);
    addRemap(originalID, newID);
}

void SetupSpawnPopup::onClose(cocos2d::CCObject* sender)
{
    GameManager::sharedState()->m_levelEditorLayer->m_unk3751 = true;
    SetupTriggerPopup::onClose(sender);
}

void SetupSpawnPopup::onDeleteRemap(cocos2d::CCObject* sender)
{
    if (m_isBusy)
        return;

    if (m_remapOriginalID == 0)
        return;

    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            obj->removeRemap(m_remapOriginalID, m_remapNewID);
        }
    }

    m_remapOriginalID = 0;
    m_remapNewID = 0;
    queueUpdateButtons();
}

void SetupSpawnPopup::onPasteRemap(cocos2d::CCObject* sender)
{
    for (auto& remap : GameManager::sharedState()->m_levelEditorLayer->m_copiedRemaps)
        addRemap(remap.first, remap.second);
}

void SetupSpawnPopup::onSelectRemap(cocos2d::CCObject* sender)
{
    if (m_isBusy)
        return;

    int tag = sender->getTag();
    if (tag < 0 || tag >= (int)m_remapGroups.size())
        return;

    int originalID = m_remapGroups[tag].first;
    int newID = m_remapGroups[tag].second;

    if (m_remapOriginalID == originalID && m_remapNewID == newID)
    {
        m_remapOriginalID = 0;
        m_remapNewID = 0;
    }
    else
    {
        m_remapOriginalID = originalID;
        m_remapNewID = newID;
        updateValue(-1, (float)originalID);
        updateValue(-2, (float)newID);
    }

    queueUpdateButtons();
}

void SetupSpawnPopup::queueUpdateButtons()
{
    if (m_isBusy)
        return;

    cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleSelector(
        schedule_selector(SetupSpawnPopup::updateRemapButtons), this, 0.0f, false);
}

void SetupSpawnPopup::updateRemapButtons(float dt)
{
    unschedule(schedule_selector(SetupSpawnPopup::updateRemapButtons));
    m_isBusy = false;

    for (unsigned int i = 0; i < m_remapButtons->count(); i++)
    {
        cocos2d::CCNode* node = (cocos2d::CCNode*)m_remapButtons->objectAtIndex(i);
        removeObjectFromPage(node, 1);
        node->removeFromParentAndCleanup(true);
    }
    m_remapButtons->removeAllObjects();
    m_remapGroups.clear();

    if (m_gameObject)
        m_gameObjects = cocos2d::CCArray::createWithObject(m_gameObject);

    if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            EffectGameObject* obj = (EffectGameObject*)m_gameObjects->objectAtIndex(i);
            for (auto& remap : obj->m_remapGroups)
            {
                m_remapGroups.push_back(remap);
            }
        }
    }

    std::sort(m_remapGroups.begin(), m_remapGroups.end());

    bool many = m_remapGroups.size() > 10;
    float colWidth = many ? 35.0f : 66.0f;
    int columns = many ? 10 : 5;
    float rowHeight = many ? 38.0f : 66.0f;
    float spriteWidth = many ? 15.0f : 40.0f;
    float spriteHeight = many ? 4.5f : 2.0f;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint base = m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(-(spriteWidth * colWidth - winSize.width * 0.5f),
                         winSize.height * 0.5f + 45.0f - 25.0f));

    for (size_t i = 0; i < m_remapGroups.size() && i < 20; i++)
    {
        int originalID = m_remapGroups[i].first;
        int newID = m_remapGroups[i].second;

        const char* bgImage;
        if (m_remapOriginalID == originalID && m_remapNewID == newID)
            bgImage = "GJ_button_03.png";
        else if ((int)m_gameObjects->count() == 0)
            bgImage = "GJ_button_04.png";
        else
            bgImage = "GJ_button_05.png";

        std::string text = cocos2d::CCString::createWithFormat("%i\n%i", originalID, newID)->getCString();
        ButtonSprite* btn = ButtonSprite::create(text.c_str(), 0, 0, 0.35f, true, "bigFont.fnt", bgImage, 30.0f);
        CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
            btn, nullptr, menu_selector(SetupSpawnPopup::onSelectRemap));
        item->setTag(i);
        m_buttonMenu->addChild(item);
        addObjectToPage(item, 1);

        item->setPosition(base + cocos2d::CCPoint(
            colWidth * (int)(i % columns),
            -rowHeight * (int)(i / columns)));

        m_remapButtons->addObject(item);
    }
}

SetupSpawnPopup::~SetupSpawnPopup()
{
}