#include "Popups/Setup/SetupSmartTemplateLayer.h"
#include "Editor/GJSmartTemplate.h"
#include "GameLevelManager.h"
#include "Layers/BrowseSmartTemplateLayer.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Cocos/CCTextInputNode.h"
#include "UI/InfoAlertButton.h"
#include "Models/GJSearchObject.h"
#include "Layers/LevelBrowserLayer.h"

void SetupSmartTemplateLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2)
{
    if (layer->getTag() == 5 && btn2)
    {
        GameLevelManager::sharedState()->deleteSmartTemplate(m_smartTemplate);
        onBack(nullptr);
    }
}

SetupSmartTemplateLayer* SetupSmartTemplateLayer::create(GJSmartTemplate* smartTemplate)
{
    SetupSmartTemplateLayer* instance = new SetupSmartTemplateLayer();
    if (instance->init(smartTemplate))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

bool SetupSmartTemplateLayer::init(GJSmartTemplate* smartTemplate)
{
    if (!FLAlertLayer::init(150))
        return false;

    if (smartTemplate)
    {
        m_smartTemplate = smartTemplate;
        m_smartTemplate->retain();
    }

    m_noElasticity = true;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(450.0f, 280.0f));
    m_mainLayer->addChild(bg, -2);
    bg->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* closeIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    closeIcon->setScale(0.7f);
    CCMenuItemSpriteExtra* closeItem = CCMenuItemSpriteExtra::create(
        closeIcon, nullptr, menu_selector(SetupSmartTemplateLayer::onBack));
    closeItem->setSizeMult(1.6f);
    m_buttonMenu->addChild(closeItem);
    closeItem->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f - 225.0f + 6.0f, winSize.height * 0.5f + 140.0f - 6.0f)));

    InfoAlertButton* helpButton = InfoAlertButton::create(
        "Help",
        "<cg>Defined</c> tells you how many variations you have in the template.\n"
        "Click <cy>Browse</c> to see your template's defined variations.",
        1.0f);
    m_buttonMenu->addChild(helpButton);
    helpButton->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 225.0f - 18.0f, winSize.height * 0.5f + 140.0f - 18.0f)));

    std::string templateName = m_smartTemplate ? m_smartTemplate->m_templateName : "Unnamed";

    cocos2d::CCPoint namePos(winSize.width * 0.5f, winSize.height * 0.5f + 140.0f - 40.0f);

    cocos2d::extension::CCScale9Sprite* nameBg = cocos2d::extension::CCScale9Sprite::create(
        "square02b_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    nameBg->setOpacity(75);
    nameBg->setContentSize(cocos2d::CCSize(600.0f, 80.0f));
    m_mainLayer->addChild(nameBg, -1);
    nameBg->setPosition(namePos);
    nameBg->setScale(0.5f);

    m_nameInput = CCTextInputNode::create(280.0f, 50.0f, "Template Name", "Thonburi");
    m_mainLayer->addChild(m_nameInput, 18);
    m_nameInput->m_delegate = this;
    m_nameInput->setTag(0);
    m_nameInput->setPosition(namePos);
    m_nameInput->m_maxLabelLength = 20;
    m_nameInput->m_filterSwearWords = true;
    m_nameInput->setString(templateName.c_str());
    m_nameInput->setLabelPlaceholderColor(cocos2d::ccColor3B(255, 255, 0));
    m_nameInput->setLabelPlaceholderScale(0.7f);

    if (m_smartTemplate)
        m_smartTemplate->logTemplateStatus(false);

    cocos2d::CCLabelBMFont* statusLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_mainLayer->addChild(statusLabel, 18);
    statusLabel->setPosition(namePos + cocos2d::CCPoint(0.0f, -40.0f));
    statusLabel->limitLabelWidth(300.0f, 0.6f, 0.0f);

    ButtonSprite* browseBtn = ButtonSprite::create("Browse", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* browseItem = CCMenuItemSpriteExtra::create(
        browseBtn, nullptr, menu_selector(SetupSmartTemplateLayer::onClick));
    browseItem->setTag(0);
    m_buttonMenu->addChild(browseItem);
    browseItem->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f)));

    ButtonSprite* deleteBtn = ButtonSprite::create("Delete", 0, 0, 0.8f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* deleteItem = CCMenuItemSpriteExtra::create(
        deleteBtn, nullptr, menu_selector(SetupSmartTemplateLayer::onClick));
    deleteItem->setTag(5);
    m_buttonMenu->addChild(deleteItem);
    deleteItem->setPosition(m_buttonMenu->convertToNodeSpace(
        cocos2d::CCPoint(winSize.width * 0.5f + 100.0f, winSize.height * 0.5f)));

    return true;
}

void SetupSmartTemplateLayer::keyBackClicked()
{
    onBack(nullptr);
}

void SetupSmartTemplateLayer::onBack(cocos2d::CCObject* sender)
{
    onClose(sender);

    GJSearchObject* search = GJSearchObject::create(SearchType::MyLevels);
    search->m_searchIsOverlay = true;
    LevelBrowserLayer* layer = LevelBrowserLayer::create(search);
    layer->show();
}

void SetupSmartTemplateLayer::onClick(cocos2d::CCObject* sender)
{
    int tag = sender->getTag();

    if (tag == 5)
    {
        FLAlertLayer* alert = FLAlertLayer::create(
            this, "Delete Template", "Are you sure you want to <cr>delete</c> this template?");
        alert->setTag(5);
        alert->show();
    }
    else
    {
        BrowseSmartTemplateLayer* browseLayer = BrowseSmartTemplateLayer::create(m_smartTemplate, SmartBrowseFilter());
        browseLayer->show();
        onClose(nullptr);
    }
}

void SetupSmartTemplateLayer::onClose(cocos2d::CCObject* sender)
{
    if (m_unk298)
        m_unk298->setVisible(true);

    m_nameInput->onClickTrackNode(false);

    if (m_smartTemplate && m_smartTemplate->m_templateName.empty())
    {
        m_smartTemplate->m_templateName = cocos2d::CCString::createWithFormat(
            "SmartTemplate_%i", m_smartTemplate->m_nameIndex)->getCString();
    }

    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);

    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
}

void SetupSmartTemplateLayer::textChanged(CCTextInputNode* node)
{
    node->getTag();
}

void SetupSmartTemplateLayer::textInputClosed(CCTextInputNode* node)
{
    int tag = node->getTag();

    if (tag == 0 && m_smartTemplate)
    {
        m_smartTemplate->m_templateName = node->getString();
    }
}

void SetupSmartTemplateLayer::textInputOpened(CCTextInputNode* node)
{
    int tag = node->getTag();

    if (tag == 0 && node->getString().empty())
    {
        node->updateLabel(" ");
    }
}

SetupSmartTemplateLayer::~SetupSmartTemplateLayer()
{
}