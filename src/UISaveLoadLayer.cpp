#include "UISaveLoadLayer.h"
#include "UIOptionsLayer.h"

UISaveLoadLayer* UISaveLoadLayer::create(UIOptionsLayer* layer)
{
    UISaveLoadLayer* saveLoadLayer = new UISaveLoadLayer();

    if (saveLoadLayer->init(layer))
    {
        saveLoadLayer->autorelease();
        return saveLoadLayer;
    }

    delete saveLoadLayer;
    return nullptr;
}

bool UISaveLoadLayer::init(UIOptionsLayer* layer)
{
    if (!SetupTriggerPopup::init(nullptr, nullptr, 300.0f, 200.0f, 1))
        return false;

    m_optionsLayer = layer;

    preSetup();

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu);

    cocos2d::CCNode* saveSprite = ButtonSprite::create("Save", 0, 0, 1.0f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* saveButton = CCMenuItemSpriteExtra::create(saveSprite, nullptr, menu_selector(UISaveLoadLayer::onSave));
    m_buttonMenu->addChild(saveButton);
    saveButton->setPosition(cocos2d::CCPoint(-60.0f, 0.0f));

    cocos2d::CCNode* loadSprite = ButtonSprite::create("Load", 0, 0, 1.0f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* loadButton = CCMenuItemSpriteExtra::create(loadSprite, nullptr, menu_selector(UISaveLoadLayer::onLoad));
    m_buttonMenu->addChild(loadButton);
    loadButton->setPosition(cocos2d::CCPoint(60.0f, 0.0f));

    postSetup();

    return true;
}

void UISaveLoadLayer::onLoad(cocos2d::CCObject* sender)
{
}

void UISaveLoadLayer::onSave(cocos2d::CCObject* sender)
{
}

UISaveLoadLayer::~UISaveLoadLayer()
{
}