#include "MapPackCell.h"
#include "GJMapPack.h"
#include "GameStatsManager.h"
#include "GameLevelManager.h"
#include "GJSearchObject.h"
#include "LevelBrowserLayer.h"
#include "FMODAudioEngine.h"
#include "ButtonSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "CCCircleWave.h"
#include "CCParticleSystemQuad.h"

MapPackCell::MapPackCell(char* charPtr, float value, float value2)
    : TableViewCell(charPtr, value, value2)
{
    init();
}

void MapPackCell::draw()
{
    cocos2d::CCSize size(m_width, m_height);
    cocos2d::ccDrawColor4B(0, 0, 0, 0x4B);
    glLineWidth(1.0f);
    cocos2d::CCPoint point(1.0f, size.height);
    cocos2d::CCPoint point2(size.width - 1.0f, size.height);
    cocos2d::ccDrawLine(point, point2);
    cocos2d::CCPoint point3(1.0f, 0.0f);
    cocos2d::CCPoint point4(size.width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(point3, point4);
}

bool MapPackCell::init()
{
    m_rewardSprites = cocos2d::CCArray::create();
    m_rewardSprites->retain();
    m_rewardLabels = cocos2d::CCArray::create();
    m_rewardLabels->retain();
    m_mapPack = nullptr;
    m_viewButton = nullptr;
    m_rewardButton = nullptr;
    return true;
}

void MapPackCell::loadFromMapPack(GJMapPack* pack)
{
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_rewardSprites->removeAllObjects();
    m_rewardLabels->removeAllObjects();

    GJDifficulty difficulty = pack->m_difficulty;
    m_mapPack = pack;

    cocos2d::CCString* str = cocos2d::CCString::createWithFormat("difficulty_%02d_btn_001.png", difficulty);
    const char* frameName = str->getCString();
    if (difficulty == GJDifficulty::Auto)
        frameName = "difficulty_auto_btn_001.png";

    cocos2d::CCSprite* diffSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName);
    diffSprite->setPosition(ccp(31.0f, m_height * 0.5f));
    diffSprite->setScale(1.1f);
    this->addChild(diffSprite, 2);

    cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create(m_mapPack->m_packName.c_str(), "bigFont.fnt");
    m_mainLayer->addChild(nameLabel);
    nameLabel->setColor(ccc3(
        m_mapPack->m_barColour.r,
        m_mapPack->m_barColour.g,
        m_mapPack->m_barColour.b));
    nameLabel->setPosition(ccp(
        (m_width * 0.5f - 14.0f) - 2.0f,
        m_height * 0.5f + 30.0f));

    float labelWidth = nameLabel->getContentSize().width;
    if (labelWidth > 205.0f)
        nameLabel->setScale(205.0f / labelWidth);

    float nameScale = nameLabel->getScale();
    if (nameScale > 0.9f)
        nameScale = 0.9f;
    nameLabel->setScale(nameScale);

    cocos2d::CCNode* viewNode = ButtonSprite::create(
        "View", 0, false, "bigFont.fnt", "GJ_button_01.png", 50.0f);
    CCMenuItemSpriteExtra* viewBtn = CCMenuItemSpriteExtra::create(
        viewNode, nullptr, this, menu_selector(MapPackCell::onClick));
    m_viewButton = viewBtn;
    viewBtn->setSizeMult(1.5f);

    cocos2d::CCMenu* viewMenu = cocos2d::CCMenu::create(viewBtn, nullptr);
    m_mainLayer->addChild(viewMenu);
    viewMenu->setPosition(ccp(
        m_width - (viewNode->getContentSize().width * 0.5f) - 9.0f,
        m_height * 0.5f));

    GameStatsManager* gsm = GameStatsManager::sharedState();
    bool completed = gsm->hasCompletedMapPack(m_mapPack->m_packID);

    if (!completed) {
        int completedMaps = m_mapPack->completedMaps();
        int totalMaps = m_mapPack->totalMaps();
        if (completedMaps >= totalMaps) {
            m_viewButton->setVisible(false);
            m_viewButton->setEnabled(false);
            cocos2d::CCSprite* rewardSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png");
            m_rewardButton = CCMenuItemSpriteExtra::create(
                rewardSprite, nullptr, this, menu_selector(MapPackCell::onClaimReward));
            m_rewardButton->setSizeMult(1.5f);
            viewMenu->addChild(m_rewardButton);
            m_rewardButton->setPosition(m_viewButton->getPosition());
        }
    }

    cocos2d::CCSprite* progressBG = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    progressBG->setColor(ccc3(0, 0, 0));
    progressBG->setOpacity(125);
    progressBG->setScaleY(0.8f);
    progressBG->setScaleX(0.6f);
    m_mainLayer->addChild(progressBG, 3);
    progressBG->setPosition(ccp(m_width * 0.5f - 14.0f, m_height * 0.5f - 2.0f));

    cocos2d::CCSprite* progressBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    progressBar->setScaleX(0.985f);
    progressBar->setScaleY(0.83f);
    progressBar->setColor(ccc3(
        m_mapPack->m_textColour.r,
        m_mapPack->m_textColour.g,
        m_mapPack->m_textColour.b));

    float barWidth = progressBar->getContentSize().width;
    float offset = barWidth - (barWidth * 0.985f);

    progressBG->addChild(progressBar, 1);
    progressBar->setAnchorPoint(ccp(0.0f, 0.5f));
    progressBar->setPosition(ccp(
        offset * 0.5f,
        progressBG->getContentSize().height * 0.5f));

    int completedMaps = m_mapPack->completedMaps();
    int totalMaps = m_mapPack->totalMaps();
    float barFullWidth = progressBar->getContentSize().width;
    float filledWidth = ((float)completedMaps / (float)totalMaps) * barFullWidth;
    if (barFullWidth <= filledWidth)
        filledWidth = barFullWidth;
    progressBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, filledWidth, progressBar->getContentSize().height));

    cocos2d::CCString* progressStr = cocos2d::CCString::createWithFormat("%i/%i", completedMaps, totalMaps);
    cocos2d::CCLabelBMFont* progressLabel = cocos2d::CCLabelBMFont::create(progressStr->getCString(), "bigFont.fnt");
    m_mainLayer->addChild(progressLabel, 4);
    progressLabel->setPosition(progressBG->getPosition());
    progressLabel->setScale(0.5f);

    int stars = m_mapPack->m_stars;
    float rewardY = m_height * 0.5f - 30.0f;
    int rewardCount = (stars > 0) + (m_mapPack->m_coins > 0);

    if (stars > 0) {
        cocos2d::CCSprite* starSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
        m_mainLayer->addChild(starSprite, 2);
        if (rewardCount == 2)
            starSprite->setPosition(ccp((m_width * 0.5f - 4.0f) - 40.0f, rewardY));
        else
            starSprite->setPosition(ccp(m_width * 0.5f - 4.0f, rewardY));

        cocos2d::CCString* starStr = cocos2d::CCString::createWithFormat("%i", m_mapPack->m_stars);
        cocos2d::CCLabelBMFont* starLabel = cocos2d::CCLabelBMFont::create(starStr->getCString(), "bigFont.fnt");
        m_mainLayer->addChild(starLabel, 1);
        starLabel->setAnchorPoint(ccp(1.0f, 0.5f));
        starLabel->setPosition(starSprite->getPosition() + ccp(-17.0f, 0.0f));
        starLabel->setScale(0.7f);
        m_rewardSprites->addObject(starSprite);
        m_rewardLabels->addObject(starLabel);
        if (completed)
            starLabel->setColor(ccc3(255, 255, 50));
    }

    if (m_mapPack->m_coins > 0) {
        cocos2d::CCSprite* coinSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
        m_mainLayer->addChild(coinSprite, 2);
        if (rewardCount == 2)
            coinSprite->setPosition(ccp((m_width * 0.5f - 4.0f) + 40.0f, rewardY));
        else
            coinSprite->setPosition(ccp(m_width * 0.5f - 4.0f, rewardY));

        cocos2d::CCString* coinStr = cocos2d::CCString::createWithFormat("%i", m_mapPack->m_coins);
        cocos2d::CCLabelBMFont* coinLabel = cocos2d::CCLabelBMFont::create(coinStr->getCString(), "bigFont.fnt");
        m_mainLayer->addChild(coinLabel, 1);
        coinLabel->setAnchorPoint(ccp(1.0f, 0.5f));
        coinLabel->setPosition(coinSprite->getPosition() + ccp(-19.0f, 0.0f));
        coinLabel->setScale(0.7f);
        m_rewardSprites->addObject(coinSprite);
        m_rewardLabels->addObject(coinLabel);
        if (completed)
            coinLabel->setColor(ccc3(255, 255, 50));
    }

    if (completed) {
        cocos2d::CCSprite* completeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
        m_mainLayer->addChild(completeSprite, 5);
        completeSprite->setScale(1.0f);
        completeSprite->setPosition(progressBG->getPosition() + ccp(86.0f, 1.0f));
    }
}

