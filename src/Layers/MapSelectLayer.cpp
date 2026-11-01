#include "Layers/MapSelectLayer.h"
#include "Sprites/GJBigSprite.h"
#include "Models/GJMapObject.h"
#include "Layers/LevelEditorLayer.h"
#include "Sprites/SimplePlayer.h"
#include "Managers/GameManager.h"
#include "GameLevelManager.h"
#include "Models/GJGameLevel.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "CCParticleSystemQuad.h"
#include "Cocos/CCCircleWave.h"
#include "Nodes/ExplodeItemNode.h"
#include "CCRenderTexture.h"
#include "Layers/CreatorLayer.h"
#include "Delegates/AppDelegate.h"
#include "Audio/FMODAudioEngine.h"

bool MapSelectLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint touchPoint = touch->getLocation();
    cocos2d::CCPoint nodeSpace = this->convertToNodeSpace(touchPoint);
    cocos2d::CCPoint spriteSpace = m_bigSprite->convertToNodeSpace(touchPoint);

    if (!checkTouchMonster(spriteSpace)) {
        createObjectAtPoint(spriteSpace);
        m_lastTouchPos = nodeSpace;
        m_mapPos = cocos2d::CCPoint(0.0f, 0.0f);
        m_inTouch = true;
        m_lastTouchTime = sub_97D408();
    }
    return true;
}

void MapSelectLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    ccTouchEnded(touch, event);
}

void MapSelectLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    m_inTouch = false;
    struct timespec ts;
    clock_gettime(0, &ts);
    double now = (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
    if (now - m_lastTouchTime > 0.0500000007) {
        m_mapPos = cocos2d::CCPoint(0.0f, 0.0f);
    }
}

void MapSelectLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (!m_inTouch)
        return;

    cocos2d::CCPoint touchPoint = touch->getLocation();
    cocos2d::CCPoint nodeSpace = this->convertToNodeSpace(touchPoint);
    cocos2d::CCPoint delta = nodeSpace - m_lastTouchPos;
    cocos2d::CCPoint spritePos = m_bigSprite->getPosition() + delta;
    cocos2d::CCPoint constrained = getConstrainedMapPos(spritePos);
    m_bigSprite->setPosition(constrained);
    m_bigSprite->updateSpriteVisibility();

    double now = sub_97D408();
    double elapsed = now - m_lastTouchTime;
    if (elapsed > 0.0) {
        float speedFactor = 1.0f / (elapsed * 60.0);
        cocos2d::CCPoint velocity = delta * speedFactor;
        m_mapPos = velocity;

        if (m_mapPos.x > 10.0f)
            m_mapPos.x = 10.0f;
        else if (m_mapPos.x < -10.0f)
            m_mapPos.x = -10.0f;

        if (m_mapPos.y > 10.0f)
            m_mapPos.y = 10.0f;
        else if (m_mapPos.y < -10.0f)
            m_mapPos.y = -10.0f;

        m_lastTouchPos = nodeSpace;
        m_lastTouchTime = now;
    }
}

bool MapSelectLayer::checkTouchMonster(cocos2d::CCPoint position)
{
    unsigned int idx = 0;
    bool touched = false;

    while (idx < m_monsters->count()) {
        GJMapObject* monster = (GJMapObject*)m_monsters->objectAtIndex(idx);
        cocos2d::CCPoint monsterPos = monster->getPosition() + cocos2d::CCPoint(0.0f, 5.0f);

        if (cocos2d::ccpDistance(position, monsterPos) < 20.0f) {
            monster->touchMonster();

            if (monster->m_monstersTouched > 9) {
                FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
                float pitch = ((float)rand() * 4.6566e-10f * 2.0f + -1.0f) * 0.05f + 0.95f;
                engine->playEffect("explode_11.ogg", pitch, 0.0f, 0.5f);

                touched = true;

                cocos2d::CCParticleSystemQuad* particles = cocos2d::CCParticleSystemQuad::create("explodeEffect.plist");
                particles->setPositionType(cocos2d::kCCPositionTypeGrouped);
                m_mainLayer->addChild(particles, 1);
                particles->setAutoRemoveOnFinish(true);
                particles->setPosition(monsterPos);
                particles->setScale(0.3f);

                cocos2d::ccColor4F startColor;
                startColor.r = 1.0f;
                startColor.g = 1.0f;
                startColor.b = 1.0f;
                startColor.a = 1.0f;
                particles->setStartColor(startColor);
                particles->resetSystem();

                CCCircleWave* wave = CCCircleWave::create(10.0f, 40.0f, 0.4f, false);
                wave->m_color.r = 255;
                wave->m_color.g = 255;
                wave->m_color.b = 0;
                wave->setPosition(monsterPos);
                m_mainLayer->addChild(wave, 0);

                cocos2d::CCRenderTexture* rt = cocos2d::CCRenderTexture::create(64, 64);
                monster->m_shadowSprite->setVisible(false);
                cocos2d::CCPoint originalPos = monster->getPosition();
                monster->setPosition(cocos2d::CCPoint(25.0f, 25.0f));
                rt->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
                monster->visit();
                rt->end();
                monster->setPosition(originalPos);

                ExplodeItemNode* explodeNode = ExplodeItemNode::create(rt);
                explodeNode->m_floorHeight = -100.0f;
                m_mainLayer->addChild(explodeNode, 101);
                explodeNode->setPosition(originalPos);

                float velX = ((float)rand() * 4.6566e-10f * 2.0f + -1.0f) * 3.0f;
                float velY = roundf((float)rand() * 4.6566e-10f * 4.0f);
                int randVal = rand();
                cocos2d::ccColor4F endColor;
                endColor.r = 1.0f;
                endColor.g = 1.0f;
                endColor.b = 1.0f;
                endColor.a = 1.0f;

                explodeNode->createSprites(
                    (int)(velY + 4.0f),
                    (int)(roundf((float)randVal * 4.6566e-10f * 4.0f) + 4.0f),
                    velX * 0.5f,
                    velX,
                    4.0f,
                    2.0f,
                    6.4f,
                    0.0f,
                    startColor,
                    endColor,
                    false);

                monster->removeFromParentAndCleanup(true);
                m_monsters->removeObjectAtIndex(idx, true);
            } else {
                ++idx;
                touched = true;
            }
        } else {
            ++idx;
        }
    }
    return touched;
}

