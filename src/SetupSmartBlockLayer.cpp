#include "SetupSmartBlockLayer.h"
#include "SmartGameObject.h"
#include "SelectArtLayer.h"
#include "GameLevelManager.h"
#include "GameManager.h"
#include "LevelEditorLayer.h"
#include "EditorUI.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "InfoAlertButton.h"
#include "GJSmartTemplate.h"
#include "GJSearchObject.h"
#include "LevelBrowserLayer.h"

SetupSmartBlockLayer* SetupSmartBlockLayer::create(SmartGameObject* object, cocos2d::CCArray* objects)
{
    SetupSmartBlockLayer* instance = new SetupSmartBlockLayer();
    if (instance->init(object, objects))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void SetupSmartBlockLayer::determineStartValues()
{
    GJSmartTemplate* smartTemplate = GameLevelManager::sharedState()->m_smartTemplate;
    if (smartTemplate)
    {
        m_allowRotation = smartTemplate->m_allowRotation;
        m_allowFlipX = smartTemplate->m_allowFlipX;
        m_allowFlipY = smartTemplate->m_allowFlipY;
        m_ignoreCorners = smartTemplate->m_ignoreCorners;
    }

    m_useNearby = GameManager::sharedState()->getGameVariable("0131");
    m_dontDelete = GameManager::sharedState()->getGameVariable("0132");

    if (m_gameObject)
    {
        m_referenceOnly = m_gameObject->m_referenceOnly;
    }
    else if (m_gameObjects)
    {
        m_referenceOnly = true;
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            SmartGameObject* obj = (SmartGameObject*)m_gameObjects->objectAtIndex(i);
            if (!obj->m_referenceOnly)
            {
                m_referenceOnly = false;
                break;
            }
        }
    }
}