void MapPackCell::onClaimReward(cocos2d::CCObject* sender)
{
    GameStatsManager* gsm = GameStatsManager::sharedState();
    gsm->completedMapPack(m_mapPack);
    m_rewardButton->setVisible(false);
    m_rewardButton->setEnabled(false);
    m_viewButton->setVisible(true);
    m_viewButton->setEnabled(true);
    playCompleteEffect();
}

void MapPackCell::onClick(cocos2d::CCObject* sender)
{
    GameLevelManager::sharedState()->m_mapPack = m_mapPack->m_packID;
    GJSearchObject* searchObj = GJSearchObject::create(SearchType::MapPackOnClick);
    cocos2d::CCScene* scene = LevelBrowserLayer::scene(searchObj);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->pushScene(transition);
}

void MapPackCell::playCompleteEffect()
{
    cocos2d::CCNode* parent = m_rewardButton->getParent();
    cocos2d::CCPoint pos = parent->getPosition();

    cocos2d::CCParticleSystemQuad* particles = cocos2d::CCParticleSystemQuad::create("starEffect.plist");
    particles->setAutoRemoveOnFinish(true);
    m_mainLayer->addChild(particles, 9);
    particles->setPosition(pos);

    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    engine->playEffect("highscoreGet02.ogg");

    CCCircleWave* wave = CCCircleWave::create(10.0f, 90.0f, 0.4f, false);
    wave->m_color.r = 255;
    wave->m_color.g = 255;
    wave->m_color.b = 0;
    wave->setPosition(pos);
    m_mainLayer->addChild(wave, 9);

    for (unsigned int i = 0; i < m_rewardSprites->count(); ++i) {
        cocos2d::CCNode* sprite = (cocos2d::CCNode*)m_rewardSprites->objectAtIndex(i);
        cocos2d::CCScaleTo* scaleUp = cocos2d::CCScaleTo::create(0.1f, 1.8f);
        cocos2d::CCScaleTo* scaleDown = cocos2d::CCScaleTo::create(0.3f, 1.0f);
        cocos2d::CCEaseBounceOut* bounce = cocos2d::CCEaseBounceOut::create(scaleDown);
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(scaleUp, bounce, nullptr);
        sprite->runAction(seq);
    }

    for (unsigned int j = 0; j < m_rewardLabels->count(); ++j) {
        cocos2d::CCLabelBMFont* label = (cocos2d::CCLabelBMFont*)m_rewardLabels->objectAtIndex(j);
        label->setColor(ccc3(255, 255, 50));
    }
}

void MapPackCell::reloadCell()
{
    loadFromMapPack(m_mapPack);
}

void MapPackCell::updateBGColor(int index)
{
    ccColor3B color;
    if ((index & 1) != 0) {
        color.r = 194;
        color.g = 114;
        color.b = 62;
    } else {
        color.r = 161;
        color.g = 88;
        color.b = 44;
    }
    m_backgroundLayer->setColor(color);
    m_backgroundLayer->setOpacity(255);
}

MapPackCell::~MapPackCell()
{
}