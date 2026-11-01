#include "Layers/PlayLayer.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "GameLevelManager.h"
#include "Utils/GameToolbox.h"
#include "Utils/LevelTools.h"
#include "Editor/ObjectToolbox.h"
#include "Objects/PlayerObject.h"
#include "Objects/PlayerCheckpoint.h"
#include "Objects/CheckpointObject.h"
#include "Objects/CheckpointGameObject.h"
#include "Objects/GameObject.h"
#include "Objects/EffectGameObject.h"
#include "Objects/EndPortalObject.h"
#include "Objects/EndTriggerGameObject.h"
#include "Objects/StartPosObject.h"
#include "Models/LevelSettingsObject.h"
#include "Managers/GJEffectManager.h"
#include "Models/GJGameLevel.h"
#include "Utils/GJGameState.h"
#include "Layers/GJGroundLayer.h"
#include "Layers/GJMGLayer.h"
#include "Layers/ShaderLayer.h"
#include "Layers/UILayer.h"
#include "Layers/AudioEffectsLayer.h"
#include "Cocos/CCCircleWave.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Models/GJSearchObject.h"
#include "Layers/LevelBrowserLayer.h"
#include "Layers/EditLevelLayer.h"
#include "Layers/LevelInfoLayer.h"
#include "Layers/RetryLevelLayer.h"
#include "Layers/EndLevelLayer.h"
#include "Managers/MusicDownloadManager.h"
#include "Models/SongInfoObject.h"

PlayLayer* PlayLayer::create(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    PlayLayer* layer = new PlayLayer();

    if (layer->init(level, useReplay, dontCreateObjects)) {
        layer->autorelease();
        return layer;
    }

    delete layer;
    return nullptr;
}

cocos2d::CCScene* PlayLayer::scene(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    PlayLayer* layer = PlayLayer::create(level, useReplay, dontCreateObjects);
    scene->addChild(layer);
    return scene;
}

PlayLayer::~PlayLayer() {
}

void PlayLayer::activateEndTrigger(int targetID, bool reverse, bool lockPlayerY) {
}

void PlayLayer::addCircle(CCCircleWave* cw) {
    if (cw) {
        m_circleWaveArray->addObject(cw);
    }
}

void PlayLayer::addObject(GameObject* object) {
    if (!object)
        return;

    if (object->getType() == GameObjectType::Slope) {
        GameObject::determineSlopeDirection(object);
    } else {
        GameObject::updateIsOriented(object);
    }

    object->setVisible(false);

    cocos2d::CCPoint startPos = object->getStartPos();
    if (startPos.x > m_maxObjectX) {
        m_maxObjectX = startPos.x;
    }

    bool isSettingsObject = GameObject::isSettingsObject(object);
    bool isModifierEffect = (object->getType() == GameObjectType::Modifier ||
                             (object->m_classType == GameObjectType::Effect && object->m_isSpecialSpawnObject) ||
                             object->m_objectID == 2063);

    if (!isSettingsObject && !isModifierEffect) {
        addToSection(object);
    } else {
        object->m_outerSectionIndex = -1;
    }

    m_objects->addObject(object);
    addToGroups(object, true);

    if (object->m_objectID == 1615) {
        addObjectCounter(static_cast<LabelGameObject*>(object));
    } else if (object->getType() == GameObjectType::Collectible ||
               object->getType() == GameObjectType::UserCoin) {
        m_collectibles->addObject(object);
        GameObject::getActiveColorForMode(object, object->m_activeMainColorID, true);
        if (GameObject::hasSecondaryColor(object)) {
            GameObject::getActiveColorForMode(object, object->m_activeDetailColorID, false);
        }
    } else {
        GameObject::getActiveColorForMode(object, object->m_activeMainColorID, true);
        if (GameObject::hasSecondaryColor(object)) {
            GameObject::getActiveColorForMode(object, object->m_activeDetailColorID, false);
        }
    }

    if (object->m_isTrigger && object->m_classType == GameObjectType::Effect &&
        (!object->m_isSpecialSpawnObject || object->isSpecialSpawnObject())) {
        m_spawnObjectsArray->addObject(object);
    }

    if (m_isPlatformer) {
        object->m_tempOffsetXRelated = 0;
    }

    int objID = object->m_objectID;

    if (objID == 747) {
        cocos2d::CCPoint pos = object->getPosition();
        cocos2d::CCPoint teleportPos = pos;
        teleportPos.y += object->m_obRect.origin.y;
        float xOff = TeleportPortalObject::getTeleportXOff(static_cast<TeleportPortalObject*>(object), m_gameLayerT1);
        teleportPos.x += xOff;

        TeleportPortalObject* portal = TeleportPortalObject::create("portal_16_front_001.png", false);
        portal->m_objectID = 749;
        portal->customSetup();
        portal->setStartPos(teleportPos);
        portal->m_zOrder = object->m_zOrder;
        portal->m_defaultZOrder = object->m_defaultZOrder;
        portal->m_startFlipX = !object->isFlipX();
        portal->setFlipX(!object->isFlipX());
        portal->m_isDontEnter = object->m_isDontEnter;
        portal->m_isDontFade = object->m_isDontFade;
        portal->m_hasNoEffects = object->m_hasNoEffects;
        portal->setRotation(object->getRotation());
        portal->m_startRotationX = object->getRotation();
        portal->m_startRotationY = object->getRotation();
        GameObject::copyGroups(portal);
        GameObject::updateCustomScaleX(portal, object->m_scaleX);
        GameObject::updateCustomScaleY(portal, object->m_scaleY);

        portal->m_mainColorKeyIndex = object->m_mainColorKeyIndex;
        portal->m_detailColorKeyIndex = object->m_detailColorKeyIndex;
        if (object->m_isHide) {
            portal->m_isHide = true;
        }
        portal->saveActiveColors();
        addObject(portal);
    }

    if (objID == 31 && !object->m_isLocked) {
        if (m_startPosObject) {
            int targetOrder = m_startPosObject->m_startSettings->m_targetOrder;
            float startX = m_startPosObject->getPositionX();
            float currentX = object->getPositionX();

            if (targetOrder < object->m_targetOrder ||
                (startX < currentX && targetOrder <= object->m_targetOrder) ||
                (startX > currentX && targetOrder <= object->m_targetOrder)) {
                setStartPosObject(static_cast<StartPosObject*>(object));
                m_isTestMode = true;
            }
        } else {
            setStartPosObject(static_cast<StartPosObject*>(object));
            m_isTestMode = true;
        }
    } else if (objID == 37) {
        const char* ballFrame = GameObject::getBallFrame(m_pulseRodIndex);
        GameObject* customObj = GameObject::createWithFrame(ballFrame);
        customObj->m_objectID = 37;
        customObj->customSetup();

        if (object->m_activeMainColorID != 1004) {
            GameObject::setMainColorMode(customObj, object->m_activeMainColorID);
        }
        GameObject::setDefaultMainColorMode(object, 0);
        object->saveActiveColors();
        customObj->saveActiveColors();
        object->m_mainColorKeyIndex = 0;
        object->m_detailColorKeyIndex = 0;
        customObj->m_mainColorKeyIndex = 0;
        customObj->m_detailColorKeyIndex = 0;

        cocos2d::CCPoint spawnPos = cocos2d::ccp(object->m_obRect.size.width * 0.5f,
                                                  object->m_obRect.size.height + 10.0f);
        spawnPos = object->convertToWorldSpace(spawnPos);
        cocos2d::CCPoint nodeSpacePos = m_gameBlendingLayerT1->convertToNodeSpace(spawnPos);

        customObj->setStartPos(spawnPos);
        customObj->setScale(0.1f);
        addToSection(customObj);
        m_objects->addObject(customObj);
        customObj->setVisible(false);
        GameObject::copyGroups(customObj);
        addToGroups(customObj, true);
    } else if (GameObject::isSpeedObject(object) ||
               (object->m_classType == GameObjectType::Effect && object->m_isSpeedObject)) {
        EffectGameObject::updateSpeedModType(static_cast<EffectGameObject*>(object));
        m_speedObjects->addObject(object);
    } else {
        const char* frameName = nullptr;
        bool addGuide = false;

        switch (objID) {
            case 12: frameName = "portal_03_back_001.png"; addGuide = true; break;
            case 13: frameName = "portal_04_back_001.png"; addGuide = true; break;
            case 45: frameName = "portal_05_back_001.png"; addGuide = true; break;
            case 46: frameName = "portal_06_back_001.png"; addGuide = true; break;
            case 47: frameName = "portal_07_back_001.png"; addGuide = true; break;
            case 99: frameName = "portal_08_back_001.png"; addGuide = true; break;
            case 101: frameName = "portal_09_back_001.png"; addGuide = true; break;
            case 111: frameName = "portal_10_back_001.png"; addGuide = true; break;
            case 286: frameName = "portal_11_back_001.png"; addGuide = true; break;
            case 287: frameName = "portal_12_back_001.png"; addGuide = true; break;
            case 660: frameName = "portal_13_back_001.png"; addGuide = true; break;
            case 745: frameName = "portal_14_back_001.png"; addGuide = true; break;
            case 747: frameName = "portal_15_back_001.png"; addGuide = true; break;
            case 749: frameName = "portal_16_back_001.png"; addGuide = true; break;
            case 1331: frameName = "portal_17_back_001.png"; addGuide = true; break;
            case 1933: frameName = "portal_18_back_001.png"; addGuide = true; break;
            case 2926: frameName = "portal_19_back_001.png"; addGuide = true; break;
            case 1022: addGuide = true; break;
            default: break;
        }

        if (addGuide) {
            addGuideArt(object);
        }

        if (frameName && !object->m_isHide) {
            GameObject* portalBack = GameObject::createWithFrame(frameName);
            portalBack->m_objectID = 38;
            portalBack->setType(GameObjectType::Decoration);
            portalBack->m_isDecoration2 = true;
            portalBack->customSetup();
            portalBack->setStartPos(object->getPosition());
            portalBack->m_defaultZLayer = 4;
            portalBack->m_defaultZOrder = GameObject::getObjectZOrder(object) - 100;
            portalBack->setFlipX(object->isFlipX());
            portalBack->setFlipY(object->isFlipY());
            portalBack->setRotationX(object->getRotationX());
            portalBack->setRotationY(object->getRotationY());
            portalBack->m_startFlipX = object->m_startFlipX;
            portalBack->m_startFlipY = object->m_startFlipY;
            portalBack->m_startRotationX = object->m_startRotationX;
            portalBack->m_startRotationY = object->m_startRotationY;
            GameObject::updateCustomScaleX(portalBack, object->m_scaleX);
            GameObject::updateCustomScaleY(portalBack, object->m_scaleY);
            portalBack->m_startScaleX = object->m_startScaleX;
            portalBack->m_startScaleY = object->m_startScaleY;
            portalBack->m_isDontEnter = object->m_isDontEnter;
            portalBack->m_isDontFade = object->m_isDontFade;
            portalBack->m_hasNoEffects = object->m_hasNoEffects;

            addToSection(portalBack);
            m_objects->addObject(portalBack);
            GameObject::copyGroups(portalBack);
            addToGroups(portalBack, true);

            portalBack->saveActiveColors();
            portalBack->m_mainColorKeyIndex = object->m_mainColorKeyIndex;
        }
    }
}

void PlayLayer::addToGroupOld(GameObject* object) {
    int index = 0;
    while (object->m_groupCount > index) {
        int groupID = object->getGroupID(index);
        cocos2d::CCArray* group = getGroup(groupID);

        if (!group->containsObject(object)) {
            group->addObject(object);

            if (object->m_objectType == GameObjectType::Decoration) {
                cocos2d::CCArray* optimizedGroup = getOptimizedGroup(groupID);
                ++index;
                if (optimizedGroup->containsObject(object)) {
                    if (object->m_groupCount <= index)
                        return;
                    continue;
                }
                optimizedGroup->addObject(object);
            } else {
                cocos2d::CCArray* staticGroup = getStaticGroup(groupID);
                ++index;
                if (!staticGroup->containsObject(object)) {
                    staticGroup->addObject(object);
                }
            }
        } else {
            if (object->m_objectType == GameObjectType::Decoration) {
                cocos2d::CCArray* optimizedGroup = getOptimizedGroup(groupID);
                ++index;
                if (!optimizedGroup->containsObject(object)) {
                    optimizedGroup->addObject(object);
                }
            } else {
                cocos2d::CCArray* staticGroup = getStaticGroup(groupID);
                ++index;
                if (!staticGroup->containsObject(object)) {
                    staticGroup->addObject(object);
                }
            }
        }

        if (object->m_groupCount <= index)
            return;
    }
}