bool SetupSmartBlockLayer::init(SmartGameObject* object, cocos2d::CCArray* objects)
{
    if (!FLAlertLayer::init(150))
        return false;

    if (object)
    {
        m_gameObject = object;
        m_gameObject->retain();
    }
    else if (objects)
    {
        m_gameObjects = objects;
        m_gameObjects->retain();
    }

    determineStartValues();
    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(300.0f, 240.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCLabelBMFont* title = cocos2d::CCLabelBMFont::create("Setup Smart Block", "bigFont.fnt");
    addChild(title);
    title->setScale(0.7f);
    title->setPosition(winSize.width * 0.5f, winSize.height * 0.5f + 120.0f - 18.0f);

    ButtonSprite* backBtn = ButtonSprite::create("Back", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_05.png", 30.0f);
    CCMenuItemSpriteExtra* backItem = CCMenuItemSpriteExtra::create(
        backBtn, nullptr, menu_selector(SetupSmartBlockLayer::onClose));

    ButtonSprite* createBtn = ButtonSprite::create("Create", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* createItem = CCMenuItemSpriteExtra::create(
        createBtn, nullptr, menu_selector(SetupSmartBlockLayer::onCreate));

    ButtonSprite* templateBtn = ButtonSprite::create("Template", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_02.png", 30.0f);
    CCMenuItemSpriteExtra* templateItem = CCMenuItemSpriteExtra::create(
        templateBtn, nullptr, menu_selector(SetupSmartBlockLayer::onCreateTemplate));

    ButtonSprite* specialBtn = ButtonSprite::create("Special", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_02.png", 30.0f);
    CCMenuItemSpriteExtra* specialItem = CCMenuItemSpriteExtra::create(
        specialBtn, nullptr, menu_selector(SetupSmartBlockLayer::onSelectPremade));

    ButtonSprite* browserBtn = ButtonSprite::create("Browser", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* browserItem = CCMenuItemSpriteExtra::create(
        browserBtn, nullptr, menu_selector(SetupSmartBlockLayer::onSelectTemplate));

    ButtonSprite* pasteBtn = ButtonSprite::create("Paste\nTemplate", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* pasteItem = CCMenuItemSpriteExtra::create(
        pasteBtn, nullptr, menu_selector(SetupSmartBlockLayer::onPasteTemplate));

    ButtonSprite* createAllBtn = ButtonSprite::create("Create\nAll", 0, 0, 0.5f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* createAllItem = CCMenuItemSpriteExtra::create(
        createAllBtn, nullptr, menu_selector(SetupSmartBlockLayer::onCreateAll));

    m_buttonMenu = cocos2d::CCMenu::create(
        backItem, createItem, templateItem, specialItem, browserItem, createAllItem, pasteItem, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->alignItemsHorizontally();
    m_buttonMenu->setPosition(winSize.width * 0.5f, winSize.height * 0.5f - 120.0f + 24.0f);

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "<cg>Auto-builds</c> structures with given templates.\n"
        "Use <cy>Browser</c> to start and select templates.\n"
        "Click <cl>Template</c> with a template made to save it.\n"
        "Create structures with these blocks and click <cp>Create</c> to paste the selected template.\n"
        "<cr>Reference Only</c> blocks are used as reference positions.\n"
        "<co>Allow Rotation, Flip X, and Flip Y</c> allow rotating the design.\n"
        "<ca>Dont Delete</c> keeps these blocks when template is pasted.\n"
        "<cy>The tool is in beta and will be enhanced in future updates.</c>",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 150.0f + 18.0f, winSize.height * 0.5f + 120.0f - 18.0f)));

    cocos2d::CCPoint toggleBase(winSize.width * 0.5f - 60.0f, winSize.height * 0.5f + 70.0f);

    GameToolbox::createToggleButton("Reference Only", menu_selector(SetupSmartBlockLayer::onReferenceOnly), 0, m_referenceOnly,
        m_buttonMenu, toggleBase, this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    GameToolbox::createToggleButton("Allow Rotation", menu_selector(SetupSmartBlockLayer::onAllowRotation), 0, m_allowRotation,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -22.0f), this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    CCMenuItemToggler* allowFlipX = GameToolbox::createToggleButton("Allow Flip X", menu_selector(SetupSmartBlockLayer::onAllowFlipping), 0, m_allowFlipX,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -44.0f), this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);
    allowFlipX->setTag(3);

    CCMenuItemToggler* allowFlipY = GameToolbox::createToggleButton("Allow Flip Y", menu_selector(SetupSmartBlockLayer::onAllowFlipping), 0, m_allowFlipY,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -66.0f), this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);
    allowFlipY->setTag(4);

    GameToolbox::createToggleButton("Ignore Corners", menu_selector(SetupSmartBlockLayer::onIgnoreCorners), 0, m_ignoreCorners,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -88.0f), this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    GameToolbox::createToggleButton("Use nearby as reference", menu_selector(SetupSmartBlockLayer::onNearbyReference), 0, m_useNearby,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -110.0f), this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    GameToolbox::createToggleButton("Dont delete", menu_selector(SetupSmartBlockLayer::onDontDelete), 0, m_dontDelete,
        m_buttonMenu, toggleBase + cocos2d::CCPoint(0.0f, -132.0f), this, m_mainLayer, 0.6f, 0.35f, 0.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false, 0, false);

    return true;
}

void SetupSmartBlockLayer::keyBackClicked()
{
    onClose(nullptr);
}

void SetupSmartBlockLayer::onAllowFlipping(cocos2d::CCObject* sender)
{
    if (!sender) return;

    GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();

    if (sender->getTag() == 3)
    {
        m_allowFlipX = !m_allowFlipX;
        if (active) active->m_allowFlipX = m_allowFlipX;
    }
    else
    {
        m_allowFlipY = !m_allowFlipY;
        if (active) active->m_allowFlipY = m_allowFlipY;
    }
}

void SetupSmartBlockLayer::onAllowRotation(cocos2d::CCObject* sender)
{
    m_allowRotation = !m_allowRotation;
    GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();
    if (active) active->m_allowRotation = m_allowRotation;
}

void SetupSmartBlockLayer::onClose(cocos2d::CCObject* sender)
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);

    if (m_gameObject)
    {
        m_gameObject->updateSmartFrame();
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
            ((SmartGameObject*)m_gameObjects->objectAtIndex(i))->updateSmartFrame();
    }

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void SetupSmartBlockLayer::onCreate(cocos2d::CCObject* sender)
{
    cocos2d::CCArray* objects = cocos2d::CCArray::create();

    if (m_gameObject)
        objects->addObject(m_gameObject);
    else if (m_gameObjects)
        objects->addObjectsFromArray(m_gameObjects);

    if (m_specialTemplate >= 0)
    {
        bool wasNine = (m_specialTemplate == 9);
        if (wasNine) m_specialTemplate = 8;

        EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
        int startKey = editorUI->getRandomStartKey(m_specialTemplate);
        int smartType = editorUI->smartTypeForKey(startKey);
        editorUI->createSmartObjectsFromType(smartType, objects, wasNine, m_dontDelete);
    }
    else
    {
        GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();
        EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
        editorUI->createSmartObjectsFromTemplate(active, objects, m_useNearby, m_dontDelete, false, false);
    }

    onClose(nullptr);
}

void SetupSmartBlockLayer::onCreateAll(cocos2d::CCObject* sender)
{
    GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();
    if (!active || !active->m_prefabArrays)
        return;

    active->m_prefabIndex = 0;
    cocos2d::CCArray* keys = active->m_prefabArrays->allKeys();

    for (unsigned int i = 0; i < keys->count(); i++)
    {
        cocos2d::CCString* key = (cocos2d::CCString*)keys->objectAtIndex(i);
        cocos2d::CCArray* prefabs = (cocos2d::CCArray*)active->m_prefabArrays->objectForKey(key->getCString());

        for (unsigned int j = 0; j < prefabs->count(); j++)
        {
            cocos2d::CCObject* prefab = prefabs->objectAtIndex(j);
            EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
            editorUI->createPrefab(active, key->getCString(), prefab->getTag());
        }
    }
}

void SetupSmartBlockLayer::onCreateTemplate(cocos2d::CCObject* sender)
{
    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    if (m_gameObject)
        objects->addObject(m_gameObject);
    else if (m_gameObjects)
        objects->addObjectsFromArray(m_gameObjects);

    GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();
    EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
    editorUI->addObjectsToSmartTemplate(active, objects);

    onClose(nullptr);
}

void SetupSmartBlockLayer::onDontDelete(cocos2d::CCObject* sender)
{
    m_dontDelete = !m_dontDelete;
    GameManager::sharedState()->setGameVariable("0132", m_dontDelete);
}

void SetupSmartBlockLayer::onIgnoreCorners(cocos2d::CCObject* sender)
{
    m_ignoreCorners = !m_ignoreCorners;
    GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();
    if (active) active->m_ignoreCorners = m_ignoreCorners;
}

void SetupSmartBlockLayer::onNearbyReference(cocos2d::CCObject* sender)
{
    m_useNearby = !m_useNearby;
    GameManager::sharedState()->setGameVariable("0131", m_useNearby);
}

void SetupSmartBlockLayer::onPasteTemplate(cocos2d::CCObject* sender)
{
    if (!GameManager::sharedState()->m_levelEditorLayer)
        return;

    EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;
    cocos2d::CCArray* pasted = editorUI->pasteObjects("SmartTemplate", false, false);

    if (pasted && pasted->count() > 0)
    {
        cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        cocos2d::CCNode* objectLayer = GameManager::sharedState()->m_levelEditorLayer->m_objectLayer;
        cocos2d::CCPoint pos(winSize.width * 0.5f, winSize.height * 0.5f);

        editorUI->getGridSnappedPos(pos);
        editorUI->repositionObjectsToCenter(pasted, pos, true);

        GJSmartTemplate* active = GameLevelManager::sharedState()->getActiveSmartTemplate();
        if (active)
            editorUI->createSmartObjectsFromTemplate(active, pasted, false, true, true, false);
    }

    onClose(nullptr);
}

void SetupSmartBlockLayer::onReferenceOnly(cocos2d::CCObject* sender)
{
    m_referenceOnly = !m_referenceOnly;

    if (m_gameObject)
    {
        m_gameObject->m_referenceOnly = m_referenceOnly;
    }
    else if (m_gameObjects)
    {
        for (unsigned int i = 0; i < m_gameObjects->count(); i++)
        {
            SmartGameObject* obj = (SmartGameObject*)m_gameObjects->objectAtIndex(i);
            obj->m_referenceOnly = m_referenceOnly;
        }
    }
}

void SetupSmartBlockLayer::onSelectPremade(cocos2d::CCObject* sender)
{
    SelectArtLayer* layer = SelectArtLayer::create(SelectArtType::Premade, -1);
    layer->updateSelectedCustom(0);
    layer->m_delegate = this;
    layer->show();
}

void SetupSmartBlockLayer::onSelectTemplate(cocos2d::CCObject* sender)
{
    GJSearchObject* search = GJSearchObject::create(SearchType::MyLevels);
    search->m_searchIsOverlay = true;
    LevelBrowserLayer* layer = LevelBrowserLayer::create(search);
    layer->show();
}

void SetupSmartBlockLayer::selectArtClosed(SelectArtLayer* layer)
{
    m_specialTemplate = layer->m_art;
}

void SetupSmartBlockLayer::show()
{
    cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    if (!m_scene)
        m_scene = runningScene;
    if (!m_ZOrder)
        m_ZOrder = 105;

    m_scene->addChild(this, m_ZOrder);
}

SetupSmartBlockLayer::~SetupSmartBlockLayer()
{
}