#include "Popups/PurchaseItemPopup.h"
#include "Models/GJStoreItem.h"
#include "Delegates/GJPurchaseDelegate.h"
#include "Managers/GameStatsManager.h"
#include "Utils/GameToolbox.h"
#include "UI/GJItemIcon.h"
#include "Popups/ItemInfoPopup.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/TextArea.h"
#include "UI/InfoAlertButton.h"

PurchaseItemPopup* PurchaseItemPopup::create(GJStoreItem* item)
{
    PurchaseItemPopup* returnVal = new PurchaseItemPopup();

    if (returnVal->init(item))
    {
        returnVal->autorelease();
        return returnVal;
    }

    delete returnVal;
    return nullptr;
}

bool PurchaseItemPopup::init(GJStoreItem* item)
{
    if (!FLAlertLayer::init(150))
        return false;

    m_storeItem = item;
    m_storeItem->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLayer* mainLayer = cocos2d::CCLayer::create();
    m_mainLayer = mainLayer;
    addChild(mainLayer);

    float bgWidth = (m_storeItem->m_unlockType.random - m_storeItem->m_unlockType.seed == 11) ? 280.0f : 260.0f;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png");
    bg->setContentSize(cocos2d::CCSize(bgWidth, 190.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 95.0f - 16.0f));
    m_mainLayer->addChild(titleLabel);
    titleLabel->setScale(0.6f);

    int typeID = m_storeItem->m_typeID.random - m_storeItem->m_typeID.seed;
    UnlockType unlockType = (UnlockType)(m_storeItem->m_unlockType.random - m_storeItem->m_unlockType.seed);

    GJItemIcon* storeIcon = GJItemIcon::createStoreItem(unlockType, typeID, false, cocos2d::ccc3(255, 255, 255));
    storeIcon->setScale(1.2f);

    bool hasCustomName = false;
    float iconYOffset = 0.0f;

    if (unlockType == UnlockType::Bird)
    {
        iconYOffset = -8.0f;
    }
    else if (unlockType == UnlockType::GJItem && (unsigned int)(typeID - 6) <= 9)
    {
        hasCustomName = true;

        switch (typeID)
        {
            case 7: iconYOffset = 12.0f; break;
            case 8:
            case 10:
            case 12: iconYOffset = 15.0f; break;
            case 9:
            case 13:
            case 14:
            case 15: iconYOffset = 10.0f; break;
            case 11: iconYOffset = 5.0f; break;
            default: iconYOffset = 0.0f; break;
        }
    }
    else
    {
        hasCustomName = (unlockType == UnlockType::GJItem);
    }

    m_mainLayer->addChild(storeIcon, 1);
    storeIcon->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 22.0f + iconYOffset));

    std::string itemName = ItemInfoPopup::nameForUnlockType(typeID, unlockType);

    const char* currencyName = (item->m_shopType == ShopType::Diamond) ? "<cc>Diamond Shards</c>" : "<cj>Mana Orbs</c>";

    std::string priceStr = GameToolbox::pointsToString(m_storeItem->m_price.random - m_storeItem->m_price.seed);

    std::string desc;
    if (hasCustomName)
        desc = cocos2d::CCString::createWithFormat(
            "Do you want to <cg>unlock</c><co>%s</c>\nfor <cy>%s</c> %s?",
            itemName.c_str(), priceStr.c_str(), currencyName)->getCString();
    else
        desc = cocos2d::CCString::createWithFormat(
            "Do you want to <cg>buy</c> this <co>%s</c>\nfor <cy>%s</c> %s?",
            itemName.c_str(), priceStr.c_str(), currencyName)->getCString();

    TextArea* textArea = TextArea::create(desc, "bigFont.fnt", 1.0f, 999.0f, cocos2d::CCPoint(0.5f, 1.0f), 35.0f, false);
    textArea->setScale(0.4f);
    m_mainLayer->addChild(textArea);
    textArea->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 20.0f));

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCArray* buttons = cocos2d::CCArray::create();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_06.png", 30.0f);
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(cancelSprite, nullptr, menu_selector(PurchaseItemPopup::onClose));
    m_buttonMenu->addChild(cancelBtn);
    buttons->addObject(cancelBtn);

    ButtonSprite* buySprite = ButtonSprite::create("Buy", 0, 0, 0.6f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f);
    CCMenuItemSpriteExtra* buyBtn = CCMenuItemSpriteExtra::create(buySprite, nullptr, menu_selector(PurchaseItemPopup::onPurchase));
    m_buttonMenu->addChild(buyBtn);
    buttons->addObject(buyBtn);

    cocos2d::CCPoint menuPos(
        winSize.width * 0.5f,
        winSize.height * 0.5f - 95.0f + 22.0f + m_mainLayer->getPosition().y);
    cocos2d::CCPoint nodePos = m_buttonMenu->convertToNodeSpace(menuPos);
    GameToolbox::alignItemsHorizontally(buttons, 15.0f, nodePos);

    if (hasCustomName)
    {
        std::string infoText;

        if (typeID == 16)
        {
            infoText =
                "This item lets you <co>customize Practice and Menu music</c>.\n"
                "After purchasing this item you will be able to change practice and menu music from the more info page on custom songs.";
        }
        else if (typeID == 17)
        {
            infoText =
                "This item lets you play\n"
                "<co>Normal mode music in sync in Practice mode</c>.\n"
                "After purchasing this item you will be able to toggle the option in the game menu.";
        }

        if ((unsigned int)(typeID - 6) <= 9)
        {
            infoText =
                "When you activate a path, <cy>Stars</c> and <cl>Moons</c> you <cg>collect</c> will level up your <co>Path</c>.\n"
                "You can unlock multiple paths, but <cr>only one</c> can be <cr>active</c> at a time.";
        }

        if (!infoText.empty())
        {
            InfoAlertButton* infoBtn = InfoAlertButton::create(itemName, infoText, 1.0f);
            m_buttonMenu->addChild(infoBtn);

            cocos2d::CCPoint infoPos(
                bgWidth * 0.5f + winSize.width * 0.5f - 20.0f,
                winSize.height * 0.5f + 95.0f - 18.0f);
            infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(infoPos));
        }
    }

    return true;
}

void PurchaseItemPopup::keyBackClicked()
{
    onClose(nullptr);
}

void PurchaseItemPopup::onClose(cocos2d::CCObject* sender)
{
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void PurchaseItemPopup::onPurchase(cocos2d::CCObject* sender)
{
    if (GameStatsManager::sharedState()->purchaseItem(m_storeItem->m_index.random - m_storeItem->m_index.seed))
    {
        if (m_delegate)
            m_delegate->didPurchaseItem(m_storeItem);
    }

    onClose(nullptr);
}

PurchaseItemPopup::~PurchaseItemPopup()
{
}