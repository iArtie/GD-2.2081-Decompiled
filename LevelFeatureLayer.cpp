#include "LevelFeatureLayer.h"
#include "GameLevelManager.h"
#include "GameToolbox.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "cocos-ext.h"

LevelFeatureLayer* LevelFeatureLayer::create(int levelID) {
    LevelFeatureLayer* layer = new LevelFeatureLayer();
    if (layer && layer->init(levelID)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

bool LevelFeatureLayer::init(int levelID) {
    if (!FLAlertLayer::init(75)) {
        return false;
    }

    m_levelID = levelID;
    m_featurePosition = 0;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    bg->setContentSize(cocos2d::CCSize(360.0f, 180.0f));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create("Feature Level", "bigFont.fnt");
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 90.0f - 20.0f));
    m_mainLayer->addChild(titleLabel);

    m_buttonMenu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCArray* buttonArray = cocos2d::CCArray::create();

    ButtonSprite* cancelSprite = ButtonSprite::create("Cancel");
    CCMenuItemSpriteExtra* cancelBtn = CCMenuItemSpriteExtra::create(cancelSprite, nullptr, this, menu_selector(LevelFeatureLayer::onClose));
    m_buttonMenu->addChild(cancelBtn);
    buttonArray->addObject(cancelBtn);

    ButtonSprite* submitSprite = ButtonSprite::create("Submit");
    CCMenuItemSpriteExtra* submitBtn = CCMenuItemSpriteExtra::create(submitSprite, nullptr, this, menu_selector(LevelFeatureLayer::onSetFeatured));
    m_buttonMenu->addChild(submitBtn);
    buttonArray->addObject(submitBtn);

    cocos2d::CCPoint bottomCenterPos(winSize.width * 0.5f, winSize.height * 0.5f - 90.0f + 25.0f);
    cocos2d::CCPoint menuBottomPos = m_buttonMenu->convertToNodeSpace(bottomCenterPos);
    GameToolbox::alignItemsHorizontally(buttonArray, 15.0f, menuBottomPos);

    ButtonSprite* epicOnlySprite = ButtonSprite::create("Epic\nOnly");
    epicOnlySprite->setScale(0.4f);
    CCMenuItemSpriteExtra* epicOnlyBtn = CCMenuItemSpriteExtra::create(epicOnlySprite, nullptr, this, menu_selector(LevelFeatureLayer::onSetEpicOnly));
    m_buttonMenu->addChild(epicOnlyBtn);
    epicOnlyBtn->setPosition(submitBtn->getPosition() + cocos2d::CCPoint(90.0f, 0.0f));

    cocos2d::CCPoint centerPos(winSize.width * 0.5f, winSize.height * 0.5f);

    m_featureLabel = cocos2d::CCLabelBMFont::create("0", "bigFont.fnt");
    m_mainLayer->addChild(m_featureLabel);
    m_featureLabel->setPosition(centerPos);

    cocos2d::CCSprite* arrowDownSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCMenuItemSpriteExtra* arrowDownBtn = CCMenuItemSpriteExtra::create(arrowDownSprite, nullptr, this, menu_selector(LevelFeatureLayer::onDown));
    m_buttonMenu->addChild(arrowDownBtn);
    arrowDownBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerPos.x - 100.0f, centerPos.y)));
    arrowDownBtn->setRotation(-90.0f);
    arrowDownBtn->setSizeMult(1.5f);

    cocos2d::CCSprite* arrowUpSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCMenuItemSpriteExtra* arrowUpBtn = CCMenuItemSpriteExtra::create(arrowUpSprite, nullptr, this, menu_selector(LevelFeatureLayer::onUp));
    m_buttonMenu->addChild(arrowUpBtn);
    arrowUpBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerPos.x + 100.0f, centerPos.y)));
    arrowUpBtn->setRotation(90.0f);
    arrowUpBtn->setSizeMult(1.5f);

    cocos2d::CCSprite* arrowDown2Sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* arrowDown2Btn = CCMenuItemSpriteExtra::create(arrowDown2Sprite, nullptr, this, menu_selector(LevelFeatureLayer::onDown2));
    m_buttonMenu->addChild(arrowDown2Btn);
    arrowDown2Btn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerPos.x - 150.0f, centerPos.y)));
    arrowDown2Btn->setRotation(-90.0f);
    arrowDown2Btn->setSizeMult(1.5f);

    cocos2d::CCSprite* arrowUp2Sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* arrowUp2Btn = CCMenuItemSpriteExtra::create(arrowUp2Sprite, nullptr, this, menu_selector(LevelFeatureLayer::onUp2));
    m_buttonMenu->addChild(arrowUp2Btn);
    arrowUp2Btn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerPos.x + 150.0f, centerPos.y)));
    arrowUp2Btn->setRotation(90.0f);
    arrowUp2Btn->setSizeMult(1.5f);

    updateStars();

    cocos2d::CCSprite* deleteSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
    CCMenuItemSpriteExtra* deleteBtn = CCMenuItemSpriteExtra::create(deleteSprite, nullptr, this, menu_selector(LevelFeatureLayer::onRemoveValues));
    deleteBtn->setSizeMult(1.6f);
    m_buttonMenu->addChild(deleteBtn);
    deleteBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f + 180.0f, winSize.height * 0.5f + 90.0f)));

    m_epicSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
    CCMenuItemSpriteExtra* epicCoinBtn = CCMenuItemSpriteExtra::create(m_epicSprite, nullptr, this, menu_selector(LevelFeatureLayer::onToggleEpic));
    m_buttonMenu->addChild(epicCoinBtn);
    epicCoinBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(winSize.width * 0.5f - 180.0f, winSize.height * 0.5f + 90.0f)));

    m_epicOnly = false;
    m_epicSprite->setColor(cocos2d::ccc3(50, 50, 50));

    return true;
}