void PlayLayer::applyCustomEnterEffect(GameObject* object, bool isRight) {
    int directionSign = isRight ? -1 : 1;
    int varianceIndex = object->m_varianceIndex;

    cocos2d::CCPoint modifiedPos = m_enterEffectPosition;

    cocos2d::ccColor3B objectColor = *object->getColor();
    bool hasSecondary = object->hasSecondaryColor();

    cocos2d::ccColor3B secondaryColor;
    if (hasSecondary) {
        secondaryColor = object->m_colorSprite->getColor();
    } else {
        secondaryColor = cocos2d::ccc3(255, 255, 255);
    }

    float cameraValue = isRight ? cameraRight : m_gameState.m_cameraPosition2.x;

    std::vector<EnterEffectInstance>* customEffects = getCustomEnterEffects(object->m_enterChannel, isRight);

    float resultOpacity = 1.0f;

    bool isNotMoving = true;
    bool isNotRotating = true;
    bool isNotScaling = true;
    float rotationAmount = 1.0f;
    float scaleXAmount = 1.0f;
    float scaleYAmount = 1.0f;

    if (customEffects && !customEffects->empty()) {
        EnterEffectInstance* current = customEffects->data();
        EnterEffectInstance* finish = current + customEffects->size();

        float directionFloat = static_cast<float>(directionSign);

        while (current != finish) {
            EffectGameObject* gameObject = current->m_gameObject;
            float offsetVariance = current->m_offsetVariance;
            float offsetValue = 0.0f;
            if (offsetVariance != 0.0f) {
                offsetValue = offsetVariance * m_varianceValues[varianceIndex + 1];
            }

            float lengthVariance = current->m_lengthVariance;
            float targetX = (m_enterEffectPosition.x - (cameraValue + current->m_offset)) + offsetValue;

            float lengthValue = 0.0f;
            float directionScaled = targetX * directionFloat;
            if (lengthVariance != 0.0f) {
                lengthValue = lengthVariance * m_varianceValues[varianceIndex];
            }

            float deadzone = current->m_deadzone;
            int totalLength = static_cast<int>(current->m_length + lengthValue);

            float progress;
            if (deadzone == 0.0f) {
                progress = directionScaled / static_cast<float>(totalLength);
            } else {
                progress = (directionScaled / static_cast<float>(totalLength) - deadzone) / (1.0f - deadzone);
            }

            if (progress < 1.0f) {
                if (progress <= 0.0f) {
                    progress = 0.0f;
                }

                switch (gameObject->m_objectID) {
                    case 3017: {
                        if (gameObject->m_xyMode) {
                            float* variancePtr = &m_varianceValues[varianceIndex];
                            float moveY = (current->m_moveY + current->m_moveYVariance * variancePtr[1106]) * directionFloat;
                            float moveX = (current->m_moveX + current->m_moveXVariance * variancePtr[1105]) * directionFloat;

                            if (moveY != 0.0f || moveX != 0.0f) {
                                isNotMoving = false;
                                float easingValue = 1.0f - getEnterEasingValue(progress,
                                    gameObject->m_easingInType, gameObject->m_easingInRate, gameObject->m_easingInBuffer);
                                modifiedPos.x += moveX * easingValue;
                                modifiedPos.y += moveY * easingValue;
                            }
                        } else {
                            float moveDistance = current->m_moveDistance +
                                current->m_moveDistanceVariance * m_varianceValues[varianceIndex + 10];
                            if (moveDistance != 0.0f) {
                                isNotMoving = false;
                                float easedValue = (1.0f - getEnterEasingValue(progress,
                                    gameObject->m_easingInType, gameObject->m_easingInRate, gameObject->m_easingInBuffer))
                                    * moveDistance;
                                float angle = (current->m_moveAngle +
                                    current->m_moveAngleVariance * m_varianceValues[varianceIndex + 11] - 90.0f) * 0.017453f;
                                cocos2d::CCPoint angleOffset = cocos2d::ccpForAngle(angle);
                                modifiedPos.x += angleOffset.x * easedValue;
                                modifiedPos.y += angleOffset.y * easedValue;
                            }
                        }
                        break;
                    }
                    case 3018: {
                        isNotMoving = false;
                        float rotation = current->m_rotation +
                            current->m_rotationVariance * m_varianceValues[varianceIndex + 18];
                        if (rotation != 0.0f) {
                            isNotRotating = false;
                            float easingValue = getEnterEasingValue(progress,
                                gameObject->m_easingInType, gameObject->m_easingInRate, gameObject->m_easingInBuffer);
                            rotationAmount += rotation * (1.0f - easingValue);
                        }
                        break;
                    }
                    case 3019: {
                        float* variancePtr = &m_varianceValues[varianceIndex];
                        float scaleY = current->m_scaleY + current->m_scaleYVariance * variancePtr[1110];
                        float scaleX = current->m_scaleX + current->m_scaleXVariance * variancePtr[1109];

                        if (scaleY != 0.0f || scaleX != 0.0f) {
                            isNotScaling = false;
                            float easingValue = getEnterEasingValue(progress,
                                gameObject->m_easingInType, gameObject->m_easingInRate, gameObject->m_easingInBuffer);
                            float value = 1.0f - easingValue;
                            scaleYAmount *= (scaleY - 1.0f) * value + 1.0f;
                            scaleXAmount *= (scaleX - 1.0f) * value + 1.0f;
                        }
                        break;
                    }
                    case 3020: {
                        isNotMoving = false;
                        if (!object->ignoreFade()) {
                            resultOpacity *= getEnterEasingValue(progress,
                                gameObject->m_easingInType, gameObject->m_easingInRate, gameObject->m_easingInBuffer);
                        }
                        break;
                    }
                    case 3021: {
                        cocos2d::ccColor3B activeColor = m_effectManager->activeColorForIndex(gameObject->m_tintChannelID);

                        if (gameObject->m_hsvEnabled) {
                            cocos2d::ccHSVValue hsvValue = current->m_hsvValue;
                            cocos2d::ccHSVValue multipliedHSV = GameToolbox::getMultipliedHSV(&hsvValue, 1.0f - progress);

                            if (!gameObject->m_pulseDetailOnly) {
                                objectColor = GameToolbox::transformColor(objectColor, multipliedHSV);
                            }
                            if (!gameObject->m_pulseMainOnly && hasSecondary) {
                                secondaryColor = GameToolbox::transformColor(secondaryColor, multipliedHSV);
                            }
                        } else {
                            float tintValue = (1.0f - current->m_tint) + current->m_tint * progress;

                            if (!gameObject->m_pulseDetailOnly) {
                                objectColor = GameToolbox::multipliedColorValue(activeColor, objectColor, tintValue);
                            }
                            if (!gameObject->m_pulseMainOnly && hasSecondary) {
                                secondaryColor = GameToolbox::multipliedColorValue(activeColor, secondaryColor, tintValue);
                            }
                        }
                        isNotMoving = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            ++current;
        }
    }

    if (isRight) {
        object->m_enterType = -15;
    } else {
        object->m_exitType = -15;
    }

    object->setOpacity(static_cast<unsigned char>(resultOpacity * 255.0f));

    if (isNotMoving) {
        if (m_enterEffectPosition.x != modifiedPos.x || m_enterEffectPosition.y != modifiedPos.y) {
            object->setPosition(&m_enterEffectPosition);
        }
    } else {
        if (modifiedPos.x != object->getPosition().x || modifiedPos.y != object->getPosition().y) {
            object->setPosition(&modifiedPos);
        }
    }

    if (isNotRotating) {
        object->setRRotation(0.0f);
    } else {
        object->setRRotation(rotationAmount);
    }

    if (isNotScaling) {
        object->setRScaleX(1.0f);
        object->setRScaleY(1.0f);
    } else {
        object->setRScaleX(scaleYAmount);
        object->setRScaleY(scaleXAmount);
    }

    object->updateMainColor(&objectColor);
    if (hasSecondary) {
        object->updateSecondaryColor(&secondaryColor);
    }

    if (isNotMoving) {
        if (isRight) {
            object->m_enterType = -1;
        } else {
            object->m_exitType = -1;
        }
    }
}

void PlayLayer::applyEnterEffect(GameObject* object, int enterType, bool isRight) {
    cocos2d::CCPoint targetPos = m_enterEffectPosition;

    cocos2d::CCPoint scaleVar;
    scaleVar.y = 0.0f;
    scaleVar.x = 70.0f;

    float relativeModNew = getRelativeModNew(scaleVar, 0.0f, 0.0f, false, false);

    bool ignoreEnter = object->ignoreEnter();
    bool relativeIsOne = (relativeModNew == 1.0f);
    bool canHandleEnter = ignoreEnter ? true : relativeIsOne;

    int currentEnterType = isRight ? object->m_enterType : object->m_exitType;

    if (currentEnterType != -1 && (currentEnterType == -14 || enterType != -14)) {
        if (canHandleEnter) {
            object->m_unk3E8 = true;
            object->m_enterType = -1;
            object->m_exitType = -1;
        }

        int activeEnterType = isRight ? object->m_enterType : object->m_exitType;

        switch (activeEnterType) {
            case -13:
            case -12:
            case -11:
            case -10:
            case -9: {
                cocos2d::CCPoint angleOffset = cocos2d::ccpForAngle((object->m_unk3D8 - 90.0f) * 0.017453f);
                float baseOffset = (1.0f - relativeModNew) * 100.0f;
                cocos2d::CCPoint finalPos(baseOffset * angleOffset.x, baseOffset * angleOffset.y);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -8: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * 100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -7: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * -100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -6: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * -100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -5: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * 100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -4: {
                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX((1.0f - relativeModNew) * 0.75f + 1.0f);
                    object->setRScaleY((1.0f - relativeModNew) * 0.75f + 1.0f);
                }
                object->setPosition(&m_enterEffectPosition);
                break;
            }
            case -3: {
                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(relativeModNew);
                    object->setRScaleY(relativeModNew);
                }
                object->setPosition(&m_enterEffectPosition);
                break;
            }
            default: {
                object->setPosition(&m_enterEffectPosition);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
        }

        if (relativeModNew == 0.0f) {
            object->m_unk3E8 = true;
            if (isRight) {
                object->m_enterType = -1;
            } else {
                object->m_exitType = -1;
            }
        }
        return;
    }

    if (!canHandleEnter) {
        if (isRight) {
            object->m_enterType = enterType;
        } else {
            object->m_exitType = enterType;
        }

        switch (enterType) {
            case -13:
            case -12: {
                float y = m_enterEffectPosition.y;
                float cameraY = m_gameState.m_cameraPosition2.y + targetPos.y * 0.5f;

                float baseAngle = (cameraY < y) ? 180.0f : 0.0f;

                if (enterType == -13) {
                    object->m_unk3D8 = baseAngle + 180.0f;
                } else {
                    object->m_unk3D8 = baseAngle;
                }
                break;
            }
            case -11: {
                object->m_unk3D8 = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * 180.0f;
                break;
            }
            case -10:
            case -9: {
                float y = m_enterEffectPosition.y;
                float cameraY = m_gameState.m_cameraPosition2.y + targetPos.y * 0.5f;

                if (enterType == -10) {
                    if (cameraY >= y) {
                        object->m_unk3D8 = -1036779520;
                    } else {
                        object->m_unk3D8 = -1022951424;
                    }
                } else {
                    if (cameraY >= y) {
                        object->m_unk3D8 = 1110704128;
                    } else {
                        object->m_unk3D8 = 1124532224;
                    }
                }
                break;
            }
            default:
                break;
        }

        int activeEnterType = isRight ? object->m_enterType : object->m_exitType;

        switch (activeEnterType) {
            case -13:
            case -12:
            case -11:
            case -10:
            case -9: {
                cocos2d::CCPoint angleOffset = cocos2d::ccpForAngle((object->m_unk3D8 - 90.0f) * 0.017453f);
                float baseOffset = (1.0f - relativeModNew) * 100.0f;
                cocos2d::CCPoint finalPos(baseOffset * angleOffset.x, baseOffset * angleOffset.y);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -8: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * 100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -7: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * -100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -6: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * -100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -5: {
                cocos2d::CCPoint finalPos(0.0f, (1.0f - relativeModNew) * 100.0f);
                cocos2d::CCPoint result = m_enterEffectPosition + finalPos;
                object->setPosition(&result);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
            case -4: {
                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX((1.0f - relativeModNew) * 0.75f + 1.0f);
                    object->setRScaleY((1.0f - relativeModNew) * 0.75f + 1.0f);
                }
                object->setPosition(&m_enterEffectPosition);
                break;
            }
            case -3: {
                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(relativeModNew);
                    object->setRScaleY(relativeModNew);
                }
                object->setPosition(&m_enterEffectPosition);
                break;
            }
            default: {
                object->setPosition(&m_enterEffectPosition);

                if (!object->m_usesAudioScale || object->m_hasNoAudioScale) {
                    object->setRScaleX(1.0f);
                    object->setRScaleY(1.0f);
                }
                break;
            }
        }

        if (relativeModNew == 0.0f) {
            object->m_unk3E8 = true;
            if (isRight) {
                object->m_enterType = -1;
            } else {
                object->m_exitType = -1;
            }
        }
        return;
    }

    cocos2d::CCPoint currentPos = object->getPosition();

    if (m_enterEffectPosition.x != currentPos.x || m_enterEffectPosition.y != currentPos.y) {
        object->setPosition(&m_enterEffectPosition);
    }

    if (object->m_unk3E8) {
        bool usesAudioScale = object->m_usesAudioScale;
        object->m_unk3E8 = false;

        if (!usesAudioScale || object->m_hasNoAudioScale) {
            object->setRScaleX(1.0f);
            object->setRScaleY(1.0f);
        }
    }
}

bool PlayLayer::canPauseGame() {
    if (!m_bgEffectDisabled)
        return !m_isPaused;
    return false;
}

void PlayLayer::checkForEnd() {
    if (!m_endPortal || m_endChecked)
        return;

    cocos2d::CCPoint spawnPos = m_endPortal->getSpawnPos();

    if (!m_endPortal->m_flippedX) {
        if (m_player1->getPosition()->x < spawnPos.x)
            return;
    } else {
        if (m_player1->getPosition()->x > spawnPos.x)
            return;
    }

    m_endChecked = true;

    if (m_sectionYFactor != 0.0f) {
        m_endPortal->updateEndPos(false);
        reorderObjectSection(m_endPortal);
    }

    m_endPortal->triggerObject(this);
}

void PlayLayer::checkSnapshot() {
}

void PlayLayer::checkpointActivated(CheckpointGameObject* object) {
    if (m_isPlatformer)
        m_activatedCheckpoint = object;

    GJBaseGameLayer::checkpointActivated(object);
}

CheckpointObject* PlayLayer::checkpointWithID(int id) {
    for (unsigned int i = 0; i < m_checkpointArray->count(); ++i) {
        CheckpointObject* checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(i));
        if (checkpoint->m_uniqueID == id)
            return checkpoint;
    }
    return nullptr;
}

void PlayLayer::circleWaveWillBeRemoved(CCCircleWave* circleWave) {
    m_circleWaveArray->removeObject(circleWave, true);
}

void PlayLayer::colorObject(int id, cocos2d::ccColor3B color) {
}

void PlayLayer::commitJumps() {
    GameStatsManager::sharedState()->incrementStat("1", m_uncommittedJumps);
    m_uncommittedJumps = 0;
}

void PlayLayer::compareStateSnapshot() {
}

CheckpointObject* PlayLayer::createCheckpoint() {
    CheckpointObject* checkpoint = CheckpointObject::create();

    PlayerCheckpoint* playerCheckpoint1 = PlayerCheckpoint::create();
    if (playerCheckpoint1 != checkpoint->m_player1Checkpoint) {
        CC_SAFE_RETAIN(playerCheckpoint1);
        CC_SAFE_RELEASE(checkpoint->m_player1Checkpoint);
        checkpoint->m_player1Checkpoint = playerCheckpoint1;
    }
    m_player1->saveToCheckpoint(checkpoint->m_player1Checkpoint);

    if (m_gameState.m_isDualMode) {
        PlayerCheckpoint* playerCheckpoint2 = PlayerCheckpoint::create();
        if (playerCheckpoint2 != checkpoint->m_player2Checkpoint) {
            CC_SAFE_RETAIN(playerCheckpoint2);
            CC_SAFE_RELEASE(checkpoint->m_player2Checkpoint);
            checkpoint->m_player2Checkpoint = playerCheckpoint2;
        }
        m_player2->saveToCheckpoint(checkpoint->m_player2Checkpoint);
    }

    checkpoint->m_gameState = m_gameState;
    checkpoint->m_commandIndex = m_gameState.m_commandIndex;

    if (m_shaderLayer) {
        checkpoint->m_shaderState = m_shaderLayer->m_state;
    }

    FMODAudioEngine::sharedEngine()->saveAudioState(&checkpoint->m_audioState);

    for (auto* trigger : m_sequenceTriggers) {
        checkpoint->m_sequenceTriggerStateUnorderedMap[trigger->m_sequenceTriggerID] =
            trigger->m_state;
    }

    checkpoint->m_ground2Invisible = !m_groundLayer2->m_showGround2;

    m_dynamicSaveObjects.clear();
    m_activeSaveObjects1.clear();
    m_activeSaveObjects2.clear();
    m_dynamicSaveObjects2.clear();

    saveDynamicSaveObjects(&checkpoint->m_vectorSavedObjectStateRef);
    saveActiveSaveObjects(checkpoint->m_vectorActiveSaveObjectState,
                          checkpoint->m_vectorSpecialSaveObjectState);

    CheckpointObject* lastCheckpoint = getLastCheckpoint();
    if (lastCheckpoint) {
        std::vector<CAState>& srcVec = lastCheckpoint->m_effectManagerState.m_unkVecCAState;
        std::vector<CAState>& dstVec = checkpoint->m_effectManagerState.m_unkVecCAState;
        dstVec.clear();
        dstVec.resize(srcVec.size());
        for (size_t i = 0; i < srcVec.size(); ++i) {
            dstVec[i] = srcVec[i];
        }
    }

    m_effectManager->saveToState(&checkpoint->m_effectManagerState);

    if (m_gradientLayers) {
        cocos2d::CCDictElement* element = m_gradientLayers->m_pElements;
        if (element) {
            cocos2d::CCDictElement* next = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            cocos2d::CCArray* gradientArray = nullptr;

            while (true) {
                GameObject* gradientObject = static_cast<GameObject*>(element->m_pObject);
                cocos2d::CCArray* layers = static_cast<cocos2d::CCArray*>(gradientObject->getUserObject());
                cocos2d::CCArray* currentLayers = gradientObject->getChildren();

                if (currentLayers && layers) {
                    if (!gradientArray) {
                        gradientArray = cocos2d::CCArray::createWithArray(layers);
                    }
                    gradientArray->addObject(layers);
                }

                if (!next)
                    break;

                element = next;
                next = static_cast<cocos2d::CCDictElement*>(next->hh.next);
            }

            if (gradientArray && gradientArray != checkpoint->m_gradientTriggerObjectArray) {
                gradientArray->retain();
                if (checkpoint->m_gradientTriggerObjectArray) {
                    checkpoint->m_gradientTriggerObjectArray->release();
                }
                checkpoint->m_gradientTriggerObjectArray = gradientArray;
            }
        }
    }

    GameObject* checkpointObject = GameObject::createWithFrame("checkpoint_01_001.png");
    checkpointObject->m_objectID = 44;
    checkpointObject->customSetup();

    cocos2d::CCPoint startPos;
    if (isFlipping()) {
        startPos = m_player1->m_position;
    } else {
        startPos = *m_player1->getPosition();
    }

    checkpointObject->setStartPos(startPos);
    checkpoint->setObject(checkpointObject);

    saveDynamicSaveObjects(&checkpoint->m_vectorSavedObjectStateRef);
    saveActiveSaveObjects(&checkpoint->m_vectorActiveSaveObjectState,
                          &checkpoint->m_vectorSpecialSaveObjectState);

    return checkpoint;
}

void PlayLayer::createObjectsFromSetupFinished() {
    if (m_coinArray && m_coinArray->count() > 0) {
        int coinIndex = 0;
        qsort(m_coinArray->data->arr, m_coinArray->data->num, 8, reinterpret_cast<int (*)(const void*, const void*)>(xCompRealPos));

        while (coinIndex < static_cast<int>(m_coinArray->count()) && coinIndex <= 2) {
            GameObject* coin = static_cast<GameObject*>(m_coinArray->objectAtIndex(coinIndex));
            coin->m_unk4C0 = coinIndex + 1;
            coin->updateUserCoin();
            ++coinIndex;
        }
    }

    if (!m_isPlatformer) {
        float screenRight = cocos2d::CCDirector::sharedDirector()->getScreenRight();
        float endX = m_maxObjectX + 340.0f;

        if (screenRight + 300.0f >= endX) {
            endX = cocos2d::CCDirector::sharedDirector()->getScreenRight() + 300.0f;
        }

        m_levelLength = endX;
        m_endXPosition = endX;

        EndPortalObject* endPortal = EndPortalObject::create();
        m_endPortal = endPortal;
        endPortal->setStartPos(cocos2d::CCPoint(m_levelLength, 225.0f));
        m_endPortal->m_defaultZOrder = 11;
        addToSection(m_endPortal);
        m_objects->addObject(m_endPortal);

        m_endPortal->updateColors(m_player1->m_playerColor1);
        m_endPortal->setVisible(false);
        m_endPosition = m_endPortal->getPosition();
    }

    LevelTools::sortSpeedObjects(m_speedObjects, this);
    orderSpawnObjects();
    updateMaxGameplayY();

    for (size_t i = 0; i < m_objectStrings.size(); ++i) {
        m_objectStrings[i].clear();
    }
    m_objectStrings.clear();

    m_coinArray->removeAllObjects();
    m_coinArray->release();
    m_coinArray = nullptr;

    int colorKeyCount = m_nextColorKey + 1;
    m_nextColorKey = colorKeyCount;

    if (colorKeyCount > static_cast<int>(m_keyColors.size())) {
        m_keyColors.resize(colorKeyCount);
    } else {
        m_keyColors.resize(colorKeyCount);
    }

    if (colorKeyCount > static_cast<int>(m_keyOpacities.size())) {
        m_keyOpacities.resize(colorKeyCount);
    } else {
        m_keyOpacities.resize(colorKeyCount);
    }

    int pulseCount = (m_nextColorKey >> 5) + 1;
    if (pulseCount >= static_cast<int>(m_keyPulses.m_bits.size())) {
        m_keyPulses.m_bits.resize(pulseCount);
    } else {
        m_keyPulses.m_bits.resize(pulseCount);
    }
}

void PlayLayer::delayedFullReset() {
    if (m_isPlatformer) {
        m_inResetDelay = false;
        m_platformerRestart = true;
    }

    cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::fullReset)),
        nullptr);
    runAction(sequence);
}

void PlayLayer::delayedResetLevel() {
    if (m_inResetDelay)
        resetLevel();
}

void PlayLayer::destroyPlayer(PlayerObject* player, GameObject* object) {
    m_player1 = player;

    if (m_player1->m_isLocked || m_playerDied)
        return;

    if (object) {
        if (m_anticheatSpike) {
            if (object->m_uniqueID == m_anticheatSpike->m_uniqueID) {
                m_damageVerified = true;
                return;
            }
        }
    }

    stopActionByTag(20);
    updateTimeWarp(1.0f);
    commitJumps();
    m_effectManager->playerDied();
    m_gameState.m_pauseCounter = 0;

    if (!m_isPracticeMode) {
        if (!GameManager::sharedState()->getGameVariable("0014"))
            shakeCamera(0.15f, 1.0f, 0.0f);
    }

    m_staticCameraShake = true;

    if (!m_hintShown) {
        int levelID = m_level->m_levelID.random - m_level->m_levelID.seed;
        if ((levelID == 1 || levelID == 1001) && !m_player1->m_hasEverJumped && m_attempts > 1) {
            showHint();
            if (!m_hintShown) {
                levelID = m_level->m_levelID.random - m_level->m_levelID.seed;
            }
        }
        if ((levelID == 3 || levelID == 1003) && !m_player1->m_hasEverHitRing && m_attempts > 1) {
            showHint();
        }
    }

    m_playerDied = true;

    if (m_hitboxesOnDeath && !m_debugDrawNode->isVisible())
        toggleDebugDraw();

    m_player1->playerDestroyed(false);

    if (m_gameState.m_isDualMode) {
        PlayerObject* other = getOtherPlayer(m_player1);
        playExitDualEffect(other);
        other->playerDestroyed(true);
    }

    int currentPercent = getCurrentPercentInt();
    bool flag = false;
    int orbs = 0;
    int diamonds = 0;
    bool demonKey = false;
    bool flag2 = false;

    if (m_isTestMode || m_isPlatformer) {
        flag = false;
        orbs = 0;
        diamonds = 0;
        demonKey = false;
        flag2 = false;
    } else {
        bool isPractice = m_isPracticeMode;
        if (currentPercent < 0)
            currentPercent = 0;

        int clampedPercent;
        if (currentPercent > 99)
            clampedPercent = 99;
        else
            clampedPercent = currentPercent;

        if (m_isPracticeMode) {
            flag2 = false;
        } else {
            int normalPercent = m_level->getNormalPercent();
            if (clampedPercent <= normalPercent)
                flag2 = false;
            else
                flag2 = true;
        }

        demonKey = false;

        if (!m_isPracticeMode) {
            if (clampedPercent <= m_level->m_orbCompletion.random - m_level->m_orbCompletion.seed &&
                m_showLeaderboardPercentage &&
                !flag2) {
                flag2 = clampedPercent > m_level->m_newNormalPercent2.random - m_level->m_newNormalPercent2.seed;
                demonKey = clampedPercent > m_level->m_newNormalPercent2.random - m_level->m_newNormalPercent2.seed;
            }
        }

        double tempMilliTime = getTempMilliTime();
        m_level->savePercentage(
            clampedPercent,
            m_isPracticeMode,
            m_clicks,
            static_cast<int>(tempMilliTime - m_currentTime - m_pauseDelta),
            m_damageVerified);

        if (m_level->m_levelType == GJLevelType::Main) {
            GameManager::sharedState()->reportPercentageForLevel(
                m_level->m_levelID.random - m_level->m_levelID.seed,
                clampedPercent,
                m_isPracticeMode);
        }

        if (m_isPracticeMode) {
            flag = false;
            orbs = 0;
            diamonds = 0;
        } else {
            if (m_level->m_stars.random - m_level->m_stars.seed <= 0 || m_isPlatformer) {
                flag = false;
                orbs = 0;
                diamonds = 0;
            } else {
                GameStatsManager::sharedState()->awardCurrencyForLevel(m_level);
                diamonds = 0;
                GJGameLevel* level = m_level;

                if (level->m_dailyID.random - level->m_dailyID.seed > 0 || level->m_gauntletLevel) {
                    GameStatsManager::sharedState()->awardDiamondsForLevel(m_level);
                    orbs = 0;
                    flag = orbs + 0 > 0;
                } else {
                    orbs = 0;
                    flag = false;
                }
            }
        }

        m_lastAttemptPercent = getCurrentPercentInt();

        if (!m_gameState.m_unkBool22) {
            if (!m_isPracticeMode) {
                if (m_practiceMusicSync)
                    FMODAudioEngine::sharedEngine()->pauseAllMusic(false);
                FMODAudioEngine::sharedEngine()->stopAllEffects();
            } else {
                if (!m_practiceMusicSync)
                    FMODAudioEngine::sharedEngine()->stopAllEffects();
                else
                    FMODAudioEngine::sharedEngine()->pauseAllMusic(false);
            }
        }
    }

    if (!m_gameState.m_unkBool26) {
        FMODAudioEngine::sharedEngine()->playEffect("explode_11.ogg", 1.0f, 0.0f, 0.65f);
    }

    if (!GameManager::sharedState()->getGameVariable("0026") && !m_isPracticeMode) {
        if (flag)
            showNewBest(!flag2, diamonds, orbs, false, false, false);

        GameManager::sharedState()->m_playLayer->m_uiLayer->disableMenu();

        float delay = m_gameState.m_unkUint14;
        shouldRefreshBanner = false;

        if (delay <= 0.5f)
            delay = 0.5f;

        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(delay),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::showRetryLayer)),
            nullptr);
        runAction(sequence);
        return;
    }

    m_inResetDelay = true;

    float resetDelay;
    if (GameManager::sharedState()->getGameVariable("0052"))
        resetDelay = 0.5f;
    else
        resetDelay = 1.0f;

    if (resetDelay < m_gameState.m_unkUint14)
        resetDelay = m_gameState.m_unkUint14;

    if (!flag && !flag2) {
        stopActionByTag(16);
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(resetDelay),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
            nullptr);
        sequence->setTag(16);
        runAction(sequence);
        return;
    }

    bool flag4 = false;
    if (!GameManager::sharedState()->getUGV("17")) {
        flag4 = GameStatsManager::sharedState()->getStat("14") > 500;
    }

    bool awardKey = GameStatsManager::sharedState()->awardSecretKey();
    showNewBest(!flag2, diamonds, orbs, awardKey, flag4, demonKey);

    if (resetDelay < 1.4f)
        resetDelay = 1.4f;

    if (flag) {
        if (!flag4) {
            stopActionByTag(16);
            return;
        }
    } else if (!flag4) {
        stopActionByTag(16);
        cocos2d::CCSequence* sequence = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(resetDelay),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
            nullptr);
        sequence->setTag(16);
        runAction(sequence);
        return;
    }

    GameManager::sharedState()->setUGV("17", true);

    DialogObject* dialog = DialogObject::create(
        "The Shopkeeper",
        "I see you have a lot of <cl>orbs</c>. You should visit my <cy>shop</c>!",
        5, 1.0f, false, cocos2d::ccc3(255, 255, 255));

    DialogLayer* dialogLayer = DialogLayer::create(dialog, 1);
    addChild(dialogLayer, 100);
    dialogLayer->setOpacity(0);
    dialogLayer->m_delegate = this;
    dialogLayer->updateChatPlacement(DialogChatPlacement::Top);
    dialogLayer->animateIn(DialogAnimationType::FromTop);

    stopActionByTag(16);
}

void PlayLayer::flipArt(bool flip) {
    if (flip) {
        m_attemptLabel->setScaleX(-1.0f);
    } else {
        m_attemptLabel->setScaleX(1.0f);
    }
}

void PlayLayer::fullReset() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_gameState.m_totalTime = 0.0;
    m_gameState.m_unkDouble3 = 0.0;
    m_gameState.m_levelTime = 0.0;
    m_player1->m_totalTime = 0.0;
    m_player2->m_totalTime = m_gameState.m_totalTime;
    m_attempts = 0;
    m_jumps = 0;
    m_objectsDeactivated = true;
    m_freezeStartCamera = true;

    if (m_isPracticeMode) {
        togglePracticeMode(false);
    } else {
        resetLevelFromStart();
    }

    float y = m_gameState.m_cameraPosition.y + winSize.height * 0.5f;
    float x = m_gameState.m_cameraPosition.x;
    m_attemptLabel->setPosition(cocos2d::CCPoint(x + winSize.width * 0.5f, y + 85.0f));
}

float PlayLayer::getCurrentPercent() {
    float x = m_player1->getPosition()->x;
    int timestamp = m_level->m_timestamp;

    if (timestamp > 0) {
        m_levelLength = static_cast<float>(2 * timestamp);
        x = static_cast<float>(static_cast<int>(m_gameState.m_currentProgress));
    }

    float percent = (x / m_levelLength) * 100.0f;

    if (percent <= 0.0f)
        return 0.0f;
    if (percent >= 100.0f)
        return 100.0f;
    return percent;
}

int PlayLayer::getCurrentPercentInt() {
    return static_cast<int>(getCurrentPercent());
}

