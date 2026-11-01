#include "Objects/PlayerObject.h"

#include "Delegates/AnimatedSpriteDelegate.h"
#include "Cocos/CCAnimatedSprite.h"
#include "Cocos/CCCircleWave.h"
#include "Objects/CheckpointObject.h"
#include "Objects/DashRingObject.h"
#include "Objects/EffectGameObject.h"
#include "Managers/GJActionManager.h"
#include "Layers/GJBaseGameLayer.h"
#include "Sprites/GJRobotSprite.h"
#include "Sprites/GJSpiderSprite.h"
#include "Managers/GameManager.h"
#include "Objects/GameObject.h"
#include "Utils/GameToolbox.h"
#include "Objects/GhostTrailEffect.h"
#include "Objects/HardStreak.h"
#include "Utils/OBB2D.h"
#include "Layers/PlayLayer.h"
#include "Objects/PlayerCheckpoint.h"
#include "Sprites/PlayerFireBoostSprite.h"
#include "Objects/RingObject.h"
#include "Managers/SpriteAnimationManager.h"

#include <algorithm>
#include <cmath>

void PlayerObject::activateStreak()
{
    if (levelFlipping() || GameManager::sharedState()->m_editorEnabled || m_isHidden)
        return;

    m_fadeOutStreak = true;
    m_regularTrail->resumeStroke();

    if (!m_isDart)
        return;

    cocos2d::CCPoint currentPos = getPosition();
    m_waveTrail->m_currentPoint = currentPos;
    m_waveTrail->stopAllActions();
    m_waveTrail->setOpacity(255);
    m_waveTrail->resumeStroke();
}

void PlayerObject::addAllParticles()
{
    m_parentLayer->addChild(m_playerGroundParticles, 39);
    m_parentLayer->addChild(m_ufoClickParticles, 39);
    m_parentLayer->addChild(m_dashParticles, 39);
    m_parentLayer->addChild(m_robotBurstParticles, 39);
    m_parentLayer->addChild(m_trailingParticles, 39);
    m_parentLayer->addChild(m_shipClickParticles, 39);
    m_parentLayer->addChild(m_vehicleGroundParticles, 61);
    m_parentLayer->addChild(m_landParticles0, 61);
    m_parentLayer->addChild(m_landParticles1, 61);
    m_parentLayer->addChild(m_swingBurstParticles1, 39);
    m_parentLayer->addChild(m_swingBurstParticles2, 39);
}

void PlayerObject::addToTouchedRings(RingObject* object)
{
    if (!m_touchingRings->containsObject(object))
        m_touchingRings->addObject(object);

    m_uniqueID = object->m_uniqueID;
    m_touchedRings.insert(m_uniqueID);
}

void PlayerObject::addToYVelocity(double yVelocity, int type)
{
    setYVelocity(yVelocity + m_yVelocity, type);
}

void PlayerObject::animatePlatformerJump(float scale)
{
    bool canAnimate = !m_holdingLeft
        && !m_holdingRight
        && !m_platformerMovingLeft
        && !m_platformerMovingRight
        && isInNormalMode();

    if (!canAnimate)
        return;

    float jumpScale = scale * 1.35f;
    float currentRotation = fabsf(getRotation());
    int rotationInt = static_cast<int>(currentRotation);

    bool onWall = (unsigned int)(rotationInt - 226) <= 0x58
        || (unsigned int)(rotationInt - 46) <= 0x58;

    float squashScale;
    float stretchScale;
    if (onWall)
    {
        squashScale = 0.9f;
        stretchScale = jumpScale;
    }
    else
    {
        squashScale = 1.1f;
        stretchScale = 0.8f;
    }

    float gravityDivisor = m_gravityMod;

    cocos2d::CCScaleTo* stretch1 = cocos2d::CCScaleTo::create(0.1f / gravityDivisor, stretchScale);
    cocos2d::CCEaseInOut* stretchEase = cocos2d::CCEaseInOut::create(stretch1, 2.0f);

    cocos2d::CCScaleTo* squash = cocos2d::CCScaleTo::create(0.25f / gravityDivisor, squashScale);
    cocos2d::CCEaseInOut* squashEase = cocos2d::CCEaseInOut::create(squash, 2.0f);

    cocos2d::CCScaleTo* stretch2 = cocos2d::CCScaleTo::create(0.15f / gravityDivisor, 1.0f);
    cocos2d::CCEaseInOut* stretchEase2 = cocos2d::CCEaseInOut::create(stretch2, 2.0f);

    cocos2d::CCAction* sequence = cocos2d::CCSequence::create(stretchEase, squashEase, stretchEase2, nullptr);
    sequence->setTag(13);

    m_iconSprite->runAction(sequence);
    m_iconGlow->runAction(sequence);
}

void PlayerObject::animationFinished(const char* key)
{
    std::string animation = key;

    if (animation == "jump_start")
    {
        m_robotSprite->runAnimation("jump_loop");
        return;
    }

    if (animation == "fall_start")
    {
        m_robotSprite->runAnimation("fall_loop");
        m_spiderSprite->runAnimation("fall_loop");
        return;
    }

    if (animation != "idle02")
        return;

    if (m_isRobot)
    {
        m_robotSprite->runAnimation("idle01");
        return;
    }

    if (m_isSpider)
        m_spiderSprite->runAnimation("idle01");
}

void PlayerObject::boostPlayer(float yVelocity)
{
    m_maybeIsBoosted = true;
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = true;
    m_isAccelerating = true;

    setYVelocity(yVelocity, 45);

    if (!m_isDashing)
    {
        bool isSpiderOnly = m_isSpider
            && !isFlying()
            && !m_isBall
            && !m_isRobot;

        if (isSpiderOnly)
        {
            m_spiderSprite->tweenToAnimation("fall_loop", 0.1f);
        }
        else
        {
            bool isFlyingOrBallOrRobot = isFlying() || m_isBall || m_isRobot;
            if (!isFlyingOrBallOrRobot)
            {
                stopRotation(true, 20);

                float divisor = (m_vehicleSize == 1.0f) ? 0.86667f : 0.66667f;
                int flip = flipMod();
                m_isRotating = true;
                m_rotationSpeed = (-180.0f * flip) / divisor;
            }
        }
    }

    m_lastGroundedPos = getPosition();
}

void PlayerObject::bumpPlayer(float bumpMod, int objectType, bool noEffects, GameObject* object)
{
    bool isRobotJumpPad = objectType == 44;

    bool handledByFix = m_fixRobotJump && !m_isPlatformer;
    if (handledByFix)
    {
        if (!isRobotJumpPad)
        {
            propellPlayer(bumpMod, noEffects, objectType);
            if (m_isPlatformer && !m_isRotating)
            {
                float scale = (objectType == 34) ? 1.2f : 1.0f;
                animatePlatformerJump(scale);
            }
            if (objectType == 34)
            {
                m_isAccelerating = true;
                m_lastGroundedPos = cocos2d::CCPoint(0.0f, 0.0f);
            }
            else
            {
                m_isAccelerating = false;
            }
            return;
        }
    }
    else
    {
        m_touchedPad = true;
        if (!isRobotJumpPad)
        {
            propellPlayer(bumpMod, noEffects, objectType);
            if (m_isPlatformer && !m_isRotating)
            {
                float scale = (objectType == 34) ? 1.2f : 1.0f;
                animatePlatformerJump(scale);
            }
            if (objectType == 34)
            {
                m_isAccelerating = true;
                m_lastGroundedPos = cocos2d::CCPoint(0.0f, 0.0f);
            }
            else
            {
                m_isAccelerating = false;
            }
            return;
        }
    }

    if (object)
    {
        bool facingOpposite;
        if (m_isSideways)
            facingOpposite = m_isUpsideDown != object->isFacingLeft();
        else
            facingOpposite = m_isUpsideDown != object->isFacingDown();

        if (facingOpposite)
            flipGravity(!m_isUpsideDown, true);
    }

    spiderTestJump(false);
    playBumpEffect(44, object);
}

bool PlayerObject::buttonDown(PlayerButton button)
{
    bool isJump = button == PlayerButton::Jump;
    if (!isJump)
        return false;

    return false;
}

bool PlayerObject::canStickToGround()
{
    if (m_isShip)
        return !m_stateRingJump;
    if (m_isDart)
        return !m_stateRingJump;
    return true;
}

void PlayerObject::checkSnapJumpToObject(GameObject* object)
{
    if (!object)
        return;

    bool hadDifferentSnap = m_objectSnappedTo
        && m_objectSnappedTo->m_uniqueID != object->m_uniqueID
        && !m_objectSnappedTo->getType();

    if (hadDifferentSnap)
    {
        cocos2d::CCPoint snappedReal = m_objectSnappedTo->getRealPosition();
        cocos2d::CCPoint objectReal = object->getRealPosition();

        float maxSnapDistance = 1.0f;
        float distanceNear = 90.0f;
        float distanceFar = 150.0f;
        float distanceSide = 120.0f;

        if (m_playerSpeed == 0.9f)
        {
            maxSnapDistance = 1.0f;
            distanceNear = 90.0f;
            distanceFar = 150.0f;
            distanceSide = (m_vehicleSize == 1.0f) ? 120.0f : 90.0f;
        }
        else if (m_playerSpeed == 0.7f)
        {
            maxSnapDistance = 1.0f;
            distanceNear = 60.0f;
            distanceFar = 120.0f;
            distanceSide = 90.0f;
        }
        else if (m_playerSpeed == 1.1f)
        {
            maxSnapDistance = 2.0f;
            distanceNear = 120.0f;
            distanceFar = 195.0f;
            distanceSide = (m_vehicleSize == 1.0f) ? 150.0f : 90.0f;
        }
        else if (m_playerSpeed == 1.3f)
        {
            maxSnapDistance = 2.0f;
            distanceNear = 135.0f;
            distanceFar = 225.0f;
            distanceSide = (m_vehicleSize == 1.0f) ? 180.0f : 90.0f;
        }
        else
        {
            maxSnapDistance = 1.0f;
            distanceNear = 90.0f;
            distanceFar = 150.0f;
            distanceSide = 120.0f;
        }

        float snappedX = snappedReal.x;
        float snappedY = snappedReal.y;
        float objectX = objectReal.x;
        float objectY = objectReal.y;
        float myY = getPosition().y;
        int flip = flipMod();

        bool nearMatch = maxSnapDistance >= fabsf(objectY - (snappedY + distanceSide))
            && maxSnapDistance >= fabsf(myY - (snappedY + flip * 30.0f));

        bool farMatch = maxSnapDistance >= fabsf(objectY - (snappedY + distanceFar))
            && maxSnapDistance >= fabsf(myY - (snappedY + flip * -30.0f));

        bool sideMatch = maxSnapDistance >= fabsf(objectY - (snappedY + distanceNear))
            && maxSnapDistance >= fabsf(myY - (snappedY + flip * 60.0f));

        if (!nearMatch && !farMatch && !sideMatch)
        {
            double targetX = objectX + m_snapDistance;
            double myX = getPosition().x;

            if (vabdd_f64(targetX, myX) > (double)maxSnapDistance)
            {
                if (targetX <= myX)
                    targetX = myX - maxSnapDistance;
                else
                    targetX = myX + maxSnapDistance;
            }

            cocos2d::CCPoint newPos(static_cast<float>(targetX), getPosition().y);
            setPosition(&newPos);
        }
    }

    m_objectSnappedTo = object;
    float myX = getPosition().x;
    m_snapDistance = myX - object->getPosition().x;
}

bool PlayerObject::collidedWithObject(float dt, GameObject* object)
{
    cocos2d::CCRect empty;
    return collidedWithObject(dt, object, empty, false);
}

bool PlayerObject::collidedWithObject(float dt, GameObject* object, cocos2d::CCRect& rect, bool skipPre)
{
    return false;
}

void PlayerObject::collidedWithSlope(float dt, GameObject* object, bool skipPre)
{
    if (m_isSideways)
        handleRotatedSlopeCollision(dt, object, skipPre);
    else
        collidedWithSlopeInternal(dt, object, skipPre);
}

float PlayerObject::convertToClosestRotation(float rotation)
{
    if (isFlying() || m_isRobot || m_isSpider || m_isDashing)
        return rotation;

    float currentRotation = (float)((int)getRotation() % 360);
    float lowerBound = rotation;

    if (rotation > currentRotation)
    {
        while (true)
        {
            rotation -= 90.0f;
            if (currentRotation >= rotation)
                break;
            lowerBound = rotation;
        }
    }
    else if (rotation < currentRotation)
    {
        while (true)
        {
            rotation += 90.0f;
            if (currentRotation <= rotation)
                break;
            lowerBound = rotation;
        }
    }

    if (vabds_f32(currentRotation, rotation) < vabds_f32(currentRotation, lowerBound))
        return rotation;
    return lowerBound;
}

void PlayerObject::copyAttributes()
{
    m_totalTime = playerObject->m_totalTime;
    m_maybeReducedEffects = true;

    cocos2d::CCPoint storedPos = playerObject->m_position;
    m_position = storedPos;
    setPosition(&m_position);

    m_shipRotation = playerObject->m_shipRotation;

    flipGravity(playerObject->m_isUpsideDown, true);
    doReversePlayer(playerObject->m_isGoingLeft);
    toggleFlyMode(playerObject->m_isShip, false);
    toggleBirdMode(playerObject->m_isBird, false);
    toggleRollMode(playerObject->m_isBall, false);
    toggleDartMode(playerObject->m_isDart, false);
    toggleRobotMode(playerObject->m_isRobot, false);
    toggleSpiderMode(playerObject->m_isSpider, false);
    toggleSwingMode(playerObject->m_isSwing, false);
    updateTimeMod(playerObject->m_playerSpeed, false);
    togglePlayerScale(playerObject->m_vehicleSize != 1.0f, false);

    double otherYVel = playerObject->getYVelocity();
    setYVelocity(otherYVel, 48);

    m_maybeReducedEffects = false;
    m_jumpBuffered = playerObject->m_jumpBuffered;
    m_stateRingJump = playerObject->m_stateRingJump;
}

void PlayerObject::createFadeOutDartStreak()
{
    HardStreak* duplicate = m_waveTrail->createDuplicate();
    cocos2d::CCNode* parent = m_waveTrail->getParent();
    parent->addChild(duplicate, m_waveTrail->getZOrder());
    duplicate->setPosition(m_waveTrail->getPosition());
    duplicate->resumeStroke();
    duplicate->scheduleAutoUpdate();

    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        cocos2d::CCFadeTo::create(0.5f, 0),
        cocos2d::CCCallFunc::create(duplicate, callfunc_selector(HardStreak::stopStroke)),
        nullptr);
    duplicate->runAction(sequence);
}

void PlayerObject::createRobot(int frame)
{
    bool hadExistingRobot = m_robotSprite != nullptr;

    cocos2d::ccColor3B oldMainColor = {0, 0, 0};
    cocos2d::ccColor3B oldSecondColor = {0, 0, 0};
    bool wasGlowVisible = false;
    std::string previousAnimation;

    if (hadExistingRobot)
    {
        oldMainColor = m_robotSprite->m_color;
        oldSecondColor = m_robotSprite->m_secondColor;
        wasGlowVisible = m_robotSprite->m_glowSprite->isVisible();
        previousAnimation = m_robotSprite->m_animationManager->m_currentAnimation;

        m_robotBatchNode->removeMeAndCleanup();
        m_robotBatchNode->release();
        m_robotSprite = nullptr;
    }

    m_robotSprite = GJRobotSprite::create(frame);
    m_robotSprite->m_delegate = m_animatedSpriteDelegate;

    cocos2d::CCTexture2D* robotTexture = m_robotSprite->getTexture();
    m_robotBatchNode = cocos2d::CCSpriteBatchNode::createWithTexture(robotTexture, 1);
    m_robotBatchNode->addChild(m_robotSprite);
    m_robotBatchNode->retain();

    if (!hadExistingRobot)
        return;

    if (m_isRobot)
        m_mainLayer->addChild(m_robotBatchNode, 2);

    m_robotSprite->m_color = oldMainColor;
    m_robotSprite->m_secondColor = oldSecondColor;
    m_robotSprite->updateColors();
    m_robotSprite->updateGlowColor(m_playerColor2, false);
    m_robotSprite->runAnimation(previousAnimation);

    if (wasGlowVisible)
        m_robotSprite->showGlow();
}

void PlayerObject::createSpider(int frame)
{
    bool hadExistingSpider = m_spiderSprite != nullptr;

    cocos2d::ccColor3B oldMainColor = {0, 0, 0};
    cocos2d::ccColor3B oldSecondColor = {0, 0, 0};
    bool wasGlowVisible = false;
    std::string previousAnimation;

    if (hadExistingSpider)
    {
        oldMainColor = m_spiderSprite->m_color;
        oldSecondColor = m_spiderSprite->m_secondColor;
        wasGlowVisible = m_spiderSprite->m_glowSprite->isVisible();
        previousAnimation = m_spiderSprite->m_animationManager->m_currentAnimation;

        m_spiderBatchNode->removeMeAndCleanup();
        m_spiderBatchNode->release();
        m_spiderSprite = nullptr;
    }

    m_spiderSprite = GJSpiderSprite::create(frame);
    m_spiderSprite->m_delegate = m_animatedSpriteDelegate;

    cocos2d::CCTexture2D* spiderTexture = m_spiderSprite->getTexture();
    m_spiderBatchNode = cocos2d::CCSpriteBatchNode::createWithTexture(spiderTexture, 1);
    m_spiderBatchNode->addChild(m_spiderSprite);
    m_spiderBatchNode->retain();

    if (!hadExistingSpider)
        return;

    if (m_isSpider)
        m_mainLayer->addChild(m_spiderBatchNode, 2);

    m_spiderSprite->m_color = oldMainColor;
    m_spiderSprite->m_secondColor = oldSecondColor;
    m_spiderSprite->updateColors();
    m_spiderSprite->updateGlowColor(m_playerColor2, false);
    m_spiderSprite->runAnimation(previousAnimation);

    if (wasGlowVisible)
        m_spiderSprite->showGlow();
}

void PlayerObject::deactivateParticle()
{
    if (m_hasGroundParticles)
        m_playerGroundParticles->stopSystem();
    m_hasGroundParticles = false;
}

void PlayerObject::deactivateStreak(bool stop)
{
    if (m_alwaysShowStreak <= (unsigned int)stop)
        m_regularTrail->stopStroke();

    if (m_fadeOutStreak)
    {
        m_fadeOutStreak = false;
        fadeOutStreak2(m_playEffects ? 0.2f : 0.6f);
    }
}

bool PlayerObject::destroyFromHitHead()
{
    if (isFlying())
        return false;
    if (m_isBall || m_isSpider)
        return false;
    return m_stateHitHead <= 0;
}

void PlayerObject::didHitHead()
{
    if (m_stateFlipGravity <= 0)
        return;

    hardFlipGravity();
    m_isOnGround2 = false;
    m_maybeIsBoosted = true;

    if (m_stateNoAutoJump <= 0)
        return;

    m_isOnGround = false;
    m_stateRingJump = false;
    m_jumpBuffered = 0;
}

void PlayerObject::disableCustomGlowColor()
{
    m_hasCustomGlowColor = false;
}

void PlayerObject::disablePlayerControls()
{
    m_controlsDisabled = false;
    m_inputsLocked = true;
    releaseButton(PlayerButton::Jump);
    releaseButton((PlayerButton)2);
    releaseButton((PlayerButton)3);
    releaseButton((PlayerButton)5);
    m_inputsLocked = false;
    m_controlsDisabled = true;
}

void PlayerObject::disableSwingFire()
{
    m_swingFireMiddle->setVisible(false);
    m_swingFireMiddle->stopAllActions();
    m_swingFireBottom->setVisible(false);
    m_swingFireBottom->stopAllActions();
    m_swingFireTop->setVisible(false);
    m_swingFireTop->stopAllActions();
    m_swingBurstParticles1->stopSystem();
    m_swingBurstParticles2->stopSystem();
}

void PlayerObject::doReversePlayer(bool reverse)
{
    bool directionChanged = m_isGoingLeft != reverse;
    m_isGoingLeft = reverse;

    float scaleMultiplier = (float)reverseMod();
    float sideSign = m_isSideways ? -1.0f : 1.0f;

    m_waveTrail->m_isFlipped = reverse;

    if (directionChanged)
    {
        m_reverseRelated = 2;
        if (m_isDart)
            createFadeOutDartStreak();
    }

    m_vehicleGroundParticles->setScaleX(sideSign * scaleMultiplier);
    updatePlayerGlow();
    updatePlayerArt();

    if (directionChanged && m_fadeOutStreak && m_isDart)
    {
        m_waveTrail->reset();
        placeStreakPoint();
    }

    if (m_isBall)
        runRotateAction(true, 1);
}

void PlayerObject::enableCustomGlowColor(const cocos2d::ccColor3B* color)
{
    m_hasCustomGlowColor = true;
    m_glowColor = *color;
}

void PlayerObject::enablePlayerControls()
{
    m_controlsDisabled = false;

    if (m_holdingButtons.find(2) == m_holdingButtons.end())
    {
        m_holdingButtons[2] = false;
        if (!m_holdingButtons[2])
            switchedDirTo((PlayerButton)2);
    }
    else if (m_holdingButtons[2])
    {
        switchedDirTo((PlayerButton)2);
    }

    if (m_holdingButtons.find(3) == m_holdingButtons.end())
    {
        m_holdingButtons[3] = false;
        if (!m_holdingButtons[3])
            switchedDirTo((PlayerButton)3);
    }
    else if (m_holdingButtons[3])
    {
        switchedDirTo((PlayerButton)3);
    }

    if (m_holdingButtons.find(1) == m_holdingButtons.end())
    {
        m_holdingButtons[1] = false;
        if (!m_holdingButtons[1])
            pushButton(PlayerButton::Jump);
    }
    else if (m_holdingButtons[1])
    {
        pushButton(PlayerButton::Jump);
    }
}

void PlayerObject::exitPlatformerAnimateJump()
{
    if (m_iconSprite->getActionByTag(13))
    {
        m_iconSprite->stopActionByTag(13);
        m_iconSprite->stopActionByTag(14);

        cocos2d::CCScaleTo* resetScale = cocos2d::CCScaleTo::create(0.1f, 1.0f);
        cocos2d::CCEaseInOut* resetEase = cocos2d::CCEaseInOut::create(resetScale, 2.0f);
        resetEase->setTag(14);
        m_iconSprite->runAction(resetEase);

        m_iconGlow->stopActionByTag(13);
        m_iconGlow->stopActionByTag(14);

        cocos2d::CCScaleTo* resetGlowScale = cocos2d::CCScaleTo::create(0.1f, 1.0f);
        cocos2d::CCEaseInOut* resetGlowEase = cocos2d::CCEaseInOut::create(resetGlowScale, 2.0f);
        resetGlowEase->setTag(14);
        m_iconGlow->runAction(resetGlowEase);
    }
}

void PlayerObject::fadeOutStreak2(float duration)
{
    if (m_waveTrail->getActionByTag(8))
        return;

    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        cocos2d::CCFadeOut::create(duration),
        cocos2d::CCCallFunc::create(m_waveTrail, callfunc_selector(HardStreak::stopStroke)),
        nullptr);
    sequence->setTag(8);
    m_waveTrail->runAction(sequence);
}

void PlayerObject::flipGravity(bool flip, bool noEffects)
{
    if (m_isUpsideDown == flip)
        return;

    placeStreakPoint();
    m_isUpsideDown = flip;

    if (m_gameLayer && flip)
        gameEventTriggered(10, 0);

    m_unkA29 = false;
    m_lastFlipTime = m_totalTime;
    m_collidedBottomMaxY = 0.0;
    m_collidedTopMinY = 0.0;

    if (m_wasOnSlope || m_isOnSlope)
        m_slopeFlipGravityRelated ^= 1;

    if (m_gameLayer && !flip)
        gameEventTriggered(11, 0);

    resetCollisionLog(true);

    if (!m_maybeReducedEffects)
    {
        m_yVelocity *= 0.5;

        if (!noEffects)
        {
            cocos2d::ccColor3B portalColor = flip
                ? cocos2d::ccc3(0, 200, 255)
                : cocos2d::ccc3(255, 200, 0);
            spawnPortalCircle(portalColor, 45.0f);
        }
    }

    updatePlayerScale();
    updatePlayerArt();

    bool skipCheckpointPlacement = m_isBall
        || !GameManager::sharedState()->m_playLayer
        || GameManager::sharedState()->m_playLayer->m_freezeStartCamera
        || m_isDart
        || m_maybeReducedEffects;

    if (skipCheckpointPlacement)
    {
        m_lastGroundedPos = m_lastPortalPos;
        m_isOnGround = false;
    }
    else
    {
        m_shouldTryPlacingCheckpoint = true;
        activateStreak();
        m_lastGroundedPos = m_lastPortalPos;
        m_isOnGround = false;
    }

    if (m_isBall)
    {
        stopRotation(true, 6);
        runBallRotation2();
        return;
    }

    if (m_isSwing)
        updateSwingFire();
}

int PlayerObject::flipMod()
{
    return m_isUpsideDown ? -1 : 1;
}

void PlayerObject::gameEventTriggered(int gameEvent, int material)
{
    if (m_gameLayer)
        m_gameLayer->gameEventTriggered((GJGameEvent)gameEvent, material, m_uniqueID);
}

GameObjectType PlayerObject::getActiveMode()
{
    if (isFlying())
        return GameObjectType::ShipPortal;
    if (m_isBall)
        return GameObjectType::BallPortal;
    if (m_isSpider)
        return (GameObjectType)33;
    return (GameObjectType)6;
}

double PlayerObject::getCurrentXVelocity()
{
    return 0.0;
}

float PlayerObject::getModifiedSlopeYVel()
{
    double timeOnSlope = m_totalTime - m_slopeStartTime;

    if (timeOnSlope >= 0.100000001)
        return m_slopeVelocity;

    double scaledTime = timeOnSlope * 10.0;

    if (scaledTime <= 0.400000006)
        return m_slopeVelocity * 0.400000006f;

    return m_slopeVelocity * (float)scaledTime;
}

float PlayerObject::getObjectRotation()
{
    return getRotation();
}

float PlayerObject::getOldPosition(float dt)
{
    if (dt <= 0.0)
        return m_obPosition.y;

    int currentIndex = m_followRelated % 200;
    int framesBack = (int)(dt / 0.01f);

    if (framesBack > 199)
        framesBack = 199;
    if (framesBack < 0)
        framesBack = 0;

    int targetIndex = currentIndex - framesBack;
    if (targetIndex < 0)
        targetIndex += 200;

    return m_playerFollowFloats[targetIndex];
}

OBB2D* PlayerObject::getOrientedBox()
{
    updateOrientedBox();
    return m_orientedBox;
}

cocos2d::CCPoint PlayerObject::getRealPosition()
{
    return getPosition();
}

cocos2d::ccColor3B PlayerObject::getSecondColor()
{
    return m_iconSpriteSecondary->getColor();
}

double PlayerObject::getYVelocity()
{
    return m_yVelocity;
}

void PlayerObject::gravityDown()
{
    m_gravity -= 0.00100000005;
    logValues();
}

void PlayerObject::gravityUp()
{
    m_gravity += 0.00100000005;
    logValues();
}

void PlayerObject::handlePlayerCommand(int command)
{
    if (command == 543)
    {
        m_isAccelerating = false;
        m_affectedByForces = false;
    }
}

void PlayerObject::handleRotatedSlopeCollision(float dt, GameObject* object, bool skipPre)
{
    cocos2d::CCRect empty;
    handleRotatedCollisionInternal(dt, object, empty, false, 0, skipPre);
}

void PlayerObject::hardFlipGravity()
{
    flipGravity(!m_isUpsideDown, true);
    int flip = flipMod();
    setYVelocity((double)(-2 * flip), 42);
}

void PlayerObject::hitGround(GameObject* object, bool notFlipped)
{
    m_groundObjectMaterial = object ? object->m_objectMaterial : 0;
    m_isOnIce = false;

    double incomingVelocity = m_yVelocity * (double)flipMod();

    if (m_gameLayer && !m_isDart)
    {
        if (playerIsFalling(-14.0f))
            gameEventTriggered(5, m_groundObjectMaterial);
        else if (playerIsFalling(-8.0f))
            gameEventTriggered(4, m_groundObjectMaterial);
        else if (playerIsFalling(-4.0f))
            gameEventTriggered(3, m_groundObjectMaterial);
        else if (playerIsFalling(-1.0f))
            gameEventTriggered(m_isOnGround2 ? 1 : 2, m_groundObjectMaterial);

        if (m_fallStartY != 0.0f)
        {
            float fallDistance = fabsf(m_fallStartY - getPosition().y);

            if (fallDistance > 0.0f)
            {
                if (fallDistance > 450.0f)
                    gameEventTriggered(68, m_groundObjectMaterial);
                else if (fallDistance > 300.0f)
                    gameEventTriggered(67, m_groundObjectMaterial);
                else if (fallDistance > 150.0f)
                    gameEventTriggered(66, m_groundObjectMaterial);
                else if (fallDistance > 7.5f)
                    gameEventTriggered(65, m_groundObjectMaterial);
            }

            m_fallStartY = 0.0f;
        }
    }

    setYVelocity(0.0, 43);
    m_isSliding = false;

    bool canSpawnLandParticles = !notFlipped
        && !m_isOnGround2
        && !levelFlipping()
        && !m_isHidden;

    if (canSpawnLandParticles)
    {
        m_useLandParticles0 = !m_useLandParticles0;

        if (m_useLandParticles0)
            m_landParticles1 = m_landParticles0;

        if (m_landParticles1->getParticleCount() <= 2)
        {
            m_landParticles1->setAngle(m_landParticlesAngle * flipMod());

            float sourceGravityX = m_landParticles0->getGravity().x;
            float sourceGravityY = m_landParticleRelatedY * flipMod();
            cocos2d::CCPoint particleGravity(sourceGravityX, sourceGravityY);
            m_landParticles1->setGravity(&particleGravity);

            cocos2d::CCPoint particleOffset(0.0f, (-15.0f * flipMod()) * m_vehicleSize);

            if (m_isSideways)
            {
                std::swap(particleOffset.x, particleOffset.y);
                m_landParticles1->setRotation(90.0f);
            }
            else
            {
                m_landParticles1->setRotation(0.0f);
            }

            if (object && object->getType() == GameObjectType::Slope)
            {
                float baseRotation = m_isSideways ? 90.0f : 0.0f;
                m_landParticles1->setRotation(baseRotation + m_slopeRotation * 57.2957802f);
            }

            cocos2d::CCPoint finalParticlePos = getPosition() + particleOffset;
            m_landParticles1->setPosition(&finalParticlePos);
            m_landParticles1->resetSystem();
        }
    }

    if (!m_justPlacedStreak)
    {
        placeStreakPoint();
        m_justPlacedStreak = true;
    }

    if (!m_isOnGround2 && !m_isDashing)
    {
        if (m_isRobot)
            m_robotSprite->runAnimation(m_currentRobotAnimation);
        else if (m_isSpider)
            playDynamicSpiderRun();
    }

    m_isOnGround2 = true;
    m_unkA29 = !notFlipped;

    if (incomingVelocity <= 5.0)
    {
        m_isOnGround = true;
        m_lastLandTime = m_totalTime;
    }

    m_isOnGround3 = true;
    m_stateOnGround = 2;

    if (m_isBall)
    {
        if (m_isBallRotating && !m_isOnSlope)
        {
            stopRotation(true, 16);
        }
        else if (m_isBallRotating2)
        {
            stopRotation(false, 18);
            if (!m_isRotating)
                runRotateAction(false, 6);
        }
        else if (!m_isRotating)
        {
            runRotateAction(false, 6);
        }
    }
    else
    {
        if (m_isRotating || m_wasOnSlope)
            stopRotation(true, 17);
    }

    if (!isFlying() && !m_isOnSlope && !m_wasOnSlope && ((int)getRotation() % 90) != 0)
        stopRotation(true, 19);

    m_lastGroundedPos = getPosition();

    if (!isFlying() && m_shouldTryPlacingCheckpoint)
    {
        deactivateStreak(false);
        m_canPlaceCheckpoint = true;
    }

    m_shouldTryPlacingCheckpoint = false;
}

void PlayerObject::hitGroundNoJump(GameObject* object, bool notFlipped)
{
    hitGround(nullptr, notFlipped);
}

void PlayerObject::incrementJumps()
{
    if (!m_playEffects)
        return;

    GameManager::sharedState()->m_playLayer->incrementJumps();
    m_hasEverJumped = true;
}

bool PlayerObject::isBoostValid(float yVelocity)
{
    if (m_isUpsideDown)
    {
        if (m_isCurrentSlopeTop || !m_slopeFlipGravityRelated)
            return m_yVelocity > yVelocity;
    }
    else if (m_isCurrentSlopeTop && m_slopeFlipGravityRelated)
    {
        return m_yVelocity > yVelocity;
    }

    return m_yVelocity < yVelocity;
}

bool PlayerObject::isFlying()
{
    if (!m_isShip)
    {
        m_isShip = m_isBird;

        if (!m_isShip)
            m_isShip = m_isDart;
    }

    return m_isShip;
}

bool PlayerObject::isInBasicMode()
{
    if (isFlying())
        return false;
    if (m_isBall)
        return false;
    return !m_isSpider;
}

bool PlayerObject::isInNormalMode()
{
    if (isFlying())
        return false;
    if (m_isBall || m_isRobot)
        return false;
    return !m_isSpider;
}

bool PlayerObject::isSafeFlip(float flipTime)
{
    if (m_lastFlipTime == 0.0)
        return false;
    return m_totalTime - m_lastFlipTime < flipTime;
}

bool PlayerObject::isSafeHeadTest()
{
    return isSafeFlip(0.2f) || isSafeMode(0.2f) || m_stateHitHead > 0;
}

bool PlayerObject::isSafeMode(float changeTime)
{
    if (m_gameModeChangedTime == 0.0)
        return false;
    return m_totalTime - m_gameModeChangedTime < changeTime;
}

bool PlayerObject::isSafeSpiderFlip(float flipTime)
{
    if (m_lastSpiderFlipTime == 0.0)
        return false;
    return m_totalTime - m_lastSpiderFlipTime < flipTime;
}

void PlayerObject::levelFlipFinished()
{
    m_trailingParticles->setLife(m_trailingParticleLife);

    if (isFlying())
    {
        resetStreak();
        activateStreak();
        if (m_isDart)
            placeStreakPoint();
    }
    else if (m_alwaysShowStreak)
    {
        resetStreak();
        activateStreak();
    }
}

bool PlayerObject::levelFlipping()
{
    if (!m_playEffects)
        return false;
    return GameManager::sharedState()->m_playLayer->isFlipping();
}

void PlayerObject::levelWillFlip()
{
    m_trailingParticles->setLife(m_trailingParticleLife * 0.3f);
    deactivateStreak(true);
}

void PlayerObject::limitDashRotation(float* rotation)
{
    float baseOffset = 0.0f;
    bool hasBaseOffset = false;

    if (m_isSideways)
    {
        baseOffset = m_isGoingLeft ? 90.0f : -90.0f;
        hasBaseOffset = true;
    }
    else if (m_isGoingLeft)
    {
        baseOffset = 180.0f;
        hasBaseOffset = true;
    }

    if (hasBaseOffset)
    {
        *rotation += baseOffset;
        snapRotation360(rotation);
    }

    float adjustedRotation = *rotation;

    if (fabsf(*rotation) > 90.0f)
    {
        if (adjustedRotation <= 0.0f)
            *rotation = -180.0f - adjustedRotation;
        else
            *rotation = 180.0f - adjustedRotation;

        snapRotation360(rotation);
        adjustedRotation = *rotation;
    }

    bool withinSafeRange = adjustedRotation <= 70.0f && adjustedRotation >= -70.0f;

    if (withinSafeRange)
    {
        if (hasBaseOffset)
        {
            *rotation -= baseOffset;
            snapRotation360(rotation);
        }
        return;
    }

    bool shouldClampPositive;
    if (adjustedRotation <= 0.0f)
        shouldClampPositive = adjustedRotation >= 70.0f || adjustedRotation <= -180.0f;
    else
        shouldClampPositive = adjustedRotation > 70.0f && adjustedRotation < 180.0f;

    *rotation = shouldClampPositive ? 70.0f : -70.0f;

    if (hasBaseOffset)
    {
        *rotation -= baseOffset;
        snapRotation360(rotation);
    }
}

