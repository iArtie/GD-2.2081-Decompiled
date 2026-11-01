#include "MenuGameLayer.h"
#include "PlayerObject.h"
#include "GJGroundLayer.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "FMODAudioEngine.h"
#include "AppDelegate.h"
#include "CCParticleSystemQuad.h"
#include "CCCircleWave.h"

MenuGameLayer* MenuGameLayer::create()
{
    MenuGameLayer* ret = new MenuGameLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool MenuGameLayer::init()
{
    if (!cocos2d::CCLayer::init())
        return false;

    setTouchEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    GameManager* gm = GameManager::sharedState();
    cocos2d::CCTexture2D* bgTexture = gm->getBGTexture(gm->m_loadedBgID);
    m_backgroundSprite = cocos2d::CCSprite::createWithTexture(bgTexture);

    cocos2d::ccTexParams texParams;
    texParams.minFilter = GL_LINEAR;
    texParams.magFilter = GL_LINEAR;
    texParams.wrapS = GL_REPEAT;
    texParams.wrapT = GL_REPEAT;
    m_backgroundSprite->getTexture()->setTexParameters(&texParams);

    this->addChild(m_backgroundSprite, -100);
    m_backgroundSprite->setAnchorPoint(ccp(0.0f, 0.0f));
    m_backgroundSprite->setScale(AppDelegate::get()->bgScale());

    cocos2d::ccColor3B bgColor = getBGColor(0);
    m_initCount++;
    m_backgroundSprite->setColor(bgColor);

    cocos2d::CCRect rect = m_backgroundSprite->getTextureRect();
    float width = rect.size.width;
    float scale = m_backgroundSprite->getScale();
    m_backgroundWrapPosition = width * scale;
    rect.size.width = rect.size.width + rect.size.width;
    m_backgroundSprite->setTextureRect(rect);

    gm = GameManager::sharedState();
    m_groundLayer = GJGroundLayer::create(gm->m_loadedGroundID, 1);
    this->addChild(m_groundLayer, 100);
    m_groundLayer->setPosition(ccp(0.0f, 90.0f));

    gm = GameManager::sharedState();
    int savedStreak = gm->m_playerStreak.random - gm->m_playerStreak.seed;
    bool savedGlow = gm->m_playerGlow;

    gm = GameManager::sharedState();
    gm->m_playerStreak.value = 1;
    int randSeed = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    gm->m_playerStreak.seed = randSeed;
    gm->m_playerStreak.random = randSeed + 1;
    gm->m_playerGlow = false;

    m_playerObject = PlayerObject::create(1, 1, 0, this, 0);
    this->addChild(m_playerObject, 60);
    m_playerObject->addAllParticles();
    m_playerObject->setPosition(ccp(0.0f, 105.0f));

    gm = GameManager::sharedState();
    int randColor1 = rand();
    cocos2d::ccColor3B color1 = GameManager::colorForIdx((int)((float)randColor1 * 4.6566e-10f * 108.0f));
    m_playerObject->setColor(&color1);

    gm = GameManager::sharedState();
    int randColor2 = rand();
    cocos2d::ccColor3B color2 = GameManager::colorForIdx((int)((float)randColor2 * 4.6566e-10f * 108.0f));
    m_playerObject->setSecondColor(&color2);
    m_playerObject->updateGlowColor();

    m_playerObject->m_audioScale = 0.5f;

    if (!GameManager::sharedState()->m_unkBool10) {
        GameManager::sharedState()->m_unkBool10 = true;
        m_playerObject->toggleDartMode(true, false);
        m_playerObject->toggleDartMode(false, false);
    }

    gm = GameManager::sharedState();
    gm->m_playerStreak.value = savedStreak;
    int randSeed2 = (int)((float)rand() * 4.6566e-10f * 1000.0f);
    gm->m_playerStreak.seed = randSeed2;
    gm->m_playerStreak.random = savedStreak + randSeed2;
    gm->m_playerGlow = savedGlow;

    this->scheduleUpdate();
    this->schedule(schedule_selector(MenuGameLayer::tryJump));
    this->schedule(schedule_selector(MenuGameLayer::updateColor));

    updateColor(0.0f);
    m_playerObject->stopStreak2();
    resetPlayer();

    return true;
}

void MenuGameLayer::registerWithTouchDispatcher()
{
    cocos2d::CCTouchDispatcher* dispatcher = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, 1, true);
}