cocos2d::CCPoint PlayLayer::getEndPosition() {
    if (m_isPlatformer || !m_endPortal || m_platformerEndTrigger)
        return m_endPosition;

    return *m_endPortal->getPosition();
}

CheckpointObject* PlayLayer::getLastCheckpoint() {
    if (m_checkpointArray->count() > 0)
        return static_cast<CheckpointObject*>(m_checkpointArray->lastObject());
    return nullptr;
}

float PlayLayer::getRelativeMod(cocos2d::CCPoint position, float right, float left, float offset) {
    float cameraX = m_gameState.m_cameraPosition2.x;

    if (offset > m_halfCameraWidth + cameraX) {
        float value = m_halfCameraWidth - ((offset - right - cameraX) - m_halfCameraWidth);
        float result = value * position.x;

        if (result < 0.0f)
            return 0.0f;
        if (result <= 1.0f)
            return result;
        return 1.0f;
    }

    float value = m_halfCameraWidth - ((m_halfCameraWidth + cameraX - offset) - right);
    float result = value * position.y;

    if (result < 0.0f)
        return 0.0f;
    if (result <= 1.0f)
        return result;
    return 1.0f;
}

double PlayLayer::getTempMilliTime() {
    time_t timer;
    time(&timer);
    return static_cast<double>(timer);
}

void PlayLayer::gravityEffectFinished() {
    int count = m_activeGravityEffects - 1;
    if (count < 0)
        count = 0;
    m_activeGravityEffects = count;
}

void PlayLayer::incrementJumps() {
    m_uncommittedJumps++;
    m_hasJumped = true;
    m_jumps++;

    m_level->m_jumps.value = m_level->m_jumps.random - m_level->m_jumps.seed + 1;
    m_level->m_jumps.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
    m_level->m_jumps.random = m_level->m_jumps.value + m_level->m_jumps.seed;
}

bool PlayLayer::isGameplayActive() {
    if (!m_hasCompletedLevel && m_started) {
        return !m_player1->m_isDead;
    }
    return false;
}

void PlayLayer::levelComplete() {
    bool noEffects = false;
    bool instant = false;

    if (m_platformerEndTrigger) {
        noEffects = m_platformerEndTrigger->m_noEffects;
        instant = m_platformerEndTrigger->m_instant;
    }

    if (m_platformerEndTrigger || m_isPlatformer) {
        PlayerObject::playCompleteEffect(m_player1, noEffects, instant);
        PlayerObject::playCompleteEffect(m_player2, noEffects, instant);
    }

    if (m_attemptTime > m_bestAttemptTime)
        m_bestAttemptTime = m_attemptTime;

    if (m_level->m_levelType != GJLevelType::Main &&
        !m_isPracticeMode &&
        !m_isTestMode &&
        !m_platformerRestart &&
        m_level->m_stars.value - m_level->m_stars.seed > 0 &&
        (!m_isPlatformer || m_timePlayed < 10.0) &&
        m_bestAttemptTime < 10.0) {
        m_unk3906 = true;
    }

    m_bgEffectDisabled = true;
    updateTimeWarp(1.0);

    bool skipCompletion = false;

    if (m_isTestMode) {
        if (m_unk3906) {
            onQuit();
            return;
        }
        skipCompletion = true;
    } else if (m_platformerRestart) {
        if (m_unk3906) {
            onQuit();
            return;
        }
        skipCompletion = true;
    } else if (m_level->m_levelType == GJLevelType::Editor) {
        std::string capacityString = GJBaseGameLayer::getCapacityString(this);
        m_level->m_capacityString = capacityString;

        if (!m_isPracticeMode) {
            int commandIndexHalf = static_cast<int>(m_gameState.m_commandIndex) / 2;
            if (m_levelSettings)
                m_levelSettings->m_propertykA44 = commandIndexHalf;
            m_level->m_timestamp = commandIndexHalf;
        }

        if (m_unk3906) {
            onQuit();
            return;
        }

        if (m_isTestMode) {
            skipCompletion = true;
        } else if (m_platformerRestart) {
            skipCompletion = true;
        }
    } else if (m_unk3906) {
        onQuit();
        return;
    }

    if (!skipCompletion && !m_isPracticeMode) {
        cocos2d::CCArray* collectedKeys = m_collectedItems->allKeys();
        unsigned int coinIndex = 0;

        while (coinIndex < collectedKeys->count()) {
            cocos2d::CCString* keyString = static_cast<cocos2d::CCString*>(collectedKeys->objectAtIndex(coinIndex));
            std::string keyStd(keyString->getCString());
            GameObject* collectedObject = static_cast<GameObject*>(m_collectedItems->objectForKey(keyStd));

            if (collectedObject->getType() == GameObjectType::UserCoin) {
                int coinIndexInLevel = collectedObject->m_unk4C0;
                switch (coinIndexInLevel) {
                    case 1: {
                        m_level->m_firstCoinVerified.value = 1;
                        m_level->m_firstCoinVerified.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
                        m_level->m_firstCoinVerified.random = m_level->m_firstCoinVerified.value + m_level->m_firstCoinVerified.seed;
                        ++coinIndex;
                        break;
                    }
                    case 2: {
                        m_level->m_secondCoinVerified.value = 1;
                        m_level->m_secondCoinVerified.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
                        m_level->m_secondCoinVerified.random = m_level->m_secondCoinVerified.value + m_level->m_secondCoinVerified.seed;
                        ++coinIndex;
                        break;
                    }
                    case 3: {
                        m_level->m_thirdCoinVerified.value = 1;
                        m_level->m_thirdCoinVerified.seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
                        m_level->m_thirdCoinVerified.random = m_level->m_thirdCoinVerified.value + m_level->m_thirdCoinVerified.seed;
                        ++coinIndex;
                        break;
                    }
                    default:
                        ++coinIndex;
                        break;
                }
            } else {
                ++coinIndex;
            }
        }

        if (m_level->m_levelType == GJLevelType::Editor && GJGameLevel::areCoinsVerified(m_level)) {
            m_level->m_isVerifiedRaw = true;
            int randVal = rand();
            bool hasRecordString = m_unk383c;
            int seedVal = static_cast<int>(static_cast<float>(randVal) * 4.6566e-10f * 1000.0f);
            m_level->m_isVerified.seed = seedVal;
            m_level->m_isVerified.random = seedVal + 1;

            if (hasRecordString) {
                std::string recordString = GJBaseGameLayer::getRecordString(this, true);
                m_level->m_recordString = recordString;
            }
        }

        if (m_unk3906) {
            onQuit();
            return;
        }
    }

    if (!m_isTestMode && !m_platformerRestart) {
        double tempMilliTime = getTempMilliTime();
        GJGameLevel::savePercentage(
            m_level,
            100,
            m_isPracticeMode,
            m_clicks,
            static_cast<int>(tempMilliTime - m_currentTime - m_pauseDelta),
            m_damageVerified);

        if (m_isPracticeMode) {
            if (m_level->m_levelType == GJLevelType::Main) {
                GameManager* gm = GameManager::sharedState();
                GameManager::reportPercentageForLevel(
                    gm,
                    m_level->m_levelID.value - m_level->m_levelID.seed,
                    100,
                    m_isPracticeMode);
            }
        } else {
            std::string recordString = GJBaseGameLayer::getRecordString(this, true);
            int collectedCoins = GJBaseGameLayer::countCollectedUserCoins(this);
            int playTimerMilli = GJBaseGameLayer::getPlayTimerMilli(this);

            if (m_isPlatformer) {
                GameManager::sharedState()->m_leaderboardLevelID = m_level->m_M_ID;
                GameManager* gm = GameManager::sharedState();
                bool unkBool32 = m_gameState.m_unkBool32;
                gm->m_leaderboardLevelTime = playTimerMilli;

                if (unkBool32) {
                    int points = m_gameState.m_points;
                    if (!m_levelSettings->m_noTimePenalty) {
                        points -= playTimerMilli;
                        m_gameState.m_points = points;
                    }

                    std::string emptyInputs = "";
                    GJGameLevel::saveNewScore(
                        m_level,
                        points,
                        1,
                        m_tickIndex,
                        m_clickIndex,
                        collectedCoins,
                        emptyInputs,
                        false);

                    GameManager::sharedState()->m_leaderboardLevelPoints = m_gameState.m_points;
                } else {
                    GameManager::sharedState()->m_leaderboardLevelPoints = 0;
                }

                std::string emptyInputs = "";
                GJGameLevel::saveNewScore(
                    m_level,
                    playTimerMilli,
                    0,
                    m_tickIndex,
                    m_clickIndex,
                    collectedCoins,
                    emptyInputs,
                    false);
            } else {
                std::string emptyInputs = "";
                GJGameLevel::saveNewScore(
                    m_level,
                    playTimerMilli,
                    0,
                    m_tickIndex,
                    m_clickIndex,
                    collectedCoins,
                    emptyInputs,
                    collectedCoins >= m_level->m_coinsTime);
            }

            if (m_level->m_levelType == GJLevelType::Main) {
                GameManager* gm = GameManager::sharedState();
                GameManager::reportPercentageForLevel(
                    gm,
                    m_level->m_levelID.value - m_level->m_levelID.seed,
                    100,
                    m_isPracticeMode);
            }
        }

        if (!m_isPracticeMode) {
            if (m_level->m_levelType == GJLevelType::Editor) {
                m_level->m_autoLevel = !m_secretKey;
            } else if (m_level->m_stars.value - m_level->m_stars.seed > 0) {
                GameStatsManager* gsm = GameStatsManager::sharedState();
                if (!GameStatsManager::hasCompletedStarLevel(gsm, m_level))
                    m_endLayerStars = true;

                gsm = GameStatsManager::sharedState();
                GameStatsManager::awardCurrencyForLevel(gsm, m_level);
                m_orbs = m_awardedCurrency;

                if (m_level->m_dailyID.value - m_level->m_dailyID.seed > 0 || m_level->m_gauntletLevel) {
                    gsm = GameStatsManager::sharedState();
                    GameStatsManager::awardDiamondsForLevel(gsm, m_level);
                    m_diamonds = m_awardedDiamonds;
                }

                if (m_orbs > 0) {
                    gsm = GameStatsManager::sharedState();
                    m_hasJumped = GameStatsManager::awardSecretKey(gsm);
                }

                GJBaseGameLayer::processItems(this);

                gsm = GameStatsManager::sharedState();
                GameStatsManager::completedLevel(gsm, m_level);

                gsm = GameStatsManager::sharedState();
                GameStatsManager::checkCoinAchievement(gsm, m_level);

                if (!m_isPracticeMode && m_level->m_levelType == GJLevelType::Editor) {
                    m_level->m_autoLevel = !m_secretKey;
                }
            }
        }
    }

    commitJumps();

    if ((m_level->m_savedTime || m_level->m_savedPoints) &&
        m_level->m_levelType != GJLevelType::Editor &&
        m_level->m_demon.value - m_level->m_demon.seed == 1 &&
        PlatformToolbox::isNetworkAvailable()) {
        if (m_level->m_savedTime) {
            GameLevelManager* glm = GameLevelManager::sharedState();
            GameLevelManager::getLevelLeaderboard(glm, m_level, LevelLeaderboardType::Friends, LeaderboardType::Time);
        }
        if (m_level->m_savedPoints) {
            GameLevelManager* glm = GameLevelManager::sharedState();
            GameLevelManager::getLevelLeaderboard(glm, m_level, LevelLeaderboardType::Friends, LeaderboardType::Points);
        }
    }

    if (!noEffects) {
        int circleIndex = 0;
        do {
            float delay = static_cast<float>(circleIndex) * 0.05f;
            ++circleIndex;
            cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(delay),
                cocos2d::CCCallFunc::create(this, menu_selector(PlayLayer::spawnCircle)),
                nullptr);
            cocos2d::CCNode::runAction(this, seq);
        } while (circleIndex != 5);

        CCCircleWave* circleWave = CCCircleWave::create(10.0, 250.0, 0.5, false);
        circleWave->m_lineWidth = 4;
        circleWave->m_color.r = m_player1->m_playerColor1.r;
        circleWave->m_color.g = m_player1->m_playerColor1.g;
        circleWave->m_color.b = m_player1->m_playerColor1.b;

        cocos2d::CCPoint endPos = getEndPosition();
        circleWave->setPosition(&endPos);
        m_objectLayer->addChild(circleWave, 0);
    }

    if (m_isPracticeMode) {
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.1),
            cocos2d::CCCallFunc::create(this, menu_selector(PlayLayer::showCompleteText)),
            nullptr);
        cocos2d::CCNode::runAction(this, seq);
    } else {
        showCompleteEffect();
    }
}

void PlayLayer::loadDefaultColors() {
    std::unordered_set<int> oldBlendingColors;

    cocos2d::CCArray* allColorActions = m_effectManager->getAllColorActions();

    for (unsigned int i = 0; i < allColorActions->count(); ++i) {
        ColorAction* colorAction = static_cast<ColorAction*>(allColorActions->objectAtIndex(i));
        if (colorAction->m_blending) {
            oldBlendingColors.insert(colorAction->m_colorID);
        }
    }

    m_effectManager->reset();

    cocos2d::CCArray* levelColorActions = m_levelSettings->m_effectManager->getAllColorActions();

    for (unsigned int i = 0; i < levelColorActions->count(); ++i) {
        ColorAction* colorAction = static_cast<ColorAction*>(levelColorActions->objectAtIndex(i));
        updateColor(
            colorAction->m_color,
            colorAction->m_duration,
            colorAction->m_colorID,
            colorAction->m_blending,
            colorAction->m_opacity,
            colorAction->m_hsv,
            colorAction->m_copyID,
            colorAction->m_copyOpacity,
            nullptr,
            0,
            0);
    }

    cocos2d::CCArray* newColorActions = m_effectManager->getAllColorActions();

    for (unsigned int i = 0; i < newColorActions->count(); ++i) {
        ColorAction* colorAction = static_cast<ColorAction*>(newColorActions->objectAtIndex(i));
        int colorID = colorAction->m_colorID;

        if (colorAction->m_blending != (oldBlendingColors.find(colorID) != oldBlendingColors.end())) {
            m_blendingColors.insert(colorID);
        }
    }

    m_effectManager->updateEffects(0.0f);
    m_effectManager->calculateBaseActiveColors();
    m_effectManager->processInheritedColors();
}

void PlayLayer::loadDynamicSaveObjects(std::vector<SavedObjectStateRef>* dynamicObjects) {
    SavedObjectStateRef* start = dynamicObjects->m_start;
    SavedObjectStateRef* finish = dynamicObjects->m_finish;

    if (finish == dynamicObjects->m_start)
        return;

    do {
        double positionX = start->m_positionX;
        GameObject* gameObject = start->m_gameObject;
        float rotationXOffset = start->m_rotationXOffset;
        ++start;

        gameObject->m_positionX = positionX;
        double positionY = start[-1].m_positionY;
        gameObject->m_rotationXOffset = rotationXOffset;
        float rotationYOffset = start[-1].m_rotationYOffset;
        gameObject->m_positionY = positionY;
        gameObject->m_rotationYOffset = rotationYOffset;

        GameObject::addToCustomScaleX(gameObject, start[-1].m_addToCustomScaleX);
        GameObject::addToCustomScaleY(gameObject, start[-1].m_addToCustomScaleY);
        gameObject->setRRotation(gameObject, 0.0);
        gameObject->setRScale(gameObject, 1.0);
        gameObject->m_unk4C4 = -1;
        gameObject->m_unk4CC = -1;

        cocos2d::CCPoint realPosition = gameObject->getRealPosition();
        gameObject->setPosition(&realPosition);

        cocos2d::CCPoint realPosition2 = gameObject->getRealPosition();
        GameObject::setLastPosition(gameObject, &realPosition2);

        gameObject->setObjectRectDirty(gameObject, true);
        gameObject->m_isDirty = true;
        gameObject->setOrientedRectDirty(gameObject, true);

        updateObjectSection(this, gameObject);
    } while (finish != start);
}

CheckpointObject* PlayLayer::loadLastCheckpoint() {
    if (!m_checkpointArray->count())
        return nullptr;

    CheckpointObject* lastCheckpoint = static_cast<CheckpointObject*>(m_checkpointArray->lastObject());
    loadFromCheckpoint(lastCheckpoint);
    return lastCheckpoint;
}

void PlayLayer::manualUpdateObjectColors(GameObject* object) {
    GameObject::updateBlendMode(object);

    const cocos2d::ccColor3B* mainColor = GameObject::colorForMode(object, object->m_activeMainColorID, 1);
    object->updateMainColor(object, mainColor);

    const cocos2d::ccColor3B* detailColor = GameObject::colorForMode(object, object->m_activeDetailColorID, 0);
    object->updateSecondaryColor(object, detailColor);
}

CheckpointObject* PlayLayer::markCheckpoint() {
    if (m_player1->m_isDead)
        return nullptr;

    CheckpointObject* checkpoint = createCheckpoint();
    storeCheckpoint(checkpoint);

    GameObject* checkpointObject = CheckpointObject::getObject(checkpoint);
    checkpointObject->activateObject(checkpointObject);

    if (m_isPlatformer &&
        m_isPracticeMode &&
        m_checkpointArray->count() > 1) {
        int checkpointCount = m_checkpointArray->count();
        cocos2d::CCObject* previousCheckpoint = m_checkpointArray->objectAtIndex(checkpointCount - 2);
        GameObject::makeInvisible(*(GameObject**)&previousCheckpoint[65].m_uOrderOfArrival);
    }

    m_tryPlaceCheckpoint = false;
    return checkpoint;
}

void PlayLayer::onEnterTransitionDidFinish() {
    if (!m_isPaused)
        AppDelegate::get()->m_gamePaused = false;
    cocos2d::CCLayer::onEnterTransitionDidFinish(this);
}

void PlayLayer::onExit() {
    if (!AppDelegate::get()->m_gamePaused) {
        cocos2d::CCDirector::sharedDirector()->m_bSmoothFixCheck = 0;
        AppDelegate::get()->m_gamePaused = true;
        cocos2d::CCLayer::onExit(this);
    }
}

void PlayLayer::onQuit() {
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getScheduler(director)->m_fTimeScale = 1.0;
    cocos2d::CCDirector::sharedDirector()->m_bSmoothFixCheck = 0;

    GameManager* gameManager = GameManager::sharedState();
    GameManager::didExitPlayscene(gameManager);

    PlatformToolbox::showCursor();
    UILayer::disableMenu(m_uiLayer);
    stopRecording();

    cocos2d::CCNode::stopAllActions(this);
    cocos2d::CCNode::unscheduleAllSelectors(this);

    GJBaseGameLayer::resetAudio(this);

    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    FMODAudioEngine::unloadAllEffects(audioEngine);

    GameManager* gameManager2 = GameManager::sharedState();
    GameManager::returnToLastScene(gameManager2, m_level);

    GameManager* gameManager3 = GameManager::sharedState();
    GameManager::fadeInMenuMusic(gameManager3);
}

float PlayLayer::opacityForObject(GameObject* object) {
    return GameObject::groupOpacityMod(object);
}

void PlayLayer::optimizeColorGroups() {
    std::set<int> colorGroups;

    unsigned int objectIndex = 0;
    while (objectIndex < m_objects->count()) {
        cocos2d::CCObject* object = m_objects->objectAtIndex(objectIndex);
        if (HIDWORD(object[18].__vftable) == 1006) {
            int orderOfArrival = object[25].m_uOrderOfArrival;
            colorGroups.insert(orderOfArrival);
        }
        ++objectIndex;
    }

    GJBaseGameLayer::addRemapTargets(this, &colorGroups);

    for (int groupID : colorGroups) {
        cocos2d::CCArray* group = GJBaseGameLayer::getGroup(this, groupID);
        unsigned int memberIndex = 0;
        while (memberIndex < group->count()) {
            GameObject* gameObject = static_cast<GameObject*>(group->objectAtIndex(memberIndex));
            GameObject::addToColorGroup(gameObject, groupID);
            ++memberIndex;
        }
    }
}

void PlayLayer::optimizeOpacityGroups() {
    std::set<int> opacityGroups;

    unsigned int objectIndex = 0;
    while (objectIndex < m_objects->count()) {
        cocos2d::CCObject* object = m_objects->objectAtIndex(objectIndex);
        if (HIDWORD(object[18].__vftable) == 1007) {
            int orderOfArrival = object[25].m_uOrderOfArrival;
            opacityGroups.insert(orderOfArrival);
        }
        ++objectIndex;
    }

    GJBaseGameLayer::addRemapTargets(this, &opacityGroups);

    for (int groupID : opacityGroups) {
        cocos2d::CCArray* group = GJBaseGameLayer::getGroup(this, groupID);
        unsigned int memberIndex = 0;
        while (memberIndex < group->count()) {
            GameObject* gameObject = static_cast<GameObject*>(group->objectAtIndex(memberIndex));
            GameObject::addToOpacityGroup(gameObject, groupID);
            ++memberIndex;
        }
    }
}

void PlayLayer::pauseGame(bool unfocused) {
    if (!canPauseGame() || BYTE1(AppDelegate::get()->m_startupScriptFilename.m_data.m_data))
        return;

    applyTimeWarp(this, 1.0);
    m_isPaused = true;
    m_unk38b0 = 0.0;
    m_unk38b8 = 0.0;
    m_unk38c0 = 0.0;
    m_pauseTime = getTempMilliTime();
    cocos2d::CCDirector::sharedDirector()->m_bSmoothFixCheck = 0;
    PlatformToolbox::showCursor();
    GJBaseGameLayer::processQueuedButtons(this, 0.0, true);
    PlayerObject::releaseAllButtons(m_player1);
    GJBaseGameLayer::pauseAudio(this);
    GameManager::sharedState();
    PlayerObject::releaseAllButtons(m_player1);
    UILayer::resetAllButtons(m_uiLayer);
    UILayer::toggleMenuVisibility(m_uiLayer, false);

    PauseLayer* pauseLayer = PauseLayer::create(unfocused);
    cocos2d::CCNode* parent = getParent(this);
    parent->addChild(pauseLayer, 10);

    onExit(this);
}