void PlayerObject::loadFromCheckpoint(PlayerCheckpoint* object)
{
    m_maybeReducedEffects = true;
    setRotation(object->m_rotation);
    m_yVelocityBeforeSlope = object->m_yVelocityBeforeSlope;
    m_wasTeleported = object->m_wasTeleported;
    m_slopeStartTime = object->m_slopeStartTime;
    m_fixGravityBug = object->m_fixGravityBug;
    m_slopeAngle = object->m_slopeAngle;
    m_reverseSync = object->m_reverseSync;
    m_justPlacedStreak = object->m_justPlacedStreak;
    m_lastCollisionBottom = object->m_lastCollisionBottom;
    m_lastCollisionTop = object->m_lastCollisionTop;
    m_lastCollisionLeft = object->m_lastCollisionLeft;
    m_lastCollisionRight = object->m_lastCollisionRight;
    m_unk50C = object->m_unk50C;
    m_unk510 = object->m_unk510;
    m_currentSlope2 = object->m_currentSlope2;
    m_preLastGroundObject = object->m_preLastGroundObject;
    m_slopeSlidingMaybeRotated = object->m_slopeSlidingMaybeRotated;
    m_quickCheckpointMode = object->m_quickCheckpointMode;
    m_scaleXRelated2 = object->m_scaleXRelated2;
    m_collidedObject = object->m_collidedObject;
    m_groundYVelocity = object->m_groundYVelocity;
    m_lastGroundObject = object->m_lastGroundObject;
    m_yVelocityRelated = object->m_yVelocityRelated;
    m_collidingWithLeft = object->m_collidingWithLeft;
    m_scaleXRelated3 = object->m_scaleXRelated3;
    m_collidingWithRight = object->m_collidingWithRight;
    m_scaleXRelated4 = object->m_scaleXRelated4;
    m_collidingBetweenSteps = object->m_collidingBetweenSteps;
    m_scaleXRelated5 = object->m_scaleXRelated5;
    m_isCollidingWithSlope = object->m_isCollidingWithSlope;
    unk_584 = object->unk_584;
    m_isBallRotating = object->m_isBallRotating;
    m_unk669 = object->m_unk669;
    m_currentPotentialSlope = object->m_currentPotentialSlope;
    m_currentSlope = object->m_currentSlope;
    m_isBallRotating2 = object->m_isBallRotating2;
    m_speedMultiplier = object->m_speedMultiplier;
    m_slopeFlipGravityRelated = object->m_slopeFlipGravityRelated;
    m_yStart = object->m_yStart;
    m_isRotating = object->m_isRotating;
    m_gravity = object->m_gravity;
    m_gameModeChangedTime = object->m_gameModeChangedTime;
    m_padRingRelated = object->m_padRingRelated;
    m_slopeAngleRadians = object->m_slopeAngleRadians;
    m_maybeIsFalling = object->m_maybeIsFalling;
    m_rotationSpeed = object->m_rotationSpeed;
    m_shouldTryPlacingCheckpoint = object->m_shouldTryPlacingCheckpoint;
    m_rotateSpeed = object->m_rotateSpeed;
    m_playEffects = object->m_playEffects;
    m_trailingParticleLife = object->m_trailingParticleLife;
    m_collidingWithSlopeId = object->m_collidingWithSlopeId;
    m_isOnGround3 = object->m_isOnGround3;
    m_lastSpiderFlipTime = object->m_lastSpiderFlipTime;
    m_unkBool5 = object->m_unkBool5;
    m_ringJumpRelated = object->m_ringJumpRelated;
    m_ringRelatedSet = object->m_ringRelatedSet;
    m_slopeRotation = object->m_slopeRotation;
    m_unk3e0 = object->m_unk3e0;
    m_currentSlopeYVelocity = object->m_currentSlopeYVelocity;
    m_unk3d0 = object->m_unk3d0;
    m_unk3e1 = object->m_unk3e1;
    m_blackOrbRelated = object->m_blackOrbRelated;
    m_isAccelerating = object->m_isAccelerating;
    m_collidedTopMinY = object->m_collidedTopMinY;
    m_isCurrentSlopeTop = object->m_isCurrentSlopeTop;
    m_collidedBottomMaxY = object->m_collidedBottomMaxY;
    m_wasJumpBuffered = object->m_wasJumpBuffered;
    m_collidedLeftMaxX = object->m_collidedLeftMaxX;
    m_wasRobotJump = object->m_wasRobotJump;
    m_collidedRightMinX = object->m_collidedRightMinX;
    m_maybeSpriteRelated = object->m_maybeSpriteRelated;
    m_landParticlesAngle = object->m_landParticlesAngle;
    m_landParticleRelatedY = object->m_landParticleRelatedY;
    m_yVelocity = object->m_yVelocity;
    m_stateJumpBuffered = object->m_stateJumpBuffered;
    m_fallSpeed = object->m_fallSpeed;
    m_stateRingJump2 = object->m_stateRingJump2;
    m_maybeReverseAcceleration = object->m_maybeReverseAcceleration;
    m_touchedRing = object->m_touchedRing;
    m_xVelocityRelated2 = object->m_xVelocityRelated2;
    m_touchedCustomRing = object->m_touchedCustomRing;
    m_touchedGravityPortal = object->m_touchedGravityPortal;
    m_maybeTouchedBreakableBlock = object->m_maybeTouchedBreakableBlock;
    m_touchedPad = object->m_touchedPad;
    m_maybeUpsideDownSlope = object->m_maybeUpsideDownSlope;
    m_reverseRelated = object->m_reverseRelated;
    m_shipRotation = object->m_shipRotation;
    m_lastPortalPos = object->m_lastPortalPos;
    m_lastLandTime = object->m_lastLandTime;
    m_scaleXRelatedTime = object->m_scaleXRelatedTime;
    m_maybeIsBoosted = object->m_maybeIsBoosted;
    m_unkUnused3 = object->m_unkUnused3;
    m_isLocked = object->m_isLocked;
    m_platformerVelocityRelated = object->m_platformerVelocityRelated;
    m_controlsDisabled = object->m_controlsDisabled;
    m_isOnGround2 = object->m_isOnGround2;
    m_lastGroundedPos = object->m_lastGroundedPos;

    m_touchingRings->removeAllObjects();

    if (object->m_touchingRings)
    {
        for (int i = 0; i < object->m_touchingRings->count(); i++)
            m_touchingRings->addObject(object->m_touchingRings->objectAtIndex(i));
    }

    m_touchedRings = object->m_touchedRings;
    m_totalTime = object->m_totalTime;
    m_lastActivatedPortal = object->m_lastActivatedPortal;
    m_platformerXVelocity = object->m_platformerXVelocity;
    m_defaultMiniIcon = object->m_defaultMiniIcon;
    m_yVelocityRelated3 = object->m_yVelocityRelated3;
    m_swapColors = object->m_swapColors;
    m_stateUnk = object->m_stateUnk;
    m_switchDashFireColor = object->m_switchDashFireColor;
    m_scaleXRelated = object->m_scaleXRelated;
    m_stateOnGround = object->m_stateOnGround;
    m_stateBoostX = object->m_stateBoostX;
    m_stateBoostY = object->m_stateBoostY;
    m_maybeStateForce2 = object->m_maybeStateForce2;
    m_stateScale = object->m_stateScale;
    m_holdingRight = object->m_holdingRight;
    m_holdingLeft = object->m_holdingLeft;
    m_leftPressedFirst = object->m_leftPressedFirst;
    m_maybeHasStopped = object->m_maybeHasStopped;
    m_maybeSlopeForce = object->m_maybeSlopeForce;
    m_maybeGoingCorrectSlopeDirection = object->m_maybeGoingCorrectSlopeDirection;
    m_physDeltaRelated = object->m_physDeltaRelated;
    m_isSliding = object->m_isSliding;
    m_maybeSlidingStartTime = object->m_maybeSlidingStartTime;
    m_isOnIce = object->m_isOnIce;
    m_changedDirectionsTime = object->m_changedDirectionsTime;
    m_isOnGround4 = object->m_isOnGround4;
    m_slopeEndTime = object->m_slopeEndTime;
    m_maybeSlidingTime = object->m_maybeSlidingTime;
    m_xVelocityRelated = object->m_xVelocityRelated;
    m_isMoving = object->m_isMoving;
    m_maybeChangedDirectionAngle = object->m_maybeChangedDirectionAngle;
    m_platformerMovingLeft = object->m_platformerMovingLeft;
    m_platformerMovingRight = object->m_platformerMovingRight;
    m_isSlidingRight = object->m_isSlidingRight;
    m_stateNoAutoJump = object->m_stateNoAutoJump;
    m_unkUnused2 = object->m_unkUnused2;
    m_stateDartSlide = object->m_stateDartSlide;
    m_stateHitHead = object->m_stateHitHead;
    m_stateFlipGravity = object->m_stateFlipGravity;
    m_stateForce = object->m_stateForce;
    m_stateForceVector = object->m_stateForceVector;
    m_affectedByForces = object->m_affectedByForces;
    m_jumpPadRelated = object->m_jumpPadRelated;
    m_fallStartY = object->m_fallStartY;

    toggleVisibility(!object->m_isHidden);
    setPosition(&object->m_position);
    flipGravity(object->m_isUpsideDown, false);
    rotateGameplayOnly(object->m_isSideways);

    m_ghostType = object->m_ghostType;
    m_gravityMod = object->m_gravityMod;
    toggleGhostEffect(m_ghostType);
    togglePlayerScale(object->m_isMini, false);

    m_maybeReverseSpeed = object->m_maybeReverseSpeed;
    m_lastPosition = object->m_lastPosition;
    m_snapDistance = object->m_snapDistance;
    m_accelerationOrSpeed = object->m_accelerationOrSpeed;
    m_followRelated = object->m_followRelated;
    m_unk838 = object->m_unk838;
    m_objectSnappedTo = object->m_objectSnappedTo;
    m_playerFollowFloats = object->m_playerFollowFloats;
    m_wasOnSlope = object->m_wasOnSlope;
    m_isOnSlope = object->m_isOnSlope;
    m_isGoingLeft = object->m_isGoingLeft;
    m_slopeVelocity = object->m_slopeVelocity;

    if (m_isGoingLeft)
        doReversePlayer(true);

    updateTimeMod(object->m_playerSpeed, false);

    GameManager* gm = GameManager::sharedState();
    GameObject* lastPortal = gm->m_playLayer->m_gameState.m_lastActivatedPortal1;

    if (object->m_isShip)
        m_gameLayer->switchToFlyMode(this, lastPortal, true, 5);
    else if (object->m_isBall)
        m_gameLayer->switchToRollMode(this, lastPortal, true);
    else if (object->m_isBird)
        m_gameLayer->switchToFlyMode(this, lastPortal, true, 19);
    else if (object->m_isSwing)
        m_gameLayer->switchToFlyMode(this, lastPortal, true, 41);
    else if (object->m_isDart)
        m_gameLayer->switchToFlyMode(this, lastPortal, true, 26);
    else if (object->m_isRobot)
        m_gameLayer->switchToRobotMode(this, lastPortal, true);
    else if (object->m_isSpider)
        m_gameLayer->switchToSpiderMode(this, lastPortal, true);

    resetStreak();
    m_jumpBuffered = object->m_jumpBuffered;

    if (m_jumpBuffered && object->m_isDashing)
    {
        m_dashRing = object->m_dashRing;
        m_dashStartTime = object->m_dashStartTime;
        m_dashX = object->m_dashX;
        m_dashY = object->m_dashY;
        m_dashAngle = object->m_dashAngle;
        startDashing(nullptr);
    }

    setYVelocity(object->m_yVelocityUnrounded, 50);
    m_decreaseBoostSlide = object->m_decreaseBoostSlide;
    m_lastFlipTime = object->m_lastFlipTime;
    m_isOnGround = object->m_isOnGround;

    if (object->m_platformerCheckpoint)
    {
        releaseAllButtons();
        setYVelocity(0.0, 50);
        m_isOnGround = false;
        m_platformerXVelocity = 0.0;
    }

    setRotation(object->m_rotation);
    m_maybeReducedEffects = false;

    if (m_isPlatformer)
        releaseAllButtons();
}

void PlayerObject::lockPlayer()
{
    if (m_isDart)
        placeStreakPoint();

    m_isLocked = true;
    stopRotation(true, 100);
    releaseButton(PlayerButton::Jump);
    deactivateParticle();
    m_isOnGround2 = false;

    if (m_isDart)
        fadeOutStreak2(0.4f);
}

void PlayerObject::logValues()
{
}

void PlayerObject::modeDidChange()
{
    updatePlayerArt();
    updateDashArt();
}

void PlayerObject::performSlideCheck()
{
    bool onSlopeState = m_isOnSlope || m_wasOnSlope;
    bool checkSlope = m_currentSlope2
        && onSlopeState
        && !m_maybeUpsideDownSlope
        && !m_currentSlope2->m_isGripSlope;

    if (!checkSlope)
    {
        m_platformerMovingLeft = 0;
        m_platformerMovingRight = 0;
        return;
    }

    float slopeAngleDegrees = fabsf(m_slopeAngle * 57.296f);

    if (slopeAngleDegrees <= 40.0f)
    {
        m_platformerMovingLeft = 0;
        m_platformerMovingRight = 0;
        return;
    }

    if (m_slopeSlidingMaybeRotated)
    {
        m_platformerMovingRight = m_isUpsideDown;

        if (m_isUpsideDown)
            m_slopeSlidingMaybeRotated = 0;

        m_platformerMovingLeft = m_slopeSlidingMaybeRotated;

        if (m_platformerMovingLeft != m_slopeSlidingMaybeRotated)
        {
            m_changedDirectionsTime = m_totalTime;
            m_maybeGoingCorrectSlopeDirection = true;
            m_maybeChangedDirectionAngle = slopeAngleDegrees;
            return;
        }
    }
    else
    {
        int wasUpsideDown = m_isUpsideDown;
        m_platformerMovingLeft = wasUpsideDown;
        m_isUpsideDown = wasUpsideDown == 0;
        m_platformerMovingRight = m_isUpsideDown;

        if (m_platformerMovingLeft != wasUpsideDown)
        {
            m_changedDirectionsTime = m_totalTime;
            m_maybeGoingCorrectSlopeDirection = true;
            m_maybeChangedDirectionAngle = slopeAngleDegrees;
            return;
        }
    }

    if (m_platformerMovingRight == m_isUpsideDown)
    {
        m_maybeGoingCorrectSlopeDirection = true;
        m_maybeChangedDirectionAngle = slopeAngleDegrees;
        return;
    }

    m_changedDirectionsTime = m_totalTime;
    m_maybeGoingCorrectSlopeDirection = true;
    m_maybeChangedDirectionAngle = slopeAngleDegrees;
}

void PlayerObject::placeStreakPoint()
{
    bool shouldPlace = !m_maybeReducedEffects
        && m_isDart
        && !m_isLocked
        && m_fadeOutStreak
        && !levelFlipping();

    if (shouldPlace)
    {
        cocos2d::CCPoint currentPos = getPosition();
        m_waveTrail->addPoint(currentPos);
    }

    m_justPlacedStreak = false;
}

void PlayerObject::playBumpEffect(int objectType, GameObject* player)
{
    if (GameManager::sharedState()->m_performanceMode || !m_playEffects)
        return;

    CCCircleWave* wave;

    if (m_vehicleSize >= 1.0f && objectType == 34)
        wave = CCCircleWave::create(12.0, 40.0, 0.25, 0);
    else
        wave = CCCircleWave::create(10.0, 40.0, 0.25, 0);

    int red = 0;
    int green = 0;
    int blue = 0;

    if (objectType == 44)
    {
        red = -1;
        green = 50;
        blue = -1;
    }
    else if (objectType == 9)
    {
        red = -1;
        green = 0;
        blue = -1;
    }
    else if (objectType == 10)
    {
        red = -1;
        green = -1;
        blue = 0;
    }
    else
    {
        red = 0;
        green = -56;
        blue = -1;
    }

    wave->m_color.r = red;
    wave->m_color.g = green;
    wave->m_color.b = blue;
    wave->setPosition(&m_lastPortalPos);

    if (m_lastActivatedPortal)
    {
        CCCircleWave::followObject(wave, m_lastActivatedPortal, true);

        PlayLayer* layer = GameManager::sharedState()->m_playLayer;
        if (layer)
            wave->m_delegate = &layer->CCCircleWaveDelegate;

        GameManager::sharedState()->m_playLayer->addCircle(wave);
    }

    m_parentLayer->addChild(wave, 0);
}

void PlayerObject::playBurstEffect()
{
    if (levelFlipping() || m_isHidden)
        return;

    m_ufoClickParticles->resumeSystem();
    stopActionByTag(7);

    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.12f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(PlayerObject::stopBurstEffect)),
        nullptr);
    sequence->setTag(7);
    runAction(sequence);
}

void PlayerObject::playCompleteEffect(bool noEffects, bool instant)
{
    if (m_isDart)
        fadeOutStreak2(0.2f);

    stopDashing();
    deactivateParticle();
    m_trailingParticles->stopSystem();
    m_shipClickParticles->stopSystem();
    m_vehicleGroundParticles->stopSystem();
    m_robotBurstParticles->stopSystem();

    if (m_robotFire)
    {
        m_robotFire->stopAllActions();
        m_robotFire->setVisible(false);
    }

    toggleGhostEffect(GhostType::Disabled);
    disableSwingFire();

    if (getParent())
        m_parentLayer = (cocos2d::CCLayer*)getParent();

    if (instant)
    {
        setOpacity(0);
    }
    else
    {
        cocos2d::CCFadeTo* fade = cocos2d::CCFadeTo::create(0.1f, 0);
        fade->setTag(4);
        runAction(fade);
    }

    if (noEffects)
        return;

    cocos2d::CCParticleSystemQuad* vortexParticle = cocos2d::CCParticleSystemQuad::create("explodeEffectVortex.plist");
    vortexParticle->setPositionType(cocos2d::kCCPositionTypeGrouped);
    m_parentLayer->addChild(vortexParticle, 99);
    vortexParticle->setAutoRemoveOnFinish(true);
    vortexParticle->setPosition(getPosition());

    cocos2d::ccColor4F startColor(
        m_playerColor1.r / 255.0f,
        m_playerColor1.g / 255.0f,
        m_playerColor1.b / 255.0f,
        1.0f);
    cocos2d::ccColor4F endColor(0.0f, 0.0f, 0.0f, 1.0f);
    cocos2d::ccColor4F colorVar(0.0f, 0.0f, 0.0f, 0.0f);

    vortexParticle->setStartColor(startColor);
    vortexParticle->setEndColor(endColor);
    vortexParticle->setBlendAdditive(true);
    vortexParticle->setStartColorVar(colorVar);
    vortexParticle->setEndColorVar(colorVar);
    vortexParticle->setSpeed(vortexParticle->getSpeed());
    vortexParticle->setLife(vortexParticle->getLife() * 3.0f);

    cocos2d::CCPoint positionVariance = vortexParticle->getPosVar();
    positionVariance *= 1.2f;
    vortexParticle->setPosVar(positionVariance);
    vortexParticle->setStartSize(vortexParticle->getStartSize() * 0.8f);
    vortexParticle->setEndSize(vortexParticle->getEndSize() * 0.4f);
    vortexParticle->resetSystem();

    CCCircleWave* wave1 = CCCircleWave::create(20.0, 80.0, 0.72, 0);
    wave1->m_color = m_playerColor1;
    wave1->setPosition(getPosition());
    m_parentLayer->addChild(wave1, 99);

    CCCircleWave* wave2 = CCCircleWave::create(30.0, 50.0, 0.84, 0);
    wave2->m_color = m_playerColor2;
    wave2->setPosition(getPosition());
    wave2->m_opacityMod = 0.8f;
    m_parentLayer->addChild(wave2, 1000);

    CCCircleWave* wave3 = CCCircleWave::create(30.0, 20.0, 0.96, 0);
    wave3->m_color = m_playerColor1;
    wave3->setPosition(getPosition());
    wave3->m_opacityMod = 0.8f;
    m_parentLayer->addChild(wave3, 1000);
}

void PlayerObject::playDeathEffect()
{
    GameManager* gameManager = GameManager::sharedState();
    int deathEffectID = gameManager->m_playerDeathEffect.random - gameManager->m_playerDeathEffect.seed;

    bool isPracticeMode = gameManager->m_playLayer && gameManager->m_playLayer->m_isPracticeMode;

    if (isPracticeMode)
    {
        if (!m_practiceDeathEffect)
            deathEffectID = 1;

        if (getParent())
            m_parentLayer = (cocos2d::CCLayer*)getParent();
    }
    else if (getParent())
    {
        m_parentLayer = (cocos2d::CCLayer*)getParent();
    }

    if (m_vehicleSize >= 1.0f)
        m_vehicleSize *= 0.9f;

    switch (deathEffectID)
    {
        case 2:  playDeathEffectCase2();  break;
        case 3:  playDeathEffectCase3();  break;
        case 4:  playDeathEffectCase4();  break;
        case 5:  playDeathEffectCase5();  break;
        case 6:  playDeathEffectCase6();  break;
        case 7:  playDeathEffectCase7();  break;
        case 8:  playDeathEffectCase8();  break;
        case 9:  playDeathEffectCase9();  break;
        case 10: playDeathEffectCase10(); break;
        case 11: playDeathEffectCase11(); break;
        case 12: playDeathEffectCase12(); break;
        case 13: playDeathEffectCase13(); break;
        case 14: playDeathEffectCase14(); break;
        case 15: playDeathEffectCase15(); break;
        case 16: playDeathEffectCase16(); break;
        case 17: playDeathEffectCase17(); break;
        case 18: playDeathEffectCase18(); break;
        case 19: playDeathEffectCase19(); break;
        case 20: playDeathEffectCase20(); break;
        default: playDeathEffectCaseDefault(); break;
    }

    gameManager = GameManager::sharedState();

    if (gameManager->getGameVariable("0153"))
        return;

    float renderSize = m_vehicleSize * 40.0f;
    cocos2d::CCRenderTexture* renderTexture = cocos2d::CCRenderTexture::create((int)renderSize, (int)renderSize);

    cocos2d::CCPoint originalPosition = getPosition();
    cocos2d::CCPoint centerOffset(renderSize * 0.5f, renderSize * 0.5f);

    setPosition(&centerOffset);
    renderTexture->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
    m_dashSpritesContainer->visit();
    visit();
    renderTexture->end();
    setPosition(&originalPosition);

    ExplodeItemNode* explodeNode = ExplodeItemNode::create(renderTexture);
    m_parentLayer->addChild(explodeNode, 101);
    explodeNode->setPosition(&originalPosition);

    float verticalVelocity = getCurrentXVelocity() * 0.899999976f * 0.800000012f;

    int countX = (int)(roundf((float)rand() * 4.6566e-10f * 2.0f) + 2.0f);
    int countY = (int)(roundf((float)rand() * 4.6566e-10f * 2.0f) + 2.0f);

    float randValue = (float)rand() * 4.6566e-10f;

    if (randValue > 0.95f)
        countX = 1;
    else if (randValue > 0.9f)
        countY = 1;

    cocos2d::ccColor4F startColor(
        m_playerColor1.r / 255.0f,
        m_playerColor1.g / 255.0f,
        m_playerColor1.b / 255.0f,
        1.0f);
    cocos2d::ccColor4F endColor(0.0f, 0.0f, 0.0f, 0.0f);

    explodeNode->createSprites(
        countX,
        countY,
        verticalVelocity * 0.5f,
        verticalVelocity,
        6.0f,
        3.0f,
        1.4f,
        0.0f,
        startColor,
        endColor,
        false);
}

void PlayerObject::playDynamicSpiderRun()
{
    if (m_isDashing)
        return;

    std::string currentAnimation = m_spiderSprite->m_animationManager->m_currentAnimation;

    if (m_playerSpeed > 1.1f)
    {
        if (currentAnimation != "walk")
            return;

        const char* targetAnimation = m_spiderAnimationEnabled ? "run2" : "run";
        m_spiderSprite->runAnimation(targetAnimation);
        m_spiderSprite->m_animationManager->offsetCurrentAnimation(0.1f);
        return;
    }

    if (m_playerSpeed > 0.9f)
    {
        if (currentAnimation != "walk")
            return;

        m_spiderSprite->runAnimation("run");
        m_spiderSprite->m_animationManager->offsetCurrentAnimation(0.1f);
        return;
    }

    m_spiderSprite->runAnimation("walk");
}

