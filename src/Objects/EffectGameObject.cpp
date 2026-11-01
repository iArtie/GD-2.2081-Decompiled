// EffectGameObject.cpp
#include "Objects/EffectGameObject.h"
#include "Managers/GameManager.h"
#include "Layers/GJBaseGameLayer.h"
#include "Editor/ObjectToolbox.h"

EffectGameObject::EffectGameObject() {
    m_followers = nullptr;
    m_followingSprite = nullptr;
    m_hasFollower = false;
    m_propagateScaleChanges = false;
    m_propagateFlipChanges = false;
    m_someOtherIndex = -1;
    m_innerSectionIndex = -1;
    m_middleSectionIndex = 0;
    m_hasExtendedCollision = false;
    m_isColorSpriteBlack = false;
    m_isObjectBlack = false;
    m_blackChildOpacityLocked = false;
    m_editorEnabled = false;
    m_isGroupDisabled = false;
    m_isGroupDisabledTemp = false;
    m_unk28c = false;
    m_activeMainColorID = 0;
    m_activeDetailColorID = 0;
    m_baseUsesHSV = false;
    m_detailUsesHSV = false;
    m_blackChildOpacity = 0.0f;
    m_positionXOffset = 0.0f;
    m_positionYOffset = 0.0f;
    m_rotationXOffset = 0.0f;
    m_unk2A8 = 0.0f;
    m_rotationYOffset = 0.0f;
    m_unk2B0 = 0.0f;
    m_scaleXOffset = 0.0f;
    m_scaleYOffset = 0.0f;
    m_tempOffsetXRelated = 0;
    m_isFlipX = false;
    m_isFlipY = false;
    m_unk2BC = 0.0f;
    m_unk2C0 = 0.0f;
    m_customBoxOffset = CCPointZero;
    m_boxOffsetCalculated = true;
    m_boxOffset = CCPointZero;
    m_orientedBox = false;
    m_shouldUseOuterOb = false;
    m_glowSprite = nullptr;
    m_isRingPoweredOn = false;
    m_addToNodeContainer = false;
    m_isActivated = false;
    m_isDisabled2 = false;
    m_particle = nullptr;
    m_width = 0.0f;
    m_height = 0.0f;
    m_hasParticles = false;
    m_particleUseObjectColor = false;
    m_particleString = "";
    m_hasColorSprite = false;
    m_particleOffset = CCPointZero;
    m_isParticleSpriteLocked = false;
    m_textureRect = CCRectZero;
    m_isDirty = false;
    m_isObjectPosDirty = false;
    m_isUnmodifiedPosDirty = true;
    m_fadeMargin = 0.0f;
    m_objectRect = CCRectZero;
    m_isObjectRectDirty = false;
    m_isOrientedBoxDirty = false;
    m_colorSpriteLocked = false;
    m_unk353 = false;
    m_canRotateFree = false;
    m_isMirroredByScale = false;
    m_linkedGroup = 0;
    m_unk35C = 0;
    m_colorType = 0;
    m_childColorType = 0;
    m_shouldBlendBase = false;
    m_shouldBlendDetail = false;
    m_hasCustomChild = false;
    m_unk367 = false;
    m_colorSprite = nullptr;
    m_unk370 = nullptr;
    m_isRotationAligned = false;
    m_uniqueID = 0;
    m_objectType = Solid;
    m_unk390 = 7;
    m_objectRadius = 0.0f;
    m_spriteWidthScale = 0.0f;
    m_spriteHeightScale = 0.0f;
    m_unmodifiedPositionX = 0.0f;
    m_unmodifiedPositionY = 0.0f;
    m_positionX = 0.0f;
    m_positionY = 0.0f;
    m_startPosition = CCPointZero;
    m_parentMode = -1;
    m_usesAudioScale = false;
    m_hasNoAudioScale = false;
    m_targetColor = 1;
    m_isDisabled = false;
    m_startScaleX = 1.0f;
    m_startFlipX = false;
    m_startRotationX = 0.0f;
    m_startFlipY = false;
    m_startRotationY = 0.0f;
    m_unk3ee = false;
    m_startScaleY = 1.0f;
    m_isInvisible = false;
    m_customScaleX = 0.0f;
    m_unk3DE = false;
    m_customScaleY = 0.0f;
    m_enterType = 0;
    m_unk3D8 = 0;
    m_exitType = 0;
    m_enterChannel = 0;
    m_objectMaterial = 0;
    m_unk3E8 = 0;
    m_hasNoGlow = false;
    m_objectID = 0;
    m_unk3F8 = 0;
    m_intrinsicDontFade = false;
    m_ignoreEnter = false;
    m_ignoreFade = false;
    m_isSolidColorBlock = false;
    m_unk3FD = false;
    m_customSpriteColor = 0;
    m_customColorType = 0;
    m_isDontEnter = false;
    m_isDontFade = false;
    m_hasNoEffects = false;
    m_hasNoParticles = false;
    m_defaultZOrder = 0;
    m_unk40C = 0;
    m_colorZLayerRelated = 0;
    m_customAudioScale = 0;
    m_particleLocked = false;
    m_property53 = false;
    m_isInvisibleBlock = false;
    m_customGlowColor = 0;
    m_glowColorIsLBG = false;
    m_cantColorGlow = false;
    m_slopeUphill = false;
    m_slopeDirection = 0;
    m_slopeIsHazard = false;
    m_baseColor = 0;
    m_detailColor = 0;
    m_baseOrDetailBlending = 0;
    m_defaultZLayer = T1;
    m_zFixedZLayer = 0;
    m_zLayer = 0;
    m_zOrder = 0;
    m_wasSelected = false;
    m_isSelected = false;
    m_unk460 = 0.0f;
    m_minAudioScale = 0.0f;
    m_maxAudioScale = 0.0f;
    m_opacityMod = 0.0f;
    m_opacityMod2 = 0.0f;
    m_unk464 = CCPointZero;
    m_updateParents = false;
    m_updateEditorColor = false;
    m_hasGroupParent = false;
    m_hasAreaParent = false;
    m_groups = nullptr;
    m_groupCount = 0;
    m_hasGroupParentsString = false;
    m_colorGroups = nullptr;
    m_colorGroupCount = 0;
    m_opacityGroups = nullptr;
    m_opacityGroupCount = 0;
    m_editorLayer = 0;
    m_editorLayer2 = 0;
    m_enabledGroupsCounter = 0;
    m_updateCustomContentSize = false;
    m_hasContentSize = false;
    m_isNoTouch = false;
    m_scaleX = 1.0f;
    m_scaleY = 1.0f;
    m_lastSize = CCSizeZero;
    m_lastPosition = CCPointZero;
    m_unk4C0 = -1;
    m_unk4C4 = -1;
    m_unk4C8 = -1;
    m_unk4CC = -1;
    m_classType = Game;
    m_isTrigger = false;
    m_isSpawnOrderTrigger = false;
    m_isColorTrigger = false;
    m_dontIgnoreDuration = false;
    m_canBeControlled = false;
    m_activateTriggerInEditor = false;
    m_isHighDetail = false;
    m_mainActionSprite = nullptr;
    m_detailActionSprite = nullptr;
    m_goEffectManager = nullptr;
    m_unk4F8 = 0;
    m_isDecoration = false;
    m_isDecoration2 = false;
    m_unk4fb = false;
    m_maybeNotColorable = false;
    m_isPassable = false;
    m_isHide = false;
    m_isNonStickX = false;
    m_isNonStickY = false;
    m_isIceBlock = false;
    m_isGripSlope = false;
    m_isScaleStick = false;
    m_isExtraSticky = false;
    m_isDontBoostY = false;
    m_isDontBoostX = false;
    m_unk507 = false;
    m_unk508 = 0;
    m_unk50C = 1.0f;
    m_pixelScaleX = 1.0f;
    m_pixelScaleY = 1.0f;
    m_areaOpacityRelated = -1;
    m_mainColorKeyIndex = 0;
    m_frames = 1;
    m_detailColorKeyIndex = 0;
    m_areaOpacityValue = 0.0f;
    m_areaOpacityIndex = 0;
    m_unk52C = 0;
    m_unk530 = 0;
    m_isUIObject = false;
    m_greenDebugDraw = false;
    m_poweredOn = false;
    m_state = 0;
    m_animationRandomizedStartValue = 0;
    m_unk548 = 0;
    m_animationStart = 0.0f;
    m_visible = false;
    m_unk540 = 0.0f;
    m_shouldNotHideAnimFreeze = false;
    m_unk544 = 0.0f;
    m_usesSpecialAnimation = false;
    m_randomFrameTime = 1.0f;
    m_hasCustomAnimation = false;
    m_frameTime = 1.0f;
    m_hasCustomRotation = false;
    m_disableRotation = false;
    m_rotationSpeed = 0.0f;
    m_animationRandomizedStart = false;
    m_rotationAngle = 0.0f;
    m_animationShouldUseSpeed = false;
    m_rotationDelta = 0.0f;
    m_rotationAnimationSpeed = 0.0f;
    m_animationSpeed = 1.0f;
    m_animateOnTrigger = false;
    m_disableDelayedLoop = false;
    m_disableAnimShine = false;
    m_singleFrame = false;
    m_animationOffset = 0;
    m_animationTriggered = false;
    m_unkAnimationInt = 0;
    m_maybeAnimationVariableXInt = 0;
    m_maybeAnimationVariableYInt = 0;
    m_animateOnlyWhenActive = false;
    m_isNoMultiActivate = false;
    m_isMultiActivate = false;
    m_activated = false;
    m_activatedByPlayer1 = false;
    m_activatedByPlayer2 = false;
    m_hasUniqueCoin = false;
    m_unknownBool = false;

    // Effect specific
    m_duration = 0.0f;
    m_triggerEffectPlaying = false;
    m_opacity = 1.0f;
    m_targetGroupID = 0;
    m_centerGroupID = 0;
    m_isTouchTriggered = false;
    m_isSpawnTriggered = false;
    m_hasCenterEffect = false;
    m_shakeStrength = 0.0f;
    m_tintGround = false;
    m_shakeInterval = 0.0f;
    m_usesPlayerColor1 = false;
    m_usesPlayerColor2 = false;
    m_usesBlending = false;
    m_moveOffset = CCPointZero;
    m_easingType = 0;
    m_directionModeDistance = 0.0f;
    m_lockToPlayerX = false;
    m_rotationDegrees = 0.0f;
    m_lockToPlayerY = false;
    m_rotationOffset = 0.0f;
    m_lockToCameraX = false;
    m_followYDelay = 0.0f;
    m_lockToCameraY = false;
    m_followYMaxSpeed = 0.0f;
    m_useMoveTarget = false;
    m_fadeInDuration = 0.0f;
    m_moveTargetMode = Both;
    m_holdDuration = 0.0f;
    m_smallStep = false;
    m_fadeOutDuration = 0.0f;
    m_isDirectionFollowSnap360 = false;
    m_easingRate = 2.0f;
    m_targetModCenterID = 0;
    m_moveModX = 1.0f;
    m_isDynamicMode = false;
    m_moveModY = 1.0f;
    m_isSilent = false;
    m_specialTarget = 0;
    m_times360 = 0;
    m_lockObjectRotation = false;
    m_rotationTargetID = 0;
    m_dynamicModeEasing = 0;
    m_followXMod = 1.0f;
    m_followYOffset = 0;
    m_followYMod = 1.0f;
    m_pulseMode = 0;
    m_followYSpeed = 1.0f;
    m_pulseTargetType = 0;
    m_hsvValue = HSV();
    m_copyColorID = 0;
    m_spawnXPosition = 0.0f;
    m_copyOpacity = false;
    m_pulseMainOnly = false;
    m_pulseDetailOnly = false;
    m_pulseExclusive = false;
    m_legacyHSV = false;
    m_activateGroup = 0;
    m_touchHoldMode = 0;
    m_touchToggleMode = 0;
    m_touchPlayerMode = 0;
    m_isDualMode = false;
    m_animationID = 0;
    m_spawnOrder = 0;
    m_isMultiTriggered = false;
    m_previewDisable = false;
    m_spawnOrdered = false;
    m_triggerOnExit = false;
    m_itemID2 = 0;
    m_controlID = 0;
    m_targetControlID = 0;
    m_isDynamicBlock = false;
    m_itemID = 0;
    m_targetPlayer1 = 0;
    m_targetPlayer2 = 0;
    m_followCPP = 0;
    m_subtractCount = false;
    m_collectibleIsPickupItem = false;
    m_collectibleIsToggleTrigger = false;
    m_collectibleParticleID = 0;
    m_collectiblePoints = 0;
    m_hasNoAnimation = false;
    m_unk6a8 = 0.0f;
    m_unk698 = 0;
    m_unk6ac = 0.0f;
    m_unk6b4 = 0;
    m_unk6b0 = 0.0f;
    m_isSinglePTouch = false;
    m_cameraIsFreeMode = false;
    m_cameraEditCameraSettings = false;
    m_cameraEasingValue = 10.0f;
    m_cameraDisableGridSnap = false;
    m_endReversed = false;
    m_shouldPreview = false;
    m_ordValue = 0;
    m_channelValue = 0;
    m_isReverse = false;
    m_speedModType = 0;
    m_gravityValue = 1.0f;
    m_zoomValue = 1.0f;
    m_cameraPaddingValue = 0.5f;
    m_timeWarpTimeMod = 1.0f;
    m_speedStart = CCPointZero;
    m_secretCoinID = 0;
    m_unk6f4 = 0;
    m_unk6f5 = false;
    m_endPosition = CCPointZero;
    m_unk708 = 0;
    m_objectLabel = nullptr;
    m_ignoreGroupParent = false;
    m_ignoreLinkedObjects = false;
    m_channelChanged = false;
    m_canSpawnTriggers = false;
    m_spawnTriggerDelay = 0.0f;
    m_gravityMod = 0.0f;
}