void PlayLayer::playGravityEffect(bool flip) {
    if (m_disableGravityEffect || GameManager::sharedState()->m_performanceMode)
        return;

    if (m_activeGravityEffects >= m_totalGravityEffects)
        return;

    m_activeGravityEffects = m_activeGravityEffects + 1;

    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = cocos2d::CCDirector::getWinSize(director);

    GravityEffectSprite* gravitySprite = static_cast<GravityEffectSprite*>(
        m_gravityEffects->objectAtIndex(m_gravityEffectIndex));
    gravitySprite->removeFromParentAndCleanup(gravitySprite, true);

    int nextIndex = (m_gravityEffectIndex < m_totalGravityEffects - 1) ? m_gravityEffectIndex + 1 : 0;
    m_gravityEffectIndex = nextIndex;

    cocos2d::CCPoint topPos(winSize.width * 0.5f, winSize.height + 95.0);
    cocos2d::CCPoint bottomPos(winSize.width * 0.5f, -95.0);

    if (m_player1->m_isSideways) {
        cocos2d::CCPoint sidewaysBottom(winSize.width + 95.0, winSize.height * 0.5f);
        bottomPos = sidewaysBottom;
        cocos2d::CCPoint sidewaysTop(-95.0, winSize.height * 0.5f);
        topPos = sidewaysTop;
    }

    cocos2d::CCPoint startPos;
    cocos2d::CCPoint endPos;
    cocos2d::ccColor3B spriteColor;

    if (flip) {
        startPos = bottomPos;
        endPos = topPos;
        spriteColor = cocos2d::ccc3(255, 255, 255);
    } else {
        startPos = topPos;
        endPos = bottomPos;
        cocos2d::CCSprite::setFlipY(gravitySprite, true);
        spriteColor = cocos2d::ccc3(0, 255, 255);
    }

    float rotation = m_player1->m_isSideways ? 90.0 : 0.0;
    gravitySprite->setRotation(rotation);

    addChild(gravitySprite, 0);
    gravitySprite->setPosition(&startPos);
    GravityEffectSprite::updateSpritesColor(gravitySprite, spriteColor);

    cocos2d::CCSequence* moveSequence = cocos2d::CCSequence::create(
        cocos2d::CCMoveTo::create(0.4, &endPos),
        cocos2d::CCCallFunc::create(gravitySprite, callfunc_selector(GravityEffectSprite::animFinished)),
        nullptr);
    cocos2d::CCNode::runAction(gravitySprite, moveSequence);

    cocos2d::CCSequence* finishSequence = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.4),
        cocos2d::CCCallFunc::create(this, menu_selector(PlayLayer::gravityEffectFinished)),
        nullptr);
    cocos2d::CCNode::runAction(this, finishSequence);
}

void PlayLayer::playReplay(std::string inputs) {
}

cocos2d::CCPoint PlayLayer::posForTime(float time) {
    cocos2d::CCPoint result = LevelTools::posForTime(
        time,
        m_speedObjects,
        m_levelSettings->m_startSpeed,
        m_player1->m_isPlatformer,
        &m_gameState.m_rotateChannel);

    cocos2d::CCPoint returnVal;
    returnVal.y = result.y;
    returnVal.x = result.x;
    return returnVal;
}

void PlayLayer::postUpdate(float dt) {
    if (!m_skipAudioStep)
        AudioEffectsLayer::audioStep(m_audioEffectsLayer, dt);

    updateVisibility(this, dt);
    GJBaseGameLayer::updateLevelColors(this);
    updateProgressbar();
    updateInfoLabel();
    updateEffectPositions();

    if (m_recordingStopped) {
        for (unsigned int index = 0; index < m_batchNodes->count(); ++index) {
            cocos2d::CCNode* batchNode = static_cast<cocos2d::CCNode*>(m_batchNodes->objectAtIndex(index));
            batchNode->removeFromParentAndCleanup(false);
        }
        onQuit();
    }
}

void PlayLayer::prepareCreateObjectsFromSetup(std::string* levelString) {
    bool hasInvalidPrefix;

    std::string spaceString = " ";
    if (levelString->compare(0, spaceString.length(), spaceString) != 0) {
        hasInvalidPrefix = true;
    } else {
        hasInvalidPrefix = levelString->length() != spaceString.length();
    }

    if (hasInvalidPrefix) {
        std::string commaString = ",";
        hasInvalidPrefix = true;
        if (levelString->compare(0, commaString.length(), commaString) == 0) {
            hasInvalidPrefix = levelString->length() != commaString.length();
        }
    }

    if (hasInvalidPrefix) {
        size_t searchStart = 0;
        size_t foundIndex = levelString->find(';', 0);

        while (true) {
            size_t substringLength = (foundIndex == std::string::npos) ?
                (levelString->length() - searchStart) : (foundIndex - searchStart);
            std::string substring = levelString->substr(searchStart, substringLength);

            if (!(levelString->length() == searchStart && substring.empty())) {
                std::string* finish = m_objectStrings.m_finish;
                if (finish == m_objectStrings.m_reserveEnd) {
                    m_objectStrings.push_back(substring);
                } else {
                    if (finish)
                        *finish = substring;
                    m_objectStrings.m_finish = finish + 1;
                }
            }

            if (foundIndex == std::string::npos)
                break;

            searchStart = foundIndex + 1;
            foundIndex = levelString->find(';', foundIndex + 1);
        }

        LevelSettingsObject* levelSettings = LevelSettingsObject::objectFromString(m_objectStrings.m_start);
        m_levelSettings = levelSettings;
        cocos2d::CCObject::retain(levelSettings);
        m_levelSettings->m_level = m_level;
        GJBaseGameLayer::loadLevelSettings(this);

        m_effectManager = m_levelSettings->m_effectManager;

        cocos2d::ccColor3B playerColor1 = m_player1->m_playerColor1;
        cocos2d::ccColor3B playerColor2 = m_player1->m_playerColor2;

        GJEffectManager::updateColors(m_effectManager, playerColor1, playerColor2);

        GameManager* gameManager = GameManager::sharedState();
        GameManager::loadFont(gameManager, m_levelSettings->m_fontIndex);
    }

    cocos2d::CCArray* coinArray = cocos2d::CCArray::create();
    m_coinArray = coinArray;
    cocos2d::CCObject::retain(coinArray);
    m_objectsCreated = 1;

    levelString->clear();
}

void PlayLayer::prepareMusic(bool dontWait) {
    if ((m_isPracticeMode && !m_practiceMusicSync) || m_playerDied)
        return;

    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    std::string audioFileName = GJGameLevel::getAudioFileName(m_level);

    FMODAudioEngine::loadMusic(
        audioEngine,
        audioFileName,
        1.0,
        0.0,
        1.0,
        m_isPlatformer,
        0,
        0,
        m_levelSettings->m_dontReset);

    double levelTime = m_gameState.m_levelTime;
    float songOffset = m_levelSettings->m_songOffset;

    if (!m_levelSettings->m_dontReset) {
        FMODAudioEngine* audioEngine2 = FMODAudioEngine::sharedEngine();
        float musicTimeMs = (songOffset + levelTime) * 1000.0f;
        FMODAudioEngine::setMusicTimeMS(audioEngine2, static_cast<unsigned int>(musicTimeMs), dontWait, false);
    }

    m_musicPrepared = true;
}

void PlayLayer::processCheckpoints() {
    if (m_tryPlaceCheckpoint) {
        m_tryPlaceCheckpoint = false;
        markCheckpoint();
    }

    if (!m_activatedCheckpoint)
        return;

    GameManager* gameManager = GameManager::sharedState();
    if (GameManager::getGameVariable(gameManager, "0146"))
        return;

    CheckpointObject* checkpoint = markCheckpoint();
    if (!checkpoint)
        return;

    cocos2d::CCPoint checkpointPosition;
    m_activatedCheckpoint->getRealPosition(&checkpointPosition);

    CheckpointGameObject* activatedCheckpoint = m_activatedCheckpoint;
    if (activatedCheckpoint->m_targetPlayer1) {
        const cocos2d::CCPoint* playerPos = m_player1->getPosition(m_player1);
        checkpointPosition = *playerPos;
    } else {
        int centerGroupID = activatedCheckpoint->m_centerGroupID;
        if (centerGroupID > 0) {
            GameObject* mainObject = GJBaseGameLayer::tryGetMainObject(this, centerGroupID);
            if (mainObject) {
                cocos2d::CCPoint mainObjectPos;
                mainObject->getRealPosition(&mainObjectPos);
                checkpointPosition = mainObjectPos;
            }
        }
    }

    GameObject* checkpointObject = CheckpointObject::getObject(checkpoint);
    checkpointObject->setStartPos(&checkpointPosition);

    PlayerCheckpoint* playerCheckpoint = checkpoint->m_player1Checkpoint;
    playerCheckpoint->m_position = checkpointPosition;
    checkpoint->m_player1Checkpoint->m_platformerCheckpoint = true;

    GameObject* checkpointGameObject = CheckpointObject::getObject(checkpoint);
    GameObject::makeInvisible(checkpointGameObject);

    checkpoint->m_uniqueID = m_activatedCheckpoint->m_uniqueID;
    checkpoint->m_respawnID = m_activatedCheckpoint->m_respawnID;

    int targetGroupID = m_activatedCheckpoint->m_targetGroupID;
    if (targetGroupID > 0) {
        std::vector<int> emptyVec;
        GJBaseGameLayer::spawnGroup(
            this,
            targetGroupID,
            false,
            &emptyVec,
            m_activatedCheckpoint->m_uniqueID,
            m_activatedCheckpoint->m_controlID);
    }

    GJBaseGameLayer::gameEventTriggered(this, GJGameEvent::Checkpoint, 0, 0);
    m_activatedCheckpoint = nullptr;
}

void PlayLayer::processCreateObjectsFromSetup() {

    std::vector<std::string> parsedValues(601);
    std::vector<void*> ownerRefs(601, nullptr);

    int totalObjects = static_cast<int>(m_objectStrings.size());
    float chunkDivisor = roundf(static_cast<float>(totalObjects) / 1000.0f) + 10.0f;
    if (chunkDivisor > 100.0f)
        chunkDivisor = 100.0f;

    int chunkSize = static_cast<int>(ceilf(static_cast<float>(totalObjects) / chunkDivisor));
    int startIndex = m_objectsCreated;
    int targetCount = startIndex + chunkSize;
    int limit = std::min(targetCount, totalObjects);

    GJLevelType levelType = m_level->m_levelType;
    bool lowDetail = m_level->m_lowDetailModeToggled;

    int currentIndex = startIndex;
    while (currentIndex < limit) {
        std::string& objectStr = m_objectStrings[currentIndex];

        for (int i = 0; i < 601; ++i) {
            ownerRefs[i] = nullptr;
        }

        size_t objectLength = objectStr.length();
        size_t commaPos = objectStr.find(',', 0);

        if (commaPos != std::string::npos) {
            size_t segmentStart = 0;
            bool parsingId = true;
            int currentId = 0;
            bool finished = false;

            while (!finished) {
                if (segmentStart != objectLength) {
                    size_t segmentLength = commaPos - segmentStart;
                    std::string segment = objectStr.substr(segmentStart, segmentLength);

                    if (parsingId) {
                        currentId = atoi(segment.c_str());
                    } else if (currentId >= 1 && currentId <= 600) {
                        parsedValues[currentId] = segment;
                        ownerRefs[currentId] = this;
                    }
                    parsingId = !parsingId;
                }

                size_t nextStart = commaPos + 1;
                commaPos = objectStr.find(',', nextStart);
                segmentStart = nextStart;

                if (commaPos == std::string::npos) {
                    commaPos = objectLength;
                    finished = true;
                }
            }
        }

        GameObject* newObj = GameObject::objectFromVector(&parsedValues, &ownerRefs, this, lowDetail);

        if (newObj) {
            int objectID = newObj->m_objectID;
            if (objectID == 31) {
                StartPosObject* startPos = static_cast<StartPosObject*>(newObj);
                StartPosObject::loadSettingsFromString(startPos, objectStr);
            } else if (objectID == 2065) {
                ParticleGameObject::updateParticleStruct(static_cast<ParticleGameObject*>(newObj));
            }

            bool isSecretCoin = (newObj->getType() == GameObjectType::SecretCoin);
            bool isMainLevel = (levelType == GJLevelType::Main);

            if (!isSecretCoin || isMainLevel) {
                if (newObj->m_mainColorKeyIndex > 0) {
                    if (newObj->m_mainColorKeyIndex > m_nextColorKey)
                        m_nextColorKey = newObj->m_mainColorKeyIndex;
                    if (newObj->m_detailColorKeyIndex > m_nextColorKey)
                        m_nextColorKey = newObj->m_detailColorKeyIndex;
                } else {
                    int iterations = GameObject::hasSecondaryColor(newObj) ? 2 : 1;
                    for (int c = 0; c < iterations; ++c) {
                        bool isMain = ((c ^ 1) & 1) != 0;
                        std::string colorKey = GameObject::getColorKey(newObj, isMain, false);

                        int assignedKey;
                        cocos2d::CCObject* existingKey = m_colorKeyDict->objectForKey(colorKey);
                        if (existingKey) {
                            assignedKey = existingKey->getTag();
                        } else {
                            assignedKey = m_nextColorKey;
                            cocos2d::CCObject* keyHolder = new cocos2d::CCObject();
                            keyHolder->setTag(assignedKey);
                            keyHolder->autorelease();
                            m_colorKeyDict->setObject(keyHolder, colorKey);
                            ++m_nextColorKey;
                        }

                        if (isMain) {
                            newObj->m_mainColorKeyIndex = assignedKey;
                        } else {
                            newObj->m_detailColorKeyIndex = assignedKey;
                        }
                    }
                }

                bool skipAdd = false;
                if (newObj->getType() == GameObjectType::UserCoin) {
                    if (m_coinArray->count() > 2) {
                        skipAdd = true;
                    } else {
                        m_coinArray->addObject(newObj);
                    }
                }

                if (!skipAdd) {
                    addObject(newObj);
                }
            }
        }

        objectStr.clear();
        currentIndex++;
    }

    m_objectsCreated = targetCount;

    float progress = static_cast<float>(m_objectsCreated - 1) / static_cast<float>(totalObjects);
    if (progress > 1.0f)
        progress = 1.0f;
    m_loadingProgress = progress;

    if (m_objectsCreated >= totalObjects) {
        createObjectsFromSetupFinished();
        m_loadingProgress = 1.0f;
        setupHasCompleted();
    }
}

void PlayLayer::processLoadedMoveActions() {
    std::unordered_map<int, std::pair<double, double>> loadedMoveOffset;
    GJEffectManager::getLoadedMoveOffset(m_effectManager, &loadedMoveOffset);

    for (auto& pair : loadedMoveOffset) {
        cocos2d::CCArray* group = GJBaseGameLayer::getGroup(this, pair.first);
        if (!group)
            continue;

        cocos2d::ccArray* data = group->data;
        if (!data->num)
            continue;

        GameObject** objects = reinterpret_cast<GameObject**>(data->arr);
        for (GameObject** iter = &objects[data->num - 1]; iter >= objects; ++iter) {
            GameObject* gameObject = *iter;
            if (!gameObject)
                break;
            if (gameObject->m_unk530)
                continue;

            GameObject::addToTempOffset(gameObject, pair.second.first, pair.second.second);
            gameObject->setObjectRectDirty(gameObject, true);
            gameObject->m_isDirty = true;
            gameObject->setOrientedRectDirty(gameObject, true);

            updateObjectSection(this, gameObject);
            gameObject->m_unk4C4 = -1;

            cocos2d::CCPoint realPosition = gameObject->getRealPosition();
            GameObject::setLastPosition(gameObject, &realPosition);
        }
    }
}

void PlayLayer::queueCheckpoint() {
    m_tryPlaceCheckpoint = true;
}

void PlayLayer::removeAllCheckpoints() {
    while (m_checkpointArray->count())
        removeCheckpoint(false);
}

void PlayLayer::removeAllObjects() {
    m_recordingStopped = false;
    UILayer::disableMenu(m_uiLayer);

    m_player1->deactivateObject(m_player1, true);
    PlayerObject::deactivateParticle(m_player1);
    PlayerObject::deactivateStreak(m_player1, true);
    PlayerObject::toggleGhostEffect(m_player1, GhostType::Disabled);

    if (m_gameState.m_isDualMode) {
        m_player2->deactivateObject(m_player2, true);
        PlayerObject::deactivateParticle(m_player2);
        PlayerObject::deactivateStreak(m_player2, true);
        PlayerObject::toggleGhostEffect(m_player2, GhostType::Disabled);
    }

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        unsigned int objectCount = data->num;
        if (objectCount) {
            cocos2d::CCObject** objects = data->arr;
            cocos2d::CCObject** lastObject = &objects[objectCount - 1];
            if (objects <= lastObject) {
                cocos2d::CCObject* object = *objects;
                if (object) {
                    do {
                        ++objects;
                        object->deactivateObject(true);
                        object->destroyObject();
                        if (lastObject < objects)
                            break;
                        object = *objects;
                    } while (*objects);
                }
            }
        }
    }

    cocos2d::CCArray::removeAllObjects(m_objects, nullptr);
}

void PlayLayer::removeCheckpoint(bool first) {
    if (!m_checkpointArray->count())
        return;

    if (first) {
        CheckpointObject* checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(0));
        GameObject* checkpointObject = CheckpointObject::getObject(checkpoint);
        GJBaseGameLayer::removeObjectFromSection(this, checkpointObject);
        GameObject::removeGlow(checkpointObject);
        checkpointObject->removeMeAndCleanup(checkpointObject);
        cocos2d::CCArray::removeObjectAtIndex(m_checkpointArray, 0, true);
    } else {
        CheckpointObject* checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->lastObject());
        GameObject* checkpointObject = CheckpointObject::getObject(checkpoint);
        GJBaseGameLayer::removeObjectFromSection(this, checkpointObject);
        GameObject::removeGlow(checkpointObject);
        checkpointObject->removeMeAndCleanup(checkpointObject);
        cocos2d::CCArray::removeLastObject(m_checkpointArray, true);

        if (m_isPlatformer && m_isPracticeMode) {
            if (m_checkpointArray->count()) {
                cocos2d::CCObject* lastCheckpoint = m_checkpointArray->lastObject();
                GameObject::makeVisible(*reinterpret_cast<GameObject**>(&lastCheckpoint[65].m_uOrderOfArrival));
                lastCheckpoint[83].m_uID = m_gameState.m_commandIndex;
            }
        }
    }
}

void PlayLayer::removeFromGroupOld(GameObject* object) {
    int groupIndex = 0;
    while (object->m_groupCount > groupIndex) {
        int currentIndex = groupIndex++;
        int groupID = GameObject::getGroupID(object, currentIndex);
        cocos2d::CCArray* group = GJBaseGameLayer::getGroup(this, groupID);
        cocos2d::CCArray::removeObject(group, reinterpret_cast<cocos2d::_ccArray*>(object), reinterpret_cast<cocos2d::CCObject*>(true));
    }
}