void PlayerObject::playSpawnEffect()
{
    if (m_isBeingSpawnedByDualPortal)
        return;

    stopActionByTag(11);

    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        cocos2d::CCBlink::create(0.4f, 4),
        cocos2d::CCShow::create(),
        nullptr);
    sequence->setTag(11);
    runAction(sequence);

    if (GameManager::sharedState()->m_performanceMode)
        return;

    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCSequence* spawnSequence = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(i * 0.1f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayerObject::spawnCircle)),
            nullptr);
        runAction(spawnSequence);
    }
}
void PlayerObject::playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to)
{
    if (!m_playEffects)
        return;
    if (m_isHidden)
        return;

    from.x += (float)reverseMod() * 7.5f;
    to.x += (float)reverseMod() * 7.5f;

    if (!GameManager::sharedState()->m_performanceMode)
    {
        float baseSize = (m_vehicleSize * 10.0f) * 1.3f;
        to.y += 6.0f;

        cocos2d::ccColor3B waveColor = m_swapColors ? m_playerColor1 : m_playerColor2;

        PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;

        CCCircleWave* wave1 = CCCircleWave::create(baseSize, 1.0f, 0.15f, 0);
        wave1->m_color = waveColor;
        wave1->setPosition(&from);
        m_parentLayer->addChild(wave1, 0);
        wave1->m_delegate = &playLayer->CCCircleWaveDelegate;
        playLayer->addCircle(wave1);
        wave1->m_opacityMod = 0.62f;

        CCCircleWave* wave2 = CCCircleWave::create(baseSize * 2.0f, 2.0f, 0.25f, 0);
        wave2->m_color = waveColor;
        wave2->setPosition(&from);
        m_parentLayer->addChild(wave2, 0);
        wave2->m_delegate = &playLayer->CCCircleWaveDelegate;
        playLayer->addCircle(wave2);
        wave2->m_opacityMod = 0.78f;

        CCCircleWave* wave3 = CCCircleWave::create(baseSize * 3.0f, 3.0f, 0.25f, 0);
        wave3->m_color = waveColor;
        wave3->setPosition(&from);
        m_parentLayer->addChild(wave3, 0);
        wave3->m_delegate = &playLayer->CCCircleWaveDelegate;
        playLayer->addCircle(wave3);
        wave3->m_circleMode = CircleMode::Outline;
    }

    cocos2d::ccColor3B white1 = {255, 255, 255};
    cocos2d::ccColor3B white2 = {255, 255, 255};
    flashPlayer(0.3f, 0.05f, white1, white2);

    m_spiderSprite->stopActionByTag(10);
    m_spiderSprite->setScale(0.4f);

    cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.45f, 1.0f);
    cocos2d::CCEaseElasticOut* elasticEase = cocos2d::CCEaseElasticOut::create(scaleTo, 0.35f);
    elasticEase->setTag(10);
    m_spiderSprite->runAction(elasticEase);

    if (m_playEffects)
    {
        cocos2d::CCSprite* dashSprite = cocos2d::CCSprite::createWithSpriteFrameName("spiderDash_002.png");

        float rotation;
        if (m_isSideways)
        {
            if (m_isUpsideDown)
                rotation = 180.0f;
            else
                rotation = 0.0f;
        }
        else
        {
            if (m_isUpsideDown)
                rotation = 90.0f;
            else
                rotation = -90.0f;
        }

        dashSprite->setRotation(rotation);

        GameManager::sharedState()->m_playLayer->m_objectLayer->addChild(dashSprite, 40);
        dashSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

        float length;
        cocos2d::CCPoint midpoint;

        if (m_isSideways)
        {
            length = fabsf(from.y - to.y) + 30.0f;
            midpoint.x = from.x + (to.y - from.y) * 0.5f;
            midpoint.y = from.y;
        }
        else
        {
            length = fabsf(from.x - to.x) + 30.0f;
            midpoint.x = from.x;
            midpoint.y = from.y + (to.y - from.y) * 0.5f;
        }

        dashSprite->setScaleX(length / dashSprite->getContentSize().width);
        dashSprite->setPosition(&midpoint);

        cocos2d::ccColor3B spriteColor = m_swapColors ? m_playerColor1 : m_playerColor2;
        dashSprite->setColor(spriteColor);

        cocos2d::CCArray* dashFrames = cocos2d::CCArray::create();
        for (int i = 1; i < 9; i++)
        {
            cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("spiderDash_%03d.png", i);
            cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
            if (frame)
                dashFrames->addObject(frame);
        }

        cocos2d::CCAnimation* animation = cocos2d::CCAnimation::createWithSpriteFrames(dashFrames, 0.04f);
        cocos2d::CCSequence* animateSequence = cocos2d::CCSequence::create(
            cocos2d::CCAnimate::create(animation),
            cocos2d::CCCallFunc::create(dashSprite, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        dashSprite->runAction(animateSequence);
    }

    if (!m_isHidden && (!m_isSpider || !m_isPlatformer))
        m_playerGroundParticles->resetSystem();
}

void PlayerObject::playerDestroyed(bool noEffects)
{
    if (m_isDart)
        fadeOutStreak2(0.2f);

    stopDashing();
    m_isDead = true;
    stopRotation(true, 5);
    stopParticles();
    m_lastPosition = getPosition();

    if (m_robotFire)
    {
        m_robotFire->stopAllActions();
        m_robotFire->setVisible(false);
    }

    toggleGhostEffect(GhostType::Disabled);
    disableSwingFire();

    if (m_playEffects <= (unsigned int)noEffects || m_isHidden)
        setOpacity(0);
    else
        playDeathEffect();
}

bool PlayerObject::playerIsFalling(float yVelocity)
{
    if (m_isUpsideDown)
        return m_yVelocity > (double)-yVelocity;
    return m_yVelocity < (double)yVelocity;
}

bool PlayerObject::playerIsFallingBugged()
{
    if (m_isSideways || m_isPlatformer || m_isSwing || m_fixGravityBug)
    {
        float doubledGravity = m_gravity * 2.0f;
        return playerIsFalling(doubledGravity);
    }

    float adjustedGravity = m_gravity;
    if (m_unkA99)
        adjustedGravity = -adjustedGravity;

    if (m_isUpsideDown)
        return m_yVelocity > (m_gravity * 2.0f);

    return m_yVelocity < (double)(adjustedGravity * 2.0f);
}

bool PlayerObject::playerIsMovingUp()
{
    if (m_isUpsideDown)
        return m_yVelocity < 0.0;
    return m_yVelocity > 0.0;
}

void PlayerObject::playerTeleported()
{
    m_isOnGround2 = false;
    m_lastGroundedPos = cocos2d::CCPoint(0.0f, 0.0f);
    placeStreakPoint();
}

void PlayerObject::playingEndEffect()
{
    if (!m_isSpider)
        return;

    m_spiderSprite->tweenToAnimation("fall_loop", 0.4f);
}
void PlayerObject::postCollision(float dt, bool betweenSteps)
{
    float value = m_enableImpulseFix ? (dt * 4.0f) : (betweenSteps ? 0.5f : 1.0f);

    if (m_holdingRight && m_holdingLeft)
    {
        m_holdingRight = !m_leftPressedFirst;
        m_holdingLeft = m_leftPressedFirst;
    }

    if (m_lastGroundObject && m_lastGroundObject->getGroupDisabled())
        m_lastGroundObject = nullptr;

    if (m_currentSlope2 && m_currentSlope2->getGroupDisabled())
        m_currentSlope2 = nullptr;

    if (m_lastGroundObject)
        m_isOnIce = m_lastGroundObject->m_isIceBlock;

    if (m_isOnSlope && m_currentSlope && !m_maybeUpsideDownSlope)
        m_isOnIce = m_currentSlope->m_isIceBlock;

    if (m_wasOnSlope != m_isOnSlope)
    {
        bool handleSlopeTransition = true;

        if (m_isDart)
        {
            placeStreakPoint();

            if (m_isOnSlope)
                handleSlopeTransition = false;
        }
        else if (m_isOnSlope)
        {
            handleSlopeTransition = false;
        }

        if (handleSlopeTransition)
        {
            m_slopeEndTime = m_totalTime;

            bool doBoost = false;

            if (m_isUpsideDown)
            {
                if (m_slopeVelocity < 0.0 && m_yVelocity > m_slopeVelocity && m_isCurrentSlopeTop)
                    doBoost = true;
                else if (m_slopeVelocity > 0.0 && m_yVelocity < m_slopeVelocity)
                    doBoost = true;
            }
            else
            {
                if (m_slopeVelocity < 0.0 && m_yVelocity > m_slopeVelocity && m_isCurrentSlopeTop)
                    doBoost = true;
                else if (m_slopeVelocity > 0.0 && m_yVelocity < m_slopeVelocity)
                    doBoost = true;
            }

            if (doBoost)
            {
                if (!m_maybeIsBoosted || isFlying() || m_isBall)
                {
                    float slopeYVel = getModifiedSlopeYVel();
                    float cosSlope = cosf(m_slopeAngle);

                    if (m_isPlatformer)
                    {
                        float tanVel = fabsf(tanf(m_slopeAngle) * m_platformerXVelocity) * 1.1f;
                        slopeYVel = tanVel * (slopeYVel < 0.0f ? -1.0f : 1.0f);
                    }

                    bool boostAllowed = isBoostValid(slopeYVel)
                        && (!m_isPlatformer
                            || (m_holdingRight || m_holdingLeft)
                            && fabsf(m_platformerXVelocity) >= (cosSlope * 5.0f - 1.0f));

                    if (boostAllowed)
                        boostPlayer(slopeYVel);
                }
                else
                {
                    boostPlayer(m_slopeVelocity);
                }
            }
            else
            {
                bool handleForce = true;

                if (m_isCurrentSlopeTop && !m_maybeIsBoosted)
                {
                    bool shouldKickOffSlope = m_isUpsideDown
                        ? m_slopeVelocity > 0.0
                        : m_slopeVelocity <= 0.0;

                    if (shouldKickOffSlope)
                    {
                        m_isOnGround2 = false;
                        setYVelocity(-m_currentSlopeYVelocity * flipMod(), 17);
                        m_isOnGround = false;
                        runRotateAction(false, 14);
                    }

                    handleForce = false;
                }
                else if (!m_isCurrentSlopeTop)
                {
                    if (!m_maybeIsBoosted || isFlying() || m_isBall)
                    {
                        float slopeYVel = getModifiedSlopeYVel();
                        float cosSlope = cosf(m_slopeAngle);

                        if (m_isPlatformer)
                        {
                            float tanVel = fabsf(tanf(m_slopeAngle) * m_platformerXVelocity) * 1.1f;
                            slopeYVel = tanVel * (slopeYVel < 0.0f ? -1.0f : 1.0f);
                        }

                        bool boostAllowed = isBoostValid(slopeYVel)
                            && (!m_isPlatformer
                                || (m_holdingRight || m_holdingLeft)
                                && fabsf(m_platformerXVelocity) >= (cosSlope * 5.0f - 1.0f));

                        if (boostAllowed)
                            boostPlayer(slopeYVel);
                    }
                    else
                    {
                        boostPlayer(m_slopeVelocity);
                    }
                }

                if (handleForce && m_maybeSlopeForce > 0.0f)
                {
                    bool movingIntoSlope = (m_holdingLeft && !m_isSlidingRight && !m_holdingRight)
                        || (!m_holdingLeft && m_holdingRight && m_isSlidingRight);

                    if (movingIntoSlope)
                    {
                        m_isSliding = false;
                        m_isMoving = true;
                        m_maybeSlopeForce = 0.0f;
                    }
                }

                m_currentSlope = nullptr;
                m_currentSlopeYVelocity = 0.0;
                m_slopeVelocity = 0.0;
                m_slopeRotation = 0.0;
            }
        }
    }

    bool hadVerticalCollision = m_collidedTopMinY != 0.0 && m_collidedBottomMaxY != 0.0;
    bool skipSqueeze = false;

    if (hadVerticalCollision && !m_disablePlayerSqueeze)
    {
        float squeezeThreshold = m_isPlatformer ? 0.8f : 0.7f;
        cocos2d::CCRect playerRect = getObjectRect();

        float collisionHeight = m_collidedTopMinY - m_collidedBottomMaxY;
        float collisionHeightAbs = fabsf(collisionHeight);
        float playerY = getPosition().y;

        bool squeezePossible;

        if (collisionHeightAbs == 0.0f)
        {
            squeezePossible = true;
        }
        else if (!m_isUpsideDown)
        {
            squeezePossible = m_collidedTopMinY >= playerY || m_collidedTopMinY >= m_lastPosition.y;
        }
        else
        {
            squeezePossible = m_collidedTopMinY <= playerY || m_collidedTopMinY <= m_lastPosition.y;
        }

        if (squeezePossible)
        {
            if (!m_isPlatformer)
            {
                if (collisionHeightAbs < squeezeThreshold * playerRect.size.height)
                    skipSqueeze = true;
            }
            else if (m_currentSlope)
            {
                if (collisionHeightAbs < squeezeThreshold * playerRect.size.height
                    && collisionHeightAbs <= playerRect.size.height)
                {
                    cocos2d::CCRect slopeRect = m_currentSlope->getObjectRect();

                    if (playerRect.intersectsRect(slopeRect))
                    {
                        bool slopeUphill = m_currentSlope->m_slopeUphill;
                        bool slopeIsFloor = m_currentSlope->slopeFloorTop();

                        if ((slopeUphill && !slopeIsFloor) || (!slopeUphill && slopeIsFloor))
                        {
                            float pushDirection = -1.0f;
                            float newX = m_lastPosition.x + (dt * pushDirection);
                            m_platformerXVelocity = 0.0;
                            cocos2d::CCPoint newPos(newX, getPosition().y);
                            setPosition(&newPos);
                        }
                    }
                }
            }
        }
        else if (!m_isPlatformer && collisionHeightAbs < squeezeThreshold * playerRect.size.height)
        {
        }
    }

    if (!skipSqueeze && !m_ignoreDamage)
    {
        bool topBottomCollision = m_collisionLogTop->count() && m_collisionLogBottom->count();

        if (topBottomCollision)
        {
            cocos2d::CCArray* keys = m_collisionLogTop->allKeys();

            for (unsigned int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCObject* key = keys->objectAtIndex(i);
                int keyValue = (int)key->getTag();

                if (m_collisionLogBottom->objectForKey(keyValue))
                {
                    setPosition(&m_lastPosition);

                    if (m_maybeCanRunIntoBlocks)
                        m_maybeIsColliding = true;
                    else
                        GameManager::sharedState()->m_playLayer->destroyPlayer(this, nullptr);

                    return;
                }
            }
        }

        bool leftRightCollision = m_collisionLogLeft->count() && m_collisionLogRight->count();

        if (leftRightCollision)
        {
            cocos2d::CCArray* keys = m_collisionLogLeft->allKeys();

            for (unsigned int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCObject* key = keys->objectAtIndex(i);
                int keyValue = (int)key->getTag();

                if (m_collisionLogRight->objectForKey(keyValue))
                {
                    setPosition(&m_lastPosition);

                    if (m_maybeCanRunIntoBlocks)
                        m_maybeIsColliding = true;
                    else
                        GameManager::sharedState()->m_playLayer->destroyPlayer(this, nullptr);

                    return;
                }
            }
        }
    }

    m_preLastGroundObject = m_currentSlope2;

    if (!m_preLastGroundObject)
    {
        m_preLastGroundObject = m_lastGroundObject;
    }
    else if (!m_lastGroundObject || m_isOnGround2)
    {
        m_preLastGroundObject = m_lastGroundObject;
    }

    if (m_preLastGroundObject
        && !m_preLastGroundObject->m_isNonStickX
        && (m_preLastGroundObject != m_lastGroundObject || m_isOnGround2))
    {
        cocos2d::CCPoint lastPos = m_preLastGroundObject->getLastPosition();
        cocos2d::CCPoint realPos = m_preLastGroundObject->getRealPosition();

        float scaleDelta = 0.0f;

        if (m_preLastGroundObject->m_isScaleStick)
        {
            cocos2d::CCPoint scaleDeltaPos = m_preLastGroundObject->getScalePosDelta();

            if (realPos.x > getPosition().x)
                scaleDelta = -scaleDeltaPos.x;
        }

        if (m_isSideways)
        {
            float deltaUse = (realPos.y - lastPos.y) + scaleDelta;
            m_scaleXRelated = deltaUse;
            m_scaleXRelated2 = deltaUse;
            cocos2d::CCPoint newPos(getPosition().x, m_scaleXRelated);
            setPosition(&newPos);
        }
        else
        {
            float deltaUse = (realPos.x - lastPos.x) + scaleDelta;
            m_scaleXRelated = deltaUse;
            m_scaleXRelated2 = deltaUse;
            cocos2d::CCPoint newPos(m_scaleXRelated, getPosition().y);
            setPosition(&newPos);
        }

        m_physDeltaRelated += dt / 60.0;
    }

    performSlideCheck();

    bool onGroundLastGroundObject = m_isOnGround2 && m_lastGroundObject;

    if (!onGroundLastGroundObject)
    {
        m_collidedObject = nullptr;
        m_lastGroundObject = nullptr;
    }
    else
    {
        bool sticking = false;

        if (!m_currentSlope2 && !m_lastGroundObject->m_isNonStickY)
        {
            cocos2d::CCPoint lastPos = m_lastGroundObject->getLastPosition();
            cocos2d::CCPoint realPos = m_lastGroundObject->getRealPosition();
            m_lastGroundObject->getScalePosDelta();

            if (realPos.x != lastPos.x || realPos.y != lastPos.y)
            {
                float a = realPos.x;
                float b = lastPos.x;

                if (!m_isSideways)
                {
                    a = realPos.y;
                    b = lastPos.y;
                }

                if (m_isUpsideDown)
                    sticking = b < a;
                else
                    sticking = b > a;
            }
        }

        bool canUseBottomCollision = m_collisionLogBottom->count() > 0;

        if (canUseBottomCollision && sticking)
        {
            m_collidedObject = m_lastGroundObject;
            m_isCollidingWithSlope = false;
            m_lastGroundObject = nullptr;

            if (m_collidedObject)
            {
                float stickTime = m_isPlatformer ? 10.0f : 5.0f;

                if (m_collidedObject->m_isExtraSticky)
                    stickTime = 20.0f;

                float stickOffset = dt * stickTime;

                cocos2d::CCRect objectRect = m_collidedObject->getObjectRect();
                cocos2d::CCRect playerRect = getObjectRect();
                cocos2d::CCRect expanded = objectRect;

                bool stickFarEnough;

                if (m_isSideways)
                {
                    expanded.size.width += stickOffset;

                    if (m_isUpsideDown)
                    {
                        stickFarEnough = stickOffset > (objectRect.getMinX() - playerRect.getMaxX());
                        expanded.origin.x -= stickOffset;
                    }
                    else
                    {
                        stickFarEnough = stickOffset > (playerRect.getMinX() - objectRect.getMaxX());
                    }
                }
                else
                {
                    expanded.size.height += stickOffset;

                    if (m_isUpsideDown)
                    {
                        stickFarEnough = stickOffset > (objectRect.getMinY() - playerRect.getMaxY());
                        expanded.origin.y -= stickOffset;
                    }
                    else
                    {
                        stickFarEnough = stickOffset > (playerRect.getMinY() - objectRect.getMaxY());
                    }
                }

                if ((playerRect.intersectsRect(expanded) || m_collidedObject) && stickFarEnough)
                {
                    m_isCollidingWithSlope = true;
                    cocos2d::CCRect empty;
                    collidedWithObject(0.0f, m_collidedObject, empty, false);
                }
            }
        }
    }

    if (m_currentSlope2)
    {
        bool slopeMoved = false;

        if (!m_currentSlope2->m_isNonStickY)
        {
            cocos2d::CCPoint lastPos = m_currentSlope2->getLastPosition();
            cocos2d::CCPoint realPos = m_currentSlope2->getRealPosition();

            if (realPos.x != lastPos.x || realPos.y != lastPos.y)
            {
                slopeMoved = realPos.y < lastPos.y;

                if (m_isUpsideDown)
                    slopeMoved = realPos.y > lastPos.y;
            }
        }

        bool canCheckSlope = m_collisionLogBottom->count() == 0 || slopeMoved;

        if (canCheckSlope)
        {
            GameObject* slopeRef = m_currentSlope2;
            m_isCollidingWithSlope = false;
            m_currentSlope2 = nullptr;

            if (slopeMoved)
            {
                m_isCollidingWithSlope = true;
                collidedWithSlope(0.0f, slopeRef, true);
            }
        }
    }

    if (m_yVelocityRelated != 0.0 && m_stateBoostX <= 0)
    {
        float velocityDifference = m_yVelocityRelated - m_groundYVelocity;
        float threshold = value * 0.5f;

        if (fabsf(velocityDifference) > threshold)
        {
            bool isUpsideDown = m_isUpsideDown;

            bool shouldBoost;

            if (m_isUpsideDown)
            {
                shouldBoost = m_yVelocityRelated < -5.0 && m_yVelocityRelated < m_groundYVelocity;
            }
            else
            {
                shouldBoost = m_yVelocityRelated > 5.0 && m_yVelocityRelated > m_groundYVelocity;
            }

            if (shouldBoost)
            {
                if (m_yVelocityRelated > 20.0)
                    m_yVelocityRelated = 20.0;
                if (m_yVelocityRelated < -20.0)
                    m_yVelocityRelated = -20.0;

                bool boostDirectionValid = isUpsideDown
                    ? (m_yVelocityRelated > m_yVelocity)
                    : (m_yVelocityRelated < m_yVelocity);

                if (boostDirectionValid)
                {
                    m_currentSlope2 = nullptr;
                    m_lastGroundObject = nullptr;
                    boostPlayer(m_yVelocityRelated);
                }
            }
        }
    }

    m_collidedObject = nullptr;

    if (m_scaleXRelated2 != 0.0)
    {
        m_scaleXRelatedTime = m_totalTime;
        m_collidingBetweenSteps = betweenSteps;
        m_scaleXRelated3 = m_scaleXRelated2;
    }
    else if (m_scaleXRelated3 == 0.0)
    {
        m_collidingBetweenSteps = betweenSteps;
        m_scaleXRelated3 = m_scaleXRelated2;
    }
    else if (m_totalTime - m_scaleXRelatedTime >= 0.0333333351)
    {
        m_scaleXRelated3 = 0.0;
    }

    m_yVelocityRelated = m_groundYVelocity;
    m_scaleXRelated2 = 0.0;
    m_groundYVelocity = 0.0;
    m_scaleXRelated5 = m_scaleXRelated4;
    m_scaleXRelated4 = 0.0;

    if (m_isOnGround2)
        m_maybeHasStopped = false;

    if (!m_isOnSlope && !m_wasOnSlope)
    {
        bool isUpsideDown = m_isUpsideDown;
        m_collidingWithSlopeId = 0;

        if (isUpsideDown)
        {
            if (m_yVelocity > 2.0 && m_fallSpeed < 2.0)
                gameEventTriggered(76, 0);
            if (m_yVelocity > 7.0 && m_fallSpeed < 7.0)
                gameEventTriggered(77, 0);
            if (m_yVelocity > 14.0 && m_fallSpeed < 14.0)
                gameEventTriggered(78, 0);
        }
        else
        {
            if (m_yVelocity < -2.0 && m_fallSpeed > -2.0)
                gameEventTriggered(76, 0);
            if (m_yVelocity < -7.0 && m_fallSpeed > -7.0)
                gameEventTriggered(77, 0);
            if (m_yVelocity < -14.0 && m_fallSpeed > -14.0)
                gameEventTriggered(78, 0);
        }
    }

    m_fallSpeed = m_yVelocity;
    m_reverseRelated -= 1;
}

void PlayerObject::preCollision()
{
    m_isOnGround4 = m_isOnGround2;
    m_ringJumpRelated = false;
    m_wasOnSlope = m_isOnSlope;
    m_isOnSlope = false;
    m_collidedTopMinY = 0.0;
    m_collidedBottomMaxY = 0.0;
    m_collidedLeftMaxX = 0.0;
    m_collidedRightMinX = 0.0;

    if (m_isOnGround2 && !m_platformerMovingLeft && !m_platformerMovingRight && m_maybeSlidingTime > 0)
    {
        m_maybeSlidingTime = 0;
        m_maybeSlidingStartTime = m_totalTime;
    }

    if (m_unk669)
        m_currentPotentialSlope = nullptr;
    else
        m_unk669 = true;

    m_potentialSlopeMap.clear();

    if (m_currentPotentialSlope)
    {
        m_uniqueID = m_currentPotentialSlope->m_uniqueID;
        m_potentialSlopeMap[m_uniqueID] = m_currentPotentialSlope;
    }

    if (m_currentSlope)
    {
        m_uniqueID = m_currentSlope->m_uniqueID;
        m_potentialSlopeMap[m_uniqueID] = m_currentSlope;
    }
}

bool PlayerObject::preSlopeCollision(float dt, GameObject* object)
{
    if (object->m_uniqueID == m_collidingWithSlopeId)
        return false;

    float edgeInset = 0.0f;
    cocos2d::CCRect slopeRect = object->getObjectRect();
    cocos2d::CCRect playerRect = getObjectRect();

    bool wallLeft = object->slopeWallLeft();
    bool floorTop = object->slopeFloorTop();

    if (m_isPlatformer && (m_isOnSlope || m_wasOnSlope))
        edgeInset = 5.0f;

    cocos2d::CCPoint realPos = object->getRealPosition();
    cocos2d::CCPoint lastPos = object->getLastPosition();
    cocos2d::CCPoint slopeMovement = realPos - lastPos;
    float slopeMovementX = m_isSideways ? slopeMovement.y : slopeMovement.x;

    float maxMovement = m_playerSpeed * m_speedMultiplier * dt;

    bool sideCollided = false;
    float sideInset = 0.0f;

    if (wallLeft)
    {
        bool movingTowardWall = !m_isGoingLeft || m_isPlatformer || maxMovement > slopeMovementX;

        if (movingTowardWall && getPosition().x < slopeRect.origin.x)
        {
            sideInset = (m_isGoingLeft && !m_isPlatformer) ? 1.0f : 0.0f;

            float inset = edgeInset + sideInset;
            float collisionY = slopeRect.origin.y + inset;
            float collisionHeight = slopeRect.size.height - inset * 2.0f;
            cocos2d::CCRect collisionRect(slopeRect.origin.x, collisionY, 1.0f, collisionHeight);

            if (playerRect.intersectsRect(collisionRect))
            {
                cocos2d::CCRect empty;
                collidedWithObject(dt, object, empty, false);
            }

            sideCollided = true;

            bool hitFloor = floorTop
                ? getPosition().y < slopeRect.origin.y + slopeRect.size.height
                : getPosition().y > slopeRect.origin.y;

            if (hitFloor)
            {
                float floorY = floorTop
                    ? slopeRect.origin.y + slopeRect.size.height - 1.0f
                    : slopeRect.origin.y;
                cocos2d::CCRect floorRect(
                    slopeRect.origin.x + sideInset,
                    floorY,
                    slopeRect.size.width - sideInset * 2.0f,
                    1.0f);

                if (playerRect.intersectsRect(floorRect))
                {
                    cocos2d::CCRect empty;
                    collidedWithObject(dt, object, empty, false);
                }
            }
        }
    }
    else
    {
        bool movingTowardWall = m_isGoingLeft || m_isPlatformer || maxMovement < slopeMovementX;

        if (movingTowardWall && getPosition().x > slopeRect.getMaxX())
        {
            sideInset = (!m_isGoingLeft && !m_isPlatformer) ? 1.0f : 0.0f;

            float inset = edgeInset + sideInset;
            float collisionY = slopeRect.origin.y + inset;
            float collisionHeight = slopeRect.size.height - inset * 2.0f;
            cocos2d::CCRect collisionRect(
                slopeRect.origin.x + slopeRect.size.width - 1.0f,
                collisionY,
                1.0f,
                collisionHeight);

            if (playerRect.intersectsRect(collisionRect))
            {
                cocos2d::CCRect empty;
                collidedWithObject(dt, object, empty, false);
            }

            sideCollided = true;
        }
    }

    if (!sideCollided)
    {
        bool hitFloor = floorTop
            ? getPosition().y < slopeRect.origin.y + slopeRect.size.height
            : getPosition().y > slopeRect.origin.y;

        if (hitFloor)
        {
            float floorY = floorTop
                ? slopeRect.origin.y + slopeRect.size.height - 1.0f
                : slopeRect.origin.y;
            cocos2d::CCRect floorRect(
                slopeRect.origin.x + sideInset,
                floorY,
                slopeRect.size.width - sideInset * 2.0f,
                1.0f);

            if (playerRect.intersectsRect(floorRect))
            {
                cocos2d::CCRect empty;
                collidedWithObject(dt, object, empty, false);
            }
        }
    }

    if (!m_isOnSlope)
    {
        bool shouldReplacePotentialSlope = false;

        if (m_currentPotentialSlope)
        {
            bool currentIsFloorTop = !m_currentPotentialSlope->m_slopeUphill || m_currentPotentialSlope->slopeFloorTop();
            bool newIsFloorTop = !object->m_slopeUphill || object->slopeFloorTop();

            if (currentIsFloorTop && newIsFloorTop)
            {
                if (m_currentPotentialSlope->getPosition().x > object->getPosition().x)
                    shouldReplacePotentialSlope = true;
            }
            else
            {
                shouldReplacePotentialSlope = true;
            }
        }
        else
        {
            shouldReplacePotentialSlope = true;
        }

        if (shouldReplacePotentialSlope)
            m_currentPotentialSlope = object;
    }

    if (slopeRect.intersectsRect(playerRect))
        return sideCollided;

    return true;
}

void PlayerObject::propellPlayer(float yVelocity, bool noEffects, int objectType)
{
    m_maybeIsBoosted = true;
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = true;
    m_isOnSlope = false;
    m_wasOnSlope = false;

    float vehicleScale = (m_vehicleSize != 1.0f) ? 0.8f : 1.0f;
    int flip = flipMod();
    float boostedVelocity = (yVelocity * 16.0f) * (float)flip * vehicleScale;

    setYVelocity(boostedVelocity, 44);

    if (m_isBall || m_isSpider || m_isSwing)
        m_yVelocity *= 0.600000024f;

    runRotateAction(false, 7);

    if (!noEffects)
    {
        playBumpEffect(objectType, nullptr);

        if (m_isRobot)
        {
            m_robotSprite->runAnimation("jump_start");
        }
        else if (m_isSpider)
        {
            m_spiderSprite->runAnimation("fall_loop");
        }
    }
    else if (m_isRobot)
    {
        m_robotSprite->runAnimation("jump_start");
    }
    else if (m_isSpider)
    {
        m_spiderSprite->runAnimation("fall_loop");
    }

    activateStreak();
    m_lastGroundedPos = getPosition();
}

void PlayerObject::playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to)
{
    if (!m_playEffects)
        return;
    if (m_isHidden)
        return;

    from.x += (float)reverseMod() * 7.5f;
    to.x += (float)reverseMod() * 7.5f;

    if (!GameManager::sharedState()->m_performanceMode)
    {
        float baseSize = (m_vehicleSize * 10.0f) * 1.3f;
        to.y += 6.0f;

        cocos2d::ccColor3B waveColor = m_swapColors ? m_playerColor1 : m_playerColor2;

        PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;

        CCCircleWave* wave1 = CCCircleWave::create(baseSize, 1.0f, 0.15f, 0);
        wave1->m_color = waveColor;
        wave1->setPosition(&from);
        m_parentLayer->addChild(wave1, 0);
        wave1->m_delegate = &playLayer->CCCircleWaveDelegate;
        playLayer->addCircle(wave1);
        wave1->m_opacityMod = 0.62f;

        CCCircleWave* wave2 = CCCircleWave::create(baseSize * 2.0f, 2.0f, 0.25f, 0);
        wave2->m_color = waveColor;
        wave2->setPosition(&from);
        m_parentLayer->addChild(wave2, 0);
        wave2->m_delegate = &playLayer->CCCircleWaveDelegate;
        playLayer->addCircle(wave2);
        wave2->m_opacityMod = 0.78f;

        CCCircleWave* wave3 = CCCircleWave::create(baseSize * 3.0f, 3.0f, 0.25f, 0);
        wave3->m_color = waveColor;
        wave3->setPosition(&from);
        m_parentLayer->addChild(wave3, 0);
        wave3->m_delegate = &playLayer->CCCircleWaveDelegate;
        playLayer->addCircle(wave3);
        wave3->m_circleMode = CircleMode::Outline;
    }

    cocos2d::ccColor3B white1 = {255, 255, 255};
    cocos2d::ccColor3B white2 = {255, 255, 255};
    flashPlayer(0.3f, 0.05f, white1, white2);

    m_spiderSprite->stopActionByTag(10);
    m_spiderSprite->setScale(0.4f);

    cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.45f, 1.0f);
    cocos2d::CCEaseElasticOut* elasticEase = cocos2d::CCEaseElasticOut::create(scaleTo, 0.35f);
    elasticEase->setTag(10);
    m_spiderSprite->runAction(elasticEase);

    if (m_playEffects)
    {
        cocos2d::CCSprite* dashSprite = cocos2d::CCSprite::createWithSpriteFrameName("spiderDash_002.png");

        float rotation;
        if (m_isSideways)
        {
            if (m_isUpsideDown)
                rotation = 180.0f;
            else
                rotation = 0.0f;
        }
        else
        {
            if (m_isUpsideDown)
                rotation = 90.0f;
            else
                rotation = -90.0f;
        }

        dashSprite->setRotation(rotation);

        GameManager::sharedState()->m_playLayer->m_objectLayer->addChild(dashSprite, 40);
        dashSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

        float length;
        cocos2d::CCPoint midpoint;

        if (m_isSideways)
        {
            length = fabsf(from.y - to.y) + 30.0f;
            midpoint.x = from.x + (to.y - from.y) * 0.5f;
            midpoint.y = from.y;
        }
        else
        {
            length = fabsf(from.x - to.x) + 30.0f;
            midpoint.x = from.x;
            midpoint.y = from.y + (to.y - from.y) * 0.5f;
        }

        dashSprite->setScaleX(length / dashSprite->getContentSize().width);
        dashSprite->setPosition(&midpoint);

        cocos2d::ccColor3B spriteColor = m_swapColors ? m_playerColor1 : m_playerColor2;
        dashSprite->setColor(spriteColor);

        cocos2d::CCArray* dashFrames = cocos2d::CCArray::create();
        for (int i = 1; i < 9; i++)
        {
            cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("spiderDash_%03d.png", i);
            cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
            if (frame)
                dashFrames->addObject(frame);
        }

        cocos2d::CCAnimation* animation = cocos2d::CCAnimation::createWithSpriteFrames(dashFrames, 0.04f);
        cocos2d::CCSequence* animateSequence = cocos2d::CCSequence::create(
            cocos2d::CCAnimate::create(animation),
            cocos2d::CCCallFunc::create(dashSprite, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        dashSprite->runAction(animateSequence);
    }

    if (!m_isHidden && (!m_isSpider || !m_isPlatformer))
        m_playerGroundParticles->resetSystem();
}

void PlayerObject::propellPlayer(float yVelocity, bool noEffects, int objectType)
{
    m_maybeIsBoosted = true;
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = true;
    m_isOnSlope = false;
    m_wasOnSlope = false;

    float vehicleScale = (m_vehicleSize != 1.0f) ? 0.8f : 1.0f;
    int flip = flipMod();
    float boostedVelocity = (yVelocity * 16.0f) * (float)flip * vehicleScale;

    setYVelocity(boostedVelocity, 44);

    if (m_isBall || m_isSpider || m_isSwing)
    {
        m_yVelocity *= 0.600000024f;
        runRotateAction(false, 7);

        if (!noEffects)
            playBumpEffect(objectType, nullptr);

        if (m_isRobot)
            m_robotSprite->runAnimation("jump_start");
        else if (m_isSpider)
            m_spiderSprite->runAnimation("fall_loop");
    }
    else
    {
        runRotateAction(false, 7);

        if (!noEffects)
            playBumpEffect(objectType, nullptr);

        if (m_isRobot)
            m_robotSprite->runAnimation("jump_start");
        else if (m_isSpider)
            m_spiderSprite->runAnimation("fall_loop");
    }

    activateStreak();
    m_lastGroundedPos = getPosition();
}

void PlayerObject::postCollision(float dt, bool betweenSteps)
{
    float impulseMultiplier = m_enableImpulseFix ? (dt * 4.0f) : (betweenSteps ? 0.5f : 1.0f);

    if (m_holdingRight && m_holdingLeft)
    {
        m_holdingRight = !m_leftPressedFirst;
        m_holdingLeft = m_leftPressedFirst;
    }

    if (m_lastGroundObject && m_lastGroundObject->getGroupDisabled())
        m_lastGroundObject = nullptr;

    if (m_currentSlope2 && m_currentSlope2->getGroupDisabled())
        m_currentSlope2 = nullptr;

    if (m_lastGroundObject)
        m_isOnIce = m_lastGroundObject->m_isIceBlock;

    if (m_isOnSlope && m_currentSlope && !m_maybeUpsideDownSlope)
        m_isOnIce = m_currentSlope->m_isIceBlock;

    if (m_wasOnSlope != m_isOnSlope)
    {
        bool skipSlopeTransition = false;

        if (m_isDart)
        {
            placeStreakPoint();
            if (m_isOnSlope)
                skipSlopeTransition = true;
        }
        else if (m_isOnSlope)
        {
            skipSlopeTransition = true;
        }

        if (!skipSlopeTransition)
        {
            m_slopeEndTime = m_totalTime;

            bool slopeBoostPending = false;

            if (m_isUpsideDown)
            {
                if (m_slopeVelocity < 0.0 && m_yVelocity > m_slopeVelocity && m_isCurrentSlopeTop)
                    slopeBoostPending = true;
                else if (m_slopeVelocity > 0.0 && m_yVelocity < m_slopeVelocity)
                    slopeBoostPending = true;
            }
            else
            {
                if (m_slopeVelocity < 0.0 && m_yVelocity > m_slopeVelocity && m_isCurrentSlopeTop)
                    slopeBoostPending = true;
                else if (m_slopeVelocity > 0.0 && m_yVelocity < m_slopeVelocity)
                    slopeBoostPending = true;
            }

            bool doSlopeBoost = false;
            bool doSlopeForce = false;
            bool doSlopeKick = false;

            if (slopeBoostPending)
            {
                if (m_maybeIsBoosted && !isFlying() && !m_isBall)
                    doSlopeBoost = true;
                else
                    doSlopeForce = true;
            }
            else
            {
                doSlopeForce = true;
            }

            if (doSlopeBoost)
            {
                boostPlayer(m_slopeVelocity);
            }
            else if (doSlopeForce)
            {
                if (m_maybeIsBoosted && !isFlying() && !m_isBall)
                {
                    boostPlayer(m_slopeVelocity);
                }
                else if (m_isCurrentSlopeTop)
                {
                    bool kickOffSlope = m_isUpsideDown
                        ? m_slopeVelocity > 0.0
                        : m_slopeVelocity <= 0.0;

                    if (kickOffSlope && !m_maybeIsBoosted && !m_jumpBuffered
                        && !m_stateRingJump && m_isCurrentSlopeTop == m_isOnSlope)
                    {
                        m_isOnGround2 = false;
                        setYVelocity(-m_currentSlopeYVelocity * flipMod(), 17);
                        m_isOnGround = false;
                        runRotateAction(false, 14);
                    }
                }
                else
                {
                    float slopeYVel = getModifiedSlopeYVel();
                    float cosSlope = cosf(m_slopeAngle);

                    if (m_isPlatformer)
                    {
                        float tanVel = fabsf(tanf(m_slopeAngle) * m_platformerXVelocity) * 1.1f;
                        slopeYVel = tanVel * (slopeYVel < 0.0f ? -1.0f : 1.0f);
                    }

                    bool canBoost = isBoostValid(slopeYVel)
                        && (!m_isPlatformer
                            || (m_holdingRight || m_holdingLeft)
                            && fabsf(m_platformerXVelocity) >= (cosSlope * 5.0f - 1.0f));

                    if (canBoost)
                        boostPlayer(slopeYVel);
                }
            }

            if (m_maybeSlopeForce > 0.0f)
            {
                bool movingIntoSlope = (m_holdingLeft && !m_isSlidingRight && !m_holdingRight)
                    || (!m_holdingLeft && m_holdingRight && m_isSlidingRight);

                if (movingIntoSlope)
                {
                    m_isSliding = false;
                    m_isMoving = true;
                    m_maybeSlopeForce = 0.0f;
                }
            }

            m_currentSlope = nullptr;
            m_currentSlopeYVelocity = 0.0;
            m_slopeVelocity = 0.0;
            m_slopeRotation = 0.0;
        }
    }

    if (m_collidedTopMinY != 0.0 && m_collidedBottomMaxY != 0.0 && !m_disablePlayerSqueeze)
    {
        float squeezeThreshold = m_isPlatformer ? 0.8f : 0.7f;
        cocos2d::CCRect playerRect = getObjectRect();

        float collisionHeight = m_collidedTopMinY - m_collidedBottomMaxY;
        float collisionHeightAbs = fabsf(collisionHeight);
        float playerY = getPosition().y;

        bool squeezeBlocked;
        if (collisionHeightAbs == 0.0f)
        {
            squeezeBlocked = !m_isPlatformer;
        }
        else if (!m_isUpsideDown)
        {
            squeezeBlocked = m_collidedTopMinY >= playerY || m_collidedTopMinY >= m_lastPosition.y;
        }
        else
        {
            squeezeBlocked = m_collidedTopMinY <= playerY || m_collidedTopMinY <= m_lastPosition.y;
        }

        if (squeezeBlocked)
        {
            if (m_isPlatformer && m_currentSlope)
            {
                if (collisionHeightAbs < squeezeThreshold * playerRect.size.height
                    && collisionHeightAbs <= playerRect.size.height)
                {
                    cocos2d::CCRect slopeRect = m_currentSlope->getObjectRect();
                    if (playerRect.intersectsRect(slopeRect))
                    {
                        bool slopeUphill = m_currentSlope->m_slopeUphill;
                        bool slopeIsFloor = m_currentSlope->slopeFloorTop();

                        bool shouldPush = (slopeUphill && !slopeIsFloor)
                            || (!slopeUphill && slopeIsFloor);

                        if (shouldPush)
                        {
                            float pushDirection = -1.0f;
                            float newX = m_lastPosition.x + (dt * pushDirection);
                            m_platformerXVelocity = 0.0;
                            cocos2d::CCPoint newPos(newX, getPosition().y);
                            setPosition(&newPos);
                        }
                    }
                }
            }
        }
        else if (!m_isPlatformer && collisionHeightAbs < squeezeThreshold * playerRect.size.height)
        {
            bool canDestroy = !m_ignoreDamage;

            if (canDestroy)
            {
                if (!m_playEffects)
                    setPosition(&m_lastPosition);

                if (m_maybeCanRunIntoBlocks)
                    m_maybeIsColliding = true;
                else
                    GameManager::sharedState()->m_playLayer->destroyPlayer(this, nullptr);
            }
        }
    }

    if (!m_ignoreDamage)
    {
        bool topBottomCollision = m_collisionLogTop->count() && m_collisionLogBottom->count();
        if (topBottomCollision)
        {
            cocos2d::CCArray* keys = m_collisionLogTop->allKeys();
            for (unsigned int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCObject* key = keys->objectAtIndex(i);
                int keyValue = (int)key->getTag();

                if (m_collisionLogBottom->objectForKey(keyValue))
                {
                    setPosition(&m_lastPosition);

                    if (m_maybeCanRunIntoBlocks)
                        m_maybeIsColliding = true;
                    else
                        GameManager::sharedState()->m_playLayer->destroyPlayer(this, nullptr);

                    return;
                }
            }
        }

        bool leftRightCollision = m_collisionLogLeft->count() && m_collisionLogRight->count();
        if (leftRightCollision)
        {
            cocos2d::CCArray* keys = m_collisionLogLeft->allKeys();
            for (unsigned int i = 0; i < keys->count(); i++)
            {
                cocos2d::CCObject* key = keys->objectAtIndex(i);
                int keyValue = (int)key->getTag();

                if (m_collisionLogRight->objectForKey(keyValue))
                {
                    setPosition(&m_lastPosition);

                    if (m_maybeCanRunIntoBlocks)
                        m_maybeIsColliding = true;
                    else
                        GameManager::sharedState()->m_playLayer->destroyPlayer(this, nullptr);

                    return;
                }
            }
        }
    }

    m_preLastGroundObject = m_currentSlope2;
    if (m_preLastGroundObject)
    {
        if (!m_lastGroundObject || m_isOnGround2)
            m_preLastGroundObject = m_lastGroundObject;
    }
    else
    {
        m_preLastGroundObject = m_lastGroundObject;
    }

    bool canFollowPreLast = m_preLastGroundObject
        && !m_preLastGroundObject->m_isNonStickX
        && (m_lastGroundObject != nullptr || m_isOnGround2);

    if (canFollowPreLast)
    {
        cocos2d::CCPoint lastPos = m_preLastGroundObject->getLastPosition();
        cocos2d::CCPoint realPos = m_preLastGroundObject->getRealPosition();

        float scaleDelta = 0.0f;
        if (m_preLastGroundObject->m_isScaleStick)
        {
            cocos2d::CCPoint scalePosDelta = m_preLastGroundObject->getScalePosDelta();
            if (realPos.x > getPosition().x)
                scaleDelta = -scalePosDelta.x;
        }

        float movementDelta;
        if (m_isSideways)
            movementDelta = (realPos.y - lastPos.y) + scaleDelta;
        else
            movementDelta = (realPos.x - lastPos.x) + scaleDelta;

        m_scaleXRelated = movementDelta;
        m_scaleXRelated2 = movementDelta;

        cocos2d::CCPoint newPos = m_isSideways
            ? cocos2d::CCPoint(getPosition().x, m_scaleXRelated)
            : cocos2d::CCPoint(m_scaleXRelated, getPosition().y);
        setPosition(&newPos);

        m_physDeltaRelated += dt / 60.0;
    }

    performSlideCheck();

    if (!m_isOnGround2 || !m_lastGroundObject)
    {
        m_collidedObject = nullptr;
        m_lastGroundObject = nullptr;
    }
    else
    {
        bool sticking = false;

        if (!m_currentSlope2 && !m_lastGroundObject->m_isNonStickY)
        {
            cocos2d::CCPoint lastPos = m_lastGroundObject->getLastPosition();
            cocos2d::CCPoint realPos = m_lastGroundObject->getRealPosition();
            m_lastGroundObject->getScalePosDelta();

            if (realPos.x != lastPos.x || realPos.y != lastPos.y)
            {
                float currentPos = realPos.x;
                float previousPos = lastPos.x;

                if (!m_isSideways)
                {
                    currentPos = realPos.y;
                    previousPos = lastPos.y;
                }

                sticking = m_isUpsideDown
                    ? previousPos < currentPos
                    : previousPos > currentPos;
            }
        }

        bool canUseBottomCollision = m_collisionLogBottom->count() > 0;

        if (canUseBottomCollision && sticking && m_collidedObject != nullptr)
        {
            m_collidedObject = m_lastGroundObject;
            m_isCollidingWithSlope = false;
            m_lastGroundObject = nullptr;

            float stickSpeed = m_isPlatformer ? 10.0f : 5.0f;
            if (m_collidedObject->m_isExtraSticky)
                stickSpeed = 20.0f;

            float stickOffset = dt * stickSpeed;

            cocos2d::CCRect objectRect = m_collidedObject->getObjectRect();
            cocos2d::CCRect playerRect = getObjectRect();
            cocos2d::CCRect expanded = objectRect;

            bool stickFarEnough;

            if (m_isSideways)
            {
                expanded.size.width += stickOffset;

                if (m_isUpsideDown)
                {
                    stickFarEnough = stickOffset > (objectRect.getMinX() - playerRect.getMaxX());
                    expanded.origin.x -= stickOffset;
                }
                else
                {
                    stickFarEnough = stickOffset > (playerRect.getMinX() - objectRect.getMaxX());
                }
            }
            else
            {
                expanded.size.height += stickOffset;

                if (m_isUpsideDown)
                {
                    stickFarEnough = stickOffset > (objectRect.getMinY() - playerRect.getMaxY());
                    expanded.origin.y -= stickOffset;
                }
                else
                {
                    stickFarEnough = stickOffset > (playerRect.getMinY() - objectRect.getMaxY());
                }
            }

            if ((playerRect.intersectsRect(expanded) || m_collidedObject) && stickFarEnough)
            {
                m_isCollidingWithSlope = true;
                cocos2d::CCRect empty;
                collidedWithObject(0.0f, m_collidedObject, empty, false);
            }
        }
    }

    if (m_currentSlope2)
    {
        bool slopeMoved = false;

        if (!m_currentSlope2->m_isNonStickY)
        {
            cocos2d::CCPoint lastPos = m_currentSlope2->getLastPosition();
            cocos2d::CCPoint realPos = m_currentSlope2->getRealPosition();

            if (realPos.x != lastPos.x || realPos.y != lastPos.y)
            {
                slopeMoved = realPos.y < lastPos.y;
                if (m_isUpsideDown)
                    slopeMoved = realPos.y > lastPos.y;
            }
        }

        bool canCheckSlope = m_collisionLogBottom->count() == 0 || slopeMoved;

        if (canCheckSlope)
        {
            GameObject* slopeRef = m_currentSlope2;
            m_isCollidingWithSlope = false;
            m_currentSlope2 = nullptr;

            if (slopeMoved)
            {
                m_isCollidingWithSlope = true;
                collidedWithSlope(0.0f, slopeRef, true);
            }
        }
    }

    if (m_yVelocityRelated != 0.0 && m_stateBoostX <= 0)
    {
        float velocityDifference = m_yVelocityRelated - m_groundYVelocity;
        float threshold = impulseMultiplier * 0.5f;

        if (fabsf(velocityDifference) > threshold)
        {
            bool isUpsideDown = m_isUpsideDown;

            bool shouldBoost;
            if (m_isUpsideDown)
                shouldBoost = m_yVelocityRelated < -5.0 && m_yVelocityRelated < m_groundYVelocity;
            else
                shouldBoost = m_yVelocityRelated > 5.0 && m_yVelocityRelated > m_groundYVelocity;

            if (shouldBoost)
            {
                if (m_yVelocityRelated > 20.0)
                    m_yVelocityRelated = 20.0;
                if (m_yVelocityRelated < -20.0)
                    m_yVelocityRelated = -20.0;

                bool boostDirectionValid = isUpsideDown
                    ? (m_yVelocityRelated > m_yVelocity)
                    : (m_yVelocityRelated < m_yVelocity);

                if (boostDirectionValid)
                {
                    m_currentSlope2 = nullptr;
                    m_lastGroundObject = nullptr;
                    boostPlayer(m_yVelocityRelated);
                }
            }
        }
    }

    m_collidedObject = nullptr;

    if (m_scaleXRelated2 != 0.0)
    {
        m_scaleXRelatedTime = m_totalTime;
        m_collidingBetweenSteps = betweenSteps;
        m_scaleXRelated3 = m_scaleXRelated2;
    }
    else if (m_scaleXRelated3 == 0.0)
    {
        m_collidingBetweenSteps = betweenSteps;
        m_scaleXRelated3 = m_scaleXRelated2;
    }
    else if (m_totalTime - m_scaleXRelatedTime >= 0.0333333351)
    {
        m_scaleXRelated3 = 0.0;
    }

    m_yVelocityRelated = m_groundYVelocity;
    m_scaleXRelated2 = 0.0;
    m_groundYVelocity = 0.0;
    m_scaleXRelated5 = m_scaleXRelated4;
    m_scaleXRelated4 = 0.0;

    if (m_isOnGround2)
        m_maybeHasStopped = false;

    if (!m_isOnSlope && !m_wasOnSlope)
    {
        bool isUpsideDown = m_isUpsideDown;
        m_collidingWithSlopeId = 0;

        if (isUpsideDown)
        {
            if (m_yVelocity > 2.0 && m_fallSpeed < 2.0)
                gameEventTriggered(76, 0);
            if (m_yVelocity > 7.0 && m_fallSpeed < 7.0)
                gameEventTriggered(77, 0);
            if (m_yVelocity > 14.0 && m_fallSpeed < 14.0)
                gameEventTriggered(78, 0);
        }
        else
        {
            if (m_yVelocity < -2.0 && m_fallSpeed > -2.0)
                gameEventTriggered(76, 0);
            if (m_yVelocity < -7.0 && m_fallSpeed > -7.0)
                gameEventTriggered(77, 0);
            if (m_yVelocity < -14.0 && m_fallSpeed > -14.0)
                gameEventTriggered(78, 0);
        }
    }

    m_fallSpeed = m_yVelocity;
    m_reverseRelated -= 1;
}

void PlayerObject::collidedWithSlopeInternal(float dt, GameObject* object, bool forced)
{
    if (!forced && preSlopeCollision(dt, object))
        return;

    cocos2d::CCRect playerRect = getObjectRect();
    cocos2d::CCRect slopeRect = object->getObjectRect();

    unsigned int slopeUphill = object->m_slopeUphill;

    if (m_isPlatformer)
    {
        if (m_platformerXVelocity >= 0.0)
            slopeUphill ^= 1;
        m_isGoingLeft = m_platformerXVelocity < 0.0;
    }
    else if (!m_isGoingLeft)
    {
        slopeUphill ^= 1;
    }

    bool slopeIsFloorTop = object->slopeFloorTop();

    object->getRealPosition();
    cocos2d::CCPoint slopeRealPos = object->getRealPosition();
    cocos2d::CCPoint slopeLastPos = object->getLastPosition();
    cocos2d::CCPoint slopeDelta = slopeRealPos - slopeLastPos;
    float slopeDeltaX = m_isSideways ? slopeDelta.y : slopeDelta.x;
    float slopeDeltaY = m_isSideways ? slopeDelta.x : slopeDelta.y;

    float playerSpeedTotal = m_playerSpeed * m_speedMultiplier;
    bool wasFasterThanSlope = false;
    unsigned int slopeBoostDirection = 0;

    if (slopeDeltaX > playerSpeedTotal * dt)
    {
        if (m_isUpsideDown)
            slopeUphill ^= 1;
        else
            slopeUphill = slopeUphill ? 0 : 1;

        slopeBoostDirection = 1;
        wasFasterThanSlope = true;
    }

    if (m_isUpsideDown != m_isSideways)
        slopeUphill ^= 1;

    float slopeVerticalSpeed = 0.0f;

    if (!m_wasOnSlope)
    {
        float widthOverSpeed = slopeRect.size.width / playerSpeedTotal;
        float heightOverSpeed = slopeRect.size.height / widthOverSpeed;
        slopeVerticalSpeed = (float)slopeUphill;
    }
    else
    {
        bool slopeMovingUp = (m_slopeVelocity * flipMod()) > 0.0f;
        bool sameSlopeState = m_isCurrentSlopeTop == slopeIsFloorTop;
        bool sameGravity = m_isUpsideDown == slopeIsFloorTop;

        if (slopeMovingUp && slopeUphill && sameSlopeState && sameGravity)
            return;

        float widthOverSpeed = slopeRect.size.width / (m_playerSpeed * m_speedMultiplier);
        float heightOverSpeed = slopeRect.size.height / widthOverSpeed;

        if (slopeMovingUp && m_currentSlopeYVelocity > heightOverSpeed)
            return;

        slopeVerticalSpeed = (float)(int)(4 * slopeUphill);
    }

    cocos2d::CCPoint lastSlopePos = object->getLastPosition();
    cocos2d::CCPoint currentSlopeRealPos = object->getRealPosition();
    cocos2d::CCPoint slopeMovement = currentSlopeRealPos - lastSlopePos;
    float slopeMovementX = m_isSideways ? slopeMovement.y : slopeMovement.x;

    int slopeDirection = 0;
    float snapHeight = 0.0f;

    if (m_isUpsideDown)
    {
        if (slopeMovementX < 0.0f)
        {
            slopeDirection = 1;
        }
        else if (!forced)
        {
            slopeDirection = 0;
            snapHeight = (playerRect.size.height * 0.5f) / cosf(m_slopeRotation);
        }
    }
    else
    {
        if (slopeMovementX > 0.0f)
        {
            slopeDirection = 1;
        }
        else if (!forced)
        {
            slopeDirection = 0;
            snapHeight = (playerRect.size.height * 0.5f) / cosf(m_slopeRotation);
        }
    }

    if (slopeDirection)
    {
        float movementMagnitude = fabsf(slopeMovementX);
        float movementLimit = m_isPlatformer ? dt * 10.0f : dt * 5.0f;
        if (movementMagnitude > movementLimit)
            movementMagnitude = movementLimit;

        slopeVerticalSpeed += movementMagnitude;
        snapHeight = (playerRect.size.height * 0.5f) / cosf(m_slopeRotation);
    }

    float targetY;
    if (m_isUpsideDown)
        targetY = (snapHeight + getPosition().y) + slopeVerticalSpeed;
    else
        targetY = (getPosition().y - snapHeight) - slopeVerticalSpeed;

    if (m_wasOnSlope)
    {
        if (m_isUpsideDown)
        {
            if (targetY < slopeRect.origin.y)
                return;
        }
        else
        {
            if (targetY > slopeRect.origin.y + slopeRect.size.height)
                return;
        }
    }
    else
    {
        cocos2d::CCRect innerRect(
            slopeRect.origin.x,
            slopeRect.origin.y + 1.0f,
            slopeRect.size.width,
            slopeRect.size.height - 2.0f);

        if (!playerRect.intersectsRect(innerRect))
            return;
    }

    cocos2d::CCPoint playerPos = getPosition();
    float slopeMinY = slopeRect.getMinY();
    float slopeMaxY = slopeRect.getMaxY();
    float slopeAngle = object->getSlopeAngle();
    float slopeHeightOffset = (playerRect.size.height / cosf(slopeAngle)) * 0.5f;

    unsigned int currentlyOnSlope = 0;
    float slopeIgnoreOffset = 0.0f;

    bool sameSlopeOrTop = !m_wasOnSlope
        || m_collidingWithSlopeId == object->m_uniqueID
        || m_isCurrentSlopeTop == slopeIsFloorTop;

    if (sameSlopeOrTop)
    {
        currentlyOnSlope = 0;
        slopeIgnoreOffset = 0.0f;
    }
    else if (m_isPlatformer)
    {
        slopeIgnoreOffset = 0.0f;
        currentlyOnSlope = 1;
    }
    else
    {
        currentlyOnSlope = 1;
        slopeIgnoreOffset = m_vehicleSize * 20.0f;
    }

    double slopeYAtPlayer = object->slopeYPos(playerPos.x);
    float slopeY = static_cast<float>(slopeYAtPlayer);

    float collisionY;
    bool slopeClampedFromTop = false;
    bool slopeClampedFromBottom = false;

    if (!slopeIsFloorTop)
    {
        collisionY = (slopeY + slopeHeightOffset) - slopeIgnoreOffset;

        float topCandidate = (slopeMaxY + playerRect.size.height * 0.5f) - slopeIgnoreOffset;
        if (topCandidate > collisionY)
            slopeClampedFromTop = false;
        else
        {
            collisionY = topCandidate;
            slopeClampedFromTop = true;
        }

        if (collisionY > slopeMinY)
            slopeClampedFromBottom = false;
        else
        {
            collisionY = slopeMinY;
            slopeClampedFromBottom = true;
        }
    }
    else
    {
        collisionY = (slopeY - slopeHeightOffset) + slopeIgnoreOffset;

        float bottomCandidate = (slopeMinY - playerRect.size.height * 0.5f) + slopeIgnoreOffset;
        if (bottomCandidate < collisionY)
            slopeClampedFromTop = false;
        else
        {
            collisionY = bottomCandidate;
            slopeClampedFromTop = true;
        }

        if (collisionY < slopeMaxY)
            slopeClampedFromBottom = false;
        else
        {
            collisionY = slopeMaxY;
            slopeClampedFromBottom = true;
        }
    }

    bool snapRangeValid;
    bool isOnSlopeState = m_isOnSlope
        || (m_maybeIsBoosted && !isFlying());

    if (slopeUphill > currentlyOnSlope || isOnSlopeState)
        snapRangeValid = false;
    else
        snapRangeValid = !m_isShip || !m_jumpBuffered;

    if (slopeUphill < static_cast<unsigned int>(forced))
        snapRangeValid = true;
    if (slopeUphill < static_cast<unsigned int>(slopeDirection))
        snapRangeValid = true;

    bool shouldSnap;
    if (m_isUpsideDown)
    {
        shouldSnap = true;
        if (playerPos.y <= collisionY)
        {
            shouldSnap = false;
            if (snapRangeValid && playerPos.y > collisionY - slopeVerticalSpeed)
            {
                shouldSnap = true;
                if (m_isBird)
                    shouldSnap = m_yVelocity >= 0.0;
            }
        }
    }
    else
    {
        shouldSnap = true;
        if (playerPos.y >= collisionY)
        {
            shouldSnap = false;
            if (snapRangeValid && playerPos.y < collisionY + slopeVerticalSpeed)
            {
                shouldSnap = true;
                if (m_isBird)
                    shouldSnap = m_yVelocity <= 0.0;
            }
        }
    }

    bool wasBoostedOnLanding = false;
    bool forceNextLanding = false;

    if (m_jumpBuffered && isFlying() && (!slopeUphill || m_isPlatformer))
    {
        forceNextLanding = false;
        wasBoostedOnLanding = true;
        m_wasOnSlope = currentlyOnSlope & m_isPlatformer ^ 1;
    }
    else
    {
        wasBoostedOnLanding = false;
        forceNextLanding = false;
        m_wasOnSlope = false;
    }

    auto applySlope = [&]()
    {
        m_currentSlope = object;
        float angle = object->getSlopeAngle();
        m_uniqueID = object->m_uniqueID;
        m_currentPotentialSlope = m_currentSlope;
        m_slopeAngleRadians = angle;
        m_potentialSlopeMap[m_uniqueID] = object;
        m_unk3e0 = slopeClampedFromTop;
        m_unk3e1 = slopeClampedFromBottom;
        m_unk3d0 = collisionY;
        m_isCurrentSlopeTop = slopeIsFloorTop;
        m_collidingWithSlopeId = object->m_uniqueID;
        m_slopeFlipGravityRelated = slopeUphill;
        m_blackOrbRelated = collisionY;
        unk_584 = slopeHeightOffset - playerRect.size.height * 0.5f;

        int flip = flipMod();
        if (slopeUphill < (unsigned int)slopeIsFloorTop
            || (unsigned int)slopeIsFloorTop < ((unsigned __int8)slopeUphill ^ 1))
        {
            flip = -flip;
        }

        int rotationSign = ((((flip ^ -wasFasterThanSlope) + wasFasterThanSlope) ^ -m_isGoingLeft) + m_isGoingLeft);
        rotationSign = ((rotationSign ^ -m_isSideways) + m_isSideways);
        m_slopeRotation = (angle * 57.296f) * (float)rotationSign * 0.017453f;

        cocos2d::CCPoint newPos(getPosition().x, collisionY);
        setPosition(&newPos);

        if (!m_wasOnSlope)
        {
            m_slopeStartTime = m_totalTime;
            m_yVelocityBeforeSlope = m_yVelocity;
        }

        m_maybeUpsideDownSlope = false;

        if (!m_wasOnSlope)
        {
            if (forceNextLanding)
            {
                if (m_isUpsideDown)
                {
                    if (m_yVelocity < 0.0)
                    {
                        setYVelocity(m_yVelocity, 20);
                        m_isOnGround = false;
                        m_isOnGround2 = false;
                        updateCollide(PlayerCollisionDirection::Bottom, object);
                    }
                    else
                    {
                        setYVelocity(0.0, 20);
                        m_isOnGround = false;
                        m_isOnGround2 = false;
                        updateCollide(PlayerCollisionDirection::Bottom, object);
                    }
                }
                else
                {
                    if (m_yVelocity > 0.0)
                    {
                        setYVelocity(m_yVelocity, 20);
                        m_isOnGround = false;
                        m_isOnGround2 = false;
                        updateCollide(PlayerCollisionDirection::Bottom, object);
                    }
                    else
                    {
                        setYVelocity(0.0, 20);
                        m_isOnGround = false;
                        m_isOnGround2 = false;
                        updateCollide(PlayerCollisionDirection::Bottom, object);
                    }
                }
            }
            else
            {
                hitGround(object, false);

                if (m_isUpsideDown)
                {
                    if (m_yVelocity < -5.0)
                        setYVelocity(m_yVelocity, 100);
                }
                else if (m_yVelocity > 5.0)
                {
                    setYVelocity(m_yVelocity, 100);
                }

                if (m_isPlatformer
                    && fabsf(m_slopeAngleRadians * 57.296f) > 40.0f
                    && m_yVelocity < -5.0)
                {
                    if (m_platformerXVelocity > 0.0)
                        m_platformerXVelocity *= 0.5;
                }

                updateCollide(PlayerCollisionDirection::Bottom, object);

                m_currentSlope2 = object;
                m_slopeAngle = object->getSlopeAngle();
                m_slopeSlidingMaybeRotated = m_currentSlope2->m_slopeUphill;

                if (m_isSideways)
                    m_slopeSlidingMaybeRotated = !m_slopeSlidingMaybeRotated;

                if (m_slopeAngle >= 1.0f && m_slopeSlidingMaybeRotated)
                    m_maybeSlidingTime = 3;

                cocos2d::CCPoint lastPos = object->getLastPosition();
                cocos2d::CCPoint realPos = object->getRealPosition();

                if (realPos.x != lastPos.x || realPos.y != lastPos.y)
                {
                    if (!m_isUpsideDown)
                    {
                        if (lastPos.y < realPos.y)
                            m_groundYVelocity = (realPos.y - lastPos.y) / dt;
                    }
                    else
                    {
                        if (realPos.x < lastPos.x)
                            m_groundYVelocity = (realPos.y - lastPos.y) / dt;
                    }
                }
            }
        }

        if (forceNextLanding)
        {
            if (m_isUpsideDown)
            {
                if (m_yVelocity >= 2.0)
                {
                    if (wasBoostedOnLanding && m_yVelocity > -2.0)
                        setYVelocity(-2.0f * flipMod(), 21);
                }
                else
                {
                    setYVelocity(-2.0f * flipMod(), 21);
                }
            }
            else
            {
                if (m_yVelocity <= -2.0)
                {
                    if (wasBoostedOnLanding && m_yVelocity < 2.0)
                        setYVelocity(2.0f * flipMod(), 22);
                }
                else
                {
                    setYVelocity(2.0f * flipMod(), 22);
                }
            }
        }
        else if (wasBoostedOnLanding)
        {
            if (m_isUpsideDown)
            {
                if (m_yVelocity <= -2.0)
                    setYVelocity(2.0f * flipMod(), 22);
            }
            else
            {
                if (m_yVelocity >= 2.0)
                    setYVelocity(2.0f * flipMod(), 22);
            }
        }

        float slopeSpeedFactor = 0.8f / angle;
        m_currentSlopeYVelocity = slopeVerticalSpeed;

        if (slopeSpeedFactor >= 1.1f)
            slopeSpeedFactor = 1.1f;

        float slopeBoostSpeed = slopeSpeedFactor * (slopeVerticalSpeed * 1.4f);
        if (slopeUphill > slopeBoostDirection)
            slopeBoostSpeed = -slopeBoostSpeed;

        m_slopeVelocity = slopeBoostSpeed * flipMod();

        if (forceNextLanding && m_isPlatformer && m_isBird)
            m_jumpBuffered = 0;

        if (isFlying() || m_isBall)
        {
            m_slopeVelocity *= 0.75f;

            if (!m_isBall)
            {
                updateSlopeRotation(dt);
            }
            else
            {
                float currentRotation = m_slopeRotation;
                if (m_slopeRotation != currentRotation)
                {
                    float cosValue = 1.0f / cosf(m_slopeRotation);
                    if (cosValue > 2.0f)
                        cosValue = (cosValue - 1.0f) * 0.7f + 1.0f;

                    stopRotation(true, 1);
                    runBallRotation(cosValue);
                }
            }
        }
        else
        {
            updateSlopeRotation(dt);
        }
    };

    if (shouldSnap)
    {
        bool skipDeath = false;

        if (!object->m_slopeIsHazard)
        {
            if (!currentlyOnSlope)
            {
                if (!m_isDart)
                    skipDeath = true;
                else
                {
                    shouldSnap = true;
                    if (m_stateDartSlide > 0)
                    {
                        applySlope();
                        return;
                    }
                }
            }

            if (!skipDeath)
            {
                if (m_isPlatformer)
                    skipDeath = true;
                else
                {
                    shouldSnap = true;
                    if (m_stateHitHead > 0)
                    {
                        applySlope();
                        return;
                    }
                }
            }
        }

        if (!skipDeath)
        {
            if (!m_ignoreDamage)
            {
                if (m_maybeCanRunIntoBlocks)
                    m_maybeIsColliding = true;
                else
                    GameManager::sharedState()->m_playLayer->destroyPlayer(this, nullptr);
                return;
            }

            shouldSnap = true;
        }

        applySlope();
        return;
    }

    if (m_isSideways)
    {
        auto it = m_rotateObjectsRelated.find(object->m_uniqueID);
        if (it != m_rotateObjectsRelated.end())
            collisionY -= static_cast<float>(it->second.y);
    }

    if (!m_wasOnSlope)
        return;

    bool slopeCollisionMatches = m_unk3d0 == collisionY
        || (m_isSideways && m_blackOrbRelated == collisionY);

    bool skipDueToPlatformerStop = slopeCollisionMatches
        && m_isPlatformer
        && fabsf(m_platformerXVelocity) < 0.100000001f;

    bool checkDartSlide = false;
    bool checkPlatformerLanding = false;

    if (slopeCollisionMatches)
    {
        if (m_isPlatformer && fabsf(m_platformerXVelocity) < 0.100000001f)
        {
            if (slopeDeltaY != 0.0f)
                checkPlatformerLanding = true;
        }
        else
        {
            checkDartSlide = true;
        }
    }
    else
    {
        if (slopeDeltaY == 0.0f)
        {
            if (m_isPlatformer)
                checkPlatformerLanding = true;
            else
                checkDartSlide = true;
        }
    }

    if (checkDartSlide)
    {
        if (slopeClampedFromTop && m_unk3e0)
        {
            if (!m_isDashing)
            {
                if (!m_isPlatformer || fabsf(m_platformerXVelocity) > 0.100000001f)
                {
                    if (!m_isPlatformer)
                    {
                        if (slopeUphill && m_isDart && m_jumpBuffered && m_isUpsideDown != slopeIsFloorTop)
                        {
                            applySlope();
                            return;
                        }
                        if (!shouldSnap)
                            return;
                        applySlope();
                        return;
                    }
                    checkPlatformerLanding = true;
                }
                else
                {
                    checkPlatformerLanding = true;
                }
            }
            else if (!m_isPlatformer)
            {
                if (slopeUphill && m_isDart && m_jumpBuffered && m_isUpsideDown != slopeIsFloorTop)
                {
                    applySlope();
                    return;
                }
                if (!shouldSnap)
                    return;
                applySlope();
                return;
            }
            else
            {
                checkPlatformerLanding = true;
            }
        }

        if (!slopeClampedFromBottom)
        {
            if (!m_isPlatformer)
            {
                if (slopeUphill && m_isDart && m_jumpBuffered && m_isUpsideDown != slopeIsFloorTop)
                {
                    applySlope();
                    return;
                }
                if (!shouldSnap)
                    return;
                applySlope();
                return;
            }
            checkPlatformerLanding = true;
        }

        if (m_unk3e1)
        {
            if (!m_isDashing)
            {
                if (!m_isPlatformer || fabsf(m_platformerXVelocity) > 0.100000001f)
                {
                    if (!m_isPlatformer)
                    {
                        if (slopeUphill && m_isDart && m_jumpBuffered && m_isUpsideDown != slopeIsFloorTop)
                        {
                            applySlope();
                            return;
                        }
                        if (!shouldSnap)
                            return;
                        applySlope();
                        return;
                    }
                    checkPlatformerLanding = true;
                }
                else
                {
                    checkPlatformerLanding = true;
                }
            }
            else if (!m_isPlatformer)
            {
                if (slopeUphill && m_isDart && m_jumpBuffered && m_isUpsideDown != slopeIsFloorTop)
                {
                    applySlope();
                    return;
                }
                if (!shouldSnap)
                    return;
                applySlope();
                return;
            }
            else
            {
                checkPlatformerLanding = true;
            }
        }

        if (!m_isPlatformer)
        {
            if (slopeUphill && m_isDart && m_jumpBuffered && m_isUpsideDown != slopeIsFloorTop)
            {
                applySlope();
                return;
            }
            if (!shouldSnap)
                return;
            applySlope();
            return;
        }
        checkPlatformerLanding = true;
    }

    if (checkPlatformerLanding)
    {
        if (!m_slopeUphill)
            return;
        if (!m_isDart)
            return;
        if (!m_jumpBuffered)
            return;
        if (m_isUpsideDown == slopeIsFloorTop)
            return;
    }

    if (!shouldSnap)
        return;

    applySlope();
}

void PlayerObject::pushButton(PlayerButton button)
{
    if (!m_inputsLocked)
    {
        m_holdingButtons[button] = true;
    }

    if (m_isLocked)
        return;

    if (m_controlsDisabled)
        return;

    if (button == PlayerButton::Jump)
    {
        if (!m_jumpBuffered)
            placeStreakPoint();

        m_padRingRelated = false;
        m_jumpBuffered = 1;
        m_stateRingJump = 1;

        m_ringRelatedSet.clear();

        updateJumpVariables();

        if (m_touchingRings->count())
        {
            int ringCount = m_touchingRings->count();
            if (ringCount > 0)
            {
                cocos2d::CCArray* customRings = cocos2d::CCArray::createWithCapacity(ringCount);
                cocos2d::CCArray* regularRings = cocos2d::CCArray::createWithCapacity(ringCount);

                for (int i = 0; i < m_touchingRings->count(); i++)
                {
                    RingObject* ring = (RingObject*)m_touchingRings->objectAtIndex(i);
                    if (!ring)
                        continue;

                    if (ring->m_claimTouch)
                    {
                        ringJump(ring, false);
                        m_jumpBuffered = 0;
                        return;
                    }

                    if (ring->getType() == GameObjectType::CustomRing)
                        customRings->addObject(ring);
                    else
                        regularRings->addObject(ring);
                }

                for (int i = 0; i < customRings->count(); i++)
                    ringJump((RingObject*)customRings->objectAtIndex(i), false);

                for (int i = 0; i < regularRings->count(); i++)
                    ringJump((RingObject*)regularRings->objectAtIndex(i), false);
            }
        }
        else if (m_isBall)
        {
            if (m_isOnGround)
                updateJump(0.0f);
        }
        else if (isFlying())
        {
        }
        else if (m_jumpBuffered)
        {
            if (m_isOnGround)
                updateJump(0.0f);
        }
        return;
    }

    if (button == (PlayerButton)2 || button == (PlayerButton)3)
    {
        switchedDirTo(button);
    }
}

void PlayerObject::pushDown()
{
    setYVelocity(0.0, 46);
    m_isOnGround2 = false;
    m_isOnGround = false;
}

void PlayerObject::pushPlayer(float yVelocity)
{
    addToYVelocity(yVelocity, 68);
}

void PlayerObject::redirectDash(float rotation)
{
    if (!m_isDashing)
        return;

    cocos2d::CCPoint dashDirection(m_dashX, m_dashY);

    float currentAngle = atan2f(dashDirection.y, dashDirection.x);
    float rotationRadians = rotation * 0.017453f;
    float angleDelta = -(currentAngle - rotationRadians);
    float nearestDirection = GJBaseGameLayer::convertToClosestDirection(angleDelta, 3.1416f);

    if (nearestDirection != 0.0f)
    {
        float cosDelta = cosf(nearestDirection);
        float sinDelta = sinf(nearestDirection);

        cocos2d::CCPoint rotated(
            -(sinDelta * dashDirection.y) + (cosDelta * dashDirection.x),
            (sinDelta * dashDirection.x) + (cosDelta * dashDirection.y));
        dashDirection = rotated;
    }

    float finalAngle = atan2f(dashDirection.y, dashDirection.x);
    float finalAngleDegrees = -(finalAngle * 57.296f);

    int angleInt = (int)finalAngleDegrees;
    m_dashAngle = finalAngleDegrees;

    float storedX;
    float storedY;

    if (m_isSideways)
    {
        int wrapped = (angleInt - 90) % 360;
        if (wrapped < 0)
            wrapped += 360;

        storedY = dashDirection.y;
        m_dashAngle = (double)-wrapped;
        float oldX = dashDirection.x;
        dashDirection.x = storedY;
        storedX = oldX;
        dashDirection.y = oldX;
    }
    else
    {
        int wrapped = angleInt % 360;
        if (angleInt % 360 < 0)
            wrapped += 360;

        storedX = dashDirection.x;
        storedY = dashDirection.y;
        m_dashAngle = (double)-wrapped;
    }

    m_dashX = storedX;
    m_dashY = storedY;

    if (storedX < 0.0f)
        doReversePlayer(!m_isSideways);
    else if (storedX > 0.0f)
        doReversePlayer(m_isSideways);

    updateDashArt();
}

void PlayerObject::releaseAllButtons()
{
    releaseButton(PlayerButton::Jump);
    releaseButton((PlayerButton)2);
    releaseButton((PlayerButton)3);
    releaseButton((PlayerButton)5);
}

bool PlayerObject::releaseButton(PlayerButton button)
{
    if (button == (PlayerButton)5)
    {
        releaseButton((PlayerButton)2);
        releaseButton((PlayerButton)3);
        m_isDashing = !m_controlsDisabled;
    }

    if (!m_inputsLocked)
    {
        auto it = m_holdingButtons.find(button);
        if (it == m_holdingButtons.end())
            m_holdingButtons[button] = false;
        else
            it->second = false;
    }

    m_isDashing = false;

    if (m_controlsDisabled)
        return false;

    if (m_jumpBuffered)
    {
        placeStreakPoint();
    }

    if (button == PlayerButton::Jump)
    {
        m_jumpBuffered = 0;
        m_stateRingJump = 0;
        m_touchedPad = button;

        if (m_isDashing)
            stopDashing();

        m_isDashing = true;
        return true;
    }

    if (button == (PlayerButton)2)
    {
        m_holdingLeft = false;
        if (m_platformerXVelocity >= 0.0)
        {
            m_isMoving = false;
            m_isDashing = true;
        }
    }
    else if (button == (PlayerButton)3)
    {
        double platformerVelocity = m_platformerXVelocity;
        m_holdingRight = 0;
        if (platformerVelocity > 0.0)
        {
            m_isMoving = false;
            m_isDashing = true;
        }
    }

    m_jumpBuffered = 0;
    m_stateRingJump = 0;
    m_touchedPad = button;

    if (m_isDashing)
        stopDashing();

    m_isDashing = true;
    return true;
}

void PlayerObject::removeAllParticles()
{
    for (unsigned int i = 0; i < (unsigned int)m_particleSystems->count(); i++)
    {
        cocos2d::CCParticleSystem* particle = (cocos2d::CCParticleSystem*)m_particleSystems->objectAtIndex(i);
        particle->resetSystem();
        particle->stopSystem();
        particle->removeFromParentAndCleanup(false);
    }
}

void PlayerObject::removePendingCheckpoint()
{
    if (!m_pendingCheckpoint)
        return;

    GameObject* checkpointObject = m_pendingCheckpoint->getObject();
    checkpointObject->removeGlow();
    checkpointObject->removeMeAndCleanup();
    m_pendingCheckpoint->release();
    m_pendingCheckpoint = nullptr;
}

void PlayerObject::removePlacedCheckpoint()
{
    if (!m_checkpointTimeout)
        return;

    PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;
    playLayer->removeCheckpoint(false);
    m_checkpointTimeout = false;
}

void PlayerObject::resetAllParticles()
{
    for (unsigned int i = 0; i < (unsigned int)m_particleSystems->count(); i++)
    {
        cocos2d::CCParticleSystem* particle = (cocos2d::CCParticleSystem*)m_particleSystems->objectAtIndex(i);
        particle->resetSystem();
        particle->stopSystem();
    }
}

void PlayerObject::resetCollisionLog(bool full)
{
    m_collisionLogTop->removeAllObjects();
    m_collisionLogBottom->removeAllObjects();
    m_collisionLogLeft->removeAllObjects();
    m_collisionLogRight->removeAllObjects();

    if (full)
    {
        m_lastCollisionTop = -1;
        m_lastCollisionBottom = -1;
    }

    m_unk510 = m_lastCollisionTop;
    m_unk50C = m_lastCollisionBottom;
    m_lastCollisionBottom = -1;
    m_lastCollisionTop = -1;
    m_lastCollisionLeft = -1;
    m_lastCollisionRight = -1;
}

void PlayerObject::resetCollisionValues()
{
    m_collidedTopMinY = 0.0;
    m_collidedBottomMaxY = 0.0;
    m_collidedLeftMaxX = 0.0;
    m_collidedRightMinX = 0.0;
}

void PlayerObject::resetObject()
{
    m_maybeReducedEffects = true;
    m_actionManager->stopAllInternalActions();

    m_wasTeleported = false;
    m_isCollidingWithSlope = false;
    m_collidedObject = nullptr;
    m_lastGroundObject = nullptr;
    m_currentSlope2 = nullptr;
    m_preLastGroundObject = nullptr;
    m_unk3e0 = false;
    m_collidingWithLeft = nullptr;
    m_collidingWithRight = nullptr;
    m_collidingBetweenSteps = false;
    m_ringJumpRelated = false;
    m_physDeltaRelated = 0.0;
    m_scaleXRelated2 = 0.0;
    m_groundYVelocity = 0.0;
    m_scaleXRelated4 = 0.0;
    m_scaleXRelated3 = 0.0;
    m_yVelocityRelated = 0.0;
    m_scaleXRelated5 = 0.0;
    m_maybeReverseSpeed = 0.0f;
    m_maybeReverseAcceleration = 0.0;

    doReversePlayer(false);
    rotateGameplay(2, 4, 0, 0.0f, 0.0f, 0, 0);

    m_maybeHasStopped = false;
    m_maybeGoingCorrectSlopeDirection = false;
    m_affectedByForces = false;
    m_isOnIce = false;
    m_platformerMovingLeft = 0;
    m_platformerMovingRight = 0;
    m_maybeUpsideDownSlope = false;
    m_maybeSlidingTime = 0;
    m_controlsDisabled = false;
    m_maybeIsColliding = false;
    m_isOnGround = false;
    m_isOnGround2 = true;
    m_dashRing = nullptr;
    m_decreaseBoostSlide = false;
    m_platformerXVelocity = 0.0;
    m_scaleXRelated = 0.0f;
    m_maybeSlidingStartTime = 0.0;
    m_changedDirectionsTime = 0.0;
    m_lastLandTime = 0.0;
    m_scaleXRelatedTime = 0.0;
    m_fallSpeed = 0.0;
    m_dashStartTime = 0.0;
    m_platformerVelocityRelated = 0.0f;
    m_gravityMod = 1.0f;

    toggleVisibility(true);
    m_flashTime = -1.0;

    setColor(&m_originalMainColor);
    setSecondColor(&m_originalSecondColor);

    stopParticles();
    m_stateUnk = 0;
    resetStateVariables();
    resetTouchedRings(true);

    for (unsigned int i = 0; i < m_playerFollowFloats.size(); i++)
        m_playerFollowFloats[i] = 0.0f;

    m_followRelated = 0;
    m_unk838 = 0.0f;

    stopDashing();
    deactivateStreak(false);
    removePendingCheckpoint();

    m_spiderSprite->stopActionByTag(10);
    m_spiderSprite->setScale(1.0f);

    m_checkpointTimeout = false;
    m_canPlaceCheckpoint = false;

    m_lastGroundedPos = cocos2d::CCPoint(0.0f, 0.0f);
    m_lastActivatedPortal = nullptr;
    m_lastPortalPos = cocos2d::CCPoint(0.0f, 0.0f);
    m_isLocked = false;

    m_potentialSlopeMap.clear();
    m_objectSnappedTo = nullptr;
    m_shouldTryPlacingCheckpoint = false;
    m_isOnSlope = false;
    m_wasOnSlope = false;
    m_collidingWithSlopeId = -1;
    m_isAccelerating = false;
    m_currentSlope = nullptr;
    m_currentPotentialSlope = nullptr;
    m_slopeVelocity = 0.0f;
    m_currentSlopeYVelocity = 0.0;
    m_slopeRotation = 0.0f;

    m_unk669 = false;
    resetCollisionValues();
    m_maybeIsBoosted = false;
    m_totalTime = 0.0;

    setYVelocity(0.0, 37);
    flipGravity(false, false);
    switchedToMode(GameObjectType::CubePortal);
    m_vehicleSize = 0.6f;
    togglePlayerScale(false, false);
    stopRotation(false, 4);
    setRotation(0.0f);
    m_isDead = false;

    stopActionByTag(4);
    setOpacity(255);
    toggleGhostEffect(GhostType::Disabled);
    updateTimeMod(0.9f, false);
    resetStreak();
    levelFlipFinished();
    m_maybeReducedEffects = false;

    if (m_alwaysShowStreak)
        activateStreak();
}

void PlayerObject::resetPlayerIcon()
{
    stopPlatformerJumpAnimation();

    m_width = 30.0f;
    m_height = 30.0f;
    m_unkAngle1 = 30.0f;

    runRotateAction(false, 5);

    m_iconSprite->setScale(1.0f);
    m_iconSprite->setPosition(&cocos2d::CCPoint(0.0f, 0.0f));
    m_vehicleSprite->setVisible(false);
    m_birdVehicle->setVisible(false);

    updatePlayerGlow();

    m_trailingParticles->stopSystem();
    m_shipClickParticles->stopSystem();
    m_vehicleGroundParticles->stopSystem();

    cocos2d::ccColor4F startColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_trailingParticles->setStartColor(startColor);

    cocos2d::ccColor4F endColor(1.0f, 0.0f, 0.0f, 1.0f);
    m_trailingParticles->setEndColor(endColor);

    if (m_disableStreakTint)
        m_regularTrail->setColor(&cocos2d::ccc3(255, 255, 255));
    else
        m_regularTrail->setColor(&m_playerColor2);

    deactivateStreak(false);

    if (m_shipStreak)
        m_shipStreak->setVisible(false);

    updatePlayerScale();
}

void PlayerObject::resetStateVariables()
{
    m_stateNoAutoJump = 0;
    m_stateDartSlide = 0;
    m_stateFlipGravity = 0;
    m_stateHitHead = 0;
    m_stateOnGround = 0;
    m_stateBoostX = 0;
    m_stateBoostY = 0;
    m_maybeStateForce2 = 0;
    m_stateScale = 0;
    m_stateForce = 0;
    m_stateForceVector = cocos2d::CCPoint(0.0f, 0.0f);
    m_jumpPadRelated.clear();
}

void PlayerObject::resetStreak()
{
    if (levelFlipping())
        return;

    m_regularTrail->reset();
    cocos2d::CCPoint trailOffset(-5.0f, 0.0f);
    cocos2d::CCPoint trailPos = getPosition() + trailOffset;
    m_regularTrail->setPosition(&trailPos);

    m_waveTrail->reset();
    m_waveTrail->setOpacity(255);
    m_waveTrail->stopAllActions();
    m_waveTrail->setPosition(&getPosition());
}

void PlayerObject::resetTouchedRings(bool removeAll)
{
    if (removeAll)
    {
        m_touchingRings->removeAllObjects();
    }
    else
    {
        unsigned int i = 0;
        while (i < (unsigned int)m_touchingRings->count())
        {
            RingObject* ring = (RingObject*)m_touchingRings->objectAtIndex(i);
            if (m_touchedRings.find(ring->m_uniqueID) == m_touchedRings.end())
            {
                m_touchingRings->removeObjectAtIndex(i, true);
            }
            else
            {
                i++;
            }
        }
    }

    m_touchedRings.clear();
}

int PlayerObject::reverseMod()
{
    return m_isGoingLeft ? -1 : 1;
}

void PlayerObject::reversePlayer(EffectGameObject* object)
{
    if (m_isPlatformer)
        return;

    if (object && m_reverseSync)
    {
        cocos2d::CCPoint objectRealPos = object->getRealPosition();
        cocos2d::CCPoint offset(m_maybeReverseSpeed, m_maybeReverseSpeed);
        cocos2d::CCPoint myPos = getPosition() + offset;
        cocos2d::CCPoint delta = objectRealPos - myPos;

        float value = m_isSideways ? delta.y * 2.0f : delta.x * 2.0f;
        m_maybeReverseSpeed += value;
    }

    doReversePlayer(!m_isGoingLeft);
}

void PlayerObject::ringJump(RingObject* object, bool skipCheck)
{
    if (!object || m_isDead || m_ringRelatedSet.find(object->m_uniqueID) != m_ringRelatedSet.end())
        return;

    unsigned int hasNoEffects = object->m_hasNoEffects;
    unsigned int isCustomRing = object->getType() == (GameObjectType)36;
    GameObjectType ringType = object->getType();
    unsigned int isGravityRing = ringType == (GameObjectType)(42 | 4);
    unsigned int isRegularRing = (ringType != (GameObjectType)(42 | 4)) & (isCustomRing ^ 1);

    bool shouldProcess = m_stateRingJump2
        && !m_isDashing
        && m_stateJumpBuffered
        && m_touchedRing < isRegularRing
        && m_touchedCustomRing < isCustomRing
        && m_touchedGravityPortal < isGravityRing;

    if (shouldProcess || skipCheck)
    {
        if (object->m_isReverse)
            reversePlayer(object);

        m_uniqueID = object->m_uniqueID;
        m_ringJumpRelated = true;
        m_ringRelatedSet.insert(m_uniqueID);

        if (m_gameLayer)
        {
            gameEventTriggered(8, 0);
            GJGameEvent event = m_gameLayer->objectTypeToGameEvent(object->getType());
            gameEventTriggered((int)event, 0);
        }

        if (isCustomRing)
            m_touchedCustomRing = true;
        else if (isGravityRing)
            m_touchedGravityPortal = true;
        else
            m_touchedRing = true;

        m_touchingRings->removeObject(object);

        if (isRegularRing)
            m_padRingRelated = true;

        if (object->getType() == GameObjectType::CustomRing)
        {
            m_gameLayer->activateCustomRing(object);

            if (m_enable22Changes)
                m_stateRingJump = false;
        }
        else if (object->getType() != (GameObjectType)(42 | 4))
        {
            if (object->getType() == (GameObjectType)(42 | 0x1))
            {
                bool facingOpposite;
                if (m_isSideways)
                {
                    bool facingLeft = object->isFacingLeft();
                    if (m_isUpsideDown != facingLeft)
                    {
                        flipGravity(!m_isUpsideDown, true);
                    }
                }
                else
                {
                    bool facingDown = object->isFacingDown();
                    if (m_isUpsideDown == facingDown)
                    {
                    }
                    else
                    {
                        flipGravity(!m_isUpsideDown, true);
                    }
                }

                spiderTestJump(false);
            }
            else if (object->getType() == GameObjectType::DashRing)
            {
                if (m_gameLayer)
                    gameEventTriggered(41, 0);

                startDashing((DashRingObject*)object);
            }
            else if (object->getType() == GameObjectType::GravityDashRing)
            {
                if (m_playEffects)
                {
                    GameManager* gm = GameManager::sharedState();
                    gm->m_playLayer->flipGravity(this, !m_isUpsideDown, true);
                    gm->m_playLayer->playGravityEffect(m_isUpsideDown);
                }
                else if (GameManager::sharedState()->m_levelEditorLayer)
                {
                    GameManager* gm = GameManager::sharedState();
                    gm->m_levelEditorLayer->flipGravity(this, !m_isUpsideDown, true);
                }
                else
                {
                    flipGravity(!m_isUpsideDown, true);
                }

                if (m_gameLayer)
                    gameEventTriggered(42, 0);

                startDashing((DashRingObject*)object);
            }
            else if (object->getType() == GameObjectType::DropRing)
            {
                m_stateRingJump = false;
                flipMod();
                int flip = flipMod();
                float dropVelocity;

                if (isFlying())
                {
                    flipMod();
                    dropVelocity = (float)flipMod() * -14.0f;
                    if (m_isBird)
                        dropVelocity *= 0.8f;
                }
                else
                {
                    dropVelocity = (float)flip * -15.0f;
                    if (!m_isRobot && m_isSpider)
                        dropVelocity *= 1.1f;
                }

                setYVelocity(dropVelocity, 39);

                if (m_isBall)
                    runBallRotation2();
                else
                    runRotateAction(false, 3);

                activateStreak();
                m_hasEverHitRing = true;
                m_isAccelerating = true;

                if (m_isBall || m_isSwing)
                    m_jumpBuffered = 0;
            }
            else
            {
                m_maybeIsBoosted = true;
                m_isOnGround2 = false;
                m_isOnGround = false;
                m_stateRingJump = false;

                float vehicleScale = (m_vehicleSize != 1.0f) ? 0.8f : 1.0f;

                if (object->getType() == GameObjectType::GravityRing)
                    m_yStart *= 0.8f;
                else if (object->getType() == GameObjectType::GreenRing)
                {
                    if (m_isShip)
                        m_yStart *= 0.7f;
                }
                else if (object->getType() == GameObjectType::PinkJumpRing)
                {
                    if (m_isShip)
                        m_yStart *= 0.37f;
                    else if (m_isBird)
                        m_yStart *= 0.42f;
                    else if (m_isBall)
                        m_yStart *= 0.77f;
                    else
                        m_yStart *= 0.72f;
                }
                else if (object->getType() == GameObjectType::RedJumpRing)
                {
                    if (m_isShip)
                    {
                        if (m_vehicleSize != 1.0f)
                            m_yStart *= 1.4f;
                    }
                    else if (m_isBird)
                    {
                        if (m_vehicleSize == 1.0f)
                            m_yStart *= 1.02f;
                        else
                            m_yStart *= 1.36f;
                    }
                    else if (!m_isBall)
                    {
                        if (m_isRobot)
                            m_yStart *= 1.28f;
                        else if (!m_isSpider)
                            m_yStart *= 1.38f;
                    }
                    else
                    {
                        m_yStart *= 1.34f;
                    }
                }
                else
                {
                    if (m_isRobot)
                        m_yStart *= 0.9f;
                }

                if (object->getType() == GameObjectType::GreenRing)
                {
                    if (m_playEffects)
                    {
                        GameManager* gm = GameManager::sharedState();
                        gm->m_playLayer->flipGravity(this, !m_isUpsideDown, true);
                        if (!hasNoEffects)
                            gm->m_playLayer->playGravityEffect(m_isUpsideDown);
                    }
                    else if (GameManager::sharedState()->m_levelEditorLayer)
                    {
                        GameManager* gm = GameManager::sharedState();
                        gm->m_levelEditorLayer->flipGravity(this, !m_isUpsideDown, true);
                    }
                    else
                    {
                        flipGravity(!m_isUpsideDown, true);
                    }
                }

                int flip = flipMod();
                setYVelocity((float)((float)flip * m_yStart) * vehicleScale, 38);

                if (m_isBall)
                    runBallRotation2();
                else
                    runRotateAction(false, 2);

                if (m_isPlatformer && !m_isRotating)
                    animatePlatformerJump(1.0f);

                m_lastGroundedPos = getPosition();
                activateStreak();
                m_hasEverHitRing = true;

                if (m_isBall)
                {
                    m_jumpBuffered = 0;
                    m_yVelocity *= 0.699999988f;
                }
                else if (m_isSwing)
                {
                    m_jumpBuffered = 0;
                    m_yVelocity *= 0.600000024f;
                }
                else if (m_isSpider)
                {
                    m_jumpBuffered = 0;
                    m_yVelocity *= 0.699999988f;
                }

                if (object->getType() == GameObjectType::GravityRing)
                {
                    if (m_playEffects)
                    {
                        GameManager* gm = GameManager::sharedState();
                        gm->m_playLayer->flipGravity(this, !m_isUpsideDown, true);
                        if (!hasNoEffects)
                            gm->m_playLayer->playGravityEffect(m_isUpsideDown);
                    }
                    else if (GameManager::sharedState()->m_levelEditorLayer)
                    {
                        GameManager* gm = GameManager::sharedState();
                        gm->m_levelEditorLayer->flipGravity(this, !m_isUpsideDown, true);
                    }
                    else
                    {
                        flipGravity(!m_isUpsideDown, true);
                    }
                }

                if (m_isRobot)
                {
                    m_robotSprite->runAnimation("jump_start");
                }
                else if (m_isSpider)
                {
                    m_spiderSprite->stopTween();
                    m_spiderSprite->m_animationManager->stopAnimations();
                    m_spiderSprite->tweenToAnimation("jump", 0.06f);
                }

                if (object->getType() == GameObjectType::RedJumpRing)
                    m_isAccelerating = true;
            }
        }
        else
        {
            m_gameLayer->teleportPlayer((TeleportPortalObject*)object, this);
            m_stateRingJump = false;
        }

        if (!m_maybeReducedEffects
            && !GameManager::sharedState()->m_performanceMode
            && !object->hasBeenActivated()
            && m_playEffects > hasNoEffects
            && object->m_objectID != 3643)
        {
            int colorKey = object->getType() - 12;
            int red = 0;
            int green = 0;
            int blue = 0;

            switch (colorKey)
            {
                case 0:
                case 26:
                    red = -1;
                    green = 0;
                    blue = -1;
                    break;
                case 1:
                    red = 0;
                    green = -1;
                    blue = -1;
                    break;
                case 17:
                case 25:
                    red = 0;
                    green = -1;
                    blue = 0;
                    break;
                case 20:
                {
                    GameManager* gm = GameManager::sharedState();
                    cocos2d::ccColor3B activeColor = gm->m_playLayer->m_effectManager->activeColorForIndex(1007);
                    red = activeColor.r;
                    green = activeColor.g;
                    blue = activeColor.b;
                    break;
                }
                case 23:
                    red = -1;
                    green = 100;
                    blue = 0;
                    break;
                case 24:
                {
                    if (object->m_colorSprite)
                    {
                        cocos2d::ccColor3B spriteColor = object->m_colorSprite->getColor();
                        red = spriteColor.r;
                        green = spriteColor.g;
                        blue = spriteColor.b;
                    }
                    else
                    {
                        cocos2d::ccColor3B objColor = object->getColor();
                        red = objColor.r;
                        green = objColor.g;
                        blue = objColor.b;
                    }
                    break;
                }
                case 31:
                    red = -1;
                    green = 50;
                    blue = -1;
                    break;
                default:
                    red = -1;
                    green = -56;
                    blue = 0;
                    break;
            }

            float waveRadius = (object->getType() == GameObjectType::RedJumpRing) ? 42.0f : 35.0f;
            CCCircleWave* wave = CCCircleWave::create(waveRadius, 5.0f, 0.35f, 1);
            wave->m_color.r = red;
            wave->m_color.g = green;
            wave->m_color.b = blue;
            wave->setPosition(&object->getPosition());
            CCCircleWave::followObject(wave, object, true);

            PlayLayer* layer = GameManager::sharedState()->m_playLayer;
            wave->m_delegate = layer ? &layer->CCCircleWaveDelegate : nullptr;
            GameManager::sharedState()->m_playLayer->addCircle(wave);

            m_parentLayer->addChild(wave, 0);
        }

        if (!m_editorEnabled)
            object->playTriggerEffect();

        object->activatedByPlayer(this);
        object->powerOffObject();
        incrementJumps();
    }
}

void PlayerObject::rotateGameplayObject(GameObject* object)
{
    if (m_rotateObjectsRelated.find(object->m_uniqueID) != m_rotateObjectsRelated.end())
        return;

    cocos2d::CCPoint objectRealPos = object->getRealPosition();
    cocos2d::CCPoint objectStartPos = objectRealPos;
    cocos2d::CCPoint myPos = getPosition();

    float newX = myPos.x - (-(objectRealPos.y - myPos.y) - ((objectRealPos.x - myPos.x) * -0.000000043711f));
    float newY = myPos.y + ((objectRealPos.x - myPos.x) + ((objectRealPos.y - myPos.y) * -0.000000043711f));

    float deltaX = newX - objectStartPos.x;
    float deltaY = newY - objectStartPos.y;

    objectRealPos.x = newX;
    objectRealPos.y = newY;

    object->addToTempOffset(deltaX, deltaY);

    cocos2d::CCPoint lastPos = object->getLastPosition();
    cocos2d::CCPoint offset(deltaX, deltaY);
    cocos2d::CCPoint newLastPos = lastPos + offset;
    object->setLastPosition(&newLastPos);

    float newRotationY = object->m_startRotationY - 90.0f;
    object->m_startRotationX = object->m_startRotationX - 90.0f;
    object->m_startRotationY = newRotationY;
    object->setRRotation(0.0f);
    object->setObjectRectDirty(true);

    if (object->getType() == GameObjectType::Slope)
        object->determineSlopeDirection();

    GJPointDouble storedOffset;
    storedOffset.x = deltaX;
    storedOffset.y = deltaY;

    m_rotateObjectsRelated[object->m_uniqueID] = storedOffset;
}

void PlayerObject::rotateGameplayOnly(bool sideways)
{
    m_isSideways = sideways;
    updatePlayerArt();
}

void PlayerObject::rotatePreSlopeObjects()
{
    for (auto it = m_potentialSlopeMap.begin(); it != m_potentialSlopeMap.end(); ++it)
        rotateGameplayObject(it->second);
}

void PlayerObject::runBallRotation(float speed)
{
    if (m_isDashing || m_isPlatformer)
        return;

    float sizeMultiplier = (m_vehicleSize == 1.0f) ? 1.0f : 0.8f;
    m_rotateSpeed = speed;
    m_isBallRotating = speed != 1.0f;

    float speedFactor;

    if (m_playerSpeed == 0.7f)
        speedFactor = 1.2406f;
    else if (m_playerSpeed == 1.1f)
        speedFactor = 0.80424f;
    else if (m_playerSpeed == 1.3f)
        speedFactor = 0.66577f;
    else if (m_playerSpeed == 1.6f)
        speedFactor = 0.54094f;
    else
        speedFactor = 1.0f;

    int flip = flipMod();
    int reverse = reverseMod();
    float rotationSpeed = (120.0f * flip * reverse) / ((sizeMultiplier * 0.2f) * speedFactor);
    m_rotationSpeed = rotationSpeed;

    if (m_isSideways)
        m_rotationSpeed = -rotationSpeed;

    m_isRotating = true;
}

void PlayerObject::runBallRotation2()
{
    if (m_isDashing)
        return;

    float sizeMultiplier = (m_vehicleSize == 1.0f) ? 1.0f : 0.8f;
    float speedFactor;

    if (m_playerSpeed == 0.7f)
        speedFactor = 1.2406f;
    else if (m_playerSpeed == 1.1f)
        speedFactor = 0.80424f;
    else if (m_playerSpeed == 1.3f)
        speedFactor = 0.66577f;
    else if (m_playerSpeed == 1.6f)
        speedFactor = 0.54094f;
    else
        speedFactor = 1.0f;

    int flip = flipMod();
    int reverse = reverseMod();
    m_isBallRotating2 = true;

    float rotationSpeed = ((-340.0f * flip * reverse) * m_gravityMod) / (sizeMultiplier * 0.8f * speedFactor);
    m_rotationSpeed = rotationSpeed;
}

void PlayerObject::runNormalRotation()
{
    runNormalRotation(false, 1.0f);
}

void PlayerObject::runNormalRotation(bool ground, float speed)
{
    m_isRotating = true;
    m_rotateSpeed = speed;
}

void PlayerObject::runRotateAction(bool ground, int type)
{
    if (m_isLocked || m_isDashing)
        return;

    stopRotation(ground, 22);

    if (m_isBall)
        runBallRotation(1.0f);
    else
        runNormalRotation();
}

void PlayerObject::saveToCheckpoint(PlayerCheckpoint* checkpoint)
{
    checkpoint->m_position = getPosition();
    checkpoint->m_lastPosition = m_lastPosition;
    checkpoint->m_isUpsideDown = m_isUpsideDown;
    checkpoint->m_isSideways = m_isSideways;
    checkpoint->m_isShip = m_isShip;
    checkpoint->m_isBall = m_isBall;
    checkpoint->m_isBird = m_isBird;
    checkpoint->m_yVelocityUnrounded = m_yVelocity;
    checkpoint->m_isDart = m_isDart;
    checkpoint->m_isRobot = m_isRobot;
    checkpoint->m_isSpider = m_isSpider;
    checkpoint->m_isSwing = m_isSwing;
    checkpoint->m_isOnGround = m_isOnGround;
    checkpoint->m_isMini = m_vehicleSize != 1.0f;
    checkpoint->m_ghostType = m_ghostType;
    checkpoint->m_isHidden = m_isHidden;
    checkpoint->m_playerSpeed = m_playerSpeed;
    checkpoint->m_isGoingLeft = m_isGoingLeft;
    checkpoint->m_maybeReverseSpeed = m_maybeReverseSpeed;
    checkpoint->m_gravityMod = m_gravityMod;
    checkpoint->m_decreaseBoostSlide = m_decreaseBoostSlide;
    checkpoint->m_followRelated = m_followRelated;
    checkpoint->m_lastFlipTime = m_lastFlipTime;
    checkpoint->m_unk838 = m_unk838;
    checkpoint->m_playerFollowFloats = m_playerFollowFloats;
    checkpoint->m_objectSnappedTo = m_objectSnappedTo;
    checkpoint->m_wasOnSlope = m_wasOnSlope;
    checkpoint->m_isOnSlope = m_isOnSlope;
    checkpoint->m_snapDistance = m_snapDistance;
    checkpoint->m_accelerationOrSpeed = m_accelerationOrSpeed;
    checkpoint->m_slopeVelocity = m_slopeVelocity;

    if (m_isDashing)
    {
        checkpoint->m_isDashing = true;
        checkpoint->m_dashAngle = m_dashAngle;
        checkpoint->m_dashX = m_dashX;
        checkpoint->m_dashRing = m_dashRing;
        checkpoint->m_dashY = m_dashY;
        checkpoint->m_dashStartTime = m_dashStartTime;
    }

    checkpoint->m_wasTeleported = m_wasTeleported;
    checkpoint->m_rotation = getRotation();
    checkpoint->m_fixGravityBug = m_fixGravityBug;
    checkpoint->m_reverseSync = m_reverseSync;
    checkpoint->m_yVelocityBeforeSlope = m_yVelocityBeforeSlope;
    checkpoint->m_justPlacedStreak = m_justPlacedStreak;
    checkpoint->m_slopeStartTime = m_slopeStartTime;
    checkpoint->m_lastCollisionBottom = m_lastCollisionBottom;
    checkpoint->m_lastCollisionTop = m_lastCollisionTop;
    checkpoint->m_lastCollisionLeft = m_lastCollisionLeft;
    checkpoint->m_lastCollisionRight = m_lastCollisionRight;
    checkpoint->m_unk50C = m_unk50C;
    checkpoint->m_unk510 = m_unk510;
    checkpoint->m_currentSlope2 = m_currentSlope2;
    checkpoint->m_preLastGroundObject = m_preLastGroundObject;
    checkpoint->m_slopeAngle = m_slopeAngle;
    checkpoint->m_slopeSlidingMaybeRotated = m_slopeSlidingMaybeRotated;
    checkpoint->m_currentPotentialSlope = m_currentPotentialSlope;
    checkpoint->m_quickCheckpointMode = m_quickCheckpointMode;
    checkpoint->m_collidedObject = m_collidedObject;
    checkpoint->m_lastGroundObject = m_lastGroundObject;
    checkpoint->m_collidingWithLeft = m_collidingWithLeft;
    checkpoint->m_collidingWithRight = m_collidingWithRight;
    checkpoint->m_scaleXRelated2 = m_scaleXRelated2;
    checkpoint->m_collidingBetweenSteps = m_collidingBetweenSteps;
    checkpoint->m_groundYVelocity = m_groundYVelocity;
    checkpoint->m_isCollidingWithSlope = m_isCollidingWithSlope;
    checkpoint->m_yVelocityRelated = m_yVelocityRelated;
    checkpoint->m_isBallRotating = m_isBallRotating;
    checkpoint->m_scaleXRelated3 = m_scaleXRelated3;
    checkpoint->m_unk669 = m_unk669;
    checkpoint->m_scaleXRelated4 = m_scaleXRelated4;
    checkpoint->m_scaleXRelated5 = m_scaleXRelated5;
    checkpoint->m_currentSlope = m_currentSlope;
    checkpoint->m_slopeFlipGravityRelated = m_slopeFlipGravityRelated;
    checkpoint->m_isRotating = m_isRotating;
    checkpoint->m_isBallRotating2 = m_isBallRotating2;
    checkpoint->m_padRingRelated = m_padRingRelated;
    checkpoint->m_slopeAngleRadians = m_slopeAngleRadians;
    checkpoint->m_maybeIsFalling = m_maybeIsFalling;
    checkpoint->m_shouldTryPlacingCheckpoint = m_shouldTryPlacingCheckpoint;
    checkpoint->m_collidingWithSlopeId = m_collidingWithSlopeId;
    checkpoint->m_trailingParticleLife = m_trailingParticleLife;
    checkpoint->unk_584 = unk_584;
    checkpoint->m_slopeAngleRadians = m_slopeAngleRadians;
    checkpoint->m_rotationSpeed = m_rotationSpeed;
    checkpoint->m_rotateSpeed = m_rotateSpeed;
    checkpoint->m_speedMultiplier = m_speedMultiplier;
    checkpoint->m_yStart = m_yStart;
    checkpoint->m_gravity = m_gravity;
    checkpoint->m_gameModeChangedTime = m_gameModeChangedTime;
    checkpoint->m_playEffects = m_playEffects;
    checkpoint->m_isOnGround3 = m_isOnGround3;
    checkpoint->m_unkBool5 = m_unkBool5;
    checkpoint->m_ringJumpRelated = m_ringJumpRelated;
    checkpoint->m_lastSpiderFlipTime = m_lastSpiderFlipTime;
    checkpoint->m_ringRelatedSet = m_ringRelatedSet;
    checkpoint->m_maybeSpriteRelated = m_maybeSpriteRelated;
    checkpoint->m_unk3e0 = m_unk3e0;
    checkpoint->m_slopeRotation = m_slopeRotation;
    checkpoint->m_unk3e1 = m_unk3e1;
    checkpoint->m_currentSlopeYVelocity = m_currentSlopeYVelocity;
    checkpoint->m_isAccelerating = m_isAccelerating;
    checkpoint->m_landParticlesAngle = m_landParticlesAngle;
    checkpoint->m_isCurrentSlopeTop = m_isCurrentSlopeTop;
    checkpoint->m_landParticleRelatedY = m_landParticleRelatedY;
    checkpoint->m_wasJumpBuffered = m_wasJumpBuffered;
    checkpoint->m_unk3d0 = m_unk3d0;
    checkpoint->m_blackOrbRelated = m_blackOrbRelated;
    checkpoint->m_collidedTopMinY = m_collidedTopMinY;
    checkpoint->m_collidedBottomMaxY = m_collidedBottomMaxY;
    checkpoint->m_collidedLeftMaxX = m_collidedLeftMaxX;
    checkpoint->m_collidedRightMinX = m_collidedRightMinX;
    checkpoint->m_wasRobotJump = m_wasRobotJump;
    checkpoint->m_stateJumpBuffered = m_stateJumpBuffered;
    checkpoint->m_stateRingJump2 = m_stateRingJump2;
    checkpoint->m_touchedRing = m_touchedRing;
    checkpoint->m_touchedCustomRing = m_touchedCustomRing;
    checkpoint->m_touchedGravityPortal = m_touchedGravityPortal;
    checkpoint->m_maybeTouchedBreakableBlock = m_maybeTouchedBreakableBlock;
    checkpoint->m_touchedPad = m_touchedPad;
    checkpoint->m_maybeUpsideDownSlope = m_maybeUpsideDownSlope;
    checkpoint->m_yVelocity = m_yVelocity;
    checkpoint->m_fallSpeed = m_fallSpeed;
    checkpoint->m_maybeReverseAcceleration = m_maybeReverseAcceleration;
    checkpoint->m_xVelocityRelated2 = m_xVelocityRelated2;
    checkpoint->m_shipRotation = m_shipRotation;
    checkpoint->m_lastPortalPos = m_lastPortalPos;
    checkpoint->m_isOnGround2 = m_isOnGround2;
    checkpoint->m_maybeIsBoosted = m_maybeIsBoosted;
    checkpoint->m_isLocked = m_isLocked;
    checkpoint->m_controlsDisabled = m_controlsDisabled;
    checkpoint->m_unkUnused3 = m_unkUnused3;
    checkpoint->m_lastLandTime = m_lastLandTime;
    checkpoint->m_platformerVelocityRelated = m_platformerVelocityRelated;
    checkpoint->m_scaleXRelatedTime = m_scaleXRelatedTime;
    checkpoint->m_lastGroundedPos = m_lastGroundedPos;

    checkpoint->m_touchingRings->removeAllObjects();

    if (m_touchingRings)
    {
        for (int i = 0; i < m_touchingRings->count(); i++)
            checkpoint->m_touchingRings->addObject(m_touchingRings->objectAtIndex(i));
    }

    checkpoint->m_touchedRings = m_touchedRings;
    checkpoint->m_leftPressedFirst = m_leftPressedFirst;
    checkpoint->m_lastActivatedPortal = m_lastActivatedPortal;
    checkpoint->m_defaultMiniIcon = m_defaultMiniIcon;
    checkpoint->m_swapColors = m_swapColors;
    checkpoint->m_switchDashFireColor = m_switchDashFireColor;
    checkpoint->m_stateOnGround = m_stateOnGround;
    checkpoint->m_stateBoostX = m_stateBoostX;
    checkpoint->m_stateBoostY = m_stateBoostY;
    checkpoint->m_maybeStateForce2 = m_maybeStateForce2;
    checkpoint->m_stateScale = m_stateScale;
    checkpoint->m_holdingRight = m_holdingRight;
    checkpoint->m_holdingLeft = m_holdingLeft;
    checkpoint->m_totalTime = m_totalTime;
    checkpoint->m_yVelocityRelated3 = m_yVelocityRelated3;
    checkpoint->m_stateUnk = m_stateUnk;
    checkpoint->m_platformerXVelocity = m_platformerXVelocity;
    checkpoint->m_scaleXRelated = m_scaleXRelated;
    checkpoint->m_isSlidingRight = m_isSlidingRight;
    checkpoint->m_maybeHasStopped = m_maybeHasStopped;
    checkpoint->m_maybeGoingCorrectSlopeDirection = m_maybeGoingCorrectSlopeDirection;
    checkpoint->m_isSliding = m_isSliding;
    checkpoint->m_isOnIce = m_isOnIce;
    checkpoint->m_isOnGround4 = m_isOnGround4;
    checkpoint->m_maybeSlidingTime = m_maybeSlidingTime;
    checkpoint->m_isMoving = m_isMoving;
    checkpoint->m_platformerMovingLeft = m_platformerMovingLeft;
    checkpoint->m_platformerMovingRight = m_platformerMovingRight;
    checkpoint->m_xVelocityRelated = m_xVelocityRelated;
    checkpoint->m_maybeSlopeForce = m_maybeSlopeForce;
    checkpoint->m_physDeltaRelated = m_physDeltaRelated;
    checkpoint->m_maybeSlidingStartTime = m_maybeSlidingStartTime;
    checkpoint->m_changedDirectionsTime = m_changedDirectionsTime;
    checkpoint->m_slopeEndTime = m_slopeEndTime;
    checkpoint->m_maybeChangedDirectionAngle = m_maybeChangedDirectionAngle;
    checkpoint->m_stateForce = m_stateForce;
    checkpoint->m_stateNoAutoJump = m_stateNoAutoJump;
    checkpoint->m_stateDartSlide = m_stateDartSlide;
    checkpoint->m_stateHitHead = m_stateHitHead;
    checkpoint->m_stateFlipGravity = m_stateFlipGravity;
    checkpoint->m_unkUnused2 = m_unkUnused2;
    checkpoint->m_stateForceVector = m_stateForceVector;
    checkpoint->m_affectedByForces = m_affectedByForces;
    checkpoint->m_jumpPadRelated = m_jumpPadRelated;
    checkpoint->m_fallStartY = m_fallStartY;
}

void PlayerObject::setSecondColor(const cocos2d::ccColor3B* color)
{
    m_iconSpriteSecondary->setColor(*color);
    m_vehicleSpriteSecondary->setColor(*color);
    m_robotSprite->updateColor02(*color);
    m_spiderSprite->updateColor02(*color);
}

void PlayerObject::setYVelocity(double velocity, int type)
{
    double integerPart = (double)(int)velocity;

    if (integerPart != velocity)
        velocity = integerPart + round((velocity - integerPart) * 1000.0) / 1000.0;

    m_yVelocity = velocity;
}

void PlayerObject::setupStreak()
{
    GameManager* gameManager = GameManager::sharedState();

    int streakID = gameManager->m_playerStreak.random - gameManager->m_playerStreak.seed;
    if (streakID > 7)
        streakID = 7;
    if (streakID <= 0)
        streakID = 1;

    m_playerStreak = streakID;
    m_hasGlow = GameManager::sharedState()->m_playerGlow;

    cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("streak_%02d_001.png", m_playerStreak);
    const char* frameCString = frameName->getCString();

    float streakWidth = 10.0f;
    float streakDuration = 0.3f;
    m_streakStrokeWidth = 10.0f;

    int streakCase = m_playerStreak - 2;

    switch (streakCase)
    {
        case 0:
        case 5:
            streakDuration = 0.3f;
            streakWidth = 14.0f;
            m_disableStreakTint = true;
            m_streakStrokeWidth = 14.0f;
            break;
        case 1:
            streakWidth = 8.5f;
            streakDuration = 0.3f;
            m_streakStrokeWidth = 8.5f;
            break;
        case 2:
            streakWidth = 10.0f;
            streakDuration = 0.4f;
            break;
        case 3:
            streakWidth = 5.0f;
            streakDuration = 0.6f;
            m_alwaysShowStreak = true;
            m_streakStrokeWidth = 5.0f;
            break;
        case 4:
            streakWidth = 3.0f;
            streakDuration = 1.0f;
            m_alwaysShowStreak = true;
            m_streakStrokeWidth = 3.0f;
            break;
        default:
            break;
    }

    m_regularTrail = cocos2d::CCMotionStreak::create(
        streakDuration,
        5.0f,
        streakWidth,
        &cocos2d::ccc3(255, 255, 255),
        frameCString);

    if (m_playerStreak == 6)
        m_regularTrail->enableRepeatMode(0.1f);

    m_regularTrail->m_fMaxSeg = 50.0f;
    m_parentLayer->addChild(m_regularTrail, -2);

    ShipStreak shipStreakID = (ShipStreak)(gameManager->m_playerShipFire.random - gameManager->m_playerShipFire.seed);
    m_shipStreakType = shipStreakID;

    if (shipStreakID <= ShipStreak::ShipFire6)
    {
        if (shipStreakID > ShipStreak::ShipFire1)
        {
            float streakStart = 0.0f;
            float streakEnd = 0.0f;

            getSettingsForStreak(shipStreakID, 1.6f, 1.0f, &streakStart, &streakEnd);

            cocos2d::CCTextureCache* textureCache = cocos2d::CCTextureCache::sharedTextureCache();
            std::string frameTexture = getFrameForStreak(shipStreakID, 0.0f);
            cocos2d::CCTexture2D* texture = cocos2d::CCTextureCache::addImage(textureCache, frameTexture.c_str(), false);

            m_shipStreak = cocos2d::CCMotionStreak::create(
                &cocos2d::ccc3(255, 255, 255),
                streakStart,
                1.0f,
                streakEnd,
                texture,
                nullptr);

            m_parentLayer->addChild(m_shipStreak);
            m_shipStreak->m_bDontOpacityFade = true;
            m_shipStreak->m_fMaxSeg = 50.0f;
            m_shipStreak->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        }
    }
    else
    {
        m_shipStreakType = ShipStreak::ShipFire1;
    }

    m_regularTrail->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

    m_waveTrail = HardStreak::create();
    m_parentLayer->addChild(m_waveTrail, -3);

    if (gameManager->m_playerColor.random - gameManager->m_playerColor.seed != 15 || m_switchWaveTrailColor)
    {
        cocos2d::ccBlendFunc blendFunc = {GL_SRC_ALPHA, GL_ONE};
        m_waveTrail->setBlendFunc(&blendFunc);
    }
    else
    {
        m_waveTrail->m_isSolid = true;
    }

    deactivateStreak(true);
}

void PlayerObject::spawnCircle()
{
    if (GameManager::sharedState()->m_performanceMode)
        return;

    CCCircleWave* wave = CCCircleWave::create(70.0, 2.0, 0.3, 1);
    wave->m_color = m_playerColor1;
    wave->setPosition(&getPosition());
    m_parentLayer->addChild(wave, 0);
    CCCircleWave::followObject(wave, this, true);

    PlayLayer* layer = GameManager::sharedState()->m_playLayer;
    wave->m_delegate = layer ? &layer->CCCircleWaveDelegate : nullptr;
    layer->addCircle(wave);
    wave->m_circleMode = CircleMode::Outline;
}

void PlayerObject::spawnCircle2()
{
    if (GameManager::sharedState()->m_performanceMode)
        return;

    CCCircleWave* wave = CCCircleWave::create(5.0, 50.0, 0.3, 0);
    wave->m_color = m_playerColor2;
    wave->setPosition(&getPosition());
    m_parentLayer->addChild(wave, 0);
}

void PlayerObject::spawnDualCircle()
{
    if (m_maybeReducedEffects)
        return;
    if (GameManager::sharedState()->m_playLayer->m_skipArtReload)
        return;
    if (GameManager::sharedState()->m_performanceMode)
        return;

    CCCircleWave* wave = CCCircleWave::create(50.0, 2.0, 0.25, 1);
    wave->m_color = m_playerColor1;
    wave->setPosition(&getPosition());
    m_parentLayer->addChild(wave, 0);
    CCCircleWave::followObject(wave, this, true);

    PlayLayer* layer = GameManager::sharedState()->m_playLayer;
    wave->m_delegate = layer ? &layer->CCCircleWaveDelegate : nullptr;
    layer->addCircle(wave);
}

void PlayerObject::spawnScaleCircle()
{
    if (m_maybeReducedEffects)
        return;
    if (GameManager::sharedState()->m_playLayer->m_skipArtReload)
        return;
    if (GameManager::sharedState()->m_performanceMode)
        return;

    float startRadius;
    float endRadius;
    float duration;
    int red;
    int green;
    int blue;
    bool outline;

    if (m_vehicleSize == 1.0f)
    {
        duration = 0.3f;
        red = -1;
        endRadius = 40.0f;
        green = 0;
        outline = false;
        startRadius = 10.0f;
        blue = -106;
    }
    else
    {
        startRadius = 50.0f;
        red = 0;
        green = -1;
        blue = -106;
        outline = true;
        duration = 0.25f;
        endRadius = 2.0f;
    }

    CCCircleWave* wave = CCCircleWave::create(startRadius, endRadius, duration, outline);
    wave->m_color.r = red;
    wave->m_color.g = green;
    wave->m_color.b = blue;
    wave->setPosition(&getPosition());
    m_parentLayer->addChild(wave, 0);
    CCCircleWave::followObject(wave, this, true);

    PlayLayer* layer = GameManager::sharedState()->m_playLayer;
    wave->m_delegate = layer ? &layer->CCCircleWaveDelegate : nullptr;
    layer->addCircle(wave);
}

void PlayerObject::specialGroundHit()
{
    int flip = flipMod();
    setYVelocity((double)(-5 * flip), 47);

    if (m_isBall || isFlying())
        m_maybeSpriteRelated = true;
    else
    {
        stopRotation(false, 21);
        m_maybeSpriteRelated = true;
    }
}

void PlayerObject::speedDown()
{
    m_speedMultiplier -= 0.00499999989;
    logValues();
}

void PlayerObject::speedUp()
{
    m_speedMultiplier += 0.00499999989;
    logValues();
}

void PlayerObject::spiderTestJump(bool dynamic)
{
    spiderTestJumpInternal(dynamic);

    if (m_gameLayer)
        gameEventTriggered(18, 0);

    if (!isInBasicMode())
        return;

    if (m_isSideways)
    {
        if (!m_isUpsideDown)
            setRotation(270.0f);
        else
            setRotation(90.0f);
    }
    else
    {
        if (m_isUpsideDown)
            setRotation(180.0f);
        else
            setRotation(0.0f);
    }
}

void PlayerObject::spiderTestJumpInternal(bool dynamic)
{
    if (m_isDead || m_isLocked || m_maybeReducedEffects)
        return;

    float collisionRadius = m_vehicleSize * 8.0f;
    cocos2d::CCRect baseRect;
    cocos2d::CCRect extendedRect;
    cocos2d::CCRect damagingRect;
    bool sidewaysAxis;
    bool verticalAxis;

    if (m_isSideways)
    {
        baseRect = getObjectRect();
        extendedRect = baseRect;

        sidewaysAxis = true;
        verticalAxis = false;
    }
    else if (!m_gameLayer->m_gameState.m_unkBool8 && !isInBasicMode())
    {
        baseRect = getObjectRect();
        extendedRect = baseRect;
        sidewaysAxis = false;
        verticalAxis = true;
    }
    else
    {
        baseRect = getObjectRect();
        extendedRect = baseRect;
        sidewaysAxis = false;
        verticalAxis = true;
    }

    float minBound;
    float maxBound;

    if (m_isSideways)
    {
        maxBound = getPosition().x + 3000.0f;
        minBound = 0.0f;
        if (getPosition().x - 3000.0f > 0.0f)
            minBound = getPosition().x - 3000.0f;

        if (m_isUpsideDown)
        {
            extendedRect.origin.x = minBound;
            extendedRect.size.width = (baseRect.origin.x + baseRect.size.width - 2.0f) - minBound + 2.0f;
        }
        else
        {
            float rightEdge = baseRect.origin.x + baseRect.size.width - 2.0f;
            extendedRect.origin.x = rightEdge;
            extendedRect.size.width = maxBound - rightEdge;
        }

        sidewaysAxis = true;
        verticalAxis = false;
    }
    else if (!m_gameLayer->m_gameState.m_unkBool8 && !isInBasicMode())
    {
        minBound = m_gameLayer->getMinPortalY();
        maxBound = m_gameLayer->getMaxPortalY();

        if (m_isUpsideDown)
        {
            extendedRect.origin.y = minBound;
            extendedRect.size.height = (baseRect.origin.y + baseRect.size.height - 2.0f) - minBound + 2.0f;
        }
        else
        {
            float topEdge = baseRect.origin.y + baseRect.size.height - 2.0f;
            extendedRect.origin.y = topEdge;
            extendedRect.size.height = maxBound - topEdge;
        }

        sidewaysAxis = false;
        verticalAxis = true;
    }
    else
    {
        minBound = 90.0f;
        maxBound = getPosition().y + 3000.0f;
        if (getPosition().y - 3000.0f > 90.0f)
            minBound = getPosition().y - 3000.0f;

        if (m_isUpsideDown)
        {
            extendedRect.origin.y = minBound;
            extendedRect.size.height = (baseRect.origin.y + baseRect.size.height - 2.0f) - minBound + 2.0f;
        }
        else
        {
            float topEdge = baseRect.origin.y + baseRect.size.height - 2.0f;
            extendedRect.origin.y = topEdge;
            extendedRect.size.height = maxBound - topEdge;
        }

        sidewaysAxis = false;
        verticalAxis = true;
    }

    damagingRect = extendedRect;

    if (sidewaysAxis)
    {
        damagingRect.origin.y = baseRect.origin.y;
        damagingRect.size.height = baseRect.size.height;
    }
    else
    {
        damagingRect.origin.x = baseRect.origin.x;
        damagingRect.size.width = baseRect.size.width;
    }

    cocos2d::CCRect probeRect = baseRect;
    if (m_isSideways)
    {
        probeRect.size.height = collisionRadius;
        probeRect.origin.y = baseRect.origin.y + (baseRect.size.height - collisionRadius) * 0.5f;

        if (m_isPlatformer)
        {
            extendedRect.size.height -= 2.0f;
            extendedRect.origin.y += 1.0f;
        }
        else
        {
            extendedRect.size.height = baseRect.size.height * 0.5f + 1.0f;
            extendedRect.origin.y = m_isGoingLeft
                ? (baseRect.origin.y - baseRect.size.height * 0.5f) - 1.0f
                : baseRect.origin.y + baseRect.size.height * 0.5f;
        }
    }
    else
    {
        probeRect.size.width = collisionRadius;
        probeRect.origin.x = baseRect.origin.x + (baseRect.size.width - collisionRadius) * 0.5f;

        if (m_isPlatformer)
        {
            extendedRect.size.width -= 2.0f;
            extendedRect.origin.x += 1.0f;
        }
        else
        {
            extendedRect.size.width = baseRect.size.width * 0.5f + 1.0f;
            extendedRect.origin.x = m_isGoingLeft
                ? (baseRect.origin.x - baseRect.size.width * 0.5f) - 1.0f
                : baseRect.origin.x + baseRect.size.width * 0.5f;
        }
    }

    cocos2d::CCArray* staticObjects = m_gameLayer->staticObjectsInRect(extendedRect);
    cocos2d::CCArray* damagingObjects = m_gameLayer->damagingObjectsInRect(damagingRect);

    bool dynamicProbe = dynamic;

    if ((!staticObjects->count() && damagingObjects->count()) || dynamicProbe)
    {
        dynamicProbe = true;
        staticObjects = m_gameLayer->staticObjectsInRect(damagingRect);
    }

    if (staticObjects->count())
        qsort(staticObjects->data->arr, staticObjects->data->num, 8, boxComp);
    if (damagingObjects->count())
        qsort(damagingObjects->data->arr, damagingObjects->data->num, 8, boxComp);

    float targetCoordinate = 0.0f;
    bool hasStaticTarget = false;
    bool hasDamagingTarget = false;
    bool hasSlopeTarget = false;
    bool hasNonSlopeTarget = false;

    if (staticObjects->count())
    {
        cocos2d::CCPoint playerPosition = getPosition();

        for (unsigned int i = 0; i < (unsigned int)staticObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)staticObjects->objectAtIndex(i);
            cocos2d::CCRect objRect = obj->getObjectRect();

            float objCoord;
            if (verticalAxis)
            {
                objCoord = (m_isUpsideDown
                    ? objRect.getMaxY()
                    : objRect.getMinY())
                    + (m_isUpsideDown ? 1.0f : -1.0f) * (baseRect.size.height * 0.5f);
            }
            else
            {
                objCoord = (m_isUpsideDown
                    ? objRect.getMaxX()
                    : objRect.getMinX())
                    + (m_isUpsideDown ? 1.0f : -1.0f) * (baseRect.size.width * 0.5f);
            }

            bool passesAxisCheck = verticalAxis
                ? (m_isUpsideDown ? (objCoord < playerPosition.y + 10.0f) : (objCoord > playerPosition.y - 10.0f))
                : (m_isUpsideDown ? (objCoord < playerPosition.x + 10.0f) : (objCoord > playerPosition.x - 10.0f));

            if (!passesAxisCheck)
                continue;

            bool horizontalOverlap = m_isUpsideDown
                ? objRect.getMinY() <= playerPosition.y
                : objRect.getMaxY() >= playerPosition.y;

            bool verticalOverlap = m_isUpsideDown
                ? objRect.getMinX() <= playerPosition.x
                : objRect.getMaxX() >= playerPosition.x;

            bool overlaps = verticalAxis ? horizontalOverlap : verticalOverlap;
            if (!overlaps)
                continue;

            hasStaticTarget = true;
            targetCoordinate = objCoord;
            break;
        }
    }

    if (!hasStaticTarget)
    {
        if (damagingObjects->count())
        {
            GameObject* obj = (GameObject*)damagingObjects->objectAtIndex(0);
            cocos2d::CCRect objRect = obj->getObjectRect();

            if (verticalAxis)
                targetCoordinate = (m_isUpsideDown ? objRect.origin.y : objRect.origin.y + objRect.size.height)
                    + (m_isUpsideDown ? baseRect.size.height : -baseRect.size.height) * 0.5f;
            else
                targetCoordinate = (m_isUpsideDown ? objRect.origin.x : objRect.origin.x + objRect.size.width)
                    + (m_isUpsideDown ? baseRect.size.width : -baseRect.size.width) * 0.5f;

            hasDamagingTarget = true;
        }

        if (!hasDamagingTarget)
        {
            if (m_isUpsideDown)
            {
                float size = verticalAxis ? baseRect.size.height : baseRect.size.width;
                targetCoordinate = minBound + size * 0.5f;
            }
            else
            {
                float size = verticalAxis ? baseRect.size.height : baseRect.size.width;
                targetCoordinate = maxBound - size * 0.5f;
            }
        }
    }

    if (damagingObjects->count())
    {
        GameObject* obj = (GameObject*)damagingObjects->objectAtIndex(0);
        cocos2d::CCRect objRect = obj->getObjectRect();

        float damagingCoord;
        if (verticalAxis)
            damagingCoord = (m_isUpsideDown ? objRect.origin.y : objRect.origin.y + objRect.size.height)
                + (m_isUpsideDown ? baseRect.size.height : -baseRect.size.height) * 0.5f;
        else
            damagingCoord = (m_isUpsideDown ? objRect.origin.x : objRect.origin.x + objRect.size.width)
                + (m_isUpsideDown ? baseRect.size.width : -baseRect.size.width) * 0.5f;

        bool takeDamagingCoord = m_isUpsideDown
            ? targetCoordinate < damagingCoord
            : targetCoordinate > damagingCoord;

        if (takeDamagingCoord)
        {
            targetCoordinate = damagingCoord;

            if (!dynamicProbe)
            {
                spiderTestJump(true);
                return;
            }
        }
    }

    if (!hasStaticTarget && !hasDamagingTarget)
        return;

    cocos2d::CCPoint playerPos = getPosition();
    cocos2d::CCPoint newPos = verticalAxis
        ? cocos2d::CCPoint(playerPos.x, targetCoordinate)
        : cocos2d::CCPoint(targetCoordinate, playerPos.y);
    setPosition(&newPos);

    flipGravity(!m_isUpsideDown, true);
    int flip = flipMod();
    setYVelocity((double)-flip, 36);
    m_isOnGround3 = false;
    m_jumpBuffered = 0;
    resetCollisionLog(true);
    m_lastSpiderFlipTime = m_totalTime;

    if (verticalAxis)
        m_position.y = targetCoordinate;
    else
        m_position.x = targetCoordinate;

    if (m_playEffects && !m_isHidden)
    {
        m_position.x += reverseMod() * 7.5f;
        m_position.y += reverseMod() * 7.5f;
    }

    if (m_gameLayer)
        m_gameLayer->checkCameraLimitAfterTeleport(this, 60.0f);
}