EffectGameObject* EffectGameObject::create(const char* frame) {
    EffectGameObject* instance = new EffectGameObject();
    if (instance && instance->init(frame)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool EffectGameObject::init(const char* frame) {
    if (!EnhancedGameObject::init(frame)) {
        return false;
    }
    
    m_classType = Effect;
    m_triggerTargetColor = ccc3(-1, -1, -1);
    m_legacyHSV = true;
    m_duration = 0.5f;
    
    return true;
}

EffectGameObject::~EffectGameObject() {
    // Cleanup handled by base class
}

bool EffectGameObject::canBeOrdered() {
    if (m_isTrigger) {
        return true;
    }
    if (canReverse()) {
        return true;
    }
    if (m_objectID == 1334 || (m_objectID >= 200 && m_objectID <= 203) || m_objectID == 2902) {
        return true;
    }
    return false;
}

bool EffectGameObject::canReverse() {
    if (m_objectID == 1022 || m_objectID == 67 || m_objectID == 84) {
        return true;
    }
    if (m_objectID >= 35 && m_objectID <= 36) {
        return true;
    }
    if (m_objectID >= 140 && m_objectID <= 141) {
        return true;
    }
    if (m_objectID == 1330 || m_objectID == 1332 || m_objectID == 1333) {
        return true;
    }
    if (m_objectID >= 3004 && m_objectID <= 3005) {
        return true;
    }
    if (m_objectID == 3027) {
        return true;
    }
    return false;
}

bool EffectGameObject::canSpawnTriggers() {
    if (m_objectID == 1912 || m_objectID == 1268 || m_objectID == 1329 || m_objectID == 142) {
        return true;
    }
    if (m_objectID == 1594 || m_objectID == 1595 || m_objectID == 1611 || m_objectID == 1811 || m_objectID == 1815) {
        return true;
    }
    if (m_objectID == 2068 || m_objectID == 2063 || m_objectID == 3604 || m_objectID == 3607 || m_objectID == 3609) {
        return true;
    }
    if (m_objectID == 3620 || m_objectID == 3640 || m_objectID == 3643) {
        return true;
    }
    if (m_objectType == Collectible) {
        return true;
    }
    return false;
}

void EffectGameObject::customSetup() {
    EnhancedGameObject::customSetup();
    
    switch (m_objectID) {
        case 29:
            m_usesBlending = false;
            m_targetColor = 1000;
            m_opacity = 1.0f;
            break;
        case 30:
            m_usesBlending = false;
            m_targetColor = 1001;
            m_opacity = 1.0f;
            break;
        case 105:
            m_usesBlending = false;
            m_targetColor = 1004;
            m_opacity = 1.0f;
            break;
        case 142:
            m_objectType = SecretCoin;
            m_defaultZOrder = 9;
            setDefaultMainColorMode(0);
            m_hasCustomAnimation = true;
            break;
        case 744:
            m_targetColor = 1003;
            break;
        case 900:
            m_usesBlending = false;
            m_targetColor = 1009;
            m_opacity = 1.0f;
            break;
        case 915:
            m_targetColor = 1002;
            m_usesBlending = true;
            break;
        case 1329:
            m_objectType = Collectible | 0x1;
            m_defaultZOrder = 9;
            if (!m_editorEnabled) {
                GameManager* gm = GameManager::sharedState();
                if (gm->m_playLayer && gm->m_playLayer->m_level) {
                    if (gm->m_playLayer->m_level->m_coinsVerified.random == gm->m_playLayer->m_level->m_coinsVerified.seed &&
                        gm->m_playLayer->m_level->m_levelType != Editor) {
                        setObjectColor(ccc3(75, 175, 255));
                    }
                }
            }
            m_isObjectBlack = true;
            setDefaultMainColorMode(0);
            m_hasCustomAnimation = true;
            break;
        case 1334:
        case 1933:
        case 1934:
            m_isTouchTriggered = true;
            break;
        case 1594:
        case 1611:
        case 1811:
        case 1815:
        case 1912:
            break;
        case 2066:
            m_gravityValue = 1.0f;
            break;
        case 2902:
        case 3022:
        case 3027:
            break;
        case 3033:
            break;
        case 3607:
            m_isMultiTriggered = true;
            break;
        case 3640:
            m_isTouchTriggered = true;
            m_isSpawnTriggered = true;
            m_isMultiTriggered = true;
            break;
        case 3643:
            m_isSpawnTriggered = false;
            m_isTouchTriggered = true;
            break;
        default:
            break;
    }
    
    m_canSpawnTriggers = canSpawnTriggers();
}

void EffectGameObject::firstSetup() {
    if (m_objectID == 2063) {
        m_isTouchTriggered = true;
    } else if (m_objectID == 3609) {
        m_unk6f5 = true;
    }
}

cocos2d::CCLabelBMFont* EffectGameObject::getObjectLabel() {
    return m_objectLabel;
}

void EffectGameObject::setObjectLabel(cocos2d::CCLabelBMFont* label) {
    m_objectLabel = label;
}

void EffectGameObject::setOpacity(unsigned __int8 opacity) {
    GameObject::setOpacity(opacity);
    if (m_objectLabel) {
        m_objectLabel->setOpacity(opacity);
    }
}

void EffectGameObject::setRScaleX(float scaleX) {
    if (!m_triggerEffectPlaying) {
        GameObject::setRScaleX(scaleX);
    }
}

void EffectGameObject::setRScaleY(float scaleY) {
    if (!m_triggerEffectPlaying) {
        GameObject::setRScaleY(scaleY);
    }
}

void EffectGameObject::setTargetID(int id) {
    if (id < 0) {
        m_targetGroupID = 0;
    } else if (id > 9999) {
        m_targetGroupID = 9999;
    } else {
        m_targetGroupID = id;
    }
}

void EffectGameObject::setTargetID2(int id) {
    if (id < 0) {
        m_centerGroupID = 0;
    } else if (id > 9999) {
        m_centerGroupID = 9999;
    } else {
        m_centerGroupID = id;
    }
}

int EffectGameObject::getTargetColorIndex() {
    switch (m_objectID) {
        case 29: return 1000;
        case 30: return 1001;
        case 105: return 1004;
        case 900: return 1009;
        case 915: return 1002;
        case 744: return 1003;
        default: return 0;
    }
}

bool EffectGameObject::hasSpawnTargetID(int id) {
    if (id <= 0) return false;
    
    if (m_objectID == 1912 || m_objectID == 3607 || m_objectID == 2068) {
        return m_targetGroupID == id || m_centerGroupID == id;
    }
    
    if (m_objectID == 1268 || m_objectID == 1329 || m_objectID == 142 || m_objectID == 1594) {
        return m_targetGroupID == id;
    }
    
    if (m_objectID == 1811 || m_objectID == 1815 || m_objectID == 1611) {
        return m_targetGroupID == id;
    }
    
    if (m_objectID == 2063) {
        return m_targetGroupID == id || m_centerGroupID == id;
    }
    
    if (m_objectID == 3604 || m_objectID == 3609 || m_objectID == 3620 || m_objectID == 3640 || m_objectID == 3643) {
        return m_targetGroupID == id;
    }
    
    if (m_objectType == Collectible) {
        return m_activateGroup != 0 && m_targetGroupID == id;
    }
    
    return false;
}

bool EffectGameObject::isSpecialSpawnObject() {
    return false;
}

void EffectGameObject::playTriggerEffect() {
    if (m_objectID == 3643 || m_objectID == 1594) {
        if (GameManager::sharedState()->getGameVariable("0141")) return;
    }
    
    if (GameManager::sharedState()->getGameVariable("0140")) return;
    
    float scale = getScale();
    float maxScale = (m_objectID == 1594) ? 2.1f : 1.7f;
    float minScale = (m_objectID == 1594) ? 1.1f : 0.7f;
    
    float targetScale = minScale + fabsf(scale) / m_scaleX;
    if (targetScale >= maxScale) targetScale = maxScale;
    
    if (m_scaleX < 0.0f) targetScale = -targetScale;
    
    m_triggerEffectPlaying = true;
    
    // Apply to main sprite and color sprite
    for (int i = 0; i < 2; i++) {
        EffectGameObject* target = this;
        if (i == 1) {
            target = (EffectGameObject*)m_colorSprite;
            if (!target) break;
        }
        
        target->stopAllActions();
        
        float duration = m_hasNoAudioScale ? 1.0f : 0.4f;
        if (targetScale < 0.0f) duration = -duration;
        
        CCScaleTo* scaleTo1 = CCScaleTo::create(0.15f, targetScale * m_scaleX);
        CCEaseExponentialOut* ease1 = CCEaseExponentialOut::create(scaleTo1);
        
        CCScaleTo* scaleTo2 = CCScaleTo::create(0.25f, duration * m_scaleX);
        CCEaseInOut* ease2 = CCEaseInOut::create(scaleTo2, 2.0f);
        
        CCArray* actions = CCArray::create();
        actions->addObject(ease1);
        actions->addObject(ease2);
        
        if (i == 0) {
            actions->addObject(CCCallFunc::create(this, callfunc_selector(EffectGameObject::triggerEffectFinished)));
        }
        
        target->runAction(CCSequence::create(actions));
    }
}

void EffectGameObject::triggerEffectFinished() {
    m_triggerEffectPlaying = false;
}

void EffectGameObject::resetSpawnTrigger() {
    m_activatedByPlayer1 = false;
    m_activatedByPlayer2 = false;
    m_spawnXPosition = getPositionX();
}

void EffectGameObject::restoreObject() {
    if (m_objectID != 2063) {
        GameObject::restoreObject();
    }
}

float EffectGameObject::spawnXPosition() {
    return m_spawnXPosition;
}

void EffectGameObject::triggerActivated(float xPosition) {
    if (!m_isMultiTriggered) {
        EnhancedGameObject::triggerActivated(xPosition);
    }
    m_spawnXPosition = xPosition;
}

void EffectGameObject::updateSpeedModType() {
    if (m_objectID == 1934) {
        m_speedModType = m_isReverse ? -1 : -3;
        return;
    }
    
    if (m_objectID == 1935) {
        m_speedModType = m_isReverse ? -1 : -4;
        return;
    }
    
    if (m_objectID == 2900) {
        m_speedModType = m_isReverse ? -1 : -4;
        return;
    }
    
    if (m_objectID == 2902 || m_objectID == 3022 || m_objectID == 3027) {
        m_speedModType = m_isReverse ? -1 : -5;
        return;
    }
    
    if (m_objectID == 200) {
        m_speedModType = m_isReverse ? -1 : 1;
        return;
    }
    
    if (m_objectID == 202) {
        m_speedModType = m_isReverse ? -1 : 2;
        return;
    }
    
    if (m_objectID == 203) {
        m_speedModType = m_isReverse ? -1 : 3;
        return;
    }
    
    if (m_objectID == 1334) {
        m_speedModType = m_isReverse ? -1 : 4;
        return;
    }
    
    if (m_objectID == 1917) {
        m_speedModType = -1;
        return;
    }
    
    m_speedModType = m_isReverse ? -1 : 0;
}

void EffectGameObject::updateSpecialColor() {
    if (m_objectID != 1049) return;
    
    CCNode* node = getChildByTag(998);
    if (!node) return;
    
    if (m_activateGroup) {
        node->setColor(ccc3(127, 0, 255));
    } else {
        node->setColor(ccc3(63, 255, 63));
    }
}

void EffectGameObject::updateInteractiveHover(float offset) {
    if (m_isDisabled2 || m_objectID == 1614 || m_hasNoAnimation) return;
    
    CCNode* node = getChildByTag(1);
    if (!node) return;
    
    CCSize size = getContentSize();
    node->setPosition(ccp(size.width * 0.5f, offset + size.height * 0.5f));
    
    CCNode* node2 = getChildByTag(2);
    if (node2) {
        node2->setPosition(ccp(size.width * 0.5f, offset + size.height * 0.5f));
    }
}

void EffectGameObject::stateSensitiveOff(GJBaseGameLayer* layer) {
    if (m_objectID != 3640) return;
    
    std::vector<int> targets;
    layer->spawnGroup(m_centerGroupID, false, targets, m_uniqueID, m_controlID, 0.0);
}

std::string EffectGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = EnhancedGameObject::getSaveString(layer);
    
    // Touch triggered
    if (m_isTouchTriggered) {
        result += ",11," + std::to_string(m_isTouchTriggered);
    }
    
    // Has center effect
    if (m_hasCenterEffect) {
        result += ",369," + std::to_string(m_hasCenterEffect);
    }
    
    // Spawn triggered
    if (m_isSpawnTriggered) {
        result += ",62," + std::to_string(m_isSpawnTriggered);
    }
    
    // Multi triggered
    if (m_isMultiTriggered) {
        result += ",87," + std::to_string(m_isMultiTriggered);
    }
    
    // Single P touch
    if (m_isSinglePTouch) {
        result += ",284," + std::to_string(m_isSinglePTouch);
    }
    
    // Should preview
    if (m_shouldPreview) {
        result += ",13," + std::to_string(m_shouldPreview);
    }
    
    // Order value
    if (m_ordValue != 0) {
        result += ",115," + std::to_string(m_ordValue);
    }
    
    // Channel value
    if (m_channelValue != 0) {
        result += ",170," + std::to_string(m_channelValue);
    }
    
    // Is reverse
    if (m_isReverse) {
        result += ",117," + std::to_string(m_isReverse);
    }
    
    // Control ID
    if (m_controlID != 0) {
        result += ",534," + std::to_string(m_controlID);
    }
    
    // Ignore group parent
    if (m_ignoreGroupParent) {
        result += ",280," + std::to_string(m_ignoreGroupParent);
    }
    
    // Ignore linked objects
    if (m_ignoreLinkedObjects) {
        result += ",281," + std::to_string(m_ignoreLinkedObjects);
    }
    
    // Collectible properties
    if (m_objectType == Collectible) {
        if (m_targetGroupID != 0) {
            result += ",51," + std::to_string(m_targetGroupID);
        }
        if (m_subtractCount) {
            result += ",78," + std::to_string(m_subtractCount);
        }
        if (m_collectibleIsPickupItem) {
            result += ",381," + std::to_string(m_collectibleIsPickupItem);
        }
        if (m_collectibleIsToggleTrigger) {
            result += ",382," + std::to_string(m_collectibleIsToggleTrigger);
        }
        if (m_activateGroup != 0) {
            result += ",56," + std::to_string(m_activateGroup);
        }
        if (m_itemID != 0) {
            result += ",80," + std::to_string(m_itemID);
        }
        if (m_collectiblePoints != 0) {
            result += ",383," + std::to_string(m_collectiblePoints);
        }
        if (m_collectibleParticleID != 0) {
            result += ",440," + std::to_string(m_collectibleParticleID);
        }
        if (m_hasNoAnimation) {
            result += ",463," + std::to_string(m_hasNoAnimation);
        }
    }
    
    // Object-specific save strings
    int objID = m_objectID;
    
    if (objID == 901) {
        if (m_targetGroupID != 0) result += ",51," + std::to_string(m_targetGroupID);
        result += ",28," + fmt::format("{:.2f}", m_moveOffset.x);
        result += ",29," + fmt::format("{:.2f}", m_moveOffset.y);
        result += ",10," + fmt::format("{:.2f}", m_duration);
        result += ",30," + std::to_string(m_easingType);
        result += ",85," + fmt::format("{:.2f}", m_easingRate);
        if (m_lockToPlayerX) result += ",58,1";
        else if (m_lockToCameraX) result += ",141,1";
        if (m_lockToPlayerY) result += ",59,1";
        else if (m_lockToCameraY) result += ",142,1";
        if ((m_lockToPlayerX || m_lockToCameraX) && m_moveModX != 1.0f && m_moveModX != 0.0f) {
            result += ",143," + fmt::format("{:.2f}", m_moveModX);
        }
        if ((m_lockToPlayerY || m_lockToCameraY) && m_moveModY != 1.0f && m_moveModY != 0.0f) {
            result += ",144," + fmt::format("{:.2f}", m_moveModY);
        }
        if (m_useMoveTarget || m_isDirectionFollowSnap360) {
            if (m_centerGroupID != 0) result += ",71," + std::to_string(m_centerGroupID);
            if (m_targetModCenterID != 0) result += ",395," + std::to_string(m_targetModCenterID);
            if (m_targetPlayer1 != 0) result += ",138," + std::to_string(m_targetPlayer1);
            if (m_targetPlayer2 != 0) result += ",200," + std::to_string(m_targetPlayer2);
            if (m_useMoveTarget) {
                result += ",100," + std::to_string(m_useMoveTarget);
                if (m_moveTargetMode != Both) {
                    result += ",101," + std::to_string(m_moveTargetMode);
                }
            }
            if (m_isDirectionFollowSnap360) {
                result += ",394," + std::to_string(m_isDirectionFollowSnap360);
                if (m_directionModeDistance != 0.0f) {
                    result += ",396," + fmt::format("{:.2f}", m_directionModeDistance);
                }
            }
        }
        if (m_smallStep) result += ",393," + std::to_string(m_smallStep);
        if (m_isDynamicMode) result += ",397," + std::to_string(m_isDynamicMode);
        if (m_isSilent) result += ",544," + std::to_string(m_isSilent);
    }
    
    if (objID == 1006) {
        result += ",51," + std::to_string(m_targetGroupID);
        if (m_fadeInDuration != 0.0f) {
            result += ",45," + fmt::format("{:.2f}", m_fadeInDuration);
        }
        if (m_holdDuration != 0.0f) {
            result += ",46," + fmt::format("{:.2f}", m_holdDuration);
        }
        if (m_fadeOutDuration != 0.0f) {
            result += ",47," + fmt::format("{:.2f}", m_fadeOutDuration);
        }
        if (m_pulseMode != 0) {
            result += ",48," + std::to_string(m_pulseMode);
            if (m_pulseTargetType != 0) {
                result += ",52," + std::to_string(m_pulseTargetType);
            }
            if (m_pulseMainOnly) result += ",65,1";
            else if (m_pulseDetailOnly) result += ",66,1";
            if (m_pulseExclusive) result += ",86,1";
            if (m_legacyHSV) result += ",210,1";
            if (m_pulseMode == 1 && m_copyColorID != 0) {
                result += ",50," + std::to_string(m_copyColorID);
                if (m_hsvValue.h != 0.0f || m_hsvValue.s != 0.0f || m_hsvValue.v != 0.0f || m_hsvValue.absoluteSaturation != 0) {
                    result += ",49," + GameToolbox::stringFromHSV(m_hsvValue);
                }
            }
        } else {
            result += ",7," + std::to_string(m_triggerTargetColor.r);
            result += ",8," + std::to_string(m_triggerTargetColor.g);
            result += ",9," + std::to_string(m_triggerTargetColor.b);
        }
    }
    
    // Return result
    return result;
}