bool MenuGameLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    cocos2d::CCPoint touchPoint = touch->getLocation();
    cocos2d::CCPoint nodeSpace = this->convertToNodeSpace(touchPoint);

    if (nodeSpace.x > 50.0f) {
        cocos2d::CCPoint playerPos = m_playerObject->getPosition();
        if (cocos2d::ccpDistance(playerPos, nodeSpace) <= 30.0f) {
            if (!m_playerObject->isFlying()) {
                if (!m_playerObject->m_isBall && !m_playerObject->m_isRobot && m_playerObject->m_vehicleSize == 1.0f) {
                    int playerFrame = m_playerObject->m_maybeSavedPlayerFrame;
                    if (playerFrame == 55) {
                        GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret11", 100, false);
                    } else if (playerFrame == 50) {
                        GameManager::sharedState()->reportAchievementWithID("geometry.ach.secret12", 100, false);
                    }
                }
            }
            destroyPlayer();
        }
    }
    return true;
}

void MenuGameLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    ccTouchEnded(touch, event);
}

void MenuGameLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void MenuGameLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
}

void MenuGameLayer::destroyPlayer()
{
    m_isDestroyingPlayer = true;

    GameStatsManager::sharedState()->incrementStat("9");

    FMODAudioEngine::sharedEngine()->playEffect("explode_11.ogg", 1.0f, 0.0f, 0.5f);

    cocos2d::CCParticleSystemQuad* particles = cocos2d::CCParticleSystemQuad::create("explodeEffect.plist");
    particles->setPositionType(cocos2d::kCCPositionTypeGrouped);
    this->addChild(particles, 1);
    particles->setAutoRemoveOnFinish(true);
    particles->setPosition(m_playerObject->getPosition());

    cocos2d::ccColor4F startColor;
    startColor.r = (float)m_playerObject->m_playerColor1.r / 255.0f;
    startColor.g = (float)m_playerObject->m_playerColor1.g / 255.0f;
    startColor.b = (float)m_playerObject->m_playerColor1.b / 255.0f;
    startColor.a = 1.0f;
    particles->setStartColor(startColor);
    particles->resetSystem();

    CCCircleWave* wave = CCCircleWave::create(10.0f, 90.0f, 0.5f, false);
    wave->m_color.r = m_playerObject->m_playerColor1.r;
    wave->m_color.g = m_playerObject->m_playerColor1.g;
    wave->m_color.b = m_playerObject->m_playerColor1.b;
    wave->setPosition(m_playerObject->getPosition());
    this->addChild(wave, 0);

    resetPlayer();
}

cocos2d::ccColor3B MenuGameLayer::getBGColor(int index)
{
    cocos2d::ccColor3B color;
    switch (index) {
        case 0:
            color.r = 0;
            color.g = 102;
            color.b = 255;
            return color;
        case 1:
            color = GameManager::sharedState()->colorForIdx(7);
            break;
        case 2:
            color = GameManager::sharedState()->colorForIdx(8);
            break;
        case 3:
            color = GameManager::sharedState()->colorForIdx(9);
            break;
        case 4:
            color = GameManager::sharedState()->colorForIdx(10);
            break;
        case 5:
            color = GameManager::sharedState()->colorForIdx(11);
            break;
        case 6:
            color = GameManager::sharedState()->colorForIdx(1);
            break;
        case 7:
            color = GameManager::sharedState()->colorForIdx(3);
            break;
        default:
            color = getBGColor(index % 8);
            break;
    }
    return color;
}