void PlayerObject::spiderTestJumpX(bool dynamic)
{
    if (m_isDead || m_isLocked || m_maybeReducedEffects)
        return;

    cocos2d::CCRect baseRect = getObjectRect();
    float baseX = getPosition().x;

    float frontBound;
    if (m_isUpsideDown)
        frontBound = baseRect.origin.x + baseRect.size.width - 2.0f;
    else
        frontBound = baseRect.origin.x - 2998.0f;

    cocos2d::CCRect extendedRect(baseRect);
    extendedRect.origin.x = frontBound;
    extendedRect.size.width = 3000.0f;

    cocos2d::CCRect verticalBand(extendedRect);
    verticalBand.size.height = baseRect.size.height;
    verticalBand.origin.y = baseRect.origin.y;

    cocos2d::CCRect probeRect(verticalBand);
    probeRect.size.height = m_vehicleSize * 8.0f;
    probeRect.origin.y = verticalBand.origin.y + (verticalBand.size.height - probeRect.size.height) * 0.5f;

    extendedRect.size.width = baseRect.size.width * 0.5f + 1.0f;
    extendedRect.origin.x = baseRect.origin.x + baseRect.size.width * 0.5f;

    verticalBand.size.width = extendedRect.size.width;
    verticalBand.origin.x = extendedRect.origin.x;

    cocos2d::CCArray* staticObjects = m_gameLayer->staticObjectsInRect(extendedRect);
    cocos2d::CCArray* damagingObjects = m_gameLayer->damagingObjectsInRect(probeRect);

    bool dynamicProbe = dynamic;

    if ((!staticObjects->count() && damagingObjects->count()) || dynamicProbe)
    {
        dynamicProbe = true;
        staticObjects = m_gameLayer->staticObjectsInRect(verticalBand);
    }

    int boxSort = m_isUpsideDown ? boxCompMinX : boxCompMaxX;
    if (staticObjects->count())
        qsort(staticObjects->data->arr, staticObjects->data->num, 8, boxSort);
    if (damagingObjects->count())
        qsort(damagingObjects->data->arr, damagingObjects->data->num, 8, boxSort);

    cocos2d::CCPoint playerPos = getPosition();
    float targetX = 0.0f;
    bool foundTarget = false;

    if (staticObjects->count())
    {
        for (unsigned int i = 0; i < (unsigned int)staticObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)staticObjects->objectAtIndex(i);
            cocos2d::CCRect objRect = obj->getObjectRect();

            float candidateX = m_isUpsideDown
                ? objRect.getMinX() - baseRect.size.width * 0.5f
                : objRect.getMaxX() + baseRect.size.width * 0.5f;

            if (m_isUpsideDown)
            {
                if (candidateX >= playerPos.x - 10.0f)
                    continue;
            }
            else
            {
                if (candidateX <= playerPos.x + 10.0f)
                    continue;
            }

            bool overlapsVertically = m_isUpsideDown
                ? objRect.getMinY() <= playerPos.y
                : objRect.getMaxY() >= playerPos.y;

            if (!overlapsVertically)
                continue;

            targetX = candidateX;
            foundTarget = true;
            break;
        }
    }

    if (!foundTarget)
    {
        if (damagingObjects->count())
        {
            GameObject* obj = (GameObject*)damagingObjects->objectAtIndex(0);
            cocos2d::CCRect objRect = obj->getObjectRect();

            targetX = m_isUpsideDown
                ? objRect.getMinX() - baseRect.size.width * 0.5f
                : objRect.getMaxX() + baseRect.size.width * 0.5f;

            foundTarget = true;
        }
        else
        {
            targetX = m_isUpsideDown
                ? baseX + 3000.0f - baseRect.size.width * 0.5f
                : baseX - 3000.0f + baseRect.size.width * 0.5f;
            foundTarget = true;
        }
    }

    if (damagingObjects->count())
    {
        GameObject* obj = (GameObject*)damagingObjects->objectAtIndex(0);
        cocos2d::CCRect objRect = obj->getObjectRect();
        float damagingX = m_isUpsideDown
            ? objRect.getMinX() - baseRect.size.width * 0.5f
            : objRect.getMaxX() + baseRect.size.width * 0.5f;

        bool takeDamaging = m_isUpsideDown ? targetX <= damagingX : targetX >= damagingX;
        if (takeDamaging)
        {
            targetX = damagingX;

            if (!dynamicProbe)
            {
                spiderTestJump(true);
                return;
            }
        }
    }

    if (!foundTarget)
        return;

    cocos2d::CCPoint newPos(targetX, playerPos.y);
    setPosition(&newPos);

    flipGravity(!m_isUpsideDown, true);
    int flip = flipMod();
    setYVelocity((double)-flip, 35);
    m_isOnGround3 = false;
    m_jumpBuffered = 0;
    resetCollisionLog(true);
    m_lastSpiderFlipTime = m_totalTime;
    m_position.x = targetX;

    if (m_playEffects && !m_isHidden)
    {
        float offset = reverseMod() * 7.5f;
        playerPos.x += offset;
        m_position.x += offset;
    }

    if (m_gameLayer)
        m_gameLayer->checkCameraLimitAfterTeleport(this, 60.0f);
}