MapSelectLayer* MapSelectLayer::create()
{
    MapSelectLayer* ret = new MapSelectLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

GJMapObject* MapSelectLayer::createObjectAtPoint(cocos2d::CCPoint position)
{
    return nullptr;
}

cocos2d::CCPoint MapSelectLayer::getConstrainedMapPos(cocos2d::CCPoint position)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float scale = m_bigSprite->getScale();

    float minX = 0.0f + (scale * -3600.0f);
    float minY = 0.0f + (scale * -1500.0f);

    if (position.x > 0.0f)
        position.x = 0.0f;
    if (position.x < minX)
        position.x = minX;

    if (position.y > 0.0f)
        position.y = 0.0f;
    if (position.y < minY)
        position.y = minY;

    return position;
}

bool MapSelectLayer::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    m_monsters = cocos2d::CCArray::create();
    m_monsters->retain();

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        backSprite, nullptr, this, menu_selector(MapSelectLayer::onBack));
    backBtn->setSizeMult(1.6f);
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backBtn, nullptr);
    this->addChild(backMenu, 1);
    backMenu->setPosition(ccp(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 25.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 22.0f));

    GameManager::sharedState()->m_somethingInMenuLayer = 0;

    m_bigSprite = GJBigSprite::create();
    m_bigSprite->setPosition(ccp(-165.0f, -800.0f));
    m_bigSprite->updateSpriteVisibility();
    m_bigSprite->m_loadSprites = true;

    m_mainLayer = cocos2d::CCNode::create();
    m_bigSprite->addChild(m_mainLayer, 20);

    GameManager* gm = GameManager::sharedState();
    m_player = SimplePlayer::create(gm->m_playerFrame.random - gm->m_playerFrame.seed);
    m_player->setScale(0.5f);

    GJMapObject* playerObj = GJMapObject::create(m_player);
    m_mainLayer->addChild(playerObj, 0);
    playerObj->setObjectOrigin(cocos2d::CCPoint(462.0f, 950.0f), 0.0f);

    gm = GameManager::sharedState();
    m_player->m_hasGlowOutline = gm->m_playerGlow;

    gm = GameManager::sharedState();
    GameManager* gm2 = GameManager::sharedState();
    cocos2d::ccColor3B color1 = GameManager::colorForIdx(gm->m_playerColor.random - gm->m_playerColor.seed);

    gm = GameManager::sharedState();
    gm2 = GameManager::sharedState();
    cocos2d::ccColor3B color2 = GameManager::colorForIdx(gm->m_playerColor2.random - gm->m_playerColor2.seed);

    m_player->setColors(color1, color2);

    gm = GameManager::sharedState();
    if (gm->m_playerGlowColor.random - gm->m_playerGlowColor.seed < 0) {
        m_player->disableCustomGlowColor();
    } else {
        gm = GameManager::sharedState();
        gm2 = GameManager::sharedState();
        cocos2d::ccColor3B glowColor = GameManager::colorForIdx(gm->m_playerGlowColor.random - gm->m_playerGlowColor.seed);
        m_player->enableCustomGlowColor(glowColor);
    }

    m_player->updateColors();
    playerObj->startPlayerJumpLoop();

    for (int i = 0; i < 10; ++i) {
        GJMapObject* monster = GJMapObject::createMonster(GJMapObjectType::Monster1);
        m_mainLayer->addChild(monster, 0);

        int randX = rand();
        int randY = rand();
        cocos2d::CCPoint pos = ccp(
            ((float)randX * 4.6566e-10f * 2.0f + -1.0f) * 100.0f + 600.0f,
            ((float)randY * 4.6566e-10f * 2.0f + -1.0f) * 100.0f + 600.0f);
        monster->setObjectOrigin(pos, 50.0f);
        m_monsters->addObject(monster);
    }

    GJMapObject* bigMonster = GJMapObject::createMonster(GJMapObjectType::Monster2);
    m_mainLayer->addChild(bigMonster, 0);
    bigMonster->setObjectOrigin(cocos2d::CCPoint(436.0f, 776.0f), 50.0f);
    bigMonster->m_shadowSprite->setVisible(false);
    m_monsters->addObject(bigMonster);

    m_editMode = false;

    GameLevelManager* glm = GameLevelManager::sharedState();
    GJGameLevel* localLevel = glm->getLocalLevelByName("gamemap");
    if (!localLevel) {
        glm = GameLevelManager::sharedState();
        localLevel = glm->createNewLevel();
        localLevel->m_levelName = "gamemap";
    }

    m_editorLayer = LevelEditorLayer::create(localLevel, !m_editMode);
    m_editorLayer->toggleBackground(false);
    m_editorLayer->toggleGround(false);

    if (m_editMode) {
        this->addChild(m_editorLayer);
        m_editorLayer->addChild(m_bigSprite, -100);
    } else {
        m_editorLayer->m_alwaysHideParticleIcons = true;
        m_editorLayer->updatePreviewParticles();
        m_editorLayer->m_alwaysPreviewMode = true;
        m_editorLayer->updateEditorMode();
        this->addChild(m_bigSprite);
        m_mainLayer->addChild(m_editorLayer, 100);
        setKeypadEnabled(true);
        setKeyboardEnabled(true);
        setTouchEnabled(true);
        setMouseEnabled(true);
    }

    this->scheduleUpdate();

    return true;
}