void MenuGameLayer::resetPlayer()
{
    m_playerObject->deactivateStreak(true);
    m_playerObject->deactivateParticle();

    int randVal = rand();
    float x = -100.0f - ((float)randVal * 4.6566e-10f * 5.0f * 100.0f);
    float y = m_playerObject->getPosition().y;
    m_playerObject->setPosition(ccp(x, y));

    m_playerObject->resetAllParticles();
    m_playerObject->togglePlayerScale(false, false);

    int randGlow = rand();
    m_playerObject->m_hasGlow = ((float)randGlow * 4.6566e-10f) > 0.8f;

    int randColor1 = rand();
    cocos2d::ccColor3B color1 = GameManager::colorForIdx((int)((float)randColor1 * 4.6566e-10f * 108.0f));
    m_playerObject->setColor(&color1);

    int randColor2 = rand();
    cocos2d::ccColor3B color2 = GameManager::colorForIdx((int)((float)randColor2 * 4.6566e-10f * 108.0f));
    m_playerObject->setSecondColor(&color2);

    m_playerObject->flipGravity(false, false);
    m_playerObject->update(0.0f);

    int randMode = rand();
    if (m_videoOptionsOpen) {
        m_playerObject->updateGlowColor();
    } else {
        float modeChance = (float)randMode * 4.6566e-10f;

        if (modeChance < 0.12f && !m_playerObject->m_isShip) {
            m_playerObject->toggleFlyMode(true, false);
            int randFrame = rand();
            m_playerObject->updatePlayerShipFrame((int)llroundf((float)randFrame * 4.6566e-10f * 169.0f));
            int randFrame2 = rand();
            int candidate = (int)llroundf((float)randFrame2 * 4.6566e-10f * 485.0f);
            int frame = (candidate > 1) ? candidate : 1;
            m_playerObject->updatePlayerFrame(frame);
        } else if (modeChance < 0.24f && !m_playerObject->m_isBall) {
            m_playerObject->releaseButton(Jump);
            m_playerObject->toggleRollMode(true, false);
            int randFrame = rand();
            int candidate = (int)llroundf((float)randFrame * 4.6566e-10f * 118.0f);
            int frame = (candidate > 1) ? candidate : 1;
            m_playerObject->updatePlayerRollFrame(frame);
        } else if (modeChance < 0.36f && !m_playerObject->m_isBird) {
            m_playerObject->toggleBirdMode(true, false);
            int randFrame = rand();
            m_playerObject->updatePlayerBirdFrame((int)llroundf((float)randFrame * 4.6566e-10f * 149.0f));
            int randFrame2 = rand();
            int candidate = (int)llroundf((float)randFrame2 * 4.6566e-10f * 485.0f);
            int frame = (candidate > 1) ? candidate : 1;
            m_playerObject->updatePlayerFrame(frame);
        } else if (modeChance >= 0.48f || m_playerObject->m_isDart) {
            m_playerObject->toggleDartMode(true, false);
            int randFrame = rand();
            m_playerObject->updatePlayerDartFrame((int)llroundf((float)randFrame * 4.6566e-10f * 96.0f));
        } else if (modeChance >= 0.6f || m_playerObject->m_isRobot) {
            m_playerObject->toggleRobotMode(true, false);
            int randFrame = rand();
            m_playerObject->updatePlayerRobotFrame((int)llroundf((float)randFrame * 4.6566e-10f * 68.0f));
        } else if (modeChance >= 0.7f || m_playerObject->m_isSpider) {
            m_playerObject->releaseButton(Jump);
            m_playerObject->toggleSpiderMode(true, false);
            int randFrame = rand();
            m_playerObject->updatePlayerSpiderFrame((int)llroundf((float)randFrame * 4.6566e-10f * 69.0f));
        } else if (modeChance >= 0.8f || m_playerObject->m_isSwing) {
            m_playerObject->releaseButton(Jump);
            m_playerObject->toggleSwingMode(true, false);
            int randFrame = rand();
            m_playerObject->updatePlayerSwingFrame((int)llroundf((float)randFrame * 4.6566e-10f * 43.0f));
        } else {
            m_playerObject->switchedToMode(CubePortal);
            int randFrame = rand();
            int candidate = (int)llroundf((float)randFrame * 4.6566e-10f * 485.0f);
            int frame = (candidate > 1) ? candidate : 1;
            m_playerObject->updatePlayerFrame(frame);
        }

        float scaleChance = (float)rand() * 4.6566e-10f;
        m_playerObject->togglePlayerScale(scaleChance <= 0.1f, false);
        m_playerObject->updateGlowColor();
    }

    float speedChance = (float)rand() * 4.6566e-10f;
    if (speedChance < 0.2f)
        m_playerObject->updateTimeMod(1.3f, false);
    else if (speedChance < 0.4f)
        m_playerObject->updateTimeMod(1.1f, false);
    else if (speedChance < 0.6f)
        m_playerObject->updateTimeMod(0.7f, false);
    else if (speedChance >= 0.65f)
        m_playerObject->updateTimeMod(0.9f, false);
    else
        m_playerObject->updateTimeMod(1.6f, false);

    if (m_playerObject->m_isDart) {
        m_playerObject->resetStreak();
        m_playerObject->placeStreakPoint();
    }
    m_playerObject->updateEffects(0.0f);
}

void MenuGameLayer::tryJump(float dt)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    float jumpChance;
    if (m_isDestroyingPlayer) {
        jumpChance = 0.4f;
        if (m_playerObject->m_isShip) {
            float y = m_playerObject->getPosition().y;
            float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
            if (y < screenBottom + 90.0f + 15.0f + 100.0f) {
                jumpChance = 0.3f;
            } else {
                jumpChance = 0.0f;
            }
        }
    } else {
        jumpChance = 0.2f;
        if (m_playerObject->m_isShip) {
            float y = m_playerObject->getPosition().y;
            float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
            if (y < screenBottom + 90.0f + 15.0f + 100.0f) {
                jumpChance = 0.3f;
            } else {
                jumpChance = 0.0f;
            }
        } else if (m_playerObject->m_isBall || m_playerObject->m_isSpider) {
            jumpChance = -1.0f;
        } else if (m_playerObject->m_isBird) {
            float y = m_playerObject->getPosition().y;
            float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
            if (y < screenBottom + 90.0f + 15.0f + 200.0f) {
                jumpChance = 0.4f;
            } else {
                jumpChance = 0.0f;
            }
        } else if (m_playerObject->m_isDart) {
            float y = m_playerObject->getPosition().y;
            float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
            if (y >= screenBottom + 90.0f + 75.0f) {
                if (m_playerObject->getPosition().y <= winSize.height - 75.0f)
                    jumpChance = 0.5f;
                else
                    jumpChance = 0.0f;
            } else {
                jumpChance = 1.0f;
            }
        } else if (m_playerObject->m_isSwing) {
            if (m_playerObject->m_isUpsideDown) {
                if (m_playerObject->getPosition().y > 210.0f)
                    jumpChance = 0.9f;
                else
                    jumpChance = 0.0f;
            } else {
                if (m_playerObject->getPosition().y < 150.0f)
                    jumpChance = 0.9f;
                else
                    jumpChance = 0.0f;
            }
        }
    }

    if (jumpChance >= (float)rand() * 4.6566e-10f) {
        if (m_playerObject->m_isOnGround2 || m_playerObject->isFlying())
            m_playerObject->pushButton(Jump);
    } else {
        m_playerObject->releaseButton(Jump);
    }
}

void MenuGameLayer::update(float dt)
{
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_deltaCount += dt;
    updateColors();

    cocos2d::CCPoint playerPos = m_playerObject->getPosition();
    m_playerObject->m_shipRotation = playerPos;
    m_playerObject->updateInternalActions(dt);
    m_playerObject->update(dt * 60.0f);

    float angleOffset = 0.0f;
    float vehicleSize = m_playerObject->m_vehicleSize;
    float unkAngle1 = m_playerObject->m_unkAngle1;
    m_playerObject->m_totalTime = m_deltaCount;

    if (vehicleSize != 1.0f)
        angleOffset = (unkAngle1 * (1.0f - vehicleSize)) * 0.5f;

    float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
    float minY = (screenBottom + 90.0f + (unkAngle1 * 0.5f)) - angleOffset;

    if (minY > m_playerObject->getPosition().y) {
        m_playerObject->setPosition(ccp(m_playerObject->getPosition().x, minY));
        m_playerObject->hitGround(nullptr, false);
    }

    m_playerObject->updateRotation(dt * 60.0f);

    if (m_playerObject->getPosition().x > winSize.width + 100.0f)
        resetPlayer();

    m_realBackgroundPosition.x += ((dt * 60.0f) * 5.77f) * 0.9f;
    m_realBackgroundPosition.y = 0.0f;

    cocos2d::CCPoint bgPos = ccp(
        -m_realBackgroundPosition.x * 0.1f,
        -m_realBackgroundPosition.y * 0.1f);
    float bgX = bgPos.x;
    while (-m_backgroundWrapPosition > bgX)
        bgX += m_backgroundWrapPosition;
    bgPos.x = bgX;
    m_backgroundSprite->setPosition(bgPos);

    cocos2d::CCPoint groundPos = ccp(-m_realBackgroundPosition.x, 0.0f);
    float groundX = groundPos.x;
    float textureWidth = m_groundLayer->m_textureWidth;
    while (-textureWidth > groundX)
        groundX += textureWidth;
    groundPos.x = groundX;
    m_groundLayer->updateGroundPos(groundPos);

    m_playerObject->updateEffects(dt);
}

void MenuGameLayer::updateColor(float dt)
{
    cocos2d::ccColor3B color = getBGColor(m_initCount);
    m_initCount++;

    m_backgroundSprite->stopAllActions();
    cocos2d::CCTintTo* tint = cocos2d::CCTintTo::create(4.0f, color.r, color.g, color.b);
    m_backgroundSprite->runAction(tint);

    float groundFactor = m_groundLayer->m_ground2Sprite ? 0.6f : 0.8f;

    m_groundLayer->m_ground1Sprite->stopAllActions();
    cocos2d::CCTintTo* tint1 = cocos2d::CCTintTo::create(
        4.0f,
        (GLubyte)(groundFactor * (float)color.r),
        (GLubyte)(groundFactor * (float)color.g),
        (GLubyte)(groundFactor * (float)color.b));
    m_groundLayer->m_ground1Sprite->runAction(tint1);

    if (m_groundLayer->m_ground2Sprite) {
        m_groundLayer->m_ground2Sprite->stopAllActions();
        cocos2d::CCTintTo* tint2 = cocos2d::CCTintTo::create(
            4.0f,
            (GLubyte)(float)color.r,
            (GLubyte)(float)color.g,
            (GLubyte)(float)color.b);
        m_groundLayer->m_ground2Sprite->runAction(tint2);
    }
}

void MenuGameLayer::updateColors()
{
    cocos2d::ccColor3B color1 = m_groundLayer->m_ground1Sprite->getColor();
    m_groundLayer->updateGround01Color(color1);

    if (m_groundLayer->m_ground2Sprite) {
        cocos2d::ccColor3B color2 = m_groundLayer->m_ground2Sprite->getColor();
        m_groundLayer->updateGround02Color(color2);
    }
}

MenuGameLayer::~MenuGameLayer()
{
}