void PlayLayer::resetLevel() {
    bool shouldSkipAudioReset = false;

    if (m_freezeStartCamera || (!m_isPlatformer && !m_isPracticeMode)) {
        if (m_platformerRestart) {
            if (!m_ignoreDamage) {
                m_platformerRestart = false;
                toggleIgnoreDamage(false);
                m_freezeStartCamera = m_hasCompletedLevel;
            }
        } else {
            m_platformerRestart = m_ignoreDamage;
        }

        if (m_freezeStartCamera) {
            shouldSkipAudioReset = true;
        }
    }

    if (!shouldSkipAudioReset) {
        FMODAudioEngine::sharedEngine()->stopAllEffects();
        if (!m_isPracticeMode || m_practiceMusicSync) {
            if (m_isPlatformer) {
                FMODAudioEngine::sharedEngine()->pauseAllMusic(false);
            } else {
                GJBaseGameLayer::pauseAudio();
            }
        }
    }

    GJBaseGameLayer::preResumeGame();
    dwTickStartp = clock();

    GJBaseGameLayer::resetLevelVariables();

    if (m_isDebugDrawEnabled && !shouldDebugDraw())
        toggleDebugDraw();

    m_gameState.m_commandIndex = 0;
    m_gameState.m_currentProgress = 0;
    m_currentStep = 0;
    m_startingFromBeginning = true;
    m_activatedCheckpoint = nullptr;
    m_tryPlaceCheckpoint = false;

    bool hasReplayRandSeed = m_isPracticeMode;
    m_unk32ec = nullptr;
    m_unk32d0 = nullptr;

    if (hasReplayRandSeed && m_replayRandSeed != 0) {
        m_randomSeed = m_replayRandSeed;
    } else {
        struct timeval timeVal;
        gettimeofday(&timeVal, nullptr);
        m_replayRandSeed = timeVal.tv_usec * timeVal.tv_sec;
        m_randomSeed = timeVal.tv_usec * timeVal.tv_sec;
    }

    GameToolbox::fast_srand(m_replayRandSeed);

    m_activeObjectsCount = 0;
    m_unk38c8 = false;
    m_unk36c8 = 0;

    UILayer::toggleMenuVisibility(m_uiLayer, true);

    m_unk3906 = false;
    m_unk38c8 = false;
    m_bgEffectDisabled = false;

    if (m_endPortal) {
        m_unk37e0 = false;
        m_endPortal->m_flippedX = false;
        m_endPortal->m_startPosHeightRelated = 0;
        m_endPortal->setScaleX(1.0);

        m_levelLength = m_endXPosition;
        cocos2d::CCPoint endPortalPos(m_endXPosition, 225.0);
        m_endPortal->setStartPos(&endPortalPos);

        GJBaseGameLayer::reorderObjectSection(m_endPortal);
        m_endPortal->deactivateObject(true);
    }

    m_endLayerStars = false;
    m_orbs = 0;
    m_diamonds = 0;
    m_hasJumped = false;
    m_secretKey = false;

    m_unk38d0 = "";

    m_activeGravityEffects = 0;
    m_unk3900 = 0.0;
    UILayer::enableMenu(m_uiLayer);

    GJBaseGameLayer::resetActiveEnterEffects();

    if (m_attemptTime > m_bestAttemptTime)
        m_bestAttemptTime = m_attemptTime;

    m_clicks = 0;
    m_attemptTime = 0.0;
    m_currentTime = 0.0;
    m_pauseDelta = 0.0;

    toggleGlitter(false);

    if (m_objects->count()) {
        unsigned int objectIndex = 0;
        while (true) {
            GameObject* gameObject = static_cast<GameObject*>(m_objects->objectAtIndex(objectIndex));
            cocos2d::CCPoint realPositionBefore = gameObject->getRealPosition();
            gameObject->resetObject();
            gameObject->getRealPosition();

            int outerSectionIndex = gameObject->m_outerSectionIndex;
            if (outerSectionIndex >= 0) {
                int newOuterSectionIndex;
                int newMiddleSectionIndex;

                double positionX = gameObject->m_positionX;
                if (positionX <= 0.0) {
                    newOuterSectionIndex = 0;
                } else if (positionX >= 10000000.0) {
                    newOuterSectionIndex = static_cast<int>(m_sectionXFactor * 10000000.0);
                } else {
                    newOuterSectionIndex = static_cast<int>(positionX * m_sectionXFactor);
                }

                double positionY = gameObject->m_positionY;
                if (positionY <= 0.0) {
                    newMiddleSectionIndex = 0;
                } else if (positionY >= 10000000.0) {
                    newMiddleSectionIndex = static_cast<int>(m_sectionYFactor * 10000000.0);
                } else {
                    newMiddleSectionIndex = static_cast<int>(positionY * m_sectionYFactor);
                }

                if (newOuterSectionIndex != outerSectionIndex ||
                    newMiddleSectionIndex != gameObject->m_middleSectionIndex) {
                    GJBaseGameLayer::reorderObjectSection(gameObject);

                    int currentOuter = gameObject->m_outerSectionIndex;
                    if (currentOuter > m_rightSectionIndex ||
                        currentOuter < m_leftSectionIndex ||
                        gameObject->m_middleSectionIndex > m_topSectionIndex ||
                        gameObject->m_middleSectionIndex < m_bottomSectionIndex) {
                        gameObject->deactivateObject(true);
                    }
                }
            }

            cocos2d::CCPoint realPositionAfter = gameObject->getRealPosition();
            bool positionUnchanged = (realPositionBefore.x == realPositionAfter.x &&
                                      realPositionBefore.y == realPositionAfter.y);

            ++objectIndex;

            if (!positionUnchanged) {
                gameObject->m_isDirty = true;
                gameObject->setObjectRectDirty(true);
                gameObject->setOrientedRectDirty(true);
            }

            if (objectIndex >= m_objects->count())
                break;
        }
    }

    AudioEffectsLayer::resetAudioVars(m_audioEffectsLayer);
    loadDefaultColors();

    CheckpointObject* lastCheckpoint = loadLastCheckpoint();

    if (!m_isPracticeMode) {
        m_unk3900 = GJEffectManager::getPersistentStateString(m_effectManager);
    }

    GJEffectManager::resetUsedItemState(m_effectManager);

    if (lastCheckpoint) {
        if (!m_isPracticeMode)
            GJBaseGameLayer::resetRecord(m_gameState.m_unkUint4, false);
        m_unk38b8 = false;
    } else {
        m_tickIndex = 0;
        m_clickIndex = 0;
        m_timePlayed = 0.0;
        GJBaseGameLayer::resetRecord(0, false);
        m_unk32e0 = m_randomSeed;

        if (m_startPosObject) {
            if (m_currentCheckpoint) {
                loadFromCheckpoint(m_currentCheckpoint);
            } else {
                GJBaseGameLayer::setupLevelStart(m_startPosObject->m_startSettings);
            }
        } else {
            GJBaseGameLayer::setupLevelStart(m_levelSettings);
        }
    }

    m_player1->m_position = *m_player1->getPosition();

    if (m_gameState.m_isDualMode) {
        m_player2->m_position = *m_player2->getPosition();
    } else {
        cocos2d::CCPoint zeroPoint(0.0f, 0.0f);
        m_player2->setPosition(&zeroPoint);
        m_player2->m_position = zeroPoint;
    }

    if (!lastCheckpoint) {
        GameObject::setLastPosition(m_player1, m_player1->getPosition());
        GameObject::setLastPosition(m_player2, m_player2->getPosition());

        if (m_startPosObject) {
            if (!m_currentCheckpoint)
                GJBaseGameLayer::loadStartPosObject();
        }
    }

    GJBaseGameLayer::updatePlayerCollisionBlocks();

    if (!m_unk37b0) {
        PlayerObject::playSpawnEffect(m_player1);
        if (m_gameState.m_isDualMode)
            PlayerObject::playSpawnEffect(m_player2);
    }

    GJBaseGameLayer::stopCameraShake();

    if (!m_startPosObject && !lastCheckpoint) {
        m_startingFromBeginning = true;
        GJBaseGameLayer::checkSpawnObjects();
    }

    GJBaseGameLayer::updateProximityVolumeEffects();
    GJBaseGameLayer::updateLevelColors();
    GJBaseGameLayer::updateCamera(0.0);
    updateVisibility(0.0);
    GJBaseGameLayer::updateGradientLayers();
    GJBaseGameLayer::updateShaderLayer(0.0);
    updateAttempts();
    m_savedAttempts = m_attempts;
    m_unk3900 = m_unk38d0;

    if (lastCheckpoint && lastCheckpoint->m_respawnID > 0) {
        std::vector<int> emptyVec;
        GJBaseGameLayer::spawnGroup(
            lastCheckpoint->m_respawnID,
            false,
            &emptyVec,
            lastCheckpoint->m_uniqueID,
            0);

        GJBaseGameLayer::gameEventTriggered(GJGameEvent::CheckpointRespawn, 0, 0);
    }

    GameStatsManager* gameStatsManager = GameStatsManager::sharedState();
    GameStatsManager::incrementStat(gameStatsManager, "2");

    int attemptsValue = m_level->m_attempts.value + 1;
    m_level->m_attempts.value = attemptsValue;
    int seed = static_cast<int>(static_cast<float>(rand()) * 4.6566e-10f * 1000.0f);
    m_level->m_attempts.seed = seed;
    m_level->m_attempts.random = attemptsValue + seed;

    updateProgressbar();
    GJBaseGameLayer::refreshCounterLabels();

    bool wasPaused = m_isPaused;
    bool isFirstAttempt = (lastCheckpoint == nullptr);

    if (m_isPlatformer) {
        m_isPaused = false;
    } else if (isFirstAttempt && wasPaused) {
        if (!m_startPosObject)
            startMusic();
    }

    if (GameManager::sharedState()->m_recordGameplay && !m_isPracticeMode && !m_isTestMode) {
        tryStartRecord();
        GJBaseGameLayer::removeReleasedButtons();

        if (UILayer::isJumpButtonPressed(m_uiLayer))
            GJBaseGameLayer::queueButton(1, true, false, 0.0);
        else
            GJBaseGameLayer::queueButton(1, false, false, 0.0);
    } else {
        GJBaseGameLayer::removeReleasedButtons();

        if (UILayer::isJumpButtonPressed(m_uiLayer))
            GJBaseGameLayer::queueButton(1, true, false, 0.0);
        else
            GJBaseGameLayer::queueButton(1, false, false, 0.0);
    }

    if (!m_isPracticeMode && !m_platformerRestart)
        m_damageVerified = false;

    m_hasCompletedLevel = false;
    m_unk37b0 = false;
    GJBaseGameLayer::syncBGTextures();

    dwTickEndp = clock();
    dwDurationp = dwTickEndp - dwTickStartp;

    if (m_batchNodes) {
        cocos2d::ccArray* data = m_batchNodes->data;
        if (data->num) {
            cocos2d::CCNode** nodes = reinterpret_cast<cocos2d::CCNode**>(data->arr);
            cocos2d::CCNode** lastNode = &nodes[data->num - 1];
            while (lastNode >= nodes) {
                cocos2d::CCNode* node = *nodes;
                if (!node)
                    break;
                ++nodes;
                cocos2d::CCNode::qsortAllChildrenWithIndex(node);
            }
        }
    }

    GJBaseGameLayer::sortSectionVector();

    if (m_isPlatformer && (wasPaused || m_inResetDelay)) {
        m_inResetDelay = false;

        if (isFirstAttempt) {
            cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0.2),
                cocos2d::CCCallFunc::create(this, menu_selector(PlayLayer::startGameDelayed)),
                nullptr);
            cocos2d::CCNode::runAction(this, seq);
            m_startingFromBeginning = false;
            m_started = true;
            return;
        }

        m_isPaused = true;
        m_startingFromBeginning = false;
        m_started = true;
    } else {
        m_startingFromBeginning = false;
        m_started = true;
        if (!lastCheckpoint)
            return;
    }

    m_gameState.m_cameraPosition2 = lastCheckpoint->m_gameState.m_cameraPosition2;
    m_gameState.m_cameraStepDiff = lastCheckpoint->m_gameState.m_cameraStepDiff;
    m_gameState.m_unkPoint4 = lastCheckpoint->m_gameState.m_unkPoint4;
}

void PlayLayer::resetLevelFromStart() {
    GameManager* gameManager = GameManager::sharedState();
    bool checkpointsDisabled = GameManager::getGameVariable(gameManager, "0146");

    GameManager* gameManager2 = GameManager::sharedState();
    GameManager::setGameVariable(gameManager2, "0146", true);

    removeAllCheckpoints();

    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    FMODAudioEngine::stopAllMusic(audioEngine, true);

    m_freezeStartCamera = true;
    resetLevel();

    GameManager* gameManager3 = GameManager::sharedState();
    GameManager::setGameVariable(gameManager3, "0146", checkpointsDisabled);
}

void PlayLayer::resetSPTriggered() {
    if (!m_speedObjects)
        return;

    cocos2d::ccArray* data = m_speedObjects->data;
    unsigned int count = data->num;
    if (!count)
        return;

    cocos2d::CCObject** objects = data->arr;
    for (cocos2d::CCObject** iter = &objects[count - 1]; iter >= objects; ++iter) {
        cocos2d::CCObject* object = *iter;
        if (!object)
            break;
        object->setTag(0);
    }
}

void PlayLayer::resume() {
    if (!AppDelegate::get()->m_gamePaused)
        return;

    if (m_playerDied) {
        FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
        FMODAudioEngine::resumeAllAudio(audioEngine);

        FMODAudioEngine* audioEngine2 = FMODAudioEngine::sharedEngine();
        audioEngine2->m_system->update();
    } else {
        GJBaseGameLayer::preResumeGame();
        m_audioPaused = true;
    }

    float refreshRate = PlatformToolbox::getDeviceRefreshRate();
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->setAnimationInterval(static_cast<float>(1.0 / refreshRate));

    FMODAudioEngine* audioEngine3 = FMODAudioEngine::sharedEngine();
    float bgmVolume = FMODAudioEngine::getBackgroundMusicVolume(audioEngine3);

    m_isPaused = false;
    m_isSilent = (bgmVolume <= 0.0);
    cocos2d::CCDirector::sharedDirector()->m_bSmoothFixCheck = true;

    GameManager* gameManager = GameManager::sharedState();
    if (GameManager::getGameVariable(gameManager, "0128")) {
        PlatformToolbox::toggleLockCursor(true);
    } else {
        GameManager* gameManager2 = GameManager::sharedState();
        if (!GameManager::getGameVariable(gameManager2, "0024"))
            PlatformToolbox::hideCursor();
    }

    AppDelegate::get()->m_gamePaused = false;
    onEnter();

    PlayerObject::releaseAllButtons(m_player1);
    UILayer::refreshDpad(m_uiLayer);
    UILayer::resetAllButtons(m_uiLayer);
    UILayer::toggleMenuVisibility(m_uiLayer, true);

    m_pauseDelta = m_pauseDelta + getTempMilliTime() - m_pauseTime;
}

void PlayLayer::resumeAndRestart(bool fromStart) {
    if (!m_isPracticeMode || m_practiceMusicSync) {
        if (!fromStart && m_isPlatformer) {
            FMODAudioEngine* audioEngine2 = FMODAudioEngine::sharedEngine();
            FMODAudioEngine::stopAllEffects(audioEngine2);

            FMODAudioEngine* audioEngine3 = FMODAudioEngine::sharedEngine();
            FMODAudioEngine::stopAllMusic(audioEngine3, false);
        } else {
            GJBaseGameLayer::resetAudio();
        }

        m_musicPrepared = false;

        FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
        m_isSilent = (FMODAudioEngine::getBackgroundMusicVolume(audioEngine) <= 0.0);
    }

    m_unk38d0 = false;

    if (!AppDelegate::get()->m_gamePaused)
        return;

    AppDelegate::get()->m_gamePaused = false;
    PlayerObject::releaseAllButtons(m_player1);
    UILayer::resetAllButtons(m_uiLayer);
    UILayer::toggleMenuVisibility(m_uiLayer, true);

    GameManager* gameManager = GameManager::sharedState();
    if (GameManager::getGameVariable(gameManager, "0128")) {
        PlatformToolbox::toggleLockCursor(true);
        onEnter();
        if (!fromStart) {
            resetLevel();
            return;
        }
    } else {
        GameManager* gameManager2 = GameManager::sharedState();
        if (!GameManager::getGameVariable(gameManager2, "0024"))
            PlatformToolbox::hideCursor();

        onEnter();
        if (!fromStart) {
            resetLevel();
            return;
        }
    }

    resetLevelFromStart();
}

void PlayLayer::saveDynamicSaveObjects(std::vector<SavedObjectStateRef>* dynamicObjects) {
    dynamicObjects->reserve(m_dynamicSaveObjects.size());

    for (GameObject* gameObject : m_dynamicSaveObjects) {
        double positionX = gameObject->m_positionX - gameObject->m_positionXOffset;
        double positionY = gameObject->m_positionY - gameObject->m_positionYOffset;
        float rotationXOffset = gameObject->m_rotationXOffset;
        float rotationYOffset = gameObject->m_rotationYOffset;
        float scaleXOffset = gameObject->m_scaleXOffset;
        float scaleYOffset = gameObject->m_scaleYOffset;

        if (positionX == gameObject->m_startPosition.x &&
            positionY == gameObject->m_startPosition.y &&
            rotationXOffset == 0.0f &&
            rotationYOffset == 0.0f &&
            scaleXOffset == 0.0f &&
            scaleYOffset == 0.0f) {
            continue;
        }

        float rotationXDelta = rotationXOffset - gameObject->m_unk2A8;
        float rotationYDelta = rotationYOffset - gameObject->m_unk2B0;
        float scaleXDelta = scaleXOffset - gameObject->m_unk2BC;
        float scaleYDelta = scaleYOffset - gameObject->m_unk2C0;

        SavedObjectStateRef stateRef;
        stateRef.m_gameObject = gameObject;
        stateRef.m_positionX = positionX;
        stateRef.m_positionY = positionY;
        stateRef.m_rotationXOffset = rotationXDelta;
        stateRef.m_rotationYOffset = rotationYDelta;
        stateRef.m_addToCustomScaleX = scaleXDelta;
        stateRef.m_addToCustomScaleY = scaleYDelta;
        stateRef.m_unkFloat3 = gameObject->m_lastPosition.x;
        stateRef.m_unkFloat4 = gameObject->m_lastPosition.y;

        dynamicObjects->push_back(stateRef);
    }
}

void PlayLayer::scanActiveSaveObjects() {
    if (!m_objects)
        return;

    cocos2d::ccArray* data = m_objects->data;
    if (!data->num)
        return;

    cocos2d::CCObject** objects = data->arr;
    cocos2d::CCObject** lastObject = &objects[data->num - 1];

    if (objects > lastObject)
        return;

    cocos2d::CCObject* object = *objects;
    if (!object)
        return;

    while (true) {
        if (object->canBeOrdered()) {
            if (!object->canAllowMultiActivate()) {
                m_activeSaveObjects1.push_back(static_cast<GameObject*>(object));
            }
        } else if (object->m_classType == GameObjectClassType::Animated &&
                   object->m_isAnimated) {
            m_activeSaveObjects2.push_back(static_cast<GameObject*>(object));
        }

        if (lastObject < ++objects)
            return;

        object = *objects;
        if (!object)
            return;
    }
}

void PlayLayer::scanDynamicSaveObjects() {
    std::set<int> groupIDs;

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        if (data->num) {
            cocos2d::CCObject** objects = data->arr;
            cocos2d::CCObject** lastObject = &objects[data->num - 1];
            if (objects <= lastObject) {
                cocos2d::CCObject* object = *objects;
                if (object) {
                    while (true) {
                        int objectID = static_cast<int>(object->m_objectID);

                        if (objectID == 2067) {
                            groupIDs.insert(object->m_uOrderOfArrival);
                            groupIDs.insert(object->m_unknown5);
                        } else if (objectID > 2067) {
                            if (objectID == 3016 || objectID == 3032 || objectID == 3033) {
                                groupIDs.insert(object->m_uOrderOfArrival);
                                groupIDs.insert(object->m_unknown5);
                            } else if (objectID < 3016) {
                                if (objectID == 1347) {
                                    groupIDs.insert(object->m_unknown5);
                                } else if (objectID == 1814 || objectID == 1346) {
                                    groupIDs.insert(object->m_uOrderOfArrival);
                                    groupIDs.insert(object->m_unknown5);
                                }
                            }
                        } else if (objectID == 2067) {
                            groupIDs.insert(object->m_uOrderOfArrival);
                            groupIDs.insert(object->m_unknown5);
                        } else if (objectID == 1347) {
                            groupIDs.insert(object->m_unknown5);
                        } else if (objectID == 1814 || objectID == 1346) {
                            groupIDs.insert(object->m_uOrderOfArrival);
                            groupIDs.insert(object->m_unknown5);
                        }

                        if (lastObject < ++objects)
                            break;
                        object = *objects;
                        if (!object)
                            break;
                    }
                }
            }
        }
    }

    GJBaseGameLayer::addRemapTargets(&groupIDs);

    for (int groupID : groupIDs) {
        cocos2d::CCArray* group = GJBaseGameLayer::getGroup(groupID);
        if (!group)
            continue;

        cocos2d::ccArray* data = group->data;
        if (!data->num)
            continue;

        GameObject** objects = reinterpret_cast<GameObject**>(data->arr);
        for (GameObject** iter = &objects[data->num - 1]; iter >= objects; ++iter) {
            GameObject* gameObject = *iter;
            if (!gameObject)
                break;

            if (!gameObject->m_unk530) {
                gameObject->m_unk530 = true;
                m_dynamicSaveObjects.push_back(gameObject);
            }
        }
    }

    int targetCount = m_targetGroupsArray->count() - 2;

    for (int index = 0; index < targetCount; index += 4) {
        cocos2d::CCObject* obj1 = m_targetGroupsArray->objectAtIndex(index);
        cocos2d::CCObject* obj2 = m_targetGroupsArray->objectAtIndex(index + 1);
        cocos2d::CCObject* obj3 = m_targetGroupsArray->objectAtIndex(index + 2);

        if (obj1) {
            cocos2d::CCArray* innerArray = static_cast<cocos2d::CCArray*>(obj1);
            for (unsigned int i = 0; i < innerArray->count(); ++i) {
                GameObject* gameObject = static_cast<GameObject*>(innerArray->objectAtIndex(i));
                if (!gameObject->m_unk530) {
                    gameObject->m_unk530 = true;
                    m_dynamicSaveObjects.push_back(gameObject);
                }
            }
        }

        if (obj2) {
            cocos2d::CCArray* innerArray = static_cast<cocos2d::CCArray*>(obj2);
            for (unsigned int i = 0; i < innerArray->count(); ++i) {
                GameObject* gameObject = static_cast<GameObject*>(innerArray->objectAtIndex(i));
                if (!gameObject->m_unk530) {
                    gameObject->m_unk530 = true;
                    m_dynamicSaveObjects.push_back(gameObject);
                }
            }
        }

        if (obj3) {
            cocos2d::CCArray* innerArray = static_cast<cocos2d::CCArray*>(obj3);
            for (unsigned int i = 0; i < innerArray->count(); ++i) {
                GameObject* gameObject = static_cast<GameObject*>(innerArray->objectAtIndex(i));
                if (!gameObject->m_unk530) {
                    gameObject->m_unk530 = true;
                    m_dynamicSaveObjects.push_back(gameObject);
                }
            }
        }
    }
}