void PlayerObject::spiderTestJumpY(bool dynamic)
{
    if (m_isDead || m_isLocked || m_maybeReducedEffects)
        return;

    float minY = m_gameLayer->getMinPortalY();
    float maxY = m_gameLayer->getMaxPortalY();

    if (m_gameLayer->m_gameState.m_unkBool8 || isInBasicMode())
    {
        maxY = 2490.0f;
        minY = 90.0f;
    }

    cocos2d::CCRect baseRect = getObjectRect();

    float extendedOrigin;
    float extendedSize;
    if (m_isUpsideDown)
    {
        extendedOrigin = minY;
        extendedSize = (baseRect.origin.y + baseRect.size.height - 2.0f) - minY + 2.0f;
    }
    else
    {
        extendedOrigin = baseRect.origin.y + baseRect.size.height - 2.0f;
        extendedSize = maxY - extendedOrigin;
    }

    cocos2d::CCRect extendedRect(baseRect);
    extendedRect.origin.y = extendedOrigin;
    extendedRect.size.height = extendedSize;

    cocos2d::CCRect probeRect(extendedRect);
    probeRect.size.width = baseRect.size.width;
    probeRect.origin.x = baseRect.origin.x;

    extendedRect.size.width = baseRect.size.width * 0.5f + 1.0f;
    extendedRect.origin.x = baseRect.origin.x + baseRect.size.width * 0.5f;

    probeRect.size.width = extendedRect.size.width;
    probeRect.origin.x = extendedRect.origin.x;

    cocos2d::CCArray* staticObjects = m_gameLayer->staticObjectsInRect(extendedRect);
    cocos2d::CCArray* damagingObjects = m_gameLayer->damagingObjectsInRect(probeRect);

    bool dynamicProbe = dynamic;

    if ((!staticObjects->count() && damagingObjects->count()) || dynamicProbe)
    {
        dynamicProbe = true;
        staticObjects = m_gameLayer->staticObjectsInRect(probeRect);
    }

    int boxSort = m_isUpsideDown ? boxCompMax : boxCompMin;
    if (staticObjects->count())
        qsort(staticObjects->data->arr, staticObjects->data->num, 8, boxSort);
    if (damagingObjects->count())
        qsort(damagingObjects->data->arr, damagingObjects->data->num, 8, boxSort);

    cocos2d::CCPoint playerPos = getPosition();
    float targetY = 0.0f;
    bool foundTarget = false;

    if (staticObjects->count())
    {
        for (unsigned int i = 0; i < (unsigned int)staticObjects->count(); i++)
        {
            GameObject* obj = (GameObject*)staticObjects->objectAtIndex(i);
            cocos2d::CCRect objRect = obj->getObjectRect();

            float candidateY = m_isUpsideDown
                ? objRect.getMaxY() + baseRect.size.height * 0.5f
                : objRect.getMinY() - baseRect.size.height * 0.5f;

            if (m_isUpsideDown)
            {
                if (candidateY > playerPos.y + 10.0f)
                    continue;
            }
            else
            {
                if (candidateY < playerPos.y - 10.0f)
                    continue;
            }

            bool overlapsHorizontally = objRect.getMinX() <= playerPos.x;
            if (!overlapsHorizontally)
                continue;

            targetY = candidateY;
            foundTarget = true;
            break;
        }
    }

    if (!foundTarget)
    {
        if (damagingObjects->count())
        {
            GameObject* obj = (GameObject*)damagingObjects->objectAtIndex(0);
            cocos2d::CCRect objRect = obj->getObjectRect();

            targetY = m_isUpsideDown
                ? objRect.getMaxY() + baseRect.size.height * 0.5f
                : objRect.getMinY() - baseRect.size.height * 0.5f;

            foundTarget = true;
        }
        else
        {
            targetY = m_isUpsideDown
                ? minY + baseRect.size.height * 0.5f
                : maxY - baseRect.size.height * 0.5f;
            foundTarget = true;
        }
    }

    if (damagingObjects->count())
    {
        GameObject* obj = (GameObject*)damagingObjects->objectAtIndex(0);
        cocos2d::CCRect objRect = obj->getObjectRect();
        float damagingY = m_isUpsideDown
            ? objRect.getMaxY() + baseRect.size.height * 0.5f
            : objRect.getMinY() - baseRect.size.height * 0.5f;

        bool takeDamaging = m_isUpsideDown ? targetY >= damagingY : targetY <= damagingY;
        if (takeDamaging)
        {
            targetY = damagingY;

            if (!dynamicProbe)
            {
                spiderTestJump(true);
                return;
            }
        }
    }

    if (!foundTarget)
        return;

    cocos2d::CCPoint newPos(playerPos.x, targetY);
    setPosition(&newPos);

    flipGravity(!m_isUpsideDown, true);
    int flip = flipMod();
    setYVelocity((double)-flip, 36);
    m_isOnGround3 = false;
    m_jumpBuffered = 0;
    resetCollisionLog(true);
    m_lastSpiderFlipTime = m_totalTime;
    m_position.y = targetY;

    if (m_playEffects && !m_isHidden)
    {
        float offset = reverseMod() * 7.5f;
        playerPos.x += offset;
        m_position.x += offset;
    }

    if (m_gameLayer)
        m_gameLayer->checkCameraLimitAfterTeleport(this, 60.0f);
}

void PlayerObject::startDashing(DashRingObject* object)
{
    if (m_isDashing)
        return;

    m_isDashing = true;
    m_lastLandTime = 0.0;

    stopRotation(false, 3);

    if (m_isPlatformer && isInNormalMode())
        stopPlatformerJumpAnimation();

    if (!m_isDart)
        deactivateStreak(false);

    m_dashFireSprite->setVisible(true);

    float dashAngle = m_dashAngle;

    if (!object)
    {
        m_hasNoEffects = false;
    }
    else
    {
        m_dashRing = object;
        m_hasNoEffects = object->m_hasNoEffects;
        dashAngle = -object->m_startRotationX;
        m_dashStartTime = m_totalTime;

        float objectRotation = object->getObjectRotation();
        if (object->isFlipX())
            objectRotation += 180.0f;

        float rotX = object->getRotationX();
        if (fabsf(rotX - object->getRotationY()) > 179.0f)
            objectRotation += 180.0f;

        dashAngle = fmod(-objectRotation, 360.0f);
        snapRotation360(&dashAngle);

        if (!m_isPlatformer)
            limitDashRotation(&dashAngle);

        cocos2d::CCPoint dashVec = cocos2d::ccpForAngle(dashAngle * 0.017453f);

        if (m_isPlatformer)
        {
            dashVec *= object->m_dashSpeed * 5.77f;

            if (m_isSideways)
                std::swap(dashVec.x, dashVec.y);

            if (dashVec.x < 0.0f)
                doReversePlayer(true);
            else if (dashVec.x > 0.0f)
                doReversePlayer(false);

            m_dashX = dashVec.x;
            m_dashY = dashVec.y;
        }
        else
        {
            dashVec *= 1.0f;

            if (m_isSideways)
                std::swap(dashVec.x, dashVec.y);

            m_dashX = fabsf(dashVec.y / fabsf(dashVec.x));
            m_dashY = dashVec.y / fabsf(dashVec.x);
        }

        m_dashAngle = dashAngle;
    }

    updateDashArt();

    m_dashFireSprite->stopAllActions();

    float currentScaleX = m_dashFireSprite->getScaleX();
    m_dashFireSprite->getScaleY();
    m_dashFireSprite->setScaleX(0.3f);
    m_dashFireSprite->setScaleY(0.2f);

    float targetScale = currentScaleX * 1.1f;

    cocos2d::CCScaleTo* grow1 = cocos2d::CCScaleTo::create(0.1f, targetScale * 0.9f);
    cocos2d::CCEaseInOut* ease1 = cocos2d::CCEaseInOut::create(grow1, 2.0f);
    cocos2d::CCScaleTo* grow2 = cocos2d::CCScaleTo::create(0.1f, targetScale);
    cocos2d::CCEaseInOut* ease2 = cocos2d::CCEaseInOut::create(grow2, 2.0f);

    cocos2d::CCAction* scaleSequence = cocos2d::CCSequence::create(ease1, ease2, nullptr);
    m_dashFireSprite->runAction(scaleSequence);

    m_dashParticles->resumeSystem();

    cocos2d::ccColor3B fireColor = m_switchDashFireColor ? m_playerColor1 : m_playerColor2;

    if (object && m_playEffects > m_hasNoEffects)
    {
        cocos2d::CCSprite* boomSprite = cocos2d::CCSprite::createWithSpriteFrameName("playerDash2_boom2_001.png");
        GameManager::sharedState()->m_playLayer->m_objectLayer->addChild(boomSprite, 40);
        boomSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        boomSprite->setRotation(-dashAngle);
        boomSprite->setScale(1.2f);
        boomSprite->setPosition(&object->getRealPosition());

        cocos2d::CCArray* frames = cocos2d::CCArray::create();
        for (int i = 1; i < 10; i++)
        {
            cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("playerDash2_boom2_%03d.png", i);
            cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
            if (frame)
                frames->addObject(frame);
        }

        cocos2d::CCAnimation* animation = cocos2d::CCAnimation::createWithSpriteFrames(frames, 0.05f);
        cocos2d::CCSequence* animateSequence = cocos2d::CCSequence::create(
            cocos2d::CCAnimate::create(animation),
            cocos2d::CCCallFunc::create(boomSprite, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        boomSprite->runAction(animateSequence);

        cocos2d::CCSequence* fadeSequence = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.2f),
            cocos2d::CCFadeOut::create(0.3f),
            nullptr);
        boomSprite->runAction(fadeSequence);

        int red = (object->m_objectID == 1704) ? -1 : 0;
        int green = (object->m_objectID == 1704) ? 0 : -1;
        int blue = (object->m_objectID == 1704) ? -1 : 0;

        cocos2d::CCTintTo* tint = cocos2d::CCTintTo::create(0.3f, red, green, blue);
        boomSprite->runAction(tint);

        flashPlayer(0.2f, 0.1f, cocos2d::ccc3(255, 255, 255), cocos2d::ccc3(255, 255, 255));

        m_dashFireSprite->setColor(&cocos2d::ccc3(255, 255, 255));

        cocos2d::CCSequence* recolorSequence = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.1f),
            cocos2d::CCTintTo::create(0.2f, fireColor.r, fireColor.g, fireColor.b),
            nullptr);
        m_dashFireSprite->runAction(recolorSequence);
    }

    m_lastGroundedPos = cocos2d::CCPoint(0.0f, 0.0f);

    if (m_gameLayer)
        gameEventTriggered(23, 0);
}

void PlayerObject::stopBurstEffect()
{
    m_ufoClickParticles->stopSystem();
}

void PlayerObject::stopDashing()
{
    if (!m_isDashing)
        return;

    m_isDashing = false;
    m_lastLandTime = 0.0;

    if (m_isPlatformer && m_dashRing)
    {
        float endBoost = m_dashRing->m_endBoost;
        cocos2d::CCPoint boostVec(endBoost * m_dashX, endBoost * m_dashY);
        updatePlayerForce(boostVec);

        if (m_dashRing->m_stopSlide)
            handlePlayerCommand(543);
    }

    m_dashRing = nullptr;
    m_dashFireSprite->setVisible(false);

    if (m_playEffects && !m_maybeReducedEffects)
    {
        cocos2d::CCSprite* dashSprite = cocos2d::CCSprite::createWithSpriteFrameName("playerDash2_001.png");
        GameManager::sharedState()->m_playLayer->m_objectLayer->addChild(dashSprite, 40);
        dashSprite->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

        cocos2d::CCSprite* outlineSprite = cocos2d::CCSprite::createWithSpriteFrameName("playerDash2_outline_001.png");
        dashSprite->addChild(outlineSprite, 1);

        cocos2d::CCSize dashSize = dashSprite->getScaledContentSize();
        outlineSprite->setPosition(cocos2d::CCPoint(dashSize.width * 0.5f, dashSize.height * 0.5f));
        outlineSprite->setOpacity(246);

        dashSprite->setPosition(getPosition() + m_dashFireSprite->getPosition());
        dashSprite->setScaleX(m_dashFireSprite->getScaleX() * m_vehicleSize);
        dashSprite->setScaleY(m_dashFireSprite->getScaleY() * m_vehicleSize);
        dashSprite->setColor(m_dashFireSprite->getColor());
        dashSprite->setRotation(-m_dashAngle);

        float startScale = dashSprite->getScaleX();
        dashSprite->getScaleY();

        cocos2d::CCSequence* scaleSequence = cocos2d::CCSequence::create(
            cocos2d::CCScaleTo::create(0.2f, startScale * 0.2f),
            cocos2d::CCCallFunc::create(dashSprite, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        dashSprite->runAction(scaleSequence);

        cocos2d::CCMoveBy* move = cocos2d::CCMoveBy::create(0.2f, cocos2d::CCPoint(-20.0f, 0.0f));
        dashSprite->runAction(move);

        cocos2d::CCFadeTo* fade = cocos2d::CCFadeTo::create(0.2f, 0);
        dashSprite->runAction(fade);

        cocos2d::CCFadeTo* outlineFade = cocos2d::CCFadeTo::create(0.2f, 0);
        outlineSprite->runAction(outlineFade);
    }

    m_dashParticles->stopSystem();

    if (!isFlying() && !m_isRobot && !m_isSpider)
    {
        float currentRotation = m_iconSprite->getRotation();
        m_iconSprite->setScale(1.0f);
        m_iconSprite->stopAllActions();
        m_iconSprite->setRotation(0.0f);

        m_iconGlow->setScale(1.0f);
        m_iconGlow->stopAllActions();
        m_iconGlow->setRotation(0.0f);

        setRotation(currentRotation);

        if (m_isPlatformer)
        {
            float speed = sqrtf(m_dashY * m_dashY + m_dashX * m_dashX);
            float rotationMultiplier = (speed > 17.31f)
                ? 2.0f
                : (speed / 17.31f) * 1.5f + 0.5f;

            runNormalRotation(true, rotationMultiplier);
        }
    }

    if (m_isBall)
    {
        if (!m_isPlatformer)
        {
            runBallRotation(1.0f);
        }
        else
        {
            stopRotation(false, 5);
            if (m_isBall)
                runBallRotation(1.0f);
        }
    }

    if (m_isOnGround2)
    {
        if (m_isRobot)
        {
            m_robotSprite->runAnimation(m_currentRobotAnimation);
        }
        else if (m_isSpider)
        {
            playDynamicSpiderRun();
        }
    }

    if (m_gameLayer)
        gameEventTriggered(24, 0);
}

void PlayerObject::stopParticles()
{
    deactivateParticle();
    m_trailingParticles->stopSystem();
    m_shipClickParticles->stopSystem();
    m_vehicleGroundParticles->stopSystem();
    m_robotBurstParticles->stopSystem();
}

void PlayerObject::stopPlatformerJumpAnimation()
{
    if (!m_isPlatformer)
        return;

    m_iconSprite->stopActionByTag(13);
    m_iconSprite->stopActionByTag(14);
    m_iconSprite->setScale(1.0f);

    m_iconGlow->stopActionByTag(13);
    m_iconGlow->stopActionByTag(14);
    m_iconGlow->setScale(1.0f);
}

void PlayerObject::stopRotation(bool ground, int type)
{
    m_isRotating = false;
    m_isBallRotating2 = false;
    m_isBallRotating = false;
    m_rotationSpeed = 0.0f;
}

void PlayerObject::stopStreak2()
{
    m_waveTrail->stopActionByTag(8);
    m_waveTrail->setOpacity(0);
    m_waveTrail->stopStroke();
}

void PlayerObject::storeCollision(PlayerCollisionDirection direction, int id)
{
    switch (direction)
    {
        case PlayerCollisionDirection::Top:
            if (m_lastCollisionTop != id)
            {
                m_lastCollisionTop = id;
                if (m_collisionLogTop)
                    m_collisionLogTop->setObject(m_maybeLastGroundObject, id);
            }
            break;
        case PlayerCollisionDirection::Bottom:
            if (m_lastCollisionBottom != id)
            {
                m_collisionLogTop = m_collisionLogBottom;
                m_lastCollisionBottom = id;
                if (m_collisionLogTop)
                    m_collisionLogTop->setObject(m_maybeLastGroundObject, id);
            }
            break;
        case PlayerCollisionDirection::Left:
            if (m_lastCollisionLeft != id)
            {
                m_collisionLogTop = m_collisionLogLeft;
                m_lastCollisionLeft = id;
                if (m_collisionLogTop)
                    m_collisionLogTop->setObject(m_maybeLastGroundObject, id);
            }
            break;
        case PlayerCollisionDirection::Right:
            if (m_lastCollisionRight != id)
            {
                m_collisionLogTop = m_collisionLogRight;
                m_lastCollisionRight = id;
                if (m_collisionLogTop)
                    m_collisionLogTop->setObject(m_maybeLastGroundObject, id);
            }
            break;
        default:
            break;
    }
}

bool PlayerObject::switchedDirTo(PlayerButton button)
{
    if (!m_inputsLocked)
    {
        m_holdingButtons[button] = (button == (PlayerButton)3);
    }

    if (m_controlsDisabled)
        return false;

    if (button == (PlayerButton)3)
    {
        m_leftPressedFirst = false;
        m_holdingRight = 1;

        if (m_isSliding && m_isSlidingRight)
        {
            m_isSliding = false;
            m_maybeSlopeForce = 0.0f;
        }

        if (m_platformerXVelocity < 0.0)
        {
            m_isMoving = false;
            return true;
        }

        return true;
    }

    m_holdingLeft = true;
    m_leftPressedFirst = true;

    if (m_isSliding && !m_isSlidingRight)
    {
        m_isSliding = false;
        m_maybeSlopeForce = 0.0f;
    }

    if (m_platformerXVelocity > 0.0)
    {
        m_isMoving = false;
        return true;
    }

    return true;
}

void PlayerObject::switchedToMode(GameObjectType type)
{
    if (type == GameObjectType::ShipPortal)
    {
        toggleBirdMode(false, false);
        toggleRollMode(false, false);
        toggleDartMode(false, false);
        toggleRobotMode(false, false);
        toggleSpiderMode(false, false);
        toggleSwingMode(false, false);
        return;
    }

    toggleFlyMode(false, false);

    if (type == GameObjectType::UfoPortal)
    {
        toggleRollMode(false, false);
        toggleDartMode(false, false);
        toggleRobotMode(false, false);
        toggleSpiderMode(false, false);
        toggleSwingMode(false, false);
        return;
    }

    toggleBirdMode(false, false);

    if (type == GameObjectType::BallPortal)
    {
        toggleDartMode(false, false);
        toggleRobotMode(false, false);
        toggleSpiderMode(false, false);
        toggleSwingMode(false, false);
        return;
    }

    toggleRollMode(false, false);

    if (type == GameObjectType::WavePortal)
    {
        toggleRobotMode(false, false);
        toggleSpiderMode(false, false);
        toggleSwingMode(false, false);
        return;
    }

    toggleDartMode(false, false);

    if (type == GameObjectType::RobotPortal)
    {
        toggleSpiderMode(false, false);
        toggleSwingMode(false, false);
        return;
    }

    toggleRobotMode(false, false);

    if (type == GameObjectType::SpiderPortal)
    {
        toggleSwingMode(false, false);
        return;
    }

    toggleSpiderMode(false, false);

    if (type != GameObjectType::SwingPortal)
        toggleSwingMode(false, false);
}

bool PlayerObject::testForMoving(float dt, GameObject* object)
{
    cocos2d::CCPoint lastPos = object->getLastPosition();
    cocos2d::CCPoint realPos = object->getRealPosition();

    bool objectMoved = realPos.x != lastPos.x || realPos.y != lastPos.y;

    if (!objectMoved)
        return false;

    bool axisDirection;
    if (m_isUpsideDown)
        axisDirection = realPos.y < lastPos.y;
    else
        axisDirection = realPos.y > lastPos.y;

    if (!axisDirection)
        return false;

    cocos2d::CCRect objRect = object->getObjectRect();
    objRect.size.height += dt * 5.0f;
    if (m_isUpsideDown)
        objRect.origin.y -= dt * 5.0f;

    cocos2d::CCRect playerRect = getObjectRect();
    bool intersects = playerRect.intersectsRect(objRect);

    if (intersects)
        m_collidedObject = object;

    return intersects;
}

void PlayerObject::toggleBirdMode(bool enable, bool noEffects)
{
    if (m_isBird == enable)
        return;

    m_isBird = enable;
    m_gameModeChangedTime = m_totalTime;

    if (enable)
        switchedToMode(GameObjectType::UfoPortal);

    stopRotation(false, 8);
    m_yVelocity *= 0.5;
    setRotation(0.0f);
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = false;
    removePendingCheckpoint();

    if (!m_isBird)
    {
        resetPlayerIcon();

        if (enable)
            modeDidChange();
        return;
    }

    GameManager* gameManager = GameManager::sharedState();
    updatePlayerBirdFrame(gameManager->m_playerBird.random - gameManager->m_playerBird.seed);
    stopPlatformerJumpAnimation();

    m_iconSprite->setScale(0.55f);
    m_iconSprite->setPosition(&cocos2d::CCPoint(0.0f, 5.0f));
    m_vehicleSprite->setVisible(true);
    m_vehicleSprite->setPosition(&cocos2d::CCPoint(0.0f, -7.0f));

    updatePlayerGlow();

    cocos2d::ccColor4F trailColor(
        m_playerColor2.r / 255.0f,
        m_playerColor2.g / 255.0f,
        m_playerColor2.b / 255.0f,
        1.0f);
    m_trailingParticles->setStartColor(trailColor);
    m_trailingParticles->setEndColor(trailColor);

    if (m_isHidden)
    {
        deactivateParticle();

        if (!noEffects)
        {
            spawnPortalCircle(cocos2d::ccc3(0, 200, 255), 50.0f);
        }
    }
    else
    {
        m_trailingParticles->resetSystem();
        deactivateParticle();

        if (!noEffects)
            spawnPortalCircle(cocos2d::ccc3(0, 200, 255), 50.0f);
    }

    activateStreak();
    updatePlayerScale();
    m_birdVehicle->setVisible(true);

    if (enable)
        modeDidChange();
}

void PlayerObject::toggleDartMode(bool enable, bool noEffects)
{
    if (m_isDart == enable)
        return;

    m_isDart = enable;
    m_gameModeChangedTime = m_totalTime;

    if (enable)
        switchedToMode(GameObjectType::WavePortal);

    stopRotation(false, 10);
    m_yVelocity *= 0.5;
    setRotation(0.0f);
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = false;
    removePendingCheckpoint();

    if (m_isDart)
    {
        m_width = 10.0f;
        m_unkAngle1 = 20.0f;
        m_height = 10.0f;

        GameManager* gameManager = GameManager::sharedState();
        updatePlayerDartFrame(gameManager->m_playerDart.random - gameManager->m_playerDart.seed);

        if (!noEffects)
            spawnPortalCircle(cocos2d::ccc3(0, 200, 255), 50.0f);

        activateStreak();
        updatePlayerScale();
        deactivateParticle();

        if (m_maybeIsVehicleGlowing)
            m_regularTrail->setColor(&cocos2d::ccc3(255, 255, 255));
        else
            m_regularTrail->setColor(&cocos2d::ccc3(255, 255, 255));

        m_waveTrail->reset();
        placeStreakPoint();

        if (m_playEffects
            && !m_maybeReducedEffects
            && !GameManager::sharedState()->m_playLayer->m_skipArtReload
            && !noEffects)
        {
            CCCircleWave* wave = CCCircleWave::create(10.0, 60.0, 0.4, 0);
            wave->m_color = m_playerColor1;
            wave->setPosition(&m_lastPortalPos);
            wave->m_circleMode = CircleMode::Outline;
            wave->m_lineWidth = 4;
            m_parentLayer->addChild(wave, 0);
        }
    }
    else
    {
        if (m_vehicleSize != 1.0f && m_defaultMiniIcon)
            updatePlayerFrame(0);
        else
            updatePlayerFrame(m_maybeSavedPlayerFrame);

        resetPlayerIcon();
    }

    float widthMultiplier = m_isDart ? 0.8f : 1.0f;
    m_regularTrail->setStroke((m_streakStrokeWidth * m_vehicleSize) * widthMultiplier);

    if (enable)
        modeDidChange();
}

void PlayerObject::toggleFlyMode(bool enable, bool noEffects)
{
    if (m_isShip == enable)
        return;

    m_isShip = enable;
    m_gameModeChangedTime = m_totalTime;

    if (enable)
        switchedToMode(GameObjectType::ShipPortal);

    stopRotation(false, 7);
    m_yVelocity *= 0.5;
    setRotation(0.0f);
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = false;
    removePendingCheckpoint();

    if (m_isShip)
    {
        stopPlatformerJumpAnimation();

        if (m_isPlatformer)
        {
            GameManager* gameManager = GameManager::sharedState();
            updatePlayerJetpackFrame(gameManager->m_playerJetpack.random - gameManager->m_playerJetpack.seed);
            m_iconSprite->setScale(0.6f);
            m_iconSprite->setPosition(&cocos2d::CCPoint(6.0f, 4.0f));
            m_vehicleSprite->setPosition(&cocos2d::CCPoint(0.0f, 0.0f));
        }
        else
        {
            GameManager* gameManager = GameManager::sharedState();
            updatePlayerShipFrame(gameManager->m_playerShip.random - gameManager->m_playerShip.seed);
            m_iconSprite->setScale(0.55f);
            m_iconSprite->setPosition(&cocos2d::CCPoint(0.0f, 5.0f));
            m_vehicleSprite->setPosition(&cocos2d::CCPoint(0.0f, -5.0f));
        }

        m_vehicleSprite->setVisible(true);
        updatePlayerGlow();

        if (!m_isHidden)
        {
            m_trailingParticles->resetSystem();
            m_shipClickParticles->resetSystem();
        }

        m_shipClickParticles->stopSystem();
        m_hasShipParticles = false;
        deactivateParticle();

        if (!noEffects)
            spawnPortalCircle(cocos2d::ccc3(255, 255, 0), 50.0f);

        activateStreak();
        updatePlayerScale();

        if (m_shipStreak && !m_isPlatformer && !m_isHidden)
            m_shipStreak->setVisible(true);
    }
    else
    {
        resetPlayerIcon();
    }

    if (enable)
        modeDidChange();
}

void PlayerObject::toggleGhostEffect(GhostType type)
{
    if (m_ghostType == type)
        return;

    m_ghostType = type;

    if (m_ghostTrail)
    {
        m_ghostTrail->stopTrail();
        m_ghostTrail = nullptr;
        type = m_ghostType;
    }

    if (type != GhostType::Enabled)
        return;

    cocos2d::ccColor3B iconColor = m_iconSprite->getColor();
    bool hasColor = iconColor.r != 0 || iconColor.g != 0 || iconColor.b != 0;

    if (hasColor)
    {
        m_ghostTrail = GhostTrailEffect::create();
        m_ghostTrail->m_playerObject = this;
        m_ghostTrail->doBlendAdditive();
        m_ghostTrail->m_playerScale = m_vehicleSize;
        m_ghostTrail->m_opacity = 200.0f;
        m_ghostTrail->m_color = m_playerColor1;
    }
    else
    {
        m_ghostTrail = GhostTrailEffect::create();
        m_ghostTrail->m_playerObject = this;
        m_ghostTrail->m_color = cocos2d::ccc3(0, 0, 0);
        m_ghostTrail->m_playerScale = m_vehicleSize;
        m_ghostTrail->m_opacity = 200.0f;
    }

    m_ghostTrail->runWithTarget(m_iconSprite, 0.05f, 0.4f, 0.0f, 0.6f, false);
    m_ghostTrail->m_objectLayer = GameManager::sharedState()->m_playLayer->m_objectLayer;
    GameManager::sharedState()->m_playLayer->m_objectLayer->addChild(m_ghostTrail);
}

void PlayerObject::togglePlatformerMode(bool val)
{
    m_isPlatformer = val;
}

void PlayerObject::togglePlayerScale(bool enable, bool noEffects)
{
    if (enable)
    {
        if (m_vehicleSize != 1.0f)
            return;
        m_vehicleSize = 0.6f;

        if (m_defaultMiniIcon)
        {
            if (m_isBall)
            {
                updatePlayerRollFrame(0);
            }
            else if (m_isRobot)
            {
                updatePlayerRobotFrame(1);
            }
            else if (m_isDart)
            {
                updatePlayerDartFrame(1);
            }
            else
            {
                updatePlayerFrame(1);
            }
        }
    }
    else
    {
        if (m_vehicleSize == 1.0f)
            return;
        if (m_isPlatformer)
            m_stateScale = 2;
        m_vehicleSize = 1.0f;

        if (m_defaultMiniIcon)
        {
            if (m_isBall)
            {
                GameManager* gameManager = GameManager::sharedState();
                updatePlayerRollFrame(gameManager->m_playerBall.random - gameManager->m_playerBall.seed);
            }
            else if (m_isRobot)
            {
                GameManager* gameManager = GameManager::sharedState();
                updatePlayerRobotFrame(gameManager->m_playerRobot.random - gameManager->m_playerRobot.seed);
            }
            else if (m_isDart)
            {
                GameManager* gameManager = GameManager::sharedState();
                updatePlayerDartFrame(gameManager->m_playerDart.random - gameManager->m_playerDart.seed);
            }
            else
            {
                updatePlayerFrame(m_maybeSavedPlayerFrame);
            }
        }
    }

    m_spriteWidthScale = m_vehicleSize;
    m_spriteHeightScale = m_vehicleSize;

    m_landParticles0->loadScaledDefaults(m_vehicleSize);
    m_landParticles1->loadScaledDefaults(m_vehicleSize);
    m_playerGroundParticles->loadScaledDefaults(m_vehicleSize);
    m_vehicleGroundParticles->loadScaledDefaults(m_vehicleSize);
    m_trailingParticles->loadScaledDefaults(m_vehicleSize);
    m_shipClickParticles->loadScaledDefaults(m_vehicleSize);
    m_ufoClickParticles->loadScaledDefaults(m_vehicleSize);
    m_robotBurstParticles->loadScaledDefaults(m_vehicleSize);
    m_dashParticles->loadScaledDefaults(m_vehicleSize);
    m_swingBurstParticles1->loadScaledDefaults(m_vehicleSize);
    m_swingBurstParticles2->loadScaledDefaults(m_vehicleSize);

    float trailWidthMultiplier = m_isDart ? 0.8f : 1.0f;
    m_regularTrail->setStroke((m_streakStrokeWidth * m_vehicleSize) * trailWidthMultiplier);

    if (m_playEffects || m_isDart)
        m_waveTrail->m_waveSize = m_vehicleSize;

    if (m_ghostTrail)
        m_ghostTrail->m_playerScale = m_vehicleSize;

    if (m_shipStreak)
        updateStreakSettings(m_shipStreak, m_shipStreakType, this);

    m_actionManager->stopInternalAction(6);

    float scale = m_vehicleSize;

    if (!m_playEffects || m_maybeReducedEffects || GameManager::sharedState()->m_playLayer->m_skipArtReload)
    {
        setScaleX(scale);
        setScaleY(scale);
    }
    else
    {
        cocos2d::CCScaleTo* scaleTo = cocos2d::CCScaleTo::create(0.5f, scale);
        cocos2d::CCEaseElasticOut* ease = cocos2d::CCEaseElasticOut::create(scaleTo);
        ease->setTag(6);
        m_actionManager->runInternalAction(ease, this);

        if (!noEffects)
        {
            cocos2d::ccColor3B color = (m_vehicleSize == 1.0f)
                ? cocos2d::ccc3(255, 0, 0)
                : cocos2d::ccc3(0, 255, 0);
            color.b = -106;

            PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;
            playLayer->lightningFlash(m_lastPortalPos, color);
            spawnPortalCircle(color, 45.0f);
            spawnScaleCircle();
        }
    }

    if (m_isBall && m_isRotating)
        runRotateAction(false, 8);

    placeStreakPoint();
    updateRobotAnimationSpeed();
}

void PlayerObject::toggleRobotMode(bool enable, bool noEffects)
{
    if (m_isRobot == enable)
        return;

    m_isRobot = enable;
    m_gameModeChangedTime = m_totalTime;

    if (!enable)
    {
        m_mainLayer->removeChild(m_robotBatchNode, false);
        m_iconSprite->setVisible(true);
        m_robotSprite->m_animationManager->stopAnimations();

        if (m_vehicleSize != 1.0f && m_defaultMiniIcon)
            updatePlayerFrame(0);
        else
            updatePlayerFrame(m_maybeSavedPlayerFrame);

        updatePlayerGlow();
        stopRotation(true, 13);
        return;
    }

    m_mainLayer->addChild(m_robotBatchNode, 2);
    switchedToMode(GameObjectType::RobotPortal);
    m_accelerationOrSpeed = 1.5;
    stopRotation(false, 12);
    setRotation(0.0f);

    GameManager* gameManager = GameManager::sharedState();
    int frameID = gameManager->m_playerRobot.random - gameManager->m_playerRobot.seed;
    if (frameID > 68)
        frameID = 68;
    if (frameID <= 0)
        frameID = 1;

    cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("robot_%02d_01_001.png", frameID);
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
    m_iconSprite->setDisplayFrame(frame);

    if (m_isPlatformer && m_platformerXVelocity == 0.0)
    {
        m_robotSprite->runAnimation("idle01");
    }
    else
    {
        m_robotSprite->runAnimation(m_currentRobotAnimation);
    }

    m_iconSprite->setVisible(false);

    if (!noEffects)
    {
        cocos2d::ccColor3B color = cocos2d::ccc3(255, 200, 50);
        spawnPortalCircle(color, 50.0f);
    }

    updatePlayerScale();
    updatePlayerGlow();
    stopRotation(true, 13);
    modeDidChange();
}

void PlayerObject::toggleRollMode(bool enable, bool noEffects)
{
    if (m_isBall == enable)
        return;

    m_isBall = enable;
    m_gameModeChangedTime = m_totalTime;

    if (enable)
    {
        switchedToMode(GameObjectType::BallPortal);

        if (m_vehicleSize != 1.0f && m_defaultMiniIcon)
        {
            updatePlayerRollFrame(0);
        }
        else
        {
            GameManager* gameManager = GameManager::sharedState();
            updatePlayerRollFrame(gameManager->m_playerBall.random - gameManager->m_playerBall.seed);
        }

        if (!noEffects)
        {
            cocos2d::ccColor3B color = cocos2d::ccc3(255, 200, 50);
            spawnPortalCircle(color, 50.0f);
        }

        stopRotation(true, 11);
        modeDidChange();
        return;
    }

    if (m_vehicleSize != 1.0f && m_defaultMiniIcon)
        updatePlayerFrame(0);
    else
        updatePlayerFrame(m_maybeSavedPlayerFrame);

    float rotation = m_isUpsideDown ? 180.0f : 0.0f;
    setRotation(rotation);
    stopRotation(true, 11);
}

void PlayerObject::toggleSpiderMode(bool enable, bool noEffects)
{
    if (m_isSpider == enable)
        return;

    m_isSpider = enable;
    m_gameModeChangedTime = m_totalTime;

    if (enable)
    {
        m_mainLayer->addChild(m_spiderBatchNode, 2);
        switchedToMode(GameObjectType::SpiderPortal);
        m_unkAngle1 = 27.0f;
        m_width = 27.0f;
        m_height = 27.0f;
        m_accelerationOrSpeed = 1.5;
        stopRotation(false, 14);
        setRotation(0.0f);

        GameManager* gameManager = GameManager::sharedState();
        int frameID = gameManager->m_playerSpider.random - gameManager->m_playerSpider.seed;
        if (frameID > 69)
            frameID = 69;
        if (frameID <= 0)
            frameID = 1;

        cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("spider_%02d_01_001.png", frameID);
        cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
        m_iconSprite->setDisplayFrame(frame);

        if (m_isPlatformer && m_platformerXVelocity == 0.0)
        {
            m_spiderSprite->runAnimation("idle01");
        }
        else
        {
            playDynamicSpiderRun();
        }

        m_iconSprite->setVisible(false);

        if (!noEffects)
        {
            cocos2d::ccColor3B color = cocos2d::ccc3(255, 200, 50);
            spawnPortalCircle(color, 50.0f);
        }

        updatePlayerScale();
        updatePlayerGlow();
        stopRotation(true, 15);
        modeDidChange();
        return;
    }

    m_mainLayer->removeChild(m_spiderBatchNode, false);
    m_iconSprite->setVisible(true);
    m_spiderSprite->m_animationManager->stopAnimations();

    if (m_vehicleSize != 1.0f && m_defaultMiniIcon)
        updatePlayerFrame(0);
    else
        updatePlayerFrame(m_maybeSavedPlayerFrame);

    resetPlayerIcon();
    updatePlayerGlow();
    stopRotation(true, 15);
}

void PlayerObject::toggleSwingMode(bool enable, bool noEffects)
{
    if (m_isSwing == enable)
        return;

    m_isSwing = enable;
    m_gameModeChangedTime = m_totalTime;

    if (enable)
        switchedToMode(GameObjectType::SwingPortal);

    stopRotation(false, 9);
    m_yVelocity *= 0.5;
    setRotation(0.0f);
    m_isOnGround2 = false;
    m_isOnGround = false;
    m_shouldTryPlacingCheckpoint = false;
    removePendingCheckpoint();

    if (!m_isSwing)
    {
        resetPlayerIcon();
        updatePlayerFrame(m_maybeSavedPlayerFrame);

        float rotation = m_isUpsideDown ? 180.0f : 0.0f;
        setRotation(rotation);
        disableSwingFire();

        if (enable)
            modeDidChange();
        return;
    }

    GameManager* gameManager = GameManager::sharedState();
    updatePlayerSwingFrame(gameManager->m_playerSwing.random - gameManager->m_playerSwing.seed);

    if (!noEffects)
    {
        cocos2d::ccColor3B color = cocos2d::ccc3(0, 200, 255);
        spawnPortalCircle(color, 50.0f);
    }

    if (!m_isHidden)
    {
        m_trailingParticles->resetSystem();
        m_shipClickParticles->resetSystem();
    }

    m_shipClickParticles->stopSystem();
    m_hasShipParticles = false;
    deactivateParticle();
    activateStreak();

    m_swingFireMiddle->setVisible(true);
    m_swingFireMiddle->loopFireAnimation();
    m_swingFireBottom->setScale(0.01f);
    m_swingFireTop->setScale(0.01f);
    m_swingFireBottom->setVisible(true);
    m_swingFireTop->setVisible(true);

    m_swingBurstParticles1->resetSystem();
    m_swingBurstParticles2->resetSystem();
    m_swingBurstParticles1->stopSystem();
    m_swingBurstParticles2->stopSystem();

    updateSwingFire();

    if (enable)
        modeDidChange();
}

void PlayerObject::toggleVisibility(bool visible)
{
    bool hidden = !visible;

    if (m_isHidden == hidden)
        return;

    if (visible)
    {
        m_isHidden = false;
        GameObject::setVisible(true);

        if (isFlying())
        {
            resetStreak();
            activateStreak();
            m_trailingParticles->resetSystem();

            if (m_isDart)
            {
                placeStreakPoint();
            }
            else if (m_isShip && !m_isPlatformer)
            {
                if (m_shipStreak)
                    m_shipStreak->setVisible(true);
            }
        }
        else if (m_alwaysShowStreak)
        {
            resetStreak();
            activateStreak();
        }
    }
    else
    {
        stopActionByTag(11);
        m_isHidden = true;
        setVisible(false);
        deactivateStreak(true);

        if (m_shipStreak)
            m_shipStreak->setVisible(false);

        resetAllParticles();
    }
}

void PlayerObject::touchedObject(GameObject* object)
{
    int objectID = object->m_objectID;

    if (objectID == 1859)
    {
        m_stateHitHead = 2;
        return;
    }

    if (objectID == 1813)
    {
        m_stateNoAutoJump = 2;
        return;
    }

    if (objectID == 1829)
    {
        if (m_isDashing)
        {
            stopDashing();
            m_jumpBuffered = 0;
        }
        return;
    }

    if (objectID == 1755)
    {
        m_stateDartSlide = 2;
        return;
    }

    if (objectID == 2866)
    {
        m_stateFlipGravity = 2;
        return;
    }

    if (objectID == 3645 || objectID == 2069)
    {
        float forceValue = object->m_sQuad.bl.texCoords.u;
        m_stateForce = 2;

        if ((int)forceValue > 0)
        {
            auto it = m_jumpPadRelated.find((int)forceValue);
            if (it == m_jumpPadRelated.end() || !it->second)
            {
                m_jumpPadRelated[(int)forceValue] = true;
                cocos2d::CCPoint force = ((ForceBlockGameObject*)object)->calculateForceToTarget(this);
                m_stateForceVector += force;

                float magnitude = sqrtf(m_stateForceVector.y * m_stateForceVector.y + m_stateForceVector.x * m_stateForceVector.x);
                if (magnitude > 9999.0f)
                    m_stateForceVector *= 9999.0f / magnitude;
            }
        }
    }
}

void PlayerObject::tryPlaceCheckpoint()
{
    PlayLayer* playLayer = GameManager::sharedState()->m_playLayer;

    if (!playLayer)
        return;
    if (!playLayer->m_isPracticeMode)
        return;

    if (!GameManager::sharedState()->getGameVariable("0027") || m_uniqueID != 1)
        return;

    bool shouldTry = isFlying()
        ? (m_platformerCheckpoint = isFlying(), true)
        : (m_shouldTryPlacingCheckpoint || m_canPlaceCheckpoint);

    if (!shouldTry)
        return;

    if (!isFlying())
        m_shouldTryPlacingCheckpoint = false;

    CheckpointObject* lastCheckpoint = playLayer->getLastCheckpoint();

    if (lastCheckpoint)
    {
        unsigned int currentProgress = playLayer->m_gameState.m_currentProgress;
        int requiredProgress = m_quickCheckpointMode ? 100 : 500;

        if ((int)(currentProgress - lastCheckpoint->m_commandIndex) <= requiredProgress)
            return;
    }

    m_checkpointTimeout = true;
    m_lastCheckpointTime = cocos2d::CCDirector::sharedDirector()->getTotalFrames();
    playLayer->markCheckpoint();
}

void PlayerObject::unrotateGameplayObject(GameObject* object)
{
    if (m_rotateObjectsRelated.find(object->m_uniqueID) == m_rotateObjectsRelated.end())
        return;

    object->setObjectRectDirty(true);

    GJPointDouble storedOffset = m_rotateObjectsRelated[object->m_uniqueID];
    object->addToTempOffset(-storedOffset.x, -storedOffset.y);

    cocos2d::CCPoint lastPos = object->getLastPosition();
    cocos2d::CCPoint offset(-storedOffset.x, -storedOffset.y);
    cocos2d::CCPoint newLastPos = lastPos + offset;
    object->setLastPosition(&newLastPos);

    float newRotationY = object->m_startRotationY + 90.0f;
    object->m_startRotationX = object->m_startRotationX + 90.0f;
    object->m_startRotationY = newRotationY;
    object->setRRotation(0.0f);

    if (object->getType() == GameObjectType::Slope)
        object->determineSlopeDirection();

    m_rotateObjectsRelated.erase(object->m_uniqueID);
}

void PlayerObject::unrotatePreSlopeObjects()
{
    for (auto it = m_potentialSlopeMap.begin(); it != m_potentialSlopeMap.end(); ++it)
        unrotateGameplayObject(it->second);
}

void PlayerObject::updateCheckpointMode(bool enable)
{
    m_quickCheckpointMode = enable;
}

void PlayerObject::updateCheckpointTest()
{
    if (m_canPlaceCheckpoint)
    {
        tryPlaceCheckpoint();
        m_canPlaceCheckpoint = false;
    }

    if (isFlying() && m_uniqueID == 1)
    {
        int tries = m_onFlyCheckpointTries + 1;

        if (tries <= 19)
        {
            m_onFlyCheckpointTries = tries;
        }
        else
        {
            m_onFlyCheckpointTries = 0;
            tryPlaceCheckpoint();
        }
    }

    if (!m_checkpointTimeout)
        return;

    if (cocos2d::CCDirector::sharedDirector()->getTotalFrames() - m_lastCheckpointTime > 0.100000001f)
        m_checkpointTimeout = false;
}

void PlayerObject::updateCollide(PlayerCollisionDirection direction, GameObject* object)
{
    float halfHeight = getObjectRect().size.height * 0.5f;
    float halfWidth = getObjectRect().size.width * 0.5f;

    switch (direction)
    {
        case PlayerCollisionDirection::Top:
        {
            float y = getPosition().y + (float)flipMod() * halfHeight;
            updateCollideTop(y, object);
            break;
        }
        case PlayerCollisionDirection::Bottom:
        {
            float y = getPosition().y - (float)flipMod() * halfHeight;
            updateCollideBottom(y, object);
            break;
        }
        case PlayerCollisionDirection::Left:
        {
            float x = getPosition().x - halfWidth;
            updateCollideLeft(x, object);
            break;
        }
        case PlayerCollisionDirection::Right:
        {
            float x = getPosition().x + halfWidth;
            updateCollideRight(x, object);
            break;
        }
        default:
            break;
    }
}

void PlayerObject::updateCollideBottom(float y, GameObject* object)
{
    if (object)
        m_uniqueID = object->m_uniqueID;
    else
        m_uniqueID = 0;

    if (m_collidedBottomMaxY == 0.0)
    {
        m_collidedBottomMaxY = y;

        if (m_uniqueID)
            storeCollision(PlayerCollisionDirection::Bottom, m_uniqueID);
        return;
    }

    double value = y;

    if (m_isUpsideDown)
    {
        if (m_collidedBottomMaxY > value)
            m_collidedBottomMaxY = value;
    }
    else if (m_collidedBottomMaxY < value)
    {
        m_collidedBottomMaxY = value;
    }

    if (m_uniqueID)
        storeCollision(PlayerCollisionDirection::Bottom, m_uniqueID);
}

void PlayerObject::updateCollideLeft(float x, GameObject* object)
{
    if (object)
        m_uniqueID = object->m_uniqueID;
    else
        m_uniqueID = 0;

    double value = x;

    if (m_collidedLeftMaxX != 0.0 && m_collidedLeftMaxX >= value)
        value = m_collidedLeftMaxX;

    m_collidedLeftMaxX = value;

    if (m_uniqueID)
    {
        storeCollision(PlayerCollisionDirection::Left, m_uniqueID);
        m_collidingWithLeft = object;
        m_collidingWithRight = nullptr;
    }
}

void PlayerObject::updateCollideRight(float x, GameObject* object)
{
    if (object)
        m_uniqueID = object->m_uniqueID;
    else
        m_uniqueID = 0;

    double value = x;

    if (m_collidedRightMinX != 0.0 && m_collidedRightMinX <= value)
        value = m_collidedRightMinX;

    m_collidedRightMinX = value;

    if (m_uniqueID)
    {
        storeCollision(PlayerCollisionDirection::Right, m_uniqueID);
        m_collidingWithRight = object;
        m_collidingWithLeft = nullptr;
    }
}

void PlayerObject::updateCollideTop(float y, GameObject* object)
{
    if (object)
        m_uniqueID = object->m_uniqueID;
    else
        m_uniqueID = 0;

    if (m_collidedTopMinY == 0.0)
    {
        m_collidedTopMinY = y;

        if (m_uniqueID)
            storeCollision(PlayerCollisionDirection::Top, m_uniqueID);
        return;
    }

    double value = y;

    if (m_isUpsideDown)
    {
        if (m_collidedTopMinY < value)
            m_collidedTopMinY = value;
    }
    else if (m_collidedTopMinY > value)
    {
        m_collidedTopMinY = value;
    }

    if (m_uniqueID)
        storeCollision(PlayerCollisionDirection::Top, m_uniqueID);
}

void PlayerObject::updateDashAnimation()
{
    float frameInterval = (0.04f / m_playerSpeed)
        * (1.0f - fabsf(m_dashFireSprite->getRotation()) / 70.0f * 0.400000006f);

    float timeSum = m_totalTime / frameInterval;
    int frame = (int)timeSum % 12 + 1;

    if (m_dashFireFrame == frame)
        return;

    m_dashFireFrame = frame;

    cocos2d::CCArray* children = m_dashFireSprite->getChildren();
    cocos2d::CCSprite* outlineSprite = (cocos2d::CCSprite*)children->objectAtIndex(0);

    cocos2d::CCString* frameName = cocos2d::CCString::createWithFormat("playerDash2_%03d.png", frame);
    const char* frameCString = frameName->getCString();

    cocos2d::CCString* outlineName = cocos2d::CCString::createWithFormat("playerDash2_outline_%03d.png", frame);
    const char* outlineCString = outlineName->getCString();

    if (!frameCString)
        return;

    cocos2d::CCSpriteFrame* mainFrame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameCString);
    m_dashFireSprite->setDisplayFrame(mainFrame);

    cocos2d::CCSpriteFrame* outlineFrame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(outlineCString);
    outlineSprite->setDisplayFrame(outlineFrame);

    cocos2d::CCSize size = m_dashFireSprite->getContentSize();
    outlineSprite->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
}


void PlayerObject::updateDashArt()
{
    if (!m_isDashing)
        return;

    m_iconSprite->stopActionByTag(12);
    m_iconGlow->stopActionByTag(12);
    m_iconSprite->setRotation(0.0f);
    m_iconGlow->setRotation(0.0f);

    float targetRotation;

    if (m_isSideways)
        targetRotation = (m_isGoingLeft ? 90.0f : -90.0f) - m_dashAngle;
    else
        targetRotation = (m_isGoingLeft ? 180.0f : 0.0f) - m_dashAngle;

    setRotation(targetRotation);

    m_dashFireSprite->stopAllActions();

    if (m_isShip)
    {
        m_dashFireSprite->setPosition(&cocos2d::CCPoint(0.0f, -2.0f));
        m_dashFireSprite->setScaleX(1.0f);
        m_dashFireSprite->setScaleY(0.85f);
    }
    else if (m_isBird)
    {
        m_dashFireSprite->setPosition(&cocos2d::CCPoint(0.0f, 0.0f));
        m_dashFireSprite->setScaleX(0.95f);
        m_dashFireSprite->setScaleY(0.9f);
    }
    else if (m_isRobot)
    {
        m_dashFireSprite->setPosition(&cocos2d::CCPoint(1.0f, 0.0f));
        m_dashFireSprite->setScaleX(0.9f);
        m_dashFireSprite->setScaleY(0.85f);
        m_robotSprite->runAnimation("fall_loop");
    }
    else if (m_isSpider)
    {
        m_dashFireSprite->setPosition(&cocos2d::CCPoint(1.0f, 0.0f));
        m_dashFireSprite->setScaleX(0.95f);
        m_dashFireSprite->setScaleY(0.8f);
        m_spiderSprite->runAnimation("fall_loop");
    }
    else
    {
        m_dashFireSprite->setPosition(&cocos2d::CCPoint(0.0f, 0.0f));
        m_dashFireSprite->setScaleX(0.9f);
        m_dashFireSprite->setScaleY(0.8f);
    }

    if (isFlying() || m_isRobot || m_isSpider)
        return;

    m_iconSprite->setScale(0.9f);
    m_iconGlow->setScale(0.9f);

    float rotationAmount;

    if (m_isSideways == m_isUpsideDown)
        rotationAmount = 288.0f;
    else
        rotationAmount = -288.0f;

    rotationAmount *= m_gravityMod;

    if (m_isPlatformer)
    {
        float speed = sqrtf(m_dashY * m_dashY + m_dashX * m_dashX);

        if (speed > 17.31f)
            rotationAmount *= 2.0f;
        else
            rotationAmount *= (speed / 17.31f) * 1.7f + 0.3f;
    }

    cocos2d::CCRotateBy* rotate = cocos2d::CCRotateBy::create(0.3f, rotationAmount);
    cocos2d::CCRepeatForever* repeat = cocos2d::CCRepeatForever::create(rotate);
    repeat->setTag(12);
    m_iconSprite->runAction(repeat);

    cocos2d::CCRotateBy* rotate2 = cocos2d::CCRotateBy::create(0.3f, rotationAmount);
    cocos2d::CCRepeatForever* repeat2 = cocos2d::CCRepeatForever::create(rotate2);
    repeat2->setTag(12);
    m_iconGlow->runAction(repeat2);
}

void PlayerObject::updateEffects(float dt)
{
    m_waveTrail->updateStroke(dt);
}

void PlayerObject::updateGlowColor()
{
    cocos2d::ccColor3B iconColor = m_iconSprite->getColor();
    cocos2d::ccColor3B iconSecondaryColor = m_iconSpriteSecondary->getColor();

    m_originalMainColor = iconColor;
    m_originalSecondColor = iconSecondaryColor;

    bool hasMainColor = iconColor.r != 0 || iconColor.g != 0 || iconColor.b != 0;
    bool hasSecondaryColor = iconSecondaryColor.r != 0 || iconSecondaryColor.g != 0 || iconSecondaryColor.b != 0;

    if (hasMainColor)
    {
        m_playerColor1 = iconColor;

        if (hasSecondaryColor)
        {
            m_playerColor2 = iconSecondaryColor;
            m_maybeIsVehicleGlowing = false;
        }
        else
        {
            m_playerColor2 = iconColor;
            m_maybeIsVehicleGlowing = false;
        }
    }
    else if (hasSecondaryColor)
    {
        m_playerColor1 = iconSecondaryColor;
        m_playerColor2 = iconSecondaryColor;
        m_maybeIsVehicleGlowing = true;
    }
    else
    {
        m_playerColor1 = cocos2d::ccc3(255, 255, 255);
        m_playerColor2 = cocos2d::ccc3(255, 255, 255);
        m_maybeIsVehicleGlowing = true;
    }

    m_playerColor1 = GameToolbox::strongColor(m_playerColor1);
    m_playerColor2 = GameToolbox::strongColor(m_playerColor2);

    cocos2d::ccColor4F startColor1(
        m_playerColor1.r / 255.0f,
        m_playerColor1.g / 255.0f,
        m_playerColor1.b / 255.0f,
        1.0f);
    cocos2d::ccColor4F startColor2(
        m_playerColor2.r / 255.0f,
        m_playerColor2.g / 255.0f,
        m_playerColor2.b / 255.0f,
        1.0f);

    m_playerGroundParticles->setStartColor(startColor1);
    m_landParticles0->setStartColor(startColor1);
    m_landParticles1->setStartColor(startColor1);
    m_ufoClickParticles->setStartColor(startColor1);
    m_ufoClickParticles->setEndColor(startColor1);

    cocos2d::ccColor4F dashStartColor = m_switchDashFireColor ? startColor2 : startColor1;
    m_dashParticles->setStartColor(dashStartColor);

    cocos2d::ccColor4F dashEndColor = dashStartColor;
    dashEndColor.a = dashStartColor.a * 0.5f;
    m_dashParticles->setEndColor(dashEndColor);

    if (!m_disableStreakTint)
        m_regularTrail->setColor(&m_playerColor2);

    if (m_playEffects || m_isDart)
        m_waveTrail->setColor(&m_playerColor2);

    if (m_maybeIsVehicleGlowing || m_hasGlow)
    {
        if (!m_isRobot && !m_isSpider)
        {
            m_iconGlow->setVisible(true);
            m_vehicleGlow->setVisible(m_vehicleSprite->isVisible());
        }
        m_maybeIsVehicleGlowing = true;
    }
    else
    {
        m_iconGlow->setVisible(false);
        m_vehicleGlow->setVisible(false);
    }

    cocos2d::ccColor3B glowColor = m_hasCustomGlowColor ? m_glowColor : m_playerColor2;

    m_iconGlow->setColor(&glowColor);
    m_vehicleGlow->setColor(&glowColor);

    if (m_maybeIsVehicleGlowing)
    {
        if (m_robotSprite->isVisible())
            m_robotSprite->showGlow();
        else
            m_robotSprite->hideGlow();

        if (m_spiderSprite->isVisible())
            m_spiderSprite->showGlow();
        else
            m_spiderSprite->hideGlow();
    }
    else
    {
        m_robotSprite->hideGlow();
        m_spiderSprite->hideGlow();
    }

    m_robotSprite->updateGlowColor(glowColor, false);
    m_spiderSprite->updateGlowColor(glowColor, false);
    m_dashFireSprite->setColor(&glowColor);
}

void PlayerObject::updateInternalActions(float dt)
{
    m_actionManager->updateInternalActions(dt, false);
}

void PlayerObject::updateJump(float dt)
{
    if (m_holdingRight && m_holdingLeft)
    {
        m_holdingRight = !m_leftPressedFirst;
        m_holdingLeft = m_leftPressedFirst;
    }

    int stateValue = 0;

    bool onSteepSlope = m_isPlatformer
        && m_isOnSlope
        && ((fabsf(m_slopeAngleRadians * 57.296f) >= 80.0f)
            || (fabsf(m_slopeAngleRadians * 57.296f) >= 40.0f && m_currentSlope->m_isIceBlock));

    if (onSteepSlope)
    {
        m_isOnGround = false;
        stateValue = 0;
    }
    else if (m_isOnGround)
    {
        stateValue = 213;
    }

    m_jumpRelatedAC2.value = stateValue;
    int randomSeed = rand();
    int randomOffset = (int)((float)randomSeed * 4.6566e-10f * 1000.0f);
    m_jumpRelatedAC2.seed = randomOffset;
    m_jumpRelatedAC2.random = stateValue + randomOffset;

    if (m_isRobot)
        m_jumpBuffered &= m_stateRingJump;

    float gravity = (m_isBall || isFlying() || m_isSpider) ? 0.9582f : m_gravity;

    if (m_gravityMod != 1.0f)
        gravity *= m_gravityMod;

    float vehicleScale = (m_vehicleSize != 1.0f) ? 0.8f : 1.0f;

    if (isFlying())
    {
        float upSpeed;
        float downSpeed;
        bool movingUp;
        bool movingDown;

        if (m_vehicleSize == 1.0f)
        {
            upSpeed = 8.0f / vehicleScale;
            movingUp = m_yVelocity > 0.0;
            movingDown = m_yVelocity < 0.0;
            downSpeed = -6.4f / vehicleScale;
        }
        else
        {
            downSpeed = -7.5294f;
            upSpeed = 9.4118f;
            vehicleScale = 0.85f;
            movingUp = m_yVelocity > 0.0;
            movingDown = m_yVelocity < 0.0;
        }

        bool accelDisabled = false;

        if (m_isUpsideDown)
        {
            if (!movingUp && m_yVelocity > -upSpeed)
                accelDisabled = true;
            else if (m_yVelocity >= 0.0 && m_yVelocity < -downSpeed)
                accelDisabled = true;
        }
        else
        {
            if (!movingDown && m_yVelocity < upSpeed)
                accelDisabled = true;
            else if (m_yVelocity <= 0.0 && m_yVelocity > downSpeed)
                accelDisabled = true;
        }

        if (accelDisabled)
            m_isAccelerating = false;

        if (m_isShip)
        {
            if (m_jumpBuffered)
            {
                if (!m_isAccelerating)
                {
                    float velocitySign = (m_isUpsideDown ? -1.0f : 1.0f) * -1.0f;
                    setYVelocity(-8.0f / vehicleScale * velocitySign, 11);

                    float speedBase = m_playerSpeed * m_speedMultiplier;
                    float velocityMultiplier = m_jumpBuffered ? 1.0f : -1.0f;
                    setYVelocity(speedBase * flipMod() * velocityMultiplier, 12);
                }
            }
        }
        else if (!m_isBird)
        {
            if (m_isSwing)
            {
                if (m_stateRingJump && m_jumpBuffered)
                {
                    double yVelocity = m_yVelocity;
                    m_stateRingJump = false;
                    flipGravity(!m_isUpsideDown, true);
                    setYVelocity(yVelocity * 0.8, 10);

                    if (m_gameLayer)
                        gameEventTriggered(20, 0);
                }

                float swingScale = (m_vehicleSize == 1.0f) ? 0.4f : 0.6f;
                int flip = flipMod();
                addToYVelocity(-(gravity * dt * (float)flip * swingScale), 67);

                if (m_jumpBuffered)
                    m_isOnGround2 = false;
            }
            else
            {
                float targetY = (float)flipMod() * 8.0f;
                float sign = m_jumpBuffered ? 1.0f : -1.0f;
                setYVelocity(targetY * sign, 11);

                double speedBase = m_playerSpeed * m_speedMultiplier;
                double velocityMultiplier = m_jumpBuffered ? 1.0 : -1.0;
                setYVelocity(speedBase * (double)flipMod() * velocityMultiplier, 12);

                if (!m_gameLayer || m_wasJumpBuffered == m_jumpBuffered)
                {
                }
                else
                {
                    if (m_jumpBuffered)
                        gameEventTriggered(21, 0);
                    else
                        gameEventTriggered(22, 0);
                }
            }
        }
        else if (m_stateRingJump && m_jumpBuffered)
        {
            m_stateRingJump = false;
            float birdRingVelocity = (m_vehicleSize == 1.0f) ? 7.0f : 8.0f;
            int flip = flipMod();
            double targetVelocity = (double)((float)flip * birdRingVelocity) * vehicleScale;
            double currentVelocity = m_yVelocity;

            bool shouldSet = m_isUpsideDown
                ? targetVelocity < currentVelocity
                : targetVelocity > currentVelocity;

            if (shouldSet)
            {
                setYVelocity(targetVelocity, 7);

                if (m_wasOnSlope || m_isOnSlope)
                {
                    if (m_slopeVelocity > 0.0f)
                    {
                        float maxVelocity = m_yVelocity * 1.39999998f;
                        addToYVelocity(m_slopeVelocity * 0.5f, 8);
                        setYVelocity(std::min(m_yVelocity, (double)maxVelocity), 9);
                    }
                }
            }

            playBurstEffect();

            if (m_gameLayer)
                gameEventTriggered(15, 0);
        }

        if (playerIsFallingBugged())
        {
            float gravityScale = 0.8f;
            int flip = flipMod();
            addToYVelocity(-(gravity * dt * (float)flip * gravityScale * 0.5f / vehicleScale), 66);
        }
        else
        {
            float gravityScale = 1.2f;
            int flip = flipMod();
            addToYVelocity(-(gravity * dt * (float)flip * gravityScale * 0.5f / vehicleScale), 66);
        }

        if (m_jumpBuffered)
            m_isOnGround2 = false;

        if (!m_isAccelerating && !m_isDart)
        {
            if (m_isUpsideDown)
            {
                double maxUp = -8.0 / vehicleScale;
                if (m_yVelocity < maxUp)
                    setYVelocity(maxUp, 15);

                double maxDown = 8.0 * 0.8f / vehicleScale;
                if (m_yVelocity > maxDown)
                    setYVelocity(maxDown, 16);
            }
            else
            {
                double maxDown = 8.0 * 0.8f * -1.0f / vehicleScale;
                if (m_yVelocity < maxDown)
                    setYVelocity(maxDown, 13);

                double maxUp = 8.0 / vehicleScale;
                if (m_yVelocity > maxUp)
                    setYVelocity(maxUp, 14);
            }
        }

        if (playerIsFallingBugged())
            m_maybeIsBoosted = false;

        m_wasJumpBuffered = m_jumpBuffered;
        m_wasRobotJump = m_touchedPad;
        return;
    }

    float gravityScale = 1.0f;

    if (m_isBall || m_isSpider || m_isSwing)
        gravityScale = 0.6f;
    else if (m_isRobot)
        gravityScale = 0.9f;

    int intVal5 = 0;
    int intVal4 = 1;

    bool movingLeft = m_holdingLeft || m_platformerMovingLeft;
    bool movingRight = m_holdingRight || m_platformerMovingRight;

    if (movingLeft && !movingRight)
    {
        if (m_isSlidingRight == m_platformerMovingLeft || m_slopeSlidingMaybeRotated)
        {
            intVal5 = 1;
            intVal4 = m_maybeSlidingTime <= 1;
        }
        else
        {
            m_maybeSlidingTime = 0;
            intVal4 = 1;
            intVal5 = 1;
        }
    }

    bool isBoostedPath = false;

    if (m_jumpBuffered)
    {
        if (m_isOnGround)
        {
            if (m_isSpider && !m_isDashing)
            {
                spiderTestJump(false);
            }
            else
            {
                if (m_maybeIsBoosted)
                {
                    isBoostedPath = true;
                }
                else
                {
                    m_fallStartY = getPosition().y;

                    if (playerIsFallingBugged()
                        && (!m_isPlatformer
                            || m_totalTime - m_lastLandTime >= 0.0500000045f
                            || !intVal4))
                    {
                        m_isOnGround = false;
                    }

                    int flip = flipMod();
                    addToYVelocity(-(gravity * dt * (float)flip * gravityScale), 63);

                    double clampedVelocity = m_yVelocity;

                    if (m_isUpsideDown)
                    {
                        if (clampedVelocity > 15.0)
                            clampedVelocity = 15.0;
                        setYVelocity(clampedVelocity, 6);
                    }
                    else
                    {
                        if (clampedVelocity < -15.0)
                            clampedVelocity = -15.0;
                        setYVelocity(clampedVelocity, 5);
                    }

                    if (playerIsFalling(-0.25f)
                        && !m_isBall && !m_isSpider && !m_isRobot
                        && !m_isRotating && !m_isOnSlope && !m_isCollidingWithSlope
                        && (!m_isPlatformer || !m_isOnGround2))
                    {
                        runRotateAction(true, 12);
                    }

                    if (playerIsFallingBugged())
                    {
                        double velocityCheck = m_yVelocity;

                        if ((m_isUpsideDown && velocityCheck > 4.0) || (!m_isUpsideDown && velocityCheck < -4.0))
                        {
                            if (m_isRobot && m_isOnGround2 && !m_maybeSpriteRelated)
                                m_robotSprite->tweenToAnimation("fall_loop", 0.1f);
                            else if (m_isSpider && m_isOnGround2 && !m_maybeSpriteRelated)
                                m_spiderSprite->tweenToAnimation("fall_loop", 0.1f);

                            m_isOnGround2 = false;
                        }
                    }
                }
            }
        }
        else if (m_maybeIsBoosted)
        {
            isBoostedPath = true;
        }
    }
    else if (m_isOnGround)
    {
        m_isOnGround2 = false;
        m_maybeIsBoosted = true;
        m_isOnGround = false;
        m_stateRingJump = false;
        m_touchedPad = false;
        m_accelerationOrSpeed = 0.0;

        if (!m_platformerMovingLeft && !m_platformerMovingRight && m_maybeSlidingTime > 0)
        {
            m_maybeSlidingTime = 0;
            m_maybeSlidingStartTime = m_totalTime;
        }

        if (m_isRobot)
            m_yStart *= 0.5;

        bool hasDefaultJump = m_jumpRelatedAC2.random - m_jumpRelatedAC2.seed == 213;
        bool isAccelerating = !hasDefaultJump;

        if (!hasDefaultJump)
            m_yStart = 9999.0;

        if (intVal5 && m_isPlatformer)
        {
            if (m_isMoving)
            {
                double platformerSpeed = fabs(m_platformerXVelocity);
                double expectedSpeed = m_playerSpeed * m_speedMultiplier;

                if (platformerSpeed > expectedSpeed)
                {
                    m_yStart += platformerSpeed / expectedSpeed;
                }
                else if (m_maybeSlidingTime > 0)
                {
                    m_yStart *= 1.0f - (float)m_maybeSlidingTime * 0.3f;
                }
            }

            m_maybeSlidingTime++;
        }

        int flip = flipMod();
        setYVelocity((float)((float)flip * m_yStart) * vehicleScale, 2);

        if (m_wasOnSlope || m_isOnSlope)
        {
            if ((m_slopeVelocity * (float)flipMod()) > 0.0f
                && (!m_isPlatformer || fabs(m_platformerXVelocity) > 4.0))
            {
                float maxVelocity = m_yVelocity * 1.39999998f;
                float modifiedSlopeYVel = m_isBall ? m_slopeVelocity : getModifiedSlopeYVel();
                addToYVelocity(modifiedSlopeYVel * 0.25f, 60);

                if (m_isUpsideDown)
                {
                    if (m_yVelocity >= maxVelocity)
                        setYVelocity(maxVelocity, 4);
                }
                else
                {
                    if (m_yVelocity <= maxVelocity)
                        setYVelocity(maxVelocity, 3);
                }
            }
        }

        m_shouldTryPlacingCheckpoint = true;
        m_lastJumpTime = cocos2d::CCDirector::sharedDirector()->getTotalFrames();
        incrementJumps();

        if (m_isBall)
        {
            flipGravity(!m_isUpsideDown, true);
            m_isOnGround3 = false;
            m_jumpBuffered = 0;
            m_yVelocity *= 0.600000024f;
        }
        else if (m_isRobot)
        {
            m_robotSprite->tweenToAnimation("jump_loop", 0.1f);
        }
        else if (m_isSpider)
        {
        }
        else
        {
            runRotateAction(true, 11);
        }

        if (m_gameLayer)
        {
            if (isInNormalMode())
                gameEventTriggered(12, 0);
            else if (m_isRobot)
                gameEventTriggered(13, 0);
            else if (m_isBall)
                gameEventTriggered(19, 0);
        }

        if (isAccelerating && !m_isOnGround)
            GameManager::sharedState()->setGameVariable("0055", true);

        if (m_isPlatformer)
            animatePlatformerJump(1.0f);

        m_wasJumpBuffered = m_jumpBuffered;
        m_wasRobotJump = m_touchedPad;
        return;
    }
    else
    {
        isBoostedPath = false;
    }

    if (isBoostedPath)
    {
        if (m_isRobot && m_jumpBuffered && !m_touchedPad && m_accelerationOrSpeed < 1.5f)
        {
            float gravityDelta = gravity * dt;
            m_accelerationOrSpeed += dt * 0.1f;
            int flip = flipMod();
            addToYVelocity(gravityDelta * (float)flip * gravityScale, 61);
        }
        else
        {
            float gravityDelta = gravity * dt;
            int flip = flipMod();
            addToYVelocity(-(gravityDelta * (float)flip * gravityScale), 62);
        }
    }
    else
    {
        float gravityDelta = gravity * dt;
        int flip = flipMod();
        addToYVelocity(-(gravityDelta * (float)flip * gravityScale), 62);
    }

    if (m_isRobot && m_touchedPad && !m_wasRobotJump && m_gameLayer)
        gameEventTriggered(14, 0);

    if (playerIsFallingBugged() || (m_isPlatformer && !playerIsMovingUp()))
    {
        m_maybeIsBoosted = false;
        m_maybeIsFalling = true;
        m_isOnGround2 = false;
        m_fallStartY = getPosition().y;

        if (m_isRobot)
            m_robotSprite->tweenToAnimation("fall_loop", 0.1f);
        else if (m_isSpider)
            m_spiderSprite->tweenToAnimation("fall_loop", 0.1f);

        if (!m_touchedPad && m_gameLayer)
            gameEventTriggered(14, 0);
    }

    m_wasJumpBuffered = m_jumpBuffered;
    m_wasRobotJump = m_touchedPad;
}

void PlayerObject::updateJumpVariables()
{
    m_stateJumpBuffered = m_jumpBuffered;
    m_stateRingJump2 = m_stateRingJump;
    m_touchedCustomRing = false;
    m_touchedGravityPortal = false;
    m_maybeTouchedBreakableBlock = false;
}

void PlayerObject::updateLastGroundObject(GameObject* object)
{
    if (!object)
        return;

    m_lastGroundObject = object;

    if (object->m_isDontBoostY)
        m_stateBoostX = 2;
    if (object->m_isDontBoostX)
        m_stateBoostY = 2;
}

void PlayerObject::updateMove(float dt)
{
    double baseSpeed = m_playerSpeed * m_speedMultiplier;

    if (m_isOnSlope && !m_slopeFlipGravityRelated && !m_maybeUpsideDownSlope)
    {
        bool wasOnSlope = m_isOnSlope;
        float cosSlope = cosf(m_slopeAngle);
        m_isOnSlope = wasOnSlope;
        baseSpeed *= cosSlope;
    }

    double previousXVelocity = m_platformerXVelocity;
    float iceModifier = m_isOnIce ? 0.2f : 1.0f;
    double value22 = iceModifier;

    bool leftHeld = m_holdingLeft;
    bool rightHeld = m_holdingRight;

    if (m_holdingRight && m_holdingLeft)
    {
        if (m_leftPressedFirst)
        {
            if (m_isOnIce)
            {
                m_platformerMovingRight = 1;
                m_holdingRight = 0;
            }
            else if (m_isOnGround2)
            {
                m_platformerMovingRight = 1;
                m_holdingRight = 0;
            }
            else
            {
                m_holdingRight = 0;
                m_platformerMovingRight = 1;
            }
        }
        else
        {
            if (m_isOnIce)
            {
                m_platformerMovingRight = 1;
            }
            else
            {
                m_platformerMovingRight = 1;
                m_holdingLeft = 0;
            }
        }
    }
    else if (m_holdingLeft)
    {
        m_platformerMovingRight = 1;
    }
    else if (m_holdingRight)
    {
        m_platformerMovingLeft = 1;
    }

    if (m_isOnGround2 || !m_holdingLeft)
    {
    }
    else if (m_isSlidingRight != m_holdingLeft)
    {
        if (m_maybeSlidingTime > 0)
        {
            value22 = 0.200000003;
            m_isOnGround2 = false;
        }
        else
        {
            double slideTime = m_totalTime - m_maybeSlidingStartTime;
            if (slideTime < 0.5)
            {
                m_isOnGround2 = false;
                value22 = (slideTime + slideTime) * 0.800000012 + 0.200000003;
            }
        }
    }

    bool leftPressed = m_holdingLeft;
    bool rightPressed = m_holdingRight;

    if (!m_holdingLeft)
    {
        m_platformerMovingLeft = m_holdingRight;
    }
    else if (m_platformerMovingLeft)
    {
        m_platformerMovingLeft = 1;
    }

    bool doSlopeForce = false;
    if (m_isSliding)
    {
        doSlopeForce = true;
    }

    double deltaTime = dt;
    double halfDt = dt * 0.5;
    double baseDelta = halfDt * value22;

    if (fabs(m_platformerXVelocity) >= baseSpeed * 0.949999988 && m_isOnSlope && !m_maybeUpsideDownSlope)
    {
        float slopeTimeModifier = (m_yVelocityBeforeSlope < 0.0)
            ? m_yVelocityBeforeSlope / -15.0f * 0.3f + 1.0f
            : 1.0f;
        m_maybeSlopeForce += slopeTimeModifier * (dt * 0.25f);
    }

    if (m_isSliding)
    {
        baseSpeed += m_maybeSlopeForce;
        m_isSliding = true;
    }

    if (m_maybeSlidingTime > 0)
        m_isSliding = true;

    double leftAccelDelta = m_holdingLeft && m_platformerXVelocity < 0.0 ? baseDelta * 1.5 : baseDelta;

    if (m_platformerMovingLeft)
    {
        if (m_holdingLeft && m_platformerXVelocity > -baseSpeed)
        {
            m_platformerXVelocity -= leftAccelDelta;
        }
        else if (baseSpeed > m_platformerXVelocity && m_holdingRight)
        {
            m_platformerXVelocity -= leftAccelDelta;
        }
    }
    else
    {
        m_platformerVelocityRelated = 0.0f;
    }

    if (m_isOnGround2)
    {
        if (m_isRobot)
            m_robotSprite->runAnimation(m_currentRobotAnimation);
        else if (m_isSpider)
            playDynamicSpiderRun();
    }

    if (fabs(m_platformerXVelocity) >= 0.5f)
    {
        if (m_affectedByForces)
        {
        }
        else
        {
            m_affectedByForces = false;
        }
    }
    else
    {
        m_affectedByForces = false;
    }

    if (m_isMoving || m_affectedByForces)
    {
    }
    else if (m_maybeHasStopped)
    {
        if (!m_isOnIce)
        {
            m_decreaseBoostSlide = true;
        }
    }
    else if (m_maybeGoingCorrectSlopeDirection)
    {
        if (!m_isOnIce)
        {
            m_decreaseBoostSlide = false;
        }
        else
        {
            m_maybeGoingCorrectSlopeDirection = m_isOnIce;
        }
    }

    bool hasSliding = m_isSliding;
    bool hasSlidingRight = m_isSlidingRight;

    bool handleSliding = true;

    if (m_platformerMovingRight || hasSlidingRight)
    {
        if (m_holdingRight < m_platformerMovingRight || hasSlidingRight)
        {
        }
    }

    m_somethingPlayerSpeedTime = m_totalTime;

    if (m_isBall && !m_isBallRotating2 && !m_isDashing)
    {
        float sign = m_isSideways ? -1.0f : 1.0f;
        float velocity = m_isOnIce ? m_xVelocityRelated2 : m_platformerXVelocity;

        float rotAmount = (sign * ((velocity * dt) * (float)flipMod()) * 2.8f);
        if (m_isOnSlope)
            rotAmount /= cosf(m_slopeRotation);

        setRotation(getRotation() + rotAmount);
    }

    if (!m_isOnIce)
        m_xVelocityRelated2 = m_platformerXVelocity;
}

void PlayerObject::updatePlayerArt()
{
    int scaleX = reverseMod();
    m_mainLayer->setScaleX((float)scaleX);

    bool isSwingBallOrNormal = m_isSwing || m_isBall || isInNormalMode();

    float scaleY;
    if (isSwingBallOrNormal)
    {
        scaleY = 1.0f;
    }
    else
    {
        float sideSign = m_isSideways ? -1.0f : 1.0f;
        scaleY = sideSign * (float)flipMod();
    }

    m_mainLayer->setScaleY(scaleY);

    float rotation = m_isSideways ? -90.0f : 0.0f;
    m_mainLayer->setRotation(rotation);

    int flip = flipMod();

    float angle;
    if (m_isSideways)
        angle = m_isUpsideDown ? 180.0f : 0.0f;
    else
        angle = (float)(90 * flip);

    m_playerGroundParticles->setAngle(angle);
    m_trailingParticles->setAngle(angle);
    m_shipClickParticles->setAngle(angle);

    cocos2d::CCPoint gravity(0.0f, (float)(-300 * flip));
    if (m_isSideways)
        std::swap(gravity.x, gravity.y);

    m_playerGroundParticles->setGravity(&gravity);
    m_trailingParticles->setGravity(&gravity);
    m_shipClickParticles->setGravity(&gravity);

    cocos2d::CCPoint posVar(0.0f, 2.0f);
    if (m_isSideways)
        std::swap(posVar.x, posVar.y);

    m_trailingParticles->setPosVar(&posVar);
    m_shipClickParticles->setPosVar(&posVar);

    int flip2 = flipMod();
    float ufoAngle;
    if (m_isSideways)
        ufoAngle = m_isUpsideDown ? 60.0f : 240.0f;
    else
        ufoAngle = (float)(330 * flip2);

    m_ufoClickParticles->setAngle(ufoAngle);
    m_robotBurstParticles->setAngle(ufoAngle);

    cocos2d::CCPoint ufoPosVar(5.0f, 1.0f);
    if (m_isSideways)
        std::swap(ufoPosVar.x, ufoPosVar.y);

    m_ufoClickParticles->setPosVar(&ufoPosVar);
    m_robotBurstParticles->setPosVar(&ufoPosVar);

    cocos2d::CCPoint groundPosVar(5.0f, 1.0f);
    if (m_isRobot || m_isSpider)
        groundPosVar = cocos2d::CCPoint(15.0f, 0.0f);

    if (m_isSideways)
        std::swap(groundPosVar.x, groundPosVar.y);

    m_playerGroundParticles->setPosVar(&groundPosVar);
}

void PlayerObject::updatePlayerBirdFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 149 ? 149 : frame);

    GameManager::sharedState()->loadIcon(clampedFrame, 3, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("bird_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("bird_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* birdVehicleName = cocos2d::CCString::createWithFormat("bird_%02d_3_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("bird_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_vehicleSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_vehicleSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_birdVehicle->setDisplayFrame(cache->spriteFrameByName(birdVehicleName->getCString()));
    m_vehicleGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_vehicleSprite->getContentSize();
    m_vehicleSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
    m_birdVehicle->setPosition(m_vehicleSpriteSecondary->getPosition());

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("bird_%02d_extra_001.png", clampedFrame);
    updateShipSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updatePlayerDartFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 96 ? 96 : frame);

    GameManager::sharedState()->loadIcon(clampedFrame, 4, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("dart_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("dart_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("dart_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_iconSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_iconSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_iconGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_iconSprite->getContentSize();
    m_iconSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("dart_%02d_extra_001.png", clampedFrame);
    updatePlayerSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updatePlayerFrame(int frame)
{
    int clampedFrame;
    if (frame < 0)
        clampedFrame = 0;
    else
    {
        clampedFrame = (frame > 485) ? 485 : frame;
        if (clampedFrame)
            m_maybeSavedPlayerFrame = clampedFrame;
    }

    GameManager::sharedState()->loadIcon(clampedFrame, 0, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("player_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("player_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("player_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_iconSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_iconSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_iconGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_iconSprite->getContentSize();
    m_iconSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("player_%02d_extra_001.png", clampedFrame);
    updatePlayerSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updatePlayerGlow()
{
    m_iconGlow->setScaleX(m_iconSprite->getScaleX());
    m_iconGlow->setScaleY(m_iconSprite->getScaleY());
    m_iconGlow->setPosition(m_iconSprite->getPosition());

    if (m_maybeIsVehicleGlowing || m_hasGlow)
    {
        if (!m_isRobot && !m_isSpider)
        {
            m_iconGlow->setVisible(true);
            m_vehicleGlow->setVisible(m_vehicleSprite->isVisible());
            m_vehicleGlow->setPosition(m_vehicleSprite->getPosition());
        }
    }
    else
    {
        m_iconGlow->setVisible(false);
        m_vehicleGlow->setVisible(false);
        m_vehicleGlow->setPosition(m_vehicleSprite->getPosition());
    }

    if (m_maybeIsVehicleGlowing)
    {
        if (m_robotSprite->isVisible())
            m_robotSprite->showGlow();
        else
            m_robotSprite->hideGlow();

        if (m_spiderSprite->isVisible())
            m_spiderSprite->showGlow();
        else
            m_spiderSprite->hideGlow();
    }
    else
    {
        m_robotSprite->hideGlow();
        m_spiderSprite->hideGlow();
    }
}

void PlayerObject::updatePlayerJetpackFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 8 ? 8 : frame);

    GameManager::sharedState()->loadIcon(clampedFrame, 8, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("jetpack_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("jetpack_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("jetpack_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_vehicleSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_vehicleSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_vehicleGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_vehicleSprite->getContentSize();
    m_vehicleSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("jetpack_%02d_extra_001.png", clampedFrame);
    updateShipSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updatePlayerRobotFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 68 ? 68 : frame);
    createRobot(clampedFrame);
}

void PlayerObject::updatePlayerRollFrame(int frame)
{
    int clampedFrame = (frame > 118) ? 118 : frame;
    if (clampedFrame < 0)
        clampedFrame = 0;

    GameManager::sharedState()->loadIcon(clampedFrame, 2, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("player_ball_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("player_ball_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("player_ball_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_iconSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_iconSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_iconGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_iconSprite->getContentSize();
    m_iconSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("player_ball_%02d_extra_001.png", clampedFrame);
    updatePlayerSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updatePlayerScale()
{
    m_actionManager->stopInternalAction(6);
    setScaleX(m_vehicleSize);
    setScaleY(m_vehicleSize);
}

void PlayerObject::updatePlayerShipFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 169 ? 169 : frame);

    GameManager::sharedState()->loadIcon(clampedFrame, 1, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("ship_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("ship_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("ship_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_vehicleSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_vehicleSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_vehicleGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_vehicleSprite->getContentSize();
    m_vehicleSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("ship_%02d_extra_001.png", clampedFrame);
    updateShipSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updatePlayerSpiderFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 69 ? 69 : frame);
    createSpider(clampedFrame);
}

void PlayerObject::updatePlayerSpriteExtra(std::string frameName)
{
    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    cocos2d::CCSpriteFrame* frame = cache->spriteFrameByName(frameName.c_str());

    if (frame)
    {
        m_iconSpriteWhitener->setDisplayFrame(frame);
        m_iconSpriteWhitener->setVisible(true);

        cocos2d::CCSize size = m_iconSprite->getContentSize();
        m_iconSpriteWhitener->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
    }
    else
    {
        m_iconSpriteWhitener->setVisible(false);
    }
}

void PlayerObject::updatePlayerSwingFrame(int frame)
{
    int clampedFrame = (frame <= 0) ? 1 : (frame > 43 ? 43 : frame);

    GameManager::sharedState()->loadIcon(clampedFrame, 7, m_iconRequestID);

    cocos2d::CCString* mainName = cocos2d::CCString::createWithFormat("swing_%02d_001.png", clampedFrame);
    cocos2d::CCString* secondaryName = cocos2d::CCString::createWithFormat("swing_%02d_2_001.png", clampedFrame);
    cocos2d::CCString* glowName = cocos2d::CCString::createWithFormat("swing_%02d_glow_001.png", clampedFrame);

    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    m_iconSprite->setDisplayFrame(cache->spriteFrameByName(mainName->getCString()));
    m_iconSpriteSecondary->setDisplayFrame(cache->spriteFrameByName(secondaryName->getCString()));
    m_iconGlow->setDisplayFrame(cache->spriteFrameByName(glowName->getCString()));

    cocos2d::CCSize size = m_iconSprite->getContentSize();
    m_iconSpriteSecondary->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));

    cocos2d::CCString* extraName = cocos2d::CCString::createWithFormat("swing_%02d_extra_001.png", clampedFrame);
    updatePlayerSpriteExtra(std::string(extraName->getCString()));
}

void PlayerObject::updateRobotAnimationSpeed()
{
    float animSpeed = 0.9f;
    float speedFactor = 1.0f;

    if (m_playerSpeed == 0.9f)
    {
        speedFactor = 1.0f;
        animSpeed = 1.0f;
        if (m_vehicleSize < 1.0f)
        {
            speedFactor = 0.85f;
            animSpeed = animSpeed / 0.85f;
        }
    }
    else if (m_playerSpeed == 0.7f)
    {
        speedFactor = 1.0f;
        if (m_vehicleSize < 1.0f)
        {
            speedFactor = 0.85f;
            animSpeed = animSpeed / 0.85f;
        }
    }
    else if (m_playerSpeed == 1.1f)
    {
        animSpeed = 1.15f;
        if (m_vehicleSize < 1.0f)
        {
            speedFactor = 0.935f;
            animSpeed = animSpeed / 0.85f;
        }
        else
        {
            speedFactor = 1.1f;
        }
    }
    else if (m_playerSpeed == 1.3f)
    {
        animSpeed = 1.3f;
        if (m_vehicleSize < 1.0f)
        {
            speedFactor = 0.935f;
            animSpeed = animSpeed / 0.85f;
        }
        else
        {
            speedFactor = 1.1f;
        }
    }
    else if (m_playerSpeed == 1.6f)
    {
        animSpeed = 1.5f;
        if (m_vehicleSize < 1.0f)
        {
            speedFactor = 0.935f;
            animSpeed = animSpeed / 0.85f;
        }
        else
        {
            speedFactor = 1.1f;
        }
    }
    else
    {
        animSpeed = 1.0f;
        speedFactor = 1.0f;
        if (m_vehicleSize < 1.0f)
        {
            animSpeed = 1.1765f;
            speedFactor = 0.85f;
        }
        if (m_playerSpeed >= 1.1f)
            speedFactor *= 1.1f;
    }

    m_robotSprite->m_animationManager->updateAnimationSpeed(animSpeed);
    m_spiderSprite->m_animationManager->updateAnimationSpeed(animSpeed / speedFactor);

    if (!m_isSpider)
    {
    }
    else
    {
        std::string currentAnim = m_spiderSprite->m_animationManager->m_currentAnimation;

        if (currentAnim == "walk")
        {
            playDynamicSpiderRun();
        }
        else if (currentAnim == "run")
        {
            if (m_robotAnimation1Enabled && m_playerSpeed == 0.7f)
                m_currentRobotAnimation = "run3";
            else if (m_robotAnimation2Enabled && (m_playerSpeed == 1.3f || m_playerSpeed == 1.6f))
                m_currentRobotAnimation = "run2";
            else
                m_currentRobotAnimation = "run";
        }
    }

    if (m_robotAnimation1Enabled && m_playerSpeed == 0.7f)
        m_currentRobotAnimation = "run3";
    else if (m_robotAnimation2Enabled && (m_playerSpeed == 1.3f || m_playerSpeed == 1.6f))
        m_currentRobotAnimation = "run2";
    else
        m_currentRobotAnimation = "run";

    if (!m_isRobot)
        return;

    std::string activeAnim = m_robotSprite->m_animationManager->m_currentAnimation;

    if (activeAnim != m_currentRobotAnimation)
        m_robotSprite->runAnimation(m_currentRobotAnimation);
}

void PlayerObject::updateRotation(float dt)
{
    if (m_isLocked || m_isDashing)
        return;

    if (m_isBall)
    {
        if (m_rotationSpeed != 0.0f)
        {
            if (m_isBallRotating && m_isBall)
                m_rotateSpeed = 1.0f;
            else
                m_rotateSpeed = 1.0f;

            float currentRotation = getRotation();
            setRotation(currentRotation + (dt / 60.0f) * m_rotationSpeed * m_rotateSpeed);
        }
        return;
    }

    if (m_isOnSlope)
    {
        updateSlopeRotation(dt);
    }
    else if (isFlying())
    {
        updateShipRotation(dt);
    }
    else if (m_isRobot || m_isSpider)
    {
        if (!m_isOnSlope && !m_wasOnSlope)
            updateRotation(dt * 0.5f, 0.0f);
    }
    else if (!m_isOnGround2 || playerIsFalling(-0.25f))
    {
        if (!m_isOnSlope && !m_wasOnSlope)
            updateRotation(dt * 0.5f, 0.0f);
    }
    else if (m_wasOnSlope)
    {
        if (!m_isRobot && m_isSpider)
            updateRotation(dt * 0.5f, 0.0f);
    }
    else
    {
        if (m_isPlatformer && m_isRotating)
            return;

        stopRotation(false, 2);
        float targetRotation = convertToClosestRotation(0.0f);
        updateRotation(dt, targetRotation);
    }

    if (m_rotationSpeed != 0.0f)
    {
        if (!(m_isBallRotating && m_isBall))
            m_rotateSpeed = 1.0f;

        float currentRotation = getRotation();
        setRotation(currentRotation + (dt / 60.0f) * m_rotationSpeed * m_rotateSpeed);
    }
}

void PlayerObject::updateRotation(float dt, float rotation)
{
    if (m_isLocked || m_isDashing)
        return;

    float currentRotation = getRotation();
    float rotationDelta = rotation - currentRotation;

    if (fabsf(rotationDelta) > 0.1f)
    {
        float step = dt * 10.0f;
        if (rotationDelta > 0.0f)
            step = -step;

        if (fabsf(rotationDelta) < fabsf(step))
            currentRotation = rotation;
        else
            currentRotation -= step * (rotationDelta > 0.0f ? -1.0f : 1.0f);

        setRotation(currentRotation);
    }
    else
    {
        setRotation(rotation);
    }

    if (m_isBall && m_rotationSpeed != 0.0f)
    {
        if (!(m_isBallRotating && m_isBall))
            m_rotateSpeed = 1.0f;

        float rot = getRotation();
        setRotation(rot + (dt / 60.0f) * m_rotationSpeed * m_rotateSpeed);
    }
}

void PlayerObject::updateShipRotation(float dt)
{
    if (m_isOnSlope || m_isDashing)
        return;

    cocos2d::CCPoint currentPos = getPosition();

    bool sidewaysOrFlying = m_isBird || m_isShip || m_isSwing || m_isDart;

    float deltaX;
    float deltaY;

    if (sidewaysOrFlying)
    {
        if (m_isSideways)
        {
            if (m_isGoingLeft)
                deltaY = fabsf(currentPos.y - m_shipRotation.y);
            else
                deltaY = currentPos.y - m_shipRotation.y;

            deltaX = currentPos.x - m_shipRotation.x;
        }
        else
        {
            if (m_isGoingLeft)
                deltaX = fabsf(currentPos.x - m_shipRotation.x);
            else
                deltaX = currentPos.x - m_shipRotation.x;

            deltaY = -(currentPos.y - m_shipRotation.y);
        }
    }
    else
    {
        deltaX = currentPos.x - m_shipRotation.x;
        deltaY = -(currentPos.y - m_shipRotation.y);
    }

    float distanceSq = deltaX * deltaX + deltaY * deltaY;

    if ((dt * 1.2f) > distanceSq
        && (!m_isPlatformer || (!m_isShip && !m_isBird)))
        return;

    float targetAngle = atan2f(deltaY, deltaX);
    float currentRotation = getRotation();

    if (m_isBird)
        targetAngle *= -0.4f;

    if (m_isCollidingWithSlope && isFlying())
        targetAngle = 0.0f;

    if (sidewaysOrFlying && m_isGoingLeft)
        targetAngle = -targetAngle;

    float currentRadians = currentRotation * 0.017453f;

    float speed;

    if (m_isBird)
    {
        if (m_isGoingLeft == m_isUpsideDown)
        {
            if (targetAngle > -0.1f)
                targetAngle = -0.1f;
        }
        else
        {
            if (targetAngle < 0.1f)
                targetAngle = 0.1f;
        }

        if (m_isPlatformer)
        {
            speed = 0.07f;
            if (fabs(m_platformerXVelocity) <= 0.5f)
                targetAngle = 0.0f;
            else if (m_isOnGround2)
                targetAngle = 0.0f;
        }
        else
        {
            speed = 0.07f;
        }
    }
    else if (m_isDart)
    {
        speed = (m_vehicleSize == 1.0f) ? 0.25f : 0.4f;
    }
    else if (m_isPlatformer && m_isShip)
    {
        if (!m_holdingLeft && !m_holdingRight && !m_platformerMovingLeft && !m_platformerMovingRight)
        {
            speed = 0.06f;
            targetAngle = 0.0f;
        }
        else
        {
            float referenceAngle = currentRadians;
            int reverse = reverseMod();
            float sign = (m_isUpsideDown == m_isSideways) ? 1.0f : -1.0f;
            float target = (float)reverse * sign;
            targetAngle = target * 0.5236f;

            if (fabsf(target * 0.5236f) - fabsf(referenceAngle) >= 0.100000001f)
            {
                speed = 0.07f;
            }
            else
            {
                speed = 0.015f;
                targetAngle = target * 0.7854f;
            }
        }
    }
    else
    {
        speed = 0.15f;
    }

    if (m_isCollidingWithSlope && isFlying())
        speed *= 0.25f;

    float step = dt * speed;
    if (dt <= dt * speed)
        step = dt;

    float newAngle = Slerp2D(currentRadians, targetAngle, step);
    setRotation(newAngle * 57.296f);
}

void PlayerObject::updateShipSpriteExtra(std::string frameName)
{
    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    cocos2d::CCSpriteFrame* frame = cache->spriteFrameByName(frameName.c_str());

    if (frame)
    {
        m_vehicleSpriteWhitener->setDisplayFrame(frame);
        m_vehicleSpriteWhitener->setVisible(true);

        cocos2d::CCSize size = m_vehicleSprite->getContentSize();
        m_vehicleSpriteWhitener->setPosition(cocos2d::CCPoint(size.width * 0.5f, size.height * 0.5f));
    }
    else
    {
        m_vehicleSpriteWhitener->setVisible(false);
    }
}

void PlayerObject::updateSlopeRotation(float dt)
{
    float targetRotation = convertToClosestRotation(m_slopeRotation * 57.2957802f);

    if (m_isPlatformer && m_maybeChangedDirectionAngle > 80.0f)
    {
        if (m_platformerMovingLeft || m_platformerMovingRight)
            targetRotation = 0.0f;
    }

    updateRotation(dt, targetRotation);
}

void PlayerObject::updateSlopeYVelocity(float yVelocity)
{
}

void PlayerObject::updateSpecial(float dt)
{
    float value = dt + m_unk838;
    m_unk838 = value;

    if (value >= 0.01f)
    {
        int newIndex = m_followRelated + 1;
        m_followRelated = newIndex;
        float y = m_obPosition.y;
        m_unk838 = value - 0.01f;
        m_playerFollowFloats[newIndex % 200] = y;
    }
    else
    {
        m_playerFollowFloats[m_followRelated % 200] = m_obPosition.y;
    }
}
void PlayerObject::updateStateVariables()
{
    int dartSlide = m_stateDartSlide - 1;
    int flipGravity = m_stateFlipGravity - 1;
    int hitHead = m_stateHitHead - 1;
    int onGround = m_stateOnGround - 1;
    int boostX = m_stateBoostX - 1;
    int boostY = m_stateBoostY - 1;
    int force2 = m_maybeStateForce2 - 1;
    int scale = m_stateScale - 1;

    m_stateNoAutoJump--;
    m_stateDartSlide = dartSlide;
    m_stateFlipGravity = flipGravity;
    m_stateHitHead = hitHead;
    m_stateOnGround = onGround;
    m_stateBoostX = boostX;
    m_stateBoostY = boostY;
    m_maybeStateForce2 = force2;
    m_stateScale = scale;
    m_stateForce--;

    m_stateForceVector = cocos2d::CCPoint(0.0f, 0.0f);
    m_jumpPadRelated.clear();
}

void PlayerObject::updateStreakBlend(bool blend)
{
    cocos2d::ccBlendFunc blendFunc;
    blendFunc.src = GL_SRC_ALPHA;

    if (blend)
        blendFunc.dst = GL_ONE;
    else
        blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

    m_regularTrail->setBlendFunc(blendFunc);
    m_waveTrail->m_isSolid = !blend;

    cocos2d::ccBlendFunc waveBlendFunc = blendFunc;
    m_waveTrail->setBlendFunc(&waveBlendFunc);
}

void PlayerObject::updateStreaks(float dt)
{
    if (m_shipStreak)
        m_shipStreak->update(dt);
}

void PlayerObject::updateSwingFire()
{
    if (m_isUpsideDown != m_isSideways)
    {
        PlayerFireBoostSprite* tempFire = m_swingFireTop;
        cocos2d::CCParticleSystemQuad* tempParticles = m_swingBurstParticles1;

        m_swingFireTop = m_swingFireBottom;
        m_swingBurstParticles1 = m_swingBurstParticles2;
        m_swingFireBottom = tempFire;
        m_swingBurstParticles2 = tempParticles;
    }

    m_swingFireTop->animateFireIn();
    m_swingBurstParticles2->resumeSystem();
    m_swingFireBottom->animateFireOut();
    m_swingBurstParticles1->stopSystem();
}

void PlayerObject::updateTimeMod(float speed, bool noEffects)
{
    if (!m_maybeReducedEffects && m_playEffects > (unsigned int)noEffects && m_playerSpeed != speed)
    {
        GameManager* gameManager = GameManager::sharedState();
        gameManager->m_playLayer->playSpeedParticle(speed);
    }

    m_playerSpeed = speed;

    if (speed == 0.9f)
    {
        m_yStart = 11.1800318f;
        m_gravity = 0.958199024;
        m_speedMultiplier = 5.77000189;
    }
    else if (speed == 0.7f)
    {
        m_yStart = 10.620032f;
        m_gravity = 0.940199;
        m_speedMultiplier = 5.980002;
    }
    else if (speed == 1.1f)
    {
        m_yStart = 11.420032f;
        m_gravity = 0.957199;
        m_speedMultiplier = 5.870002;
    }
    else if (speed == 1.3f || speed == 1.6f)
    {
        m_yStart = 11.230032f;
        m_gravity = 0.961199;
        m_speedMultiplier = 6.000002;
    }
    else
    {
        m_speedMultiplier = 5.77000189;
    }

    updateRobotAnimationSpeed();

    if (m_isBall)
        runRotateAction(false, 9);

    if (m_shipStreak)
        updateStreakSettings(m_shipStreak, m_shipStreakType, this);
}

bool PlayerObject::usingWallLimitedMode()
{
    if (!isFlying() && !m_isBall)
        return false;

    return true;
}

void PlayerObject::yStartDown()
{
    m_yStart -= 0.00999999978f;
    logValues();
}

void PlayerObject::yStartUp()
{
    m_yStart += 0.00999999978f;
    logValues();
}

PlayerObject::~PlayerObject()
{
}