void MapSelectLayer::keyBackClicked()
{
    onBack(nullptr);
}

void MapSelectLayer::onBack(cocos2d::CCObject* sender)
{
    if (!m_backPressed && !m_blockExit) {
        m_backPressed = true;
        m_unloadTexturesOnExit = true;
        setKeypadEnabled(false);
        cocos2d::CCScene* scene = CreatorLayer::scene();
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    }
}

void MapSelectLayer::onExit()
{
    m_bigSprite->unloadAll();
    cocos2d::CCLayer::onExit();
    if (m_unloadTexturesOnExit) {
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("mapimages/image_small.png");
    }
}

void MapSelectLayer::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, 0, true);
}

cocos2d::CCScene* MapSelectLayer::scene()
{
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    MapSelectLayer* layer = MapSelectLayer::create();
    scene->addChild(layer);
    return scene;
}

void MapSelectLayer::scrollWheel(float y, float x)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint nodeSpace = m_bigSprite->convertToNodeSpace(center);

    float currentScale = m_bigSprite->getScale();

    if (y > 0.0f) {
        if (currentScale + 0.01f <= 1.2f) {
            float newScale = currentScale - 0.01f;
            if (newScale + currentScale <= 0.7f)
                return;
            m_bigSprite->setScale(newScale);
        }
    } else {
        if (currentScale - 0.01f <= 1.2f) {
            float newScale = currentScale + 0.01f;
            if (newScale + currentScale <= 1.2f)
                m_bigSprite->setScale(newScale);
        }
    }

    float finalScale = m_bigSprite->getScale();
    cocos2d::CCPoint newCenter = m_bigSprite->convertToNodeSpace(center);
    cocos2d::CCPoint delta = newCenter - nodeSpace;
    m_bigSprite->setPosition(m_bigSprite->getPosition() + delta);
}

void MapSelectLayer::update(float dt)
{
    if (m_inTouch)
        return;

    if (m_editMode) {
        cocos2d::CCPoint objectLayerPos = m_editorLayer->m_objectLayer->getPosition();
        m_bigSprite->setPosition(objectLayerPos);
        m_bigSprite->setScale(m_editorLayer->m_objectLayer->getScale());
        m_bigSprite->updateSpriteVisibility();
    } else {
        float factor = dt * 60.0f;
        cocos2d::CCPoint delta = m_mapPos * factor;
        cocos2d::CCPoint newPos = m_bigSprite->getPosition() + delta;
        cocos2d::CCPoint constrained = getConstrainedMapPos(newPos);
        m_bigSprite->setPosition(constrained);
        m_bigSprite->updateSpriteVisibility();

        float decay = 1.0f - (factor * 0.05f);
        m_mapPos = m_mapPos * decay;

        if (fabsf(m_mapPos.x) <= 0.1f)
            m_mapPos.x = 0.0f;
        if (fabsf(m_mapPos.y) <= 0.1f)
            m_mapPos.y = 0.0f;
    }
}

MapSelectLayer::~MapSelectLayer()
{
}