void LevelFeatureLayer::keyBackClicked() {
    onClose(nullptr);
}

void LevelFeatureLayer::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void LevelFeatureLayer::onDown(cocos2d::CCObject* sender) {
    int position = m_featurePosition - 1;
    if (position < 0) {
        position = 0;
    }
    m_featurePosition = position;
    updateStars();
}

void LevelFeatureLayer::onDown2(cocos2d::CCObject* sender) {
    int position = m_featurePosition - 10;
    if (position < 0) {
        position = 0;
    }
    m_featurePosition = position;
    updateStars();
}

void LevelFeatureLayer::onRemoveValues(cocos2d::CCObject* sender) {
    GameLevelManager::sharedState()->setLevelFeatured(m_levelID, -1, false);
    onClose(nullptr);
}

void LevelFeatureLayer::onSetEpicOnly(cocos2d::CCObject* sender) {
    GameLevelManager::sharedState()->setLevelFeatured(m_levelID, -2, m_epicOnly);
    onClose(nullptr);
}

void LevelFeatureLayer::onSetFeatured(cocos2d::CCObject* sender) {
    if (m_featurePosition != 0) {
        GameLevelManager::sharedState()->setLevelFeatured(m_levelID, m_featurePosition, m_epicOnly);
        onClose(nullptr);
    }
}

void LevelFeatureLayer::onToggleEpic(cocos2d::CCObject* sender) {
    m_epicOnly = !m_epicOnly;
    if (m_epicOnly) {
        m_epicSprite->setColor(cocos2d::ccc3(255, 255, 255));
    } else {
        m_epicSprite->setColor(cocos2d::ccc3(50, 50, 50));
    }
}

void LevelFeatureLayer::onUp(cocos2d::CCObject* sender) {
    m_featurePosition++;
    updateStars();
}

void LevelFeatureLayer::onUp2(cocos2d::CCObject* sender) {
    m_featurePosition += 10;
    updateStars();
}

void LevelFeatureLayer::updateStars() {
    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("%i", m_featurePosition);
    m_featureLabel->setString(str->getCString());
}

LevelFeatureLayer::~LevelFeatureLayer() {
}