void PlayLayer::setupHasCompleted() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    GJBaseGameLayer::createTextLayers();
    GJBaseGameLayer::updateSpecialGroupData();
    optimizeColorGroups();
    optimizeOpacityGroups();
    scanDynamicSaveObjects();
    scanActiveSaveObjects();

    GJBaseGameLayer::updateLayerCapacity(m_level->m_capacityString);
    GJBaseGameLayer::increaseBatchNodeCapacity();

    if (!m_levelSettings) {
        m_levelSettings = LevelSettingsObject::create();
        m_levelSettings->m_level = m_level;
        CC_SAFE_RETAIN(m_levelSettings);
    }

    m_effectManager = m_levelSettings->m_effectManager;
    m_player1->m_isPlatformer = m_isPlatformer;
    m_player2->m_isPlatformer = m_isPlatformer;

    ColorAction* col1 = GJEffectManager::getColorAction(m_effectManager, 1005);
    col1->m_duration = 0.0f;
    col1->m_fromColor = m_player1->m_playerColor1;
    col1->m_blending = true;

    ColorAction* col2 = GJEffectManager::getColorAction(m_effectManager, 1006);
    col2->m_duration = 0.0f;
    col2->m_fromColor = m_player1->m_playerColor2;
    col2->m_blending = true;

    if (GameManager::sharedState()->m_playerDeathEffect > 0) {
        GameManager::sharedState()->loadDeathEffect(GameManager::sharedState()->m_playerDeathEffect);
    }

    GJBaseGameLayer::createBackground(m_levelSettings->m_backgroundIndex);
    if (m_levelSettings->m_middleGroundIndex > 0) {
        GJBaseGameLayer::createMiddleground(m_levelSettings->m_middleGroundIndex);
    }
    GJBaseGameLayer::createGroundLayer(m_levelSettings->m_groundIndex, m_levelSettings->m_groundLineIndex);
    UILayer::togglePlatformerMode(m_uiLayer, m_isPlatformer);

    cocos2d::ccColor4F startColor = {
        m_player1->m_playerColor1.r / 255.0f,
        m_player1->m_playerColor1.g / 255.0f,
        m_player1->m_playerColor1.b / 255.0f,
        1.0f
    };
    cocos2d::ccColor4F endColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_glitterParticles->setStartColor(startColor);
    m_glitterParticles->setEndColor(endColor);

    GJBaseGameLayer::resetPlayer();

    FMODAudioEngine::sharedEngine()->loadMusic(
        GJGameLevel::getAudioFileName(m_level),
        1.0f, 0.0f, 1.0f, true, 0, 0, m_levelSettings->m_dontReset);

    if (m_level->m_songID || m_level->m_audioTrack > 19) {
        FMODAudioEngine::sharedEngine()->enableMetering();
        m_skipAudioStep = true;
        MusicDownloadManager::sharedState()->incrementPriorityForSong(m_level->m_songID);
    } else {
        FMODAudioEngine::sharedEngine()->disableMetering();
    }

    float bgmVol = FMODAudioEngine::sharedEngine()->getBackgroundMusicVolume();
    m_isSilent = bgmVol <= 0.0f;

    std::string audioStr = LevelTools::getAudioString(m_level->m_audioTrack);
    m_audioEffectsLayer = AudioEffectsLayer::create(audioStr);
    m_objectLayer->addChild(m_audioEffectsLayer);
    m_audioEffectsLayer->setVisible(false);

    GameManager::sharedState()->getFontFile(GameManager::sharedState()->m_loadedFont);
    m_attemptLabel = cocos2d::CCLabelBMFont::create("", GameManager::sharedState()->m_loadedFont.c_str());
    m_objectLayer->addChild(m_attemptLabel, 1400);
    if (m_doNot) {
        m_attemptLabel->setScaleY(-1.0f);
    }

    m_infoLabel = cocos2d::CCLabelBMFont::create("", GameManager::sharedState()->m_loadedFont.c_str());
    addChild(m_infoLabel, 1000);
    m_infoLabel->setAnchorPoint(cocos2d::ccp(0.0f, 1.0f));
    m_infoLabel->setPosition(cocos2d::ccp(5.0f, winSize.height - 10.0f));
    m_infoLabel->setOpacity(150);
    m_infoLabel->setScale(0.5f);
    updateInfoLabel();
    toggleInfoLabel();

    if (!m_levelSettings->m_twoPlayerMode ||
        m_isTestMode ||
        m_level->m_levelType == GJLevelType::Editor ||
        m_isPlatformer) {
    } else {
        m_attemptLabel->setVisible(false);
        cocos2d::CCAction* guideAction = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.5f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::showTwoPlayerGuide)),
            nullptr);
        runAction(guideAction);
    }

    m_freezeStartCamera = 1;
    m_unk322a = true;
    m_skipArtReload = true;
    GJBaseGameLayer::updateCamera(0.0f);

    m_progressBar = cocos2d::CCSprite::createWithSpriteFrameName("slidergroove2.png");
    addChild(m_progressBar, 10);
    m_progressFill = cocos2d::CCSprite::createWithSpriteFrameName("sliderBar2.png");

    cocos2d::ccTexParams texParams = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
    m_progressFill->getTexture()->setTexParameters(&texParams);

    m_progressHeight = 8.0f;
    m_progressWidth = m_progressBar->getContentSize().width - 4.0f;
    m_progressFill->setColor(m_player1->m_playerColor1);
    m_progressBar->addChild(m_progressFill, -1);
    m_progressFill->setAnchorPoint(cocos2d::ccp(0.0f, 0.0f));
    m_progressFill->setPosition(cocos2d::ccp(2.0f, 4.0f));
    m_progressBar->setPosition(cocos2d::ccp(winSize.width * 0.5f, winSize.height - 8.0f));

    m_percentageLabel = cocos2d::CCLabelBMFont::create("0%", GameManager::sharedState()->m_loadedFont.c_str());
    m_percentageLabel->setScale(0.5f);
    addChild(m_percentageLabel, 15);
    m_percentageLabel->setPosition(cocos2d::ccp(winSize.width * 0.5f + 110.0f, winSize.height - 8.0f));
    m_percentageLabel->setAnchorPoint(cocos2d::ccp(0.0f, 0.5f));

    updateProgressbar();
    toggleProgressbar();

    if (m_isTestMode || m_levelSettings->m_propertykA24 || m_levelSettings->m_spawnGroup > 0) {
        m_player1->setVisible(true);
        m_player2->setVisible(true);
        loadDefaultColors();
        GJBaseGameLayer::updateLevelColors();
    } else {
        m_player1->setVisible(false);
        m_player2->setVisible(false);
        loadDefaultColors();
        GJBaseGameLayer::updateLevelColors();
    }

    LevelSettingsObject* startSettings = m_startPosObject ? m_startPosObject->m_startSettings : m_levelSettings;
    GJBaseGameLayer::setupLevelStart(startSettings);

    m_objectsDeactivated = true;
    updateVisibility(0.0f);
    GJBaseGameLayer::updateCamera(0.0f);
    toggleGlitter(false);

    m_disableGravityEffect = GameManager::sharedState()->getGameVariable("0072");

    m_anticheatSpike = GameObject::createWithKey(8);
    m_anticheatSpike->customSetup();
    GameObject::addColorSprite(m_anticheatSpike, "white");
    m_anticheatSpike->setupCustomSprites(m_player1->getPosition());
    m_anticheatSpike->setStartPos(m_player1->getPosition());
    m_anticheatSpike->m_bDontDraw = true;
    addObject(m_anticheatSpike);

    GJBaseGameLayer::createPlayerCollisionBlock();

    m_attemptLabel->setPosition(cocos2d::ccp(
        m_gameState.m_cameraPosition.x + winSize.width * 0.5f,
        m_gameState.m_cameraPosition.y + winSize.height * 0.5f + 85.0f));

    if (m_isTestMode)
        m_attemptLabel->setOpacity(50);

    m_freezeStartCamera = 1;
    resetLevel();
    m_started = false;
    prepareMusic(false);

    cocos2d::CCAction* startAction;
    if (m_passedIntegrity) {
        startAction = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::startGame)),
            nullptr);
    } else {
        startAction = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(1.0f),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::onQuit)),
            nullptr);
    }
    runAction(startAction);
    scheduleUpdate();

    GJBaseGameLayer::checkSpawnObjects();
    GJEffectManager::updateEffects(m_effectManager, 0.0f);
    updateVisibility(0.0f);
    GJBaseGameLayer::updateLevelColors();
    GJBaseGameLayer::updateProximityVolumeEffects();
    updateTestModeLabel();

    if (GJBaseGameLayer::shouldExitHackedLevel()) {
        removeAllObjects();
        onQuit();
    }
}

bool PlayLayer::shouldBlend(int colorID) {
    return GJEffectManager::shouldBlend(m_effectManager, colorID);
}

bool PlayLayer::shouldDebugDraw() {
    if (!m_isDebugDrawEnabled || (!m_isPracticeMode && !m_unk3906)) {
        if (m_unk3906)
            return m_showLeaderboardPercentage;
        return false;
    }
    return true;
}

void PlayLayer::showCompleteEffect() {
    bool noEffects = false;
    int noSFX = 0;

    if (m_platformerEndTrigger) {
        noEffects = m_platformerEndTrigger->m_noEffects;
        if (m_platformerEndTrigger->m_noSFX && noEffects) {
            noSFX = 1;
            if (m_platformerEndTrigger->m_instant)
                noSFX = 0;
        }
        if (m_platformerEndTrigger->m_noSFX)
            noSFX = 1;
    }

    if (!noSFX) {
        FMODAudioEngine::sharedEngine()->playEffect("endStart_02.ogg");
    }

    float stripStartDelay = -90.0f;
    float posXOff = 30.0f;
    if (m_endPortal && m_endPortal->m_flippedX) {
        stripStartDelay = 90.0f;
        posXOff = -30.0f;
    }

    float value2 = 1.0f;
    float stripStartDelayVariation = 45.0f;
    float value3 = 8.0f;

    if (m_isPlatformer || m_platformerEndTrigger) {
        stripStartDelay = 0.0f;
        value2 = 0.5f;
        stripStartDelayVariation = 180.0f;
        value3 = 16.0f;
        posXOff = 0.0f;
    }

    if (!noEffects) {
        cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        float value4 = std::sqrt(
            std::pow((winSize.width / m_gameState.m_cameraZoom) * 0.5f, 2) +
            std::pow(winSize.height / m_gameState.m_cameraZoom, 2));

        CCLightFlash* flash = CCLightFlash::create();
        m_objectLayer->addChild(flash, 0);
        flash->m_mainLayer = getParent();
        flash->m_layerColorZOrder = 100;

        cocos2d::CCPoint targetPos = getEndPosition() + cocos2d::ccp(posXOff, 0.0f);

        flash->playEffect(
            cocos2d::ccp(1.0f, 0.0f),
            m_player1->m_playerColor1,
            30.0f,
            20.0f,
            posXOff + value4 + 10.0f,
            value2 * 0.18f,
            value2 * 0.04f,
            value2 * 0.195f,
            value2 * 0.04f,
            value2 * 0.04f,
            stripStartDelay,
            stripStartDelayVariation,
            155.0f,
            100.0f,
            0.4f,
            stripStartDelayVariation,
            m_player1->m_playerColor1.r,
            static_cast<int>(value3),
            true,
            0.0f);

        m_skipCameraShake = false;
        GJBaseGameLayer::shakeCamera((value3 + 2.0f) * 0.195f * value2, 3.0f, 0.0f);
    }

    if (noSFX) {
        showCompleteText();
    } else {
        cocos2d::CCAction* action = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create((value3 + 2.0f) * 0.195f * value2),
            cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::showCompleteText)),
            nullptr);
        runAction(action);
    }

    FMOD::Channel* activeChannel = FMODAudioEngine::sharedEngine()->getActiveMusicChannel(0);
    if (activeChannel) {
        activeChannel->setLoopCount(0);
    }

    if (m_levelSettings->m_fadeOut || m_level->m_levelID == 3001) {
        FMODAudioEngine::sharedEngine()->fadeOutMusic(2.0f, 0);
    }
}

void PlayLayer::showCompleteText() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::ccp(winSize.width * 0.5f, winSize.height * 0.5f + 35.0f);

    std::string textFrame = m_isPracticeMode ? "GJ_practiceComplete_001.png" : "GJ_levelComplete_001.png";
    float baseScale = m_isPracticeMode ? 1.0f : 1.1f;

    cocos2d::CCSprite* textSprite = cocos2d::CCSprite::createWithSpriteFrameName(textFrame.c_str());
    addChild(textSprite, 15);
    textSprite->setPosition(center);
    textSprite->setScale(0.01f);

    cocos2d::CCAction* textAnim = cocos2d::CCSequence::create(
        cocos2d::CCEaseElasticOut::create(cocos2d::CCScaleTo::create(0.6f, baseScale), 0.66f),
        cocos2d::CCDelayTime::create(0.88f),
        cocos2d::CCEaseIn::create(cocos2d::CCScaleTo::create(0.22f, 0.01f), 2.0f),
        cocos2d::CCHide::create(),
        cocos2d::CCCallFunc::create(textSprite, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr);
    textSprite->runAction(textAnim);

    cocos2d::CCParticleSystemQuad* particles1 = cocos2d::CCParticleSystemQuad::create("levelComplete01.plist", false);
    particles1->onEnter();
    addChild(particles1, 13);
    particles1->setPosition(center);
    particles1->setStartColor(cocos2d::ccc4f(
        m_player1->m_playerColor1.r / 255.0f,
        m_player1->m_playerColor1.g / 255.0f,
        m_player1->m_playerColor1.b / 255.0f,
        1.0f));

    cocos2d::CCParticleSystemQuad* particles2 = cocos2d::CCParticleSystemQuad::create("levelComplete01.plist", false);
    particles2->onEnter();
    addChild(particles2, 13);
    particles2->setPosition(center);
    particles2->setStartColor(cocos2d::ccc4f(
        m_player1->m_playerColor2.r / 255.0f,
        m_player1->m_playerColor2.g / 255.0f,
        m_player1->m_playerColor2.b / 255.0f,
        1.0f));

    if (!m_platformerEndTrigger || !m_platformerEndTrigger->m_noEffects) {
        if (!m_isPracticeMode) {
            float screenRight = cocos2d::CCDirector::sharedDirector()->getScreenRight();
            CCCircleWave* wave1 = CCCircleWave::create(10.0f, screenRight, 0.8f, false);
            wave1->m_lineWidth = 4;
            wave1->m_color = m_player1->m_playerColor1;
            cocos2d::CCPoint endPos = getEndPosition();
            wave1->setPosition(endPos);
            m_objectLayer->addChild(wave1, 0);

            CCCircleWave* wave2 = CCCircleWave::create(10.0f, 250.0f, 0.8f, false);
            wave2->m_lineWidth = 4;
            wave2->m_color = m_player1->m_playerColor2;
            wave2->setPosition(textSprite->getPosition());
            addChild(wave2, 9);

            for (int i = 0; i < 5; ++i) {
                cocos2d::CCAction* waveAction = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(i * 0.05f),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::spawnCircle)),
                    nullptr);
                runAction(waveAction);
            }

            for (int i = 0; i < 10; ++i) {
                float randomOffset = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * 0.08f;
                cocos2d::CCAction* fireworkAction = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(randomOffset + (i * 0.15f)),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::spawnFirework)),
                    nullptr);
                runAction(fireworkAction);
            }
        }
    }

    cocos2d::CCAction* endLayerAction = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::showEndLayer)),
        nullptr);
    runAction(endLayerAction);
}

void PlayLayer::showEndLayer() {
    PlatformToolbox::showCursor();
    PlayerObject::releaseAllButtons(m_player1);
    UILayer::resetAllButtons(m_uiLayer);
    UILayer::toggleMenuVisibility(m_uiLayer, false);
    m_hasCompletedLevel = true;

    EndLevelLayer* endLayer = EndLevelLayer::create();
    addChild(endLayer, 1001);
    endLayer->showLayer(false);
}

void PlayLayer::showHint() {
    m_hintShown = true;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    float scale = 0.0f;
    float delay = 0.0f;

    if (m_level->m_levelID == 1 || m_level->m_levelID == 1001) {
        scale = 1.060320051f;
        delay = 3.0f;
    } else {
        scale = 1.058642330f;
        delay = 4.0f;
    }

    cocos2d::CCLabelBMFont* hintLabel = cocos2d::CCLabelBMFont::create("Tap to Jump!",
        GameManager::sharedState()->m_loadedFont.c_str());
    hintLabel->setScale(scale);
    addChild(hintLabel, 3);
    hintLabel->setPosition(cocos2d::ccp(winSize.width * 0.5f, winSize.height * 0.5f + 60.0f));
    hintLabel->setOpacity(0);

    cocos2d::CCAction* action = cocos2d::CCSequence::create(
        cocos2d::CCFadeIn::create(0.5f),
        cocos2d::CCDelayTime::create(delay),
        cocos2d::CCFadeOut::create(0.5f),
        cocos2d::CCCallFunc::create(hintLabel, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr);
    hintLabel->runAction(action);
}

void PlayLayer::showRetryLayer() {
    PlatformToolbox::showCursor();
    PlayerObject::releaseAllButtons(m_player1);
    UILayer::resetAllButtons(m_uiLayer);
    UILayer::toggleMenuVisibility(m_uiLayer, false);
    m_hasCompletedLevel = true;

    RetryLevelLayer* retryLayer = RetryLevelLayer::create();
    addChild(retryLayer, 100);
    retryLayer->showLayer(false);
}

void PlayLayer::showTwoPlayerGuide() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCLabelBMFont* p1Label = cocos2d::CCLabelBMFont::create("P1",
        GameManager::sharedState()->m_loadedFont.c_str());
    p1Label->setScale(0.8f);
    addChild(p1Label, 3);
    p1Label->setPosition(cocos2d::ccp((winSize.width * 0.5f) - (winSize.width * 0.25f), winSize.height * 0.5f));

    cocos2d::CCLabelBMFont* p2Label = cocos2d::CCLabelBMFont::create("P2",
        GameManager::sharedState()->m_loadedFont.c_str());
    p2Label->setScale(0.8f);
    addChild(p2Label, 3);
    p2Label->setPosition(cocos2d::ccp((winSize.width * 0.5f) + (winSize.width * 0.25f), winSize.height * 0.5f));

    if (GameManager::sharedState()->getGameVariable("0010")) {
        cocos2d::CCPoint temp = p1Label->getPosition();
        p1Label->setPosition(p2Label->getPosition());
        p2Label->setPosition(temp);
    }

    cocos2d::CCSprite* separator = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
    separator->setScale((winSize.height * 1.2f) / separator->getContentSize().width);
    separator->setRotation(90.0f);
    addChild(separator, 3);
    separator->setPosition(cocos2d::ccp(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::ccBlendFunc blend = { GL_SRC_ALPHA, GL_ONE };
    separator->setBlendFunc(blend);

    p1Label->setOpacity(0);
    p2Label->setOpacity(0);
    separator->setOpacity(0);

    for (int i = 0; i < 3; ++i) {
        cocos2d::CCNode* target = (i == 0)
            ? static_cast<cocos2d::CCNode*>(p1Label)
            : (i == 1)
                ? static_cast<cocos2d::CCNode*>(p2Label)
                : static_cast<cocos2d::CCNode*>(separator);

        cocos2d::CCAction* action = cocos2d::CCSequence::create(
            cocos2d::CCFadeTo::create(0.2f, 255),
            cocos2d::CCDelayTime::create(0.8f),
            cocos2d::CCFadeTo::create(0.2f, 0),
            cocos2d::CCCallFunc::create(target, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        target->runAction(action);
    }
}

void PlayLayer::spawnCircle() {
    float screenRight = cocos2d::CCDirector::sharedDirector()->getScreenRight();
    CCCircleWave* wave = CCCircleWave::create(10.0f, screenRight, 0.5f, true);
    wave->m_lineWidth = 4;
    wave->m_color = m_player1->m_playerColor1;
    wave->setPosition(getEndPosition());
    m_objectLayer->addChild(wave, 0);
    wave->m_circleMode = CircleMode::Outline;
}

void PlayLayer::spawnFirework() {
    cocos2d::ccColor3B pColor1 = m_player1->m_playerColor1;
    cocos2d::ccColor3B pColor2 = m_player1->m_playerColor2;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float randX = ((static_cast<float>(rand()) / RAND_MAX) * (winSize.width - 50.0f)) + 50.0f;
    float randY = ((static_cast<float>(rand()) / RAND_MAX) * (winSize.height - 50.0f)) + 50.0f;
    cocos2d::CCPoint spawnPos = cocos2d::ccp(randX, randY);

    float randRadius = ((static_cast<float>(rand()) / RAND_MAX) * 15.0f) + 35.0f;
    CCCircleWave* wave = CCCircleWave::create(10.0f, randRadius, 0.5f, true);
    wave->m_color = pColor2;
    wave->m_lineWidth = 4;
    wave->setPosition(spawnPos);
    addChild(wave, 10);

    cocos2d::CCParticleSystemQuad* firework = cocos2d::CCParticleSystemQuad::create("firework.plist", false);
    firework->onEnter();
    addChild(firework, 13);
    firework->setPosition(spawnPos);

    cocos2d::CCNode* child = firework->getChildByTag(1);
    if (child) {
        child->getParent()->removeChild(child, false);
    }
}

void PlayLayer::startGame() {
    cocos2d::CCDirector::sharedDirector()->m_bSmoothFixCheck = true;
    m_started = true;
    m_player1->setVisible(true);
    m_player2->setVisible(true);
    startMusic();
}

void PlayLayer::startGameDelayed() {
    m_started = true;
    startMusic();
}

void PlayLayer::startMusic() {
    if (m_playerDied || !m_started || m_currentCheckpoint)
        return;

    if (!m_musicPrepared)
        prepareMusic(false);

    GJBaseGameLayer::processQueuedAudioTriggers();
    GJBaseGameLayer::updateProximityVolumeEffects();

    if (m_gameState.m_levelTime <= 0.0 && m_levelSettings->m_fadeIn) {
        FMODAudioEngine::sharedEngine()->fadeInMusic(2.0f, 0);
    }

    if (m_startPosObject) {
        GJBaseGameLayer::pauseAudio();
        GJBaseGameLayer::processActivatedAudioTriggers(m_gameState.m_levelTime);

        if (!m_currentCheckpoint) {
            m_currentCheckpoint = createCheckpoint();
            CC_SAFE_RETAIN(m_currentCheckpoint);
        }
    }

    m_musicPrepared = false;
    m_started = true;
}

void PlayLayer::startRecording() {
    m_recordingStopped = false;
}

void PlayLayer::startRecordingDelayed() {
    stopActionByTag(31);
    cocos2d::CCAction* action = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(2.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::startRecording)),
        nullptr);
    action->setTag(31);
    runAction(action);
}

void PlayLayer::stopRecording() {
    stopActionByTag(31);
}

void PlayLayer::storeCheckpoint(CheckpointObject* checkpoint) {
    m_checkpointArray->addObject(checkpoint);
    addToSection(CheckpointObject::getObject(checkpoint));
    if (m_checkpointArray->count() > 50) {
        removeCheckpoint(true);
    }
}

void PlayLayer::takeStateSnapshot() {
}

void PlayLayer::testTime() {
}

void PlayLayer::toggleBGEffectVisibility(bool enabled) {
    m_bgEffectDisabled = !enabled;
    if (!enabled) {
        m_glitterParticles->stopSystem();
    } else if (m_glitterEnabled) {
        m_glitterParticles->resumeSystem();
    }
}

void PlayLayer::toggleDebugDraw() {
    bool debugEnabled = shouldDebugDraw();
    if (debugEnabled != m_debugDrawNode->isVisible()) {
        m_debugDrawNode->clear();
        m_debugDrawNode->setVisible(debugEnabled);
        if (debugEnabled) {
            updateDebugDraw();
        }
    }
}

void PlayLayer::toggleGhostEffect(int type) {
    PlayerObject::toggleGhostEffect(m_player1, static_cast<GhostType>(type));
    if (m_gameState.m_isDualMode) {
        PlayerObject::toggleGhostEffect(m_player2, static_cast<GhostType>(type));
    }
}

void PlayLayer::toggleGlitter(bool visible) {
    if (!GameManager::sharedState()->m_performanceMode) {
        m_glitterEnabled = visible;
        if (m_bgEffectDisabled || !visible) {
            m_glitterParticles->stopSystem();
        } else {
            m_glitterParticles->resumeSystem();
        }
    }
}

void PlayLayer::toggleGroundVisibility(bool visible) {
    m_groundLayer->toggleVisible02(visible);
    m_groundLayer2->toggleVisible02(visible);
}

void PlayLayer::toggleHideAttempts(bool hide) {
    if (m_unk36cf != hide) {
        m_unk36cf = hide;
        if (hide ||
            (m_isPracticeMode && GameManager::sharedState()->getGameVariable("0134")) ||
            GameManager::sharedState()->getGameVariable("0135")) {
            m_attemptLabel->setVisible(false);
        } else if (m_attempts > 0) {
            m_attemptLabel->setVisible(true);
        }
    }
}

void PlayLayer::toggleIgnoreDamage(bool value) {
    m_ignoreDamage = value;
    if (value)
        m_isIgnoreDamageEnabled = true;
    m_player1->m_maybeCanRunIntoBlocks = value;
    m_player2->m_maybeCanRunIntoBlocks = value;
    updateTestModeLabel();
}

void PlayLayer::toggleInfoLabel() {
    bool isVisible = GameManager::sharedState()->getGameVariable("0109");
    m_infoLabel->setVisible(isVisible);
    updateInfoLabel();
}

void PlayLayer::toggleMGVisibility(bool visible) {
    if (m_middleground) {
        m_middleground->toggleVisible02(visible);
    }
}

void PlayLayer::toggleMusicInPractice() {
    if (GameStatsManager::sharedState()->isItemEnabled(UnlockType::GJItem, 17) ||
        !GameManager::sharedState()->getGameVariable("0125")) {
        m_practiceMusicSync = true;
    } else {
        m_practiceMusicSync = m_level->m_levelType == GJLevelType::Editor;
    }
}

void PlayLayer::togglePracticeMode(bool practiceMode) {
    if (m_isPracticeMode != practiceMode) {
        m_isPracticeMode = practiceMode;
        toggleDebugDraw();
        UILayer::toggleCheckpointsMenu(m_uiLayer, practiceMode);

        if (practiceMode) {
            if (!m_practiceMusicSync) {
                FMODAudioEngine::sharedEngine()->stopAllMusic(true);
                std::string practiceMusic = GameManager::sharedState()->getPracticeMusicFile();
                FMODAudioEngine::sharedEngine()->playMusic(practiceMusic, true, 0.0f, 0);
            }
        } else {
            resetLevelFromStart();
        }

        if (m_isPracticeMode) {
            stopRecording();
        }
    }
}

void PlayLayer::toggleProgressbar() {
    bool showBar = GameManager::sharedState()->m_showProgressBar;
    float offset = 0.0f;
    bool showPercent = false;

    if (m_isPlatformer) {
        showBar = false;
        showPercent = GameManager::sharedState()->getGameVariable("0145");
    } else {
        showPercent = GameManager::sharedState()->getGameVariable("0040");
        if (showBar)
            offset = 110.0f;
    }

    m_progressBar->setVisible(showBar);
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_percentageLabel->setVisible(showPercent);
    m_percentageLabel->setPosition(cocos2d::ccp(offset + (winSize.width * 0.5f),
                                                m_percentageLabel->getPositionY()));

    float anchorX = (!showBar && !m_isPlatformer) ? 0.5f : 0.0f;
    m_percentageLabel->setAnchorPoint(cocos2d::ccp(anchorX, 0.5f));

    updateProgressbar();

    if (showPercent && m_isPlatformer) {
        GJBaseGameLayer::updatePlatformerTime();
    }
}

void PlayLayer::tryStartRecord() {
    stopActionByTag(31);
    startRecording();
}

void PlayLayer::updateAttemptTime(float attemptTime) {
    m_attemptTime += attemptTime;
    if (m_currentTime <= 0.0) {
        m_currentTime = getTempMilliTime();
    }
}

void PlayLayer::updateAttempts() {
    m_attempts++;
    GameManager::sharedState()->m_sessionAttempts++;
    GameManager::sharedState()->m_sessionAttempts2++;

    if (!m_isPracticeMode) {
        GameManager::sharedState()->m_sessionNormalAttempts++;
    }

    m_attemptLabel->setString(cocos2d::CCString::createWithFormat("Attempt %i", m_attempts)->getCString());

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    if (m_attempts != 1 || m_isPlatformer) {
        m_attemptLabel->setPosition(cocos2d::ccp(
            m_gameState.m_cameraPosition.x + winSize.width * 0.5f + 50.0f,
            m_gameState.m_cameraPosition.y + winSize.height * 0.5f + 85.0f));
    }

    if (m_unk36cf ||
        (m_isPracticeMode && GameManager::sharedState()->getGameVariable("0134")) ||
        GameManager::sharedState()->getGameVariable("0135")) {
        m_attemptLabel->setVisible(false);
    } else if (m_attempts > 0) {
        m_attemptLabel->setVisible(true);
    }
}

void PlayLayer::updateDebugDrawSettings() {
    m_isDebugDrawEnabled = GameManager::sharedState()->getGameVariable("0166");
    m_disablePlayerHitbox = GameManager::sharedState()->getGameVariable("0171");
    m_hitboxesOnDeath = GameManager::sharedState()->getGameVariable("0179");
    toggleDebugDraw();
}

void PlayLayer::updateEffectPositions() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint glitterPos(
        (winSize.width * 0.5f) / m_gameState.m_cameraZoom + m_gameState.m_cameraPosition.x,
        (winSize.height * 0.5f) / m_gameState.m_cameraZoom + m_gameState.m_cameraPosition.y);
    m_glitterParticles->setPosition(glitterPos);

    for (unsigned int i = 0; i < m_circleWaveArray->count(); ++i) {
        CCCircleWave* wave = static_cast<CCCircleWave*>(m_circleWaveArray->objectAtIndex(i));
        wave->updatePosition(0.0f);
    }
}

void PlayLayer::updateInfoLabel() {
    if (!m_infoLabel->isVisible())
        return;

    int levelID = m_level->m_levelID;

    std::string infoStr;
    infoStr.reserve(512);

    infoStr += "LevelID: ";
    infoStr += std::to_string(levelID);
    infoStr += "\nTime: ";
    infoStr += std::to_string(std::round(m_attemptTime * 100.0) / 100.0);
    infoStr += "\nAttempt: ";
    infoStr += std::to_string(m_attempts);
    infoStr += "\nTaps: ";
    infoStr += std::to_string(m_clicks);
    infoStr += "\nTimeWarp: ";
    infoStr += std::to_string(m_gameState.m_timeWarp);
    infoStr += "\nGravity: ";
    infoStr += std::to_string(m_player1->m_gravityMod);

    if (m_gameState.m_isDualMode && m_player1->m_gravityMod != m_player2->m_gravityMod) {
        infoStr += " / ";
        infoStr += std::to_string(m_player2->m_gravityMod);
    }

    infoStr += "\nActive: ";
    infoStr += std::to_string(m_activeObjectsCount);
    infoStr += "\nGradients: ";
    infoStr += std::to_string(m_activeGradients);
    infoStr += "\nParticles: ";
    infoStr += std::to_string(m_particleCount);

    infoStr += "\n-- Audio --\nSongs: ";
    infoStr += std::to_string(FMODAudioEngine::sharedEngine()->countActiveMusic());
    infoStr += "\nSFX: ";
    infoStr += std::to_string(FMODAudioEngine::sharedEngine()->countActiveEffects());

    infoStr += "\n-- Perf --\nMove: ";
    infoStr += std::to_string(m_movedCountDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaMovedCountDisplay);
    infoStr += "\nRotate: ";
    infoStr += std::to_string(m_rotatedCountDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaRotatedCountDisplay);
    infoStr += "\nScale: ";
    infoStr += std::to_string(m_scaledCountDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaScaledCountDisplay);
    infoStr += "\nFollow: ";
    infoStr += std::to_string(m_followedCountDisplay);

    infoStr += "\n-- Area --\nMove: ";
    infoStr += std::to_string(m_areaMovedCountTotalDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaMovedCountDisplay);
    infoStr += "\nRotate: ";
    infoStr += std::to_string(m_areaRotatedCountTotalDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaRotatedCountDisplay);
    infoStr += "\nScale: ";
    infoStr += std::to_string(m_areaScaledCountTotalDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaScaledCountDisplay);
    infoStr += "\nColOp: ";
    infoStr += std::to_string(m_areaColorCountTotalDisplay);
    infoStr += " / ";
    infoStr += std::to_string(m_areaColorCountDisplay);
    infoStr += "\n";

    m_infoLabel->setString(infoStr.c_str());
}

void PlayLayer::updateProgressbar() {
    if (m_isPlatformer)
        return;

    float currentPercent = getCurrentPercent();

    if (currentPercent > 0.01f) {
        float fillWidth = m_progressWidth * (currentPercent / 100.0f);
        if (fillWidth > m_progressWidth)
            fillWidth = m_progressWidth;
        m_progressFill->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, fillWidth, m_progressHeight));
    } else {
        float minWidth = m_progressWidth * 0.0001f;
        m_progressFill->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, minWidth, m_progressHeight));
    }

    if (m_decimalPercentage) {
        float displayPercent = currentPercent;
        if (displayPercent < 0.0f)
            displayPercent = 0.0f;
        if (displayPercent > 100.0f)
            displayPercent = 100.0f;

        m_percentageLabel->setString(
            cocos2d::CCString::createWithFormat("%.2f%%", displayPercent)->getCString());
    } else {
        int intPercent = static_cast<int>(currentPercent);
        if (intPercent < 0)
            intPercent = 0;
        if (intPercent > 100)
            intPercent = 100;

        std::string percentStr = std::to_string(intPercent) + "%";
        m_percentageLabel->setString(percentStr.c_str());
    }
}

void PlayLayer::updateTestModeLabel() {
    if (!m_statusLabel)
        return;

    if (m_isIgnoreDamageEnabled) {
        m_statusLabel->setVisible(true);
    } else {
        m_statusLabel->setVisible(m_isTestMode);
    }

    if (m_ignoreDamage) {
        m_statusLabel->setString("Ignore Damage");
    } else if (m_platformerRestart) {
        m_statusLabel->setString("Used Ignore Damage");
    } else if (m_isTestMode) {
        m_statusLabel->setString("Testmode");
    }
}

void PlayLayer::updateTimeLabel(int seconds, int centiseconds, bool decimals) {
    if (!m_percentageLabel->isVisible() || !m_isPlatformer)
        return;

    std::string timeStr;
    if (decimals) {
        timeStr = cocos2d::CCString::createWithFormat("%i.%02d", seconds, centiseconds)->getCString();
    } else {
        timeStr = std::to_string(seconds);
    }

    m_percentageLabel->setString(timeStr.c_str());
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    float widthOffset = timeStr.length() * 5.0f;
    m_percentageLabel->setPosition(cocos2d::ccp(
        winSize.width * 0.5f - widthOffset,
        m_percentageLabel->getPositionY()));
}

void PlayLayer::updateTimeWarp(float timeWarp) {
    if (m_levelEndAnimationStarted)
        timeWarp = 1.0f;
    GJBaseGameLayer::updateTimeWarp(timeWarp);
}

void PlayLayer::updateVerifyDamage() {
    if (!m_anticheatSpike || m_damageVerified)
        return;

    if (m_isIgnoreDamageEnabled)
        m_damageVerified = true;

    cocos2d::CCPoint playerPos = m_player1->getPosition();
    m_anticheatSpike->setStartPos(cocos2d::ccp(playerPos.x, playerPos.y));
    updateObjectSection(m_anticheatSpike);
}

void PlayLayer::updateVisibility(float dt) {
    cameraRight = m_gameState.m_cameraPosition2.x + m_cameraWidth;
    GJBaseGameLayer::preUpdateVisibility(dt);
    GJEffectManager::processColors(m_effectManager);

    cocos2d::ccColor3B lightColor = GJEffectManager::activeColorForIndex(m_effectManager, 1005);
    GJEffectManager::calculateLightBGColor(m_effectManager, lightColor);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::ccColor3B bgCol = GJEffectManager::activeColorForIndex(m_effectManager, 1000);
    m_unk38c0 = bgCol.r;

    for (unsigned int i = 0; i < m_keyPulses.size(); ++i) {
        m_keyPulses[i] = 0;
    }

    bool isDead = m_player1->m_isDead;
    bool isFlip = isFlipping();

    float audioScale;
    if (m_skipAudioStep) {
        audioScale = FMODAudioEngine::sharedEngine()->getMeteringValue();
    } else {
        audioScale = m_audioEffectsLayer->m_audioScale;
    }

    if (!m_isPracticeMode) {
        if (!m_isSilent) {
            audioScale = audioScale;
        } else {
            audioScale = 0.5f;
        }
    } else {
        if (!m_practiceMusicSync) {
            audioScale = 0.5f;
        } else {
            if (!m_isSilent) {
                audioScale = audioScale;
            } else {
                audioScale = 0.5f;
            }
        }
    }

    m_player1->m_audioScale = audioScale;
    m_player2->m_audioScale = audioScale;

    bool hasBlending = !m_blendingColors.empty();

    for (unsigned int i = 0; i < m_activeObjectsCount; ++i) {
        AnimatedGameObject* animObj = static_cast<AnimatedGameObject*>(m_activeObjects[i]);

        int mainKey = animObj->m_mainColorKeyIndex;
        if (mainKey <= 0) {
            cocos2d::ccColor3B defaultColor = cocos2d::ccc3(255, 255, 255);
            animObj->updateMainColor(&defaultColor);
            if (GameObject::hasSecondaryColor(animObj)) {
                animObj->updateSecondaryColor(&defaultColor);
            }
        } else {
            if ((m_keyPulses[mainKey >> 5] & (1 << (mainKey & 0x1F))) == 0) {
                m_keyColors[mainKey] = *GameObject::colorForMode(animObj, animObj->m_activeMainColorID, true);
                m_keyOpacities[mainKey] = GameObject::opacityModForMode(animObj, animObj->m_activeMainColorID, true);
                m_keyPulses[mainKey >> 5] |= (1 << (mainKey & 0x1F));
            }
            animObj->updateMainColor(&m_keyColors[mainKey]);
            animObj->m_baseColor->setOpacity(static_cast<GLubyte>(m_keyOpacities[mainKey] * 255.0f));

            if (GameObject::hasSecondaryColor(animObj)) {
                int detailKey = animObj->m_detailColorKeyIndex;
                if ((m_keyPulses[detailKey >> 5] & (1 << (detailKey & 0x1F))) == 0) {
                    m_keyColors[detailKey] = *GameObject::colorForMode(animObj, animObj->m_activeDetailColorID, false);
                    m_keyOpacities[detailKey] = GameObject::opacityModForMode(animObj, animObj->m_activeDetailColorID, false);
                    m_keyPulses[detailKey >> 5] |= (1 << (detailKey & 0x1F));
                }
                animObj->updateSecondaryColor(&m_keyColors[detailKey]);
                animObj->m_detailColor->setOpacity(static_cast<GLubyte>(m_keyOpacities[detailKey] * 255.0f));
            }
        }

        if (hasBlending && animObj->m_isActivated) {
            bool blendedMain = m_blendingColors.find(animObj->m_activeMainColorID) != m_blendingColors.end();
            bool blendedDetail = GameObject::hasSecondaryColor(animObj) &&
                m_blendingColors.find(animObj->m_activeDetailColorID) != m_blendingColors.end();
            if (blendedMain || blendedDetail) {
                animObj->addMainSpriteToParent(false);
                GameObject::addColorSpriteToParent(animObj, false);
            }
        }

        animObj->activateObject();

        cocos2d::CCPoint animPos = animObj->getRealPosition();
        m_enterEffectPosition = animPos;

        int enterType = -1;
        bool isRight = true;
        if (animObj->m_enterType == -1 &&
            (animObj->m_exitType != -1 ||
             m_enterEffectPosition.x <= m_gameState.m_cameraPosition2.x + m_halfCameraWidth)) {
            isRight = false;
            enterType = animObj->m_exitType;
        } else {
            enterType = animObj->m_enterType;
        }

        if (animObj->m_isUIObject)
            enterType = -14;

        if (animObj->getHasSyncedAnimation()) {
            animObj->updateSyncedAnimation(m_gameState.m_totalTime, -1);
        }

        if (animObj->getHasRotateAction()) {
            EnhancedGameObject::updateRotateAction(animObj, dt);
        }

        if (animObj->m_unk367) {
            cocos2d::ccColor3B transformed = GameToolbox::transformColor(bgCol, cocos2d::ccHSVValue(0.0f, -0.3f, 0.4f));
            AnimatedGameObject::updateChildSpriteColor(animObj, transformed);
            if (animObj->getType() == GameObjectType::Collectible) {
                EffectGameObject::updateInteractiveHover(static_cast<EffectGameObject*>(animObj),
                    m_hoverNode->getPositionY());
            }
        } else if (animObj->getType() == GameObjectType::Collectible) {
            EffectGameObject::updateInteractiveHover(static_cast<EffectGameObject*>(animObj),
                m_hoverNode->getPositionY());
        }

        if (animObj->m_objectID == 143) {
            cocos2d::ccColor3B white = cocos2d::ccc3(255, 255, 255);
            animObj->setGlowColor(&white);
        }

        if (animObj->m_unk3F8)
            continue;

        if (animObj->m_usesAudioScale && !animObj->m_hasNoAudioScale) {
            if (animObj->m_customAudioScale) {
                animObj->setScale(animObj->m_minAudioScale +
                    ((audioScale - 0.1f) * (animObj->m_maxAudioScale - animObj->m_minAudioScale)));
            } else {
                animObj->setScale(audioScale);
            }
        }

        if (animObj->m_customGlowColor) {
            cocos2d::ccColor3B lbgColor = GJEffectManager::activeColorForIndex(m_effectManager, 1007);
            cocos2d::ccColor3B lightBG = cocos2d::ccc3(m_unk38c0, m_unk38c0, m_unk38c0);
            if (animObj->m_glowColorIsLBG) {
                animObj->setGlowColor(&lbgColor);
            } else {
                animObj->setGlowColor(&lightBG);
            }
        }

        if (animObj->m_isInvisibleBlock) {
            if (isDead) {
                cocos2d::ccColor3B lightBG = cocos2d::ccc3(m_unk38c0, m_unk38c0, m_unk38c0);
                animObj->setGlowColor(&lightBG);
                animObj->setOpacity(255);
                GameObject::setGlowOpacity(animObj, 255);
            } else {
                float relativeMod = getRelativeModNew(cocos2d::ccp(70.0f, 0.0f), 0.0f, 0.0f, isRight, false);
                animObj->setOpacity(static_cast<GLubyte>(relativeMod * 255.0f));
                applyEnterEffect(animObj, enterType, isRight);
            }
        } else if (GameObject::ignoreFade(animObj) ||
                   enterType == -14 ||
                   (animObj->m_intrinsicDontFade &&
                    (!animObj->m_isSolidColorBlock || !animObj->m_baseOrDetailBlending))) {
            animObj->setOpacity(255);
            applyEnterEffect(animObj, enterType, isRight);
        } else if (enterType != -15) {
            float relativeMod = getRelativeModNew(cocos2d::ccp(70.0f, 0.0f), 0.0f, 0.0f, isRight, false);
            animObj->setOpacity(static_cast<GLubyte>(relativeMod * 255.0f));
            applyEnterEffect(animObj, enterType, isRight);

            if (isFlip) {
                float flipMod = m_gameState.m_levelFlipping;
                if (m_cameraFlip == -1.0f)
                    flipMod = 1.0f - flipMod;

                cocos2d::CCPoint pos = animObj->getPosition();
                animObj->setPosition(cocos2d::ccp(
                    pos.x + ((winSize.width / m_gameState.m_cameraZoom -
                             (pos.x - m_gameState.m_cameraPosition.x) * 2.0f) * flipMod),
                    pos.y));

                float rot = std::abs(animObj->getRotation());
                bool needsRot = static_cast<int>(rot) % 90 != 0;
                bool ortho = rot != 270.0f && rot != 90.0f;

                if (m_cameraFlip == 1.0f) {
                    if (flipMod >= 0.5f) {
                        if (ortho)
                            animObj->setFlipY(!animObj->m_startFlipY);
                        if (needsRot)
                            animObj->setRotation(animObj->m_startRotationX);
                    }
                } else if (flipMod > 0.5f) {
                    if (ortho)
                        animObj->setFlipX(!animObj->m_startFlipX);
                    if (needsRot)
                        animObj->setRotation(animObj->m_startRotationX);
                } else {
                    animObj->setFlipX(!animObj->m_startFlipX);
                    if (needsRot)
                        animObj->setRotation(-animObj->m_startRotationX);
                }
            }
        } else {
            if (!GameObject::ignoreEnter(animObj)) {
                applyCustomEnterEffect(animObj, isRight);
                if (isFlip && m_resetActiveObjects) {
                    animObj->setFlipX(animObj->m_startFlipX);
                    animObj->setFlipY(animObj->m_startFlipY);
                    animObj->setRotation(animObj->m_startRotationX);
                    animObj->setPosition(animObj->getPosition());
                }
            }
        }
    }

    GJBaseGameLayer::updateEnterEffects(dt);
    GJBaseGameLayer::processAreaVisualActions(dt);
    GJBaseGameLayer::updateParticles(dt);

    m_resetActiveObjects = false;
    m_blendingColors.clear();

    if (shouldDebugDraw()) {
        updateDebugDraw();
    }
}

bool PlayLayer::init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
    if (!GJBaseGameLayer::init())
        return false;

    m_level = level;
    CC_SAFE_RETAIN(m_level);

    cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->m_bBlockRepeat = true;

    if (GameManager::sharedState()->getGameVariable("0128")) {
        PlatformToolbox::toggleLockCursor(true);
    } else {
        if (!GameManager::sharedState()->getGameVariable("0024"))
            PlatformToolbox::hideCursor();
    }

    toggleMusicInPractice();
    GameManager::sharedState()->m_somethingInMenuLayer = true;

    if (m_level->m_cbsOverride) {
        m_clickBetweenSteps = (m_level->m_cbsOverride == 1);
    }

    m_damageVerified = false;
    m_nextColorKey = 1;
    m_unk376d = false;

    if (!m_level->m_disableShakeToggled) {
        m_skipCameraShake = GameManager::sharedState()->getGameVariable("0172");
    } else {
        m_skipCameraShake = true;
    }

    m_doNot = GameManager::sharedState()->getGameVariable("0095");
    m_showLeaderboardPercentage = GameManager::sharedState()->getGameVariable("0099");
    m_decimalPercentage = GameManager::sharedState()->getGameVariable("0126");

    m_lowDetailMode = level->m_lowDetailModeToggled;

    if (!m_lowDetailMode || !m_extraLDM) {
        bool perfMode = GameManager::sharedState()->m_performanceMode;
        GameManager::sharedState()->m_addGlow = perfMode;
    }

    GameManager::sharedState()->m_unkBool6 = true;

    if (GameManager::sharedState()->m_showedMenu) {
        GameManager::sharedState()->m_adTimer = getTempMilliTime() - 180.0;
        GameManager::sharedState()->m_showedMenu = false;
    }

    if (!GameManager::sharedState()->m_recordGameplay) {
        m_isTestMode = false;
        if (m_level->m_levelType != GJLevelType::Main) {
            m_recordInputs = (level->m_demon.value == 1);
        } else {
            m_recordInputs = false;
        }
    } else {
        stopRecording();
        m_isTestMode = false;
        m_recordInputs = false;
    }

    m_attempts = 0;
    GJBaseGameLayer::resetActiveEnterEffects();
    m_jumps = 0;
    m_gameState.m_totalTime = 0.0;
    m_maxObjectX = 0.0f;
    m_gameState.m_unkDouble3 = 0.0;
    m_gameState.m_levelTime = 0.0;
    m_skipArtReload = true;
    m_levelEndAnimationStarted = false;
    m_gameState.m_isDualMode = false;

    m_totalGravityEffects = 4;
    m_gravityEffects = cocos2d::CCArray::create();
    CC_SAFE_RETAIN(m_gravityEffects);

    for (int i = 0; i < m_totalGravityEffects; ++i) {
        GravityEffectSprite* gravSprite = GravityEffectSprite::create();
        m_gravityEffects->addObject(gravSprite);
    }

    m_collectibles = cocos2d::CCArray::create();
    CC_SAFE_RETAIN(m_collectibles);

    GameManager::sharedState()->m_editorEnabled = false;
    GameManager::sharedState()->m_playLayer = this;
    GameManager::sharedState()->m_isParticleObject = false;

    m_colorKeyDict = cocos2d::CCDictionary::create();
    CC_SAFE_RETAIN(m_colorKeyDict);

    m_checkpointArray = cocos2d::CCArray::create();
    CC_SAFE_RETAIN(m_checkpointArray);

    m_speedObjects = cocos2d::CCArray::create();
    CC_SAFE_RETAIN(m_speedObjects);

    m_circleWaveArray = cocos2d::CCArray::create();
    CC_SAFE_RETAIN(m_circleWaveArray);

    m_unk37c0 = cocos2d::CCArray::create();
    CC_SAFE_RETAIN(m_unk37c0);

    GJBaseGameLayer::setupLayers();

    m_pulseRodIndex = static_cast<int>(std::round(
        (static_cast<float>(rand()) / RAND_MAX) + 1.0f));

    m_glitterParticles = cocos2d::CCParticleSystemQuad::create("glitterEffect.plist", false);
    m_glitterParticles->setPositionType(kCCPositionTypeRelative);
    m_objectLayer->addChild(m_glitterParticles, 0);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float scaleW = cocos2d::CCDirector::sharedDirector()->getContentScaleFactor();
    m_glitterParticles->setPosVar(cocos2d::ccp(
        (scaleW * 480.0f) / 1.8f,
        winSize.height * 0.5f));
    m_glitterParticles->stopSystem();

    m_debugDrawNode = cocos2d::CCDrawNode::create();
    cocos2d::ccBlendFunc blendFunc = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
    m_debugDrawNode->setBlendFunc(blendFunc);
    m_debugDrawNode->setTag(-9999);
    m_aboveShaderObjectLayer->addChild(m_debugDrawNode, 1401);
    updateDebugDrawSettings();

    GJBaseGameLayer::createPlayer();

    if (m_level->m_levelType == GJLevelType::Editor) {
        if (!GameManager::sharedState()->getGameVariable("0174")) {
            m_statusLabel = cocos2d::CCLabelBMFont::create("",
                GameManager::sharedState()->m_loadedFont.c_str());
            m_statusLabel->setOpacity(50);
            m_statusLabel->setScale(0.4f);
            addChild(m_statusLabel, 1000);
            m_statusLabel->setAnchorPoint(cocos2d::ccp(0.0f, 1.0f));

            float screenTop = cocos2d::CCDirector::sharedDirector()->getScreenTop();
            m_statusLabel->setPosition(cocos2d::ccp(5.0f, screenTop - 4.0f));
            m_statusLabel->setVisible(false);
        }
        bool ignoreDmg = GameManager::sharedState()->getGameVariable("0173");
        toggleIgnoreDamage(ignoreDmg);
    }

    m_passedIntegrity = true;
    std::string levelStr = m_level->m_levelString;

    if (m_level->m_levelType == GJLevelType::Main) {
        m_passedIntegrity = LevelTools::verifyLevelIntegrity(levelStr, m_level->m_levelID);
    } else if (m_level->m_levelType == GJLevelType::Editor && m_level->m_stars <= 0) {
    } else {
        m_passedIntegrity = !levelStr.empty();
    }

    if (!m_passedIntegrity) {
        cocos2d::CCLabelBMFont* integrityLabel = cocos2d::CCLabelBMFont::create("Level Corrupted",
            GameManager::sharedState()->m_loadedFont.c_str());
        addChild(integrityLabel, 100);
        integrityLabel->setPosition(cocos2d::ccp(winSize.width * 0.5f, winSize.height * 0.5f));
    }

    prepareCreateObjectsFromSetup(&levelStr);

    if (!dontCreateObjects) {
        while (m_loadingProgress < 1.0f) {
            processCreateObjectsFromSetup();
        }
    }

    return true;
}

void PlayLayer::loadFromCheckpoint(CheckpointObject* object) {
    if (!object)
        return;

    m_gameState = object->m_gameState;

    if (m_shaderLayer) {
        m_shaderLayer->m_state = object->m_shaderState;
    }

    if (!m_isPracticeMode || m_practiceMusicSync) {
        if (m_isPlatformer) {
            FMODAudioEngine::sharedEngine()->pauseAllMusic(false);
        } else {
            GJBaseGameLayer::pauseAudio();
        }

        FMODAudioEngine::sharedEngine()->loadAudioState(&object->m_audioState);

        if (m_isPlatformer) {
            if (!FMODAudioEngine::sharedEngine()->isAnyPersistentPlaying()) {
                FMODAudioEngine::sharedEngine()->fadeInBackgroundMusic(0.1f);
            }
        }

        GJBaseGameLayer::processActivatedAudioTriggers(m_gameState.m_levelTime);

        m_musicPrepared = false;
        m_audioPaused = true;
    }

    for (auto* trigger : m_sequenceTriggers) {
        auto it = object->m_sequenceTriggerStateUnorderedMap.find(trigger->m_sequenceTriggerID);
        if (it != object->m_sequenceTriggerStateUnorderedMap.end()) {
            trigger->m_state = it->second;
        }
    }

    if (m_gameState.m_isDualMode) {
        m_gameState.m_isDualMode = false;
        GJBaseGameLayer::toggleDualMode(nullptr, true, false, true);
    }

    bool jumpPressed = UILayer::isJumpButtonPressed(m_uiLayer) && !m_isPlatformer;

    PlayerCheckpoint* player1Checkpoint = object->m_player1Checkpoint;
    player1Checkpoint->m_jumpBuffered = jumpPressed && !player1Checkpoint->m_controlsDisabled;
    PlayerObject::loadFromCheckpoint(m_player1, player1Checkpoint);

    if (m_gameState.m_isDualMode) {
        PlayerCheckpoint* player2Checkpoint = object->m_player2Checkpoint;
        player2Checkpoint->m_jumpBuffered = jumpPressed && !player2Checkpoint->m_controlsDisabled;
        PlayerObject::loadFromCheckpoint(m_player2, player2Checkpoint);

        if (!PlayerObject::isFlying(m_player1) && !m_player1->m_isBall) {
            GJBaseGameLayer::enterDualMode(m_gameState.m_lastActivatedPortal1, true);
        }
    }

    GJEffectManager::loadFromState(m_effectManager, &object->m_effectManagerState);

    if (object->m_gradientTriggerObjectArray) {
        for (unsigned int i = 0; i < object->m_gradientTriggerObjectArray->count(); ++i) {
            GradientTriggerObject* gradientObj = static_cast<GradientTriggerObject*>(
                object->m_gradientTriggerObjectArray->objectAtIndex(i));
            GJBaseGameLayer::triggerGradientCommand(gradientObj);
        }
    }

    processLoadedMoveActions();
    loadDynamicSaveObjects(&object->m_vectorSavedObjectStateRef);
    loadActiveSaveObjects(&object->m_vectorActiveSaveObjectState,
                          &object->m_vectorSpecialSaveObjectState);

    toggleGroundVisibility(!object->m_ground2Invisible);
    GJBaseGameLayer::togglePlayerStreakBlend(m_gameState.m_playerStreakBlend, true);

    if (m_collectibles) {
        cocos2d::ccArray* data = m_collectibles->data;
        if (data->num) {
            cocos2d::CCObject** objects = data->arr;
            cocos2d::CCObject** lastObject = &objects[data->num - 1];
            for (cocos2d::CCObject** iter = objects; iter <= lastObject; ++iter) {
                EffectGameObject* effectObj = static_cast<EffectGameObject*>(*iter);
                if (!effectObj)
                    break;
                if (GJEffectManager::hasBeenTriggered(m_effectManager, effectObj->m_uniqueID, 0)) {
                    GameObject::disableObject(effectObj);
                    if (effectObj->getType() == GameObjectType::UserCoin) {
                        GJBaseGameLayer::pickupItem(effectObj);
                    }
                }
            }
        }
    }

    for (int groupID = 0; groupID < 10000; ++groupID) {
        if (!GJEffectManager::isGroupEnabled(m_effectManager, groupID)) {
            cocos2d::CCArray* group = GJBaseGameLayer::getGroup(groupID);
            if (group) {
                cocos2d::ccArray* data = group->data;
                if (data->num) {
                    GameObject** objects = reinterpret_cast<GameObject**>(data->arr);
                    GameObject** lastObject = &objects[data->num - 1];
                    for (GameObject** iter = objects; iter <= lastObject; ++iter) {
                        GameObject* gameObject = *iter;
                        if (!gameObject)
                            break;
                        GameObject::groupWasDisabled(gameObject);
                    }
                }
            }
        }
    }

    GameObjectType activeMode = PlayerObject::getActiveMode(m_player1);
    GJBaseGameLayer::updateDualGround(m_player1, activeMode, true, 0.0f);

    m_gameState.m_timeWarpRelated = m_gameState.m_timeWarp;
    cocos2d::CCDirector::sharedDirector()->getScheduler()->m_fTimeScale = m_gameState.m_timeWarpRelated;
}

void PlayLayer::loadActiveSaveObjects(std::vector<SavedActiveObjectState>* activeObjects,
                                       std::vector<SavedSpecialObjectState>* specialObjects) {
    for (SavedActiveObjectState& state : *activeObjects) {
        state.m_gameObject->m_activatedByPlayer1 = state.m_activatedByPlayer1;
        state.m_gameObject->m_activatedByPlayer2 = state.m_activatedByPlayer2;
    }

    for (SavedSpecialObjectState& state : *specialObjects) {
        GameObject* gameObject = state.m_gameObject;

        if (gameObject->m_classType == GameObjectClassType::Animated) {
            AnimatedGameObject::playAnimation(static_cast<AnimatedGameObject*>(gameObject), state.m_animationID);
        } else if (gameObject->m_objectID == 1615) {
            gameObject->m_updateParents = true;
        }
    }
}

void PlayLayer::dialogClosed(DialogLayer* layer) {
    delayedResetLevel();
}

void PlayLayer::showNewBest(bool newBest, int diamonds, int orbs, bool key, bool highAttempts, bool leaderboard) {
    float displayDuration = 0.7f;
    int totalRewards = diamonds + orbs;
    if (totalRewards > 0)
        displayDuration = 1.3f;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float yOffset = (totalRewards > 0) ? 10.0f : 0.0f;
    cocos2d::CCPoint center(winSize.width * 0.5f, (winSize.height * 0.5f) + 20.0f + yOffset);

    cocos2d::CCNode* container = cocos2d::CCNode::create();
    addChild(container, 100);
    container->setPosition(center);

    if (!leaderboard) {
        if (newBest && !m_doNot) {
            cocos2d::CCLabelBMFont* newBestLabel = cocos2d::CCLabelBMFont::create("New Best!", "bigFont.fnt");
            newBestLabel->setScale(0.65f);
            newBestLabel->setPosition(cocos2d::ccp(0.0f, 5.0f));
            newBestLabel->setAnchorPoint(cocos2d::ccp(0.5f, 0.0f));
            container->addChild(newBestLabel);
        } else if (m_doNot) {
            static const char* ripMessages[] = {
                "RIP",
                "Y u die?",
                "Not 100%",
                "Try jump?",
                "Kappa",
                "Too slow",
                "Ples omg why no",
                "Maybe jump?",
                "Problem?",
                "Nice crash",
                "Try again?",
                "Almost...",
                "Hehe...",
                "U no like?",
                "RIP",
                "Rage time?",
                "One more?",
                "Noscope",
                "Feel the pain",
                "Over 1%!",
                "BOOM!",
                "Click faster",
                "Dont be sad",
                "Go again",
                "So close",
                "What!?",
                "You ok?",
                "Did he died?",
                "Go?",
                "Game Over",
                "LOL",
                "RubRub?",
                "You Died",
                "Continue?",
                "Fail",
                "Wrong button",
                "Too slow",
                "Insert coin",
                "You Lose",
                "Combo Breaker!",
                "Time's Up!",
                "GGWP",
                "VAMOS",
                "Not ok",
                "Dead, you are",
                "Slow down",
                "Nice try",
                "Take a break",
                "Explosion!",
                "So random",
                "...",
                "Is that it?",
                "Not even close",
                "Give up",
                "Hotel?",
                "liaf uoy",
                "Go home",
                "Spam?",
                "Try harder",
                "PogChamp",
                "Hang in there",
                "That's progress for ya",
                "Unlucky",
                "Why tho?",
                "Nope!",
                "Brutal",
                "Not like this",
                "Oopsie",
                "Crushed",
                "A for effort",
                "Denied",
                "So far, so bad",
                "Was that the plan?",
                "Spike loves you.",
                "Spike: 1, You: 0",
                "Need a Map?",
                "Who put that there?",
                "That looked painful",
                "Don't quit your day job",
                "Again? Really?",
                "Stop testing gravity",
                "I blame 240fps",
                "Wrong way!",
                "RobTop pls fix!",
                "Who buffed this?",
                "Geometry Dash'nt",
                "I like that part",
                "Checkpoints, pls",
                "Why jump there?",
                "Who said GO?!",
                "New Best'nt"
            };

            int messageCount = sizeof(ripMessages) / sizeof(ripMessages[0]);
            int randomIndex = lroundf((static_cast<float>(rand()) / RAND_MAX) * (messageCount - 1));
            if (randomIndex < 0) randomIndex = 0;
            if (randomIndex >= messageCount) randomIndex = messageCount - 1;

            cocos2d::CCLabelBMFont* ripLabel = cocos2d::CCLabelBMFont::create(ripMessages[randomIndex], "bigFont.fnt");
            ripLabel->setScale(0.65f);
            ripLabel->setPosition(cocos2d::ccp(0.0f, 5.0f));
            ripLabel->setAnchorPoint(cocos2d::ccp(0.5f, 0.0f));
            container->addChild(ripLabel);
        } else {
            cocos2d::CCSprite* newBestSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_newBest_001.png");
            container->addChild(newBestSprite);
            newBestSprite->setAnchorPoint(cocos2d::ccp(0.5f, 0.0f));
        }
    }

    std::string percentText = cocos2d::CCString::createWithFormat("%i%%", m_lastAttemptPercent)->getCString();
    cocos2d::CCLabelBMFont* percentLabel = cocos2d::CCLabelBMFont::create(percentText.c_str(), "bigFont.fnt");
    percentLabel->setAnchorPoint(cocos2d::ccp(0.5f, 1.0f));
    percentLabel->setScale(0.6f);
    percentLabel->setPosition(cocos2d::ccp(0.0f, -4.0f));
    container->addChild(percentLabel);

    float labelOffset = -5.0f;

    if (diamonds > 0) {
        if (orbs > 0)
            labelOffset = -20.0f;

        std::string diamondText = cocos2d::CCString::createWithFormat("+%i", diamonds)->getCString();
        cocos2d::CCLabelBMFont* diamondLabel = cocos2d::CCLabelBMFont::create(diamondText.c_str(), "bigFont.fnt");
        diamondLabel->setScale(0.55f);
        container->addChild(diamondLabel);

        cocos2d::CCSprite* diamondSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
        diamondSprite->setAnchorPoint(cocos2d::ccp(0.0f, 0.5f));
        diamondSprite->setScale(0.5f);
        container->addChild(diamondSprite);

        if (!key) {
            FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");
        }

        float diamondLabelX;
        if (orbs > 0) {
            diamondLabelX = 40.0f;
        } else {
            float labelWidth = diamondLabel->getContentSize().width * diamondLabel->getScale();
            float spriteWidth = diamondSprite->getContentSize().width * diamondSprite->getScale();
            float percentWidth = percentLabel->getContentSize().width * percentLabel->getScale();
            diamondLabelX = -((labelWidth + 1.0f + spriteWidth) * 0.5f - percentWidth * 0.5f);
        }

        diamondLabel->setPosition(cocos2d::ccp(diamondLabelX + labelOffset, -62.0f));

        cocos2d::CCPoint diamondLabelPos = diamondLabel->getPosition();
        float labelHalfWidth = (diamondLabel->getContentSize().width * 0.5f) * diamondLabel->getScale();
        diamondSprite->setPosition(cocos2d::ccp(diamondLabelPos.x + labelHalfWidth + 1.0f, diamondLabelPos.y + (-1.0f)));
    }

    if (orbs > 0) {
        std::string orbText = cocos2d::CCString::createWithFormat("+%i", orbs)->getCString();
        cocos2d::CCLabelBMFont* orbLabel = cocos2d::CCLabelBMFont::create(orbText.c_str(), "bigFont.fnt");
        orbLabel->setScale(0.55f);
        container->addChild(orbLabel);

        cocos2d::CCSprite* orbSprite = cocos2d::CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
        orbSprite->setAnchorPoint(cocos2d::ccp(0.0f, 0.5f));
        container->addChild(orbSprite);

        FMODAudioEngine::sharedEngine()->playEffect("magicExplosion.ogg");

        float orbLabelX;
        if (diamonds > 0) {
            orbLabelX = -40.0f;
        } else {
            float labelWidth = orbLabel->getContentSize().width * orbLabel->getScale();
            float spriteWidth = orbSprite->getContentSize().width * orbSprite->getScale();
            float percentWidth = percentLabel->getContentSize().width * percentLabel->getScale();
            orbLabelX = -((labelWidth + 5.0f + spriteWidth) * 0.5f - percentWidth * 0.5f);
        }

        orbLabel->setPosition(cocos2d::ccp(orbLabelX + labelOffset, -62.0f));

        cocos2d::CCPoint orbLabelPos = orbLabel->getPosition();
        float orbHalfWidth = (orbLabel->getContentSize().width * 0.5f) * orbLabel->getScale();
        orbSprite->setPosition(cocos2d::ccp(orbLabelPos.x + orbHalfWidth + 5.0f, orbLabelPos.y + (-0.5f)));

        cocos2d::CCPoint worldPos = m_player1->convertToWorldSpace(cocos2d::CCPoint(0.0f, 0.0f));
        CurrencySpriteType currencyType = key ? CurrencySpriteType::DemonKey : CurrencySpriteType::Icon;

        CurrencyRewardLayer* rewardLayer = CurrencyRewardLayer::create(
            orbs,
            0,
            0,
            diamonds,
            currencyType,
            key,
            CurrencySpriteType::Icon,
            0,
            worldPos,
            CurrencyRewardType::Default,
            0.9f,
            static_cast<float>(diamonds));

        addChild(rewardLayer, 101);

        if (key) {
            FMODAudioEngine::sharedEngine()->playEffect("secretKey.ogg");
        }

        if (!highAttempts) {
            rewardLayer->m_delegate = this;
        }
    }

    container->setScale(0.1f);

    cocos2d::CCAction* popupAction = cocos2d::CCSequence::create(
        cocos2d::CCEaseElasticOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f), 0.6f),
        cocos2d::CCDelayTime::create(displayDuration),
        cocos2d::CCEaseIn::create(cocos2d::CCScaleTo::create(0.2f, 0.01f), 2.0f),
        cocos2d::CCHide::create(),
        cocos2d::CCCallFunc::create(container, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr);
    container->runAction(popupAction);

    if (totalRewards > 0) {
        cocos2d::CCLayerColor* overlay = cocos2d::CCLayerColor::create();
        addChild(overlay, 99);

        cocos2d::CCAction* overlayAction = cocos2d::CCSequence::create(
            cocos2d::CCFadeTo::create(0.3f, 100),
            cocos2d::CCDelayTime::create(displayDuration + 0.3f),
            cocos2d::CCFadeTo::create(0.4f, 0),
            cocos2d::CCCallFunc::create(overlay, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        overlay->runAction(overlayAction);
    }
}