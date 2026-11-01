#include "GJBaseGameLayer.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "GameToolbox.h"
#include "GJEffectManager.h"
#include "GJGameLevel.h"
#include "GJGameState.h"
#include "GJGroundLayer.h"
#include "GJMGLayer.h"
#include "GJGradientLayer.h"
#include "FMODAudioEngine.h"
#include "FMODLevelVisualizer.h"
#include "MusicDownloadManager.h"
#include "ObjectToolbox.h"
#include "OBB2D.h"
#include "ShaderLayer.h"
#include "SimplePlayer.h"
#include "UILayer.h"
#include "CCNodeContainer.h"
#include "CCCircleWave.h"
#include "CCMenuItemSpriteExtra.h"
#include "ButtonSprite.h"
#include "AdvancedFollowEditObject.h"
#include "AdvancedFollowTriggerObject.h"
#include "AnimatedGameObject.h"
#include "CheckpointGameObject.h"
#include "CountTriggerGameObject.h"
#include "EnhancedGameObject.h"
#include "EnhancedTriggerObject.h"
#include "EnterEffectObject.h"
#include "EventTriggerGameObject.h"
#include "GameOptionsTrigger.h"
#include "GradientTriggerObject.h"
#include "ItemCompareTriggerGameObject.h"
#include "ItemTriggerGameObject.h"
#include "KeyframeGameObject.h"
#include "LabelGameObject.h"
#include "LevelSettingsObject.h"
#include "ObjectControlGameObject.h"
#include "PlatformerEndTriggerObject.h"
#include "PlayerControlGameObject.h"
#include "RingObject.h"
#include "RotateGameplayGameObject.h"
#include "SFXTriggerGameObject.h"
#include "ShaderGameObject.h"
#include "SongTriggerGameObject.h"
#include "SpawnParticleGameObject.h"
#include "StartPosObject.h"
#include "TeleportPortalObject.h"
#include "TimerTriggerGameObject.h"
#include "TransformTriggerGameObject.h"
#include <algorithm>
#include <cmath>
#include <cstring>

void GJBaseGameLayer::activateCustomRing(RingObject* object) {
    std::vector<int> remapKeys;
    if (object->m_isSpawnOnly) {
        spawnGroup(object->m_targetGroupID, false, 0.0, &remapKeys, object->m_uniqueID, object->m_controlID);
    } else {
        toggleGroupTriggered(object->m_targetGroupID, object->m_activateGroup, &remapKeys, object->m_uniqueID, object->m_controlID);
    }
}

void GJBaseGameLayer::activateEndTrigger(int targetID, bool reverse, bool lockPlayerY) {
}

void GJBaseGameLayer::activateItemEditTrigger(ItemTriggerGameObject* object) {
    int resultType2 = object->m_resultType2;
    int resultType3 = object->m_resultType3;
    int targetItemMode = object->m_targetItemMode;
    if (resultType2 <= 0) resultType2 = 1;
    if (resultType3 <= 2) resultType3 = 3;
    if (targetItemMode <= 0) targetItemMode = 1;

    if (targetItemMode != 3 && object->m_targetGroupID <= 0) return;

    float mod1 = roundf(object->m_mod1 * 1000.0f) / 1000.0f;

    bool useItem1 = false;
    if (object->m_item1Mode) {
        useItem1 = true;
        if (object->m_itemID <= 0) useItem1 = (object->m_item1Mode - 3) <= 1;
    }

    bool useItem2 = false;
    if (object->m_item2Mode) {
        useItem2 = true;
        if (object->m_itemID2 <= 0) useItem2 = (object->m_item2Mode - 3) <= 1;
    }

    double baseValue;
    if (useItem1) {
        baseValue = getItemValue(object->m_item1Mode, object->m_itemID);
        if (useItem2) {
            double operand = getItemValue(object->m_item2Mode, object->m_itemID2);
            baseValue = performMathOperation(baseValue, operand, resultType2);
        }
    } else {
        baseValue = getItemValue(object->m_item1Mode, object->m_itemID);
    }

    double targetValue = getItemValue(targetItemMode, object->m_targetGroupID);

    double value;
    if (useItem2) {
        value = performMathOperation(baseValue, mod1, resultType3);
    } else {
        value = mod1;
    }

    if (object->m_roundType1) value = performMathRounding(value, object->m_roundType1);
    if (object->m_signType1) value = performMathSign(value, object->m_signType1);
    if (object->m_resultType1) value = performMathOperation(targetValue, value, object->m_resultType1);
    if (object->m_roundType2) value = performMathRounding(value, object->m_roundType2);
    if (object->m_signType2) value = performMathSign(value, object->m_signType2);

    if (targetItemMode == 2) {
        GJEffectManager::updateTimer(m_effectManager, object->m_targetGroupID, value);
        return;
    }
    if (targetItemMode == 3) {
        m_gameState.m_unkBool32 = 1;
        m_gameState.m_points = (int)value;
    } else if (targetItemMode == 1) {
        int intValue = (int)value;
        GJEffectManager::updateCountForItem(m_effectManager, object->m_targetGroupID, intValue);
        updateCounters(object->m_targetGroupID, intValue);
    }
}

void GJBaseGameLayer::activateObjectControlTrigger(ObjectControlGameObject* object) {
}

void GJBaseGameLayer::activatePersistentItemTrigger(ItemTriggerGameObject* object) {
    if (object->m_timer) {
        if (object->m_targetAll) {
            if (object->m_reset) {
                auto& timerSet = m_effectManager->m_persistentTimerItemSet;
                std::vector<int> timers;
                timers.reserve(timerSet.size());
                for (int id : timerSet) timers.push_back(id);
                for (int id : timers) {
                    if (GJEffectManager::timerExists(m_effectManager, id)) {
                        GJEffectManager::updateTimer(m_effectManager, id, 0.0);
                    }
                }
            }
            if (!object->m_persistent) {
                GJEffectManager::removePersistentFromAllTimers(m_effectManager);
            }
        } else {
            GJEffectManager::toggleTimerPersistent(m_effectManager, object->m_itemID, object->m_persistent);
            if (object->m_reset && GJEffectManager::timerExists(m_effectManager, object->m_itemID)) {
                GJEffectManager::updateTimer(m_effectManager, object->m_itemID, 0.0);
            }
        }
    } else if (object->m_targetAll) {
        if (object->m_reset) {
            std::vector<int> items;
            items.reserve(m_effectManager->m_persistentItemCountMap.size());
            for (auto& pair : m_effectManager->m_persistentItemCountMap) {
                items.push_back(pair.first);
            }
            for (int id : items) {
                GJEffectManager::updateCountForItem(m_effectManager, id, 0);
                updateCounters(id, 0);
            }
        }
        if (!object->m_persistent) {
            GJEffectManager::removePersistentFromAllItems(m_effectManager);
        }
    } else {
        GJEffectManager::toggleItemPersistent(m_effectManager, object->m_itemID, object->m_persistent);
        if (object->m_reset) {
            GJEffectManager::updateCountForItem(m_effectManager, object->m_itemID, 0);
            updateCounters(object->m_itemID, 0);
        }
    }
}

void GJBaseGameLayer::activatePlatformerEndTrigger(PlatformerEndTriggerObject* object) {
}

void GJBaseGameLayer::activatePlayerControlTrigger(PlayerControlGameObject* object) {
    bool targetPlayer2 = object->m_targetPlayer2;
    bool processPlayer1 = true;

    if (object->m_targetPlayer1 < !targetPlayer2) {
        targetPlayer2 = true;
    } else if (!object->m_targetPlayer1) {
        processPlayer1 = false;
    }

    if (processPlayer1) {
        if (object->m_stopJump) {
            m_player1->releaseButton(Jump);
        }
        if (m_isPlatformer && object->m_stopMove) {
            m_player1->releaseButton(static_cast<PlayerButton>(5));
            m_player1->releaseButton(static_cast<PlayerButton>(2));
            m_player1->releaseButton(static_cast<PlayerButton>(3));
        }
        if (object->m_stopRotation) {
            m_player1->stopRotation(true, 0);
        }
        if (object->m_stopSlide) {
            m_player1->handlePlayerCommand(543);
        }
    }

    if (!targetPlayer2) return;

    if (object->m_stopJump) {
        m_player2->releaseButton(Jump);
    }
    if (m_isPlatformer && object->m_stopMove) {
        m_player2->releaseButton(static_cast<PlayerButton>(5));
        m_player2->releaseButton(static_cast<PlayerButton>(2));
        m_player2->releaseButton(static_cast<PlayerButton>(3));
    }
    if (object->m_stopRotation) {
        m_player2->stopRotation(true, 0);
    }
    if (object->m_stopSlide) {
        m_player2->handlePlayerCommand(543);
    }
}

void GJBaseGameLayer::activateResetTrigger(EffectGameObject* object) {
    cocos2d::CCArray* group = getGroup(object->m_targetGroupID);
    if (!group) return;
    if (!cocos2d::CCArray::count(group)) return;
    cocos2d::ccArray* data = group->data;
    if (!data->num) return;

    cocos2d::CCObject** arr = data->arr;
    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = static_cast<GameObject*>(arr[i]);
        if (!obj) return;
        if (obj->m_objectID == 2063) {
            GJEffectManager::removeTriggeredID(m_effectManager, obj->m_uniqueID, m_player1->m_uniqueID);
            GJEffectManager::removeTriggeredID(m_effectManager, obj->m_uniqueID, m_player2->m_uniqueID);
        } else if (obj->getType() == 30 || obj->getType() == 21) {
            GJEffectManager::removeTriggeredID(m_effectManager, obj->m_uniqueID, 0);
        }
        obj->resetObject();
    }
}

void GJBaseGameLayer::activateSFXEditTrigger(SFXTriggerGameObject* object) {
    if (m_practiceMusicSync || !m_isPracticeMode) {
        if (m_loadingStartPosition) {
            activatedAudioTrigger(object);
        } else if (m_startingFromBeginning) {
            m_activeSfxTriggers.push_back(object);
        } else {
            if (object->m_groupID > 0) {
                cocos2d::CCArray* group = getGroup(object->m_groupID);
                if (group && cocos2d::CCArray::count(group)) {
                    std::set<int> sfxIDs;
                    cocos2d::ccArray* data = group->data;
                    for (unsigned int i = 0; i < data->num; ++i) {
                        GameObject* obj = static_cast<GameObject*>(data->arr[i]);
                        if (!obj) break;
                        if (obj->m_objectID == 3602) {
                            auto& ids = m_gameState.m_unkUMap8[obj->m_uniqueID];
                            for (int id : ids) sfxIDs.insert(id);
                        }
                    }
                    for (int id : sfxIDs) {
                        if (!object->m_applyDisabled) {
                            applySFXEditTrigger(id, 0, object);
                        }
                        if (object->m_targetGroupID > 0) {
                            addProximityVolumeEffect(id, 0, object);
                        }
                    }
                }
            }
            if (object->m_sfxGroup > 0) {
                if (!object->m_applyDisabled) {
                    applySFXEditTrigger(object->m_sfxGroup, 1, object);
                }
                if (object->m_targetGroupID > 0) {
                    addProximityVolumeEffect(object->m_sfxGroup, 1, object);
                }
            }
            if (object->m_sfxUniqueID > 0) {
                int channelID = FMODAudioEngine::sharedEngine()->channelIDForUniqueID(object->m_sfxUniqueID);
                if (!object->m_applyDisabled) {
                    applySFXEditTrigger(channelID, 0, object);
                }
            }
        }
    }
}

void GJBaseGameLayer::activateSFXTrigger(SFXTriggerGameObject* object) {
    if (!(m_practiceMusicSync || !m_isPracticeMode)) return;
    if (object->m_soundID <= 0) return;

    if (m_loadingStartPosition) {
        activatedAudioTrigger(object);
        return;
    }

    if (m_startingFromBeginning) {
        m_activeSfxTriggers.push_back(object);
        return;
    }

    if (!object->m_ignoreVolumeTest && object->m_targetGroupID > 0) {
        SFXTriggerInstance instance;
        instance.m_controlID = object->m_controlID;
        instance.m_sfxTriggerGameObject = object;
        instance.m_groupID1 = object->m_targetGroupID;
        instance.m_groupID2 = object->m_centerGroupID;
        if (volumeForProximityEffect(&instance) <= 0.0f) return;
    }

    bool reverbEnabled = object->m_reverbEnabled;
    bool preload = object->m_preload;
    object->m_preload = false;

    if (reverbEnabled) {
        FMODAudioEngine::sharedEngine()->updateReverb(object->m_reverbPreset, 0);
    }

    int channelID = GameManager::sharedState()->playSFXTrigger(object);
    object->m_preload = preload;

    if (channelID > 0) {
        auto& ids = m_gameState.m_unkUMap8[object->m_uniqueID];
        ids.push_back(channelID);
        if (object->m_targetGroupID > 0) {
            addProximityVolumeEffect(channelID, 0, object);
        }
    }
}

void GJBaseGameLayer::activateSongEditTrigger(SongTriggerGameObject* object) {
    if (!(m_practiceMusicSync || !m_isPracticeMode)) return;
    activatedAudioTrigger(object);
    if (m_loadingStartPosition) return;
    if (m_startingFromBeginning) {
        m_activeSfxTriggers.push_back(object);
        return;
    }
    applySFXEditTrigger(object->m_songChannel, 2, object);
    if (object->m_targetGroupID > 0) {
        addProximityVolumeEffect(object->m_songChannel, 2, object);
    }
}

void GJBaseGameLayer::activateSongTrigger(SongTriggerGameObject* object) {
    if (!(m_practiceMusicSync || !m_isPracticeMode)) return;
    activatedAudioTrigger(object);
    if (m_loadingStartPosition) return;
    if (m_startingFromBeginning) {
        m_activeSfxTriggers.push_back(object);
        return;
    }

    int channelID = object->m_songChannel;
    int musicID = ~channelID;

    if (object->m_loadPrep) {
        FMODAudioEngine::sharedEngine()->activateQueuedMusic(musicID);
        return;
    }

    std::string path = MusicDownloadManager::sharedState()->pathForSong(object->m_soundID);
    float pitch = FMODAudioEngine::pitchForIdx(object->m_speed);
    FMODAudioEngine::sharedEngine()->queueStartMusic(
        path,
        pitch,
        0.0f,
        object->m_volume,
        object->m_loop,
        object->m_start,
        object->m_end,
        object->m_fadeIn,
        object->m_fadeOut,
        musicID,
        1,
        channelID,
        !object->m_prep,
        object->m_dontReset);
}

void GJBaseGameLayer::activatedAudioTrigger(SFXTriggerGameObject* object) {
    activatedAudioTrigger(object, m_gameState.m_levelTime);
}

void GJBaseGameLayer::addCustomEnterEffect(EnterEffectObject* object, bool enter) {
    int enterChannel = object->m_enterChannel;
    if (enterChannel > 100) return;

    std::unordered_map<int, std::vector<EnterEffectInstance>>& vectors =
        enter ? m_gameState.m_enterEffectInstanceVectors : m_gameState.m_exitEffectInstanceVectors;
    std::vector<EnterEffectInstance>& instances = vectors[enterChannel];

    int effectID = object->m_effectID;
    if (effectID > 0) {
        for (auto it = instances.begin(); it != instances.end(); ++it) {
            if (it->m_gameObject->m_effectID == effectID) {
                if (it->m_gameObject->m_uniqueID == object->m_uniqueID
                    && it->m_targetID == object->m_targetGroupID
                    && it->m_centerID == object->m_centerGroupID) {
                    EnterEffectInstance::loadValuesFromObject(&*it, object);
                    return;
                }
                instances.erase(it);
                break;
            }
        }
    }

    int specialKey = getSpecialKey(object->m_targetGroupID, object->m_ignoreGroupParent, object->m_ignoreLinkedObjects);
    int targetGroupIndex = 0;
    int controlID = 0;
    auto it = m_targetGroups.find(specialKey);
    if (it != m_targetGroups.end()) {
        targetGroupIndex = it->second.first;
        controlID = it->second.second;
    }

    EnterEffectInstance instance{};
    instance.m_gameObject = object;
    instance.m_targetID = object->m_targetGroupID;
    instance.m_centerID = object->m_centerGroupID;
    instance.m_targetGroupIndex = targetGroupIndex;
    instance.m_controlID = object->m_controlID;
    if (controlID > 0) instance.m_easeIndices.resize(controlID);
    EnterEffectInstance::loadValuesFromObject(&instance, object);
    instances.push_back(std::move(instance));
}

void GJBaseGameLayer::addKeyframe(KeyframeGameObject* object) {
    object->m_keyframeActive = true;
    cocos2d::CCArray* group = static_cast<cocos2d::CCArray*>(
        cocos2d::CCDictionary::objectForKey(m_keyframeGroups, object->m_keyframeGroup));
    if (!group) return;
    unsigned int index = object->m_keyframeIndex;
    if (index >= cocos2d::CCArray::count(group)) {
        cocos2d::CCArray::addObject(group, object);
    } else {
        cocos2d::CCArray::insertObject(group, object, index);
    }
    updateKeyframeOrder(object->m_keyframeGroup);
}

void GJBaseGameLayer::addObjectCounter(LabelGameObject* object) {
    int itemID = object->m_itemID;
    if (object->m_shownSpecial < 0) itemID = object->m_shownSpecial;
    auto& targetMap = object->m_isTimeCounter ? m_timeLabelObjects : m_labelObjects;
    targetMap[itemID].push_back(object);
}

void GJBaseGameLayer::addPickupTrigger(CountTriggerGameObject* object) {
    int itemID = object->m_itemID;
    if (object->m_pickupTriggerMode == 1) {
        int count = GJEffectManager::countForItem(m_effectManager, itemID);
        GJEffectManager::updateCountForItem(m_effectManager, itemID,
            static_cast<int>(llroundf(static_cast<float>(count) * object->m_pickupTriggerMultiplier)));
    } else if (object->m_pickupTriggerMode == 2) {
        int count = GJEffectManager::countForItem(m_effectManager, itemID);
        float multiplier = object->m_pickupTriggerMultiplier;
        if (multiplier != 0.0f) {
            GJEffectManager::updateCountForItem(m_effectManager, itemID,
                static_cast<int>(llroundf(static_cast<float>(count) / multiplier)));
        }
    } else if (object->m_isOverride) {
        GJEffectManager::updateCountForItem(m_effectManager, itemID, object->m_pickupCount);
    } else {
        GJEffectManager::addCountToItem(m_effectManager, itemID, object->m_pickupCount);
    }
    int newCount = GJEffectManager::countForItem(m_effectManager, itemID);
    updateCounters(itemID, newCount);
}

void GJBaseGameLayer::addPoints(int points) {
    m_gameState.m_unkBool32 = true;
    m_gameState.m_points += points;
}

void GJBaseGameLayer::addRemapTargets(std::set<int>* targets) {
    std::unordered_map<int, std::unordered_set<int>> remapMap;
    for (auto& trigger : m_spawnRemapTriggers) {
        for (auto& pair : trigger) {
            remapMap[pair.first].insert(pair.second);
            remapMap[pair.second].insert(pair.first);
        }
    }

    std::vector<int> worklist;
    worklist.reserve(targets->size());
    for (int id : *targets) worklist.push_back(id);

    size_t idx = 0;
    while (idx < worklist.size()) {
        int current = worklist[idx++];
        auto it = remapMap.find(current);
        if (it == remapMap.end()) continue;
        for (int linked : it->second) {
            if (targets->count(linked)) continue;
            targets->insert(linked);
            worklist.push_back(linked);
        }
    }
}

void GJBaseGameLayer::addToGroup(GameObject* object, int groupID, bool triggerGroup) {
    if (groupID > 9999) return;
    cocos2d::CCArray* group = getGroup(groupID);
    cocos2d::CCArray::addObject(group, object);
    cocos2d::CCArray* subGroup = object->m_isDecoration2 ? getOptimizedGroup(groupID) : getStaticGroup(groupID);
    cocos2d::CCArray::addObject(subGroup, object);
}

void GJBaseGameLayer::addToGroupParents(GameObject* object) {
    if (!object->m_hasGroupParentsString) return;
    cocos2d::CCArray* parents = static_cast<cocos2d::CCArray*>(
        cocos2d::CCDictionary::objectForKey(m_removedParentGroupIDs, object->m_uniqueID));
    if (!parents) return;
    object->m_hasGroupParentsString = false;
    unsigned int count = cocos2d::CCArray::count(parents);
    for (unsigned int i = 0; i < count; ++i) {
        cocos2d::CCObject* entry = cocos2d::CCArray::objectAtIndex(parents, i);
        int groupID = static_cast<int>(reinterpret_cast<uintptr_t>(entry->__vftable));
        if (!getGroupParent(groupID)) {
            setGroupParent(object, groupID);
        }
    }
    cocos2d::CCDictionary::removeObjectForKey(m_removedParentGroupIDs, object->m_uniqueID);
}

void GJBaseGameLayer::addToGroups(GameObject* object, bool duplicateTrigger) {
    for (int i = 0; i < object->m_groupCount; ++i) {
        int groupID = object->getGroupID(i);
        addToGroup(object, groupID, duplicateTrigger);
    }
}

void GJBaseGameLayer::addToSpeedObjects(EffectGameObject* object) {
}

void GJBaseGameLayer::addUIObject(GameObject* object) {
    if (object->m_isUIObject) return;
    auto& positions = m_uiObjectPositions[object->m_uniqueID];
    positions = object->getStartPos();
    removeObjectFromSection(object);
    object->deactivateObject(true);
    object->m_outerSectionIndex = -1;
    object->m_isUIObject = true;
    cocos2d::CCArray::addObject(m_uiObjects, object, cocos2d::CCObject::create());
    if (object->m_objectID == 1816) {
       int oldSize = m_calcCollisionBlockObjects2Size;
         if ((size_t)oldSize < m_calcCollisionBlockObjects2.size()) {
             m_calcCollisionBlockObjects2[oldSize] = static_cast<EffectGameObject*>(object);
         } else {
             m_calcCollisionBlockObjects2.push_back(static_cast<EffectGameObject*>(object));
         }
            object->m_innerSectionIndex = oldSize;                            
            m_calcCollisionBlockObjects2Size = oldSize + 1;
        m_calcCollisionBlockObjects2.push_back(static_cast<EffectGameObject*>(object));
    }
}

void GJBaseGameLayer::animateInGroundNew(bool unk1, float unk2, bool unk3) {
}

void GJBaseGameLayer::animateOutGroundNew(bool instant) {
    bool wasStatic = m_gameState.m_unkBool1;
    m_gameState.m_unkBool7 = false;
    if (wasStatic) resetStaticCamera(false, true);
    float duration = m_gameState.m_isDualMode ? 0.32f : 0.4f;
    m_gameState.tweenValue(m_gameState.m_unkFloat9, 0.0f, 25, duration, 1, 1.5f, -1, -1);
}

void GJBaseGameLayer::animatePortalY(float fromY, float toY, float duration, float easingRate) {
    if (fromY == toY) {
        m_gameState.m_portalY = fromY;
        return;
    }
    m_gameState.m_portalY = fromY;
    m_gameState.tweenValue(fromY, toY, 9, duration, 1, easingRate, -1, -1);
    if (m_gameState.m_cameraZoom != m_gameState.m_targetCameraZoom) {
        m_gameState.m_unkBool6 = true;
    }
}

void GJBaseGameLayer::applyLevelSettings(GameObject* object) {
    if (m_allowStaticRotate) {
        object->m_canRotateFree = true;
    } else {
        object->m_canRotateFree = object->canRotateFree();
    }
    if (object->m_isMirroredByScale != m_fixNegativeScale) {
        object->m_isObjectRectDirty = true;
    }
    object->m_isMirroredByScale = m_fixNegativeScale;
}

void GJBaseGameLayer::applyShake(cocos2d::CCPoint* point) {
    double lastTime = m_gameState.m_unkUint64_1;
    double totalTime = m_gameState.m_totalTime;

    if (lastTime >= 0.0) {
        float interval = m_gameState.m_unkUint16;
        if (interval > 0.0f && (totalTime - lastTime) <= interval) {
            point->x += m_gameState.m_unkPoint34.x;
            point->y += m_gameState.m_unkPoint34.y;
            return;
        }
    }

    m_gameState.m_unkUint64_1 = totalTime;

    float x, y;
    if (m_staticCameraShake) {
        x = ((float)rand() * 4.6566e-10f) > 0.5f ? 1.0f : -1.0f;
        y = ((float)rand() * 4.6566e-10f) > 0.5f ? 1.0f : -1.0f;
    } else {
        x = ((float)rand() * 4.6566e-10f) * 2.0f - 1.0f;
        y = ((float)rand() * 4.6566e-10f) * 2.0f - 1.0f;
    }

    float strength = m_gameState.m_unkUint15;
    m_gameState.m_unkPoint34.x = strength * x;
    m_gameState.m_unkPoint34.y = strength * y;
    point->x += m_gameState.m_unkPoint34.x;
    point->y += m_gameState.m_unkPoint34.y;
}

void GJBaseGameLayer::applyTimeWarp(float timeWarp) {
    if (m_gameState.m_timeWarpRelated == timeWarp) return;
    if (!m_started) return;
    m_gameState.m_timeWarpRelated = timeWarp;
    cocos2d::CCDirector::sharedDirector()->getScheduler()->m_fTimeScale = timeWarp;
}

void GJBaseGameLayer::assignNewStickyGroups(cocos2d::CCArray* objects) {
    if (!objects) return;
    cocos2d::CCDictionary* groups = nullptr;
    unsigned int count = cocos2d::CCArray::count(objects);

    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(objects, i));
        int linked = obj->m_linkedGroup;
        if (linked <= 0) continue;

        cocos2d::CCArray* group;
        if (!groups) {
            groups = cocos2d::CCDictionary::create();
            group = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(groups, obj->m_linkedGroup));
            if (!group) {
                group = cocos2d::CCArray::create();
                cocos2d::CCDictionary::setObject(groups, group, obj->m_linkedGroup);
            }
        } else {
            group = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(groups, linked));
            if (!group) {
                group = cocos2d::CCArray::create();
                cocos2d::CCDictionary::setObject(groups, group, obj->m_linkedGroup);
            }
        }
        cocos2d::CCArray::addObject(group, obj);
    }

    if (!groups) return;
    cocos2d::CCDictElement* element = groups->m_pElements;
    while (element) {
        cocos2d::CCArray* group = static_cast<cocos2d::CCArray*>(element->m_pObject);
        ungroupStickyObjects(group);
        groupStickyObjects(group);
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJBaseGameLayer::asyncBGLoaded(int background) {
    createBackground(background);
    updateLevelColors();
}

void GJBaseGameLayer::asyncGLoaded(int ground) {
    createGroundLayer(ground, 0);
    updateLevelColors();
}

void GJBaseGameLayer::asyncMGLoaded(int middleground) {
    createMiddleground(middleground);
    updateLevelColors();
}

int GJBaseGameLayer::atlasValue(int atlas) {
    if (atlas <= 29) return 0;
    float value = (float)atlas * 1.1f;
    if (value < 9999.0f) return static_cast<int>(value);
    return 9999;
}


static std::string replaceAll(const std::string& source, const std::string& from, const std::string& to) {
    std::string result = source;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool GJBaseGameLayer::addGuideArt(GameObject* object) {
    int objectID = object->m_objectID;

    bool isPortal17 = false;
    bool isPortal19 = false;
    bool isOrb = false;

    switch (objectID) {
        case 660:
        case 1331:
        case 1933:
        case 745:
        case 111:
        case 47:
        case 12:
        case 13:
            isPortal17 = true;
            break;
        case 2926:
        case 10:
        case 11:
            isPortal19 = true;
            break;
        case 84:
        case 1022:
        case 1330:
        case 1333:
            isOrb = true;
            break;
        default:
            return false;
    }

    if (isOrb) {
        if (!m_orbIndicators) return false;
        std::string frameName = ObjectToolbox::sharedState()->intKeyToFrame(objectID);
        std::string extraName = replaceAll(frameName, "_001", "_extra_001");
        cocos2d::CCSprite* sprite = object->addCustomChild(extraName, cocos2d::CCPoint(0.0f, 0.0f), 100);
        sprite->setPosition(cocos2d::CCPoint(
            object->m_obRect.size.width * 0.5f,
            object->m_obRect.size.height * 0.5f));
        cocos2d::CCArray::addObject(m_indicatorSprites, sprite);
        return m_orbIndicators;
    }

    if (!m_portalIndicators) return false;

    float offsetX = isPortal17 ? 17.0f : 19.0f;
    float offset2X = isPortal17 ? -2.0f : -1.5f;

    std::string frameName = ObjectToolbox::sharedState()->intKeyToFrame(objectID);

    std::string extraName = replaceAll(frameName, "front", "extra");
    cocos2d::CCSprite* mainSprite = object->addCustomChild(extraName, cocos2d::CCPoint(0.0f, 0.0f), 100);
    mainSprite->setPosition(cocos2d::CCPoint(
        offsetX + object->m_obRect.size.width * 0.5f,
        object->m_obRect.size.height * 0.5f));
    mainSprite->setTag(91);

    std::string extra2Name = replaceAll(frameName, "front", "extra_2");
    cocos2d::CCSprite* extraSprite = object->addCustomChild(extra2Name, cocos2d::CCPoint(0.0f, 0.0f), 101);
    cocos2d::CCPoint mainPos = mainSprite->getPosition();
    extraSprite->setPosition(cocos2d::CCPoint(mainPos.x + offset2X, mainPos.y));

    cocos2d::CCArray::addObject(m_indicatorSprites, extraSprite);
    return m_portalIndicators;
}

void GJBaseGameLayer::addToSection(GameObject* object) {
    int sectionX = (object->m_positionX <= 0.0f) ? 0
        : (object->m_positionX >= 10000000.0f ? (int)(m_sectionXFactor * 10000000.0f) : (int)(object->m_positionX * m_sectionXFactor));
    int sectionY = (object->m_positionY <= 0.0f) ? 0
        : (object->m_positionY >= 10000000.0f ? (int)(m_sectionYFactor * 10000000.0f) : (int)(object->m_positionY * m_sectionYFactor));

    if (sectionX >= (int)m_sections.size()) {
        m_sections.resize(sectionX + 1);
        m_sectionSizes.resize(sectionX + 1);
    }
    if (!m_sections[sectionX]) {
        m_sections[sectionX] = new std::vector<std::vector<GameObject*>*>();
        m_sectionSizes[sectionX] = new std::vector<int>();
    }
    auto* inner = m_sections[sectionX];
    if (sectionY >= (int)inner->size()) {
        inner->resize(sectionY + 1);
        m_sectionSizes[sectionX]->resize(sectionY + 1);
    }
    if (!(*inner)[sectionY]) (*inner)[sectionY] = new std::vector<GameObject*>();

    auto* targetVec = (*inner)[sectionY];
    int idx = (*m_sectionSizes[sectionX])[sectionY]++;
    if ((size_t)idx < targetVec->size()) (*targetVec)[idx] = object;
    else targetVec->push_back(object);

    object->m_someOtherIndex = idx;
    object->m_outerSectionIndex = sectionX;
    object->m_middleSectionIndex = sectionY;

    if (object->m_objectID == 1816) {
        if (object->m_hasExtendedCollision) {
            int oldSize = m_calcCollisionBlockObjectsSize;
            if ((size_t)oldSize < m_calcCollisionBlockObjects.size()) m_calcCollisionBlockObjects[oldSize] = object;
            else m_calcCollisionBlockObjects.push_back(object);
            object->m_innerSectionIndex = oldSize;
            m_calcCollisionBlockObjectsSize = oldSize + 1;
            return;
        }
        if (sectionX >= (int)m_collisionBlockSections.size()) m_collisionBlockSections.resize(sectionX + 1);
        m_collisionBlockSectionSizes.resize(sectionX + 1);
        if (!m_collisionBlockSections[sectionX]) {
            m_collisionBlockSections[sectionX] = new std::vector<GameObject*>();
            m_collisionBlockSectionSizes[sectionX] = new std::vector<int>();
        }
        auto* cbInner = m_collisionBlockSections[sectionX];
        if (sectionY >= (int)cbInner->size()) cbInner->resize(sectionY + 1);
        m_collisionBlockSectionSizes[sectionX]->resize(sectionY + 1);
        if (!(*cbInner)[sectionY]) (*cbInner)[sectionY] = (GameObject*)new std::vector<GameObject*>();
        auto* cbVec = (std::vector<GameObject*>*)(*cbInner)[sectionY];
        int cbIdx = (*m_collisionBlockSectionSizes[sectionX])[sectionY]++;
        if ((size_t)cbIdx < cbVec->size()) (*cbVec)[cbIdx] = object;
        else cbVec->push_back(object);
        object->m_innerSectionIndex = cbIdx;
        return;
    }

    if (object->m_objectType == Decoration) return;

    if (object->m_hasExtendedCollision) {
        int oldSize = m_calcNonEffectObjectsSize;
        if ((size_t)oldSize < m_calcNonEffectObjects.size()) m_calcNonEffectObjects[oldSize] = object;
        else m_calcNonEffectObjects.push_back(object);
        object->m_innerSectionIndex = oldSize;
        m_calcNonEffectObjectsSize = oldSize + 1;
        return;
    }

    if (sectionX >= (int)m_nonEffectObjects.size()) {
        m_nonEffectObjects.resize(sectionX + 1);
        m_nonEffectObjectsSizes.resize(sectionX + 1);
        m_nonEffectObjectsFlags.resize(sectionX + 1);
    }
    if (!m_nonEffectObjects[sectionX]) {
        m_nonEffectObjects[sectionX] = new std::vector<std::vector<GameObject*>*>();
        m_nonEffectObjectsSizes[sectionX] = new std::vector<int>();
        m_nonEffectObjectsFlags[sectionX] = new std::vector<bool>();
    }
    auto* neInner = m_nonEffectObjects[sectionX];
    if (sectionY >= (int)neInner->size()) neInner->resize(sectionY + 1);
    m_nonEffectObjectsSizes[sectionX]->resize(sectionY + 1);
    m_nonEffectObjectsFlags[sectionX]->resize(sectionY + 1, false);
    if (!(*neInner)[sectionY]) (*neInner)[sectionY] = new std::vector<GameObject*>();

    auto* neVec = (*neInner)[sectionY];
    int neIdx = (*m_nonEffectObjectsSizes[sectionX])[sectionY]++;
    if ((size_t)neIdx < neVec->size()) (*neVec)[neIdx] = object;
    else neVec->push_back(object);
    (*m_nonEffectObjectsFlags[sectionX])[sectionY] = true;
    object->m_innerSectionIndex = neIdx;
}

void GJBaseGameLayer::activateEventTrigger(EventLinkTrigger* object, std::vector<int>* remapKeys) {
    int eventKey = getEventKey(object->m_extraID, object->m_extraID2);
    for (int eventID : object->m_eventIDs) {
        auto key = std::make_pair((GJGameEvent)eventID, eventKey);
        auto& map = m_gameState.m_unkMapPairGJGameEventIntVectorEventTriggerInstance;
        if (object->m_resetRemap) {
            auto it = map.find(key);
            if (it != map.end()) {
                auto& vec = it->second;
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [&](const EventTriggerInstance& e) {
                        return e.m_targetGroupID == object->m_targetGroupID;
                    }), vec.end());
            }
        } else {
            auto& vec = map[key];
            EventTriggerInstance instance{};
            instance.m_targetGroupID = object->m_targetGroupID;
            instance.m_uniqueID = object->m_uniqueID;
            instance.m_controlID = object->m_controlID;
            instance.m_reset = false;
            instance.m_remapKeys = *remapKeys;
            vec.push_back(std::move(instance));
        }
    }
}

void GJBaseGameLayer::activateItemCompareTrigger(ItemTriggerGameObject* object, std::vector<int>* remapKeys) {
    int resultType1 = object->m_resultType1;
    int resultType2 = object->m_resultType2;
    int resultType3 = object->m_resultType3;
    if (resultType1 <= 0) resultType1 = 3;
    if (resultType2 <= 0) resultType2 = 3;
    int item1Mode = object->m_item1Mode;
    if (item1Mode < 1) item1Mode = 1;
    int item2Mode = object->m_item2Mode;
    int itemID2 = object->m_itemID2;

    float mod1 = roundf(object->m_mod1 * 1000.0f) / 1000.0f;
    float tolerance = roundf(object->m_tolerance * 1000.0f) / 1000.0f;
    float mod2 = roundf(object->m_mod2 * 1000.0f) / 1000.0f;

    bool useItem2 = item2Mode && (itemID2 > 0 || (unsigned)(item2Mode - 3) <= 1);

    double lhs = getItemValue(item1Mode, object->m_itemID);
    lhs = performMathOperation(lhs, mod1, resultType1);
    if (object->m_roundType1) lhs = performMathRounding(lhs, object->m_roundType1);
    if (object->m_signType1) lhs = performMathSign(lhs, object->m_signType1);

    double rhs = mod2;
    if (useItem2) {
        double item2Value = getItemValue(item2Mode, itemID2);
        rhs = performMathOperation(item2Value, mod2, resultType2);
    }
    if (object->m_roundType2) rhs = performMathRounding(rhs, object->m_roundType2);
    if (object->m_signType2) rhs = performMathSign(rhs, object->m_signType2);

    int targetGroupID = object->m_targetGroupID;
    int centerGroupID = object->m_centerGroupID;
    bool activate = false;
    switch (resultType3) {
        case 0: activate = fabs(lhs - rhs) <= tolerance; break;
        case 1: activate = lhs + tolerance >  rhs; break;
        case 2: activate = lhs + tolerance >= rhs; break;
        case 3: activate = lhs - tolerance <  rhs; break;
        case 4: activate = lhs - tolerance <= rhs; break;
        case 5: activate = fabs(lhs - rhs) >  tolerance; break;
    }
    if (activate) centerGroupID = targetGroupID;
    spawnGroup(centerGroupID, false, 0.0, remapKeys, object->m_uniqueID, object->m_controlID);
}

void GJBaseGameLayer::activateTimerTrigger(TimerTriggerGameObject* object, std::vector<int>* remapKeys) {
    switch (object->m_objectID) {
        case 3615:
            GJEffectManager::runTimerTrigger(m_effectManager, object->m_itemID, object->m_targetTime,
                object->m_multiActivate, object->m_targetGroupID, remapKeys,
                object->m_uniqueID, object->m_controlID);
            break;
        case 3617:
            if (object->m_controlType == 0) {
                GJEffectManager::resumeTimer(m_effectManager, object->m_itemID);
            } else if (object->m_controlType == 1) {
                GJEffectManager::pauseTimer(m_effectManager, object->m_itemID);
            }
            break;
        case 3614:
            GJEffectManager::startTimer(m_effectManager, object->m_itemID, object->m_startTime,
                object->m_targetTime, object->m_stopTimeEnabled, !object->m_startPaused,
                object->m_dontOverride, object->m_timeMod, object->m_ignoreTimeWarp,
                object->m_targetGroupID, remapKeys, object->m_uniqueID, object->m_controlID);
            break;
    }
}

void GJBaseGameLayer::activatedAudioTrigger(SFXTriggerGameObject* object) {
    activatedAudioTrigger(object, m_gameState.m_levelTime);
}

void GJBaseGameLayer::activatedAudioTrigger(SFXTriggerGameObject* object, float time) {
}

void GJBaseGameLayer::addAreaEffect(EnterEffectObject* object, std::vector<EnterEffectInstance>* instances, GJAreaActionType type) {
    for (auto& instance : *instances) {
        if (instance.m_gameObject == object
            && instance.m_targetID == object->m_targetGroupID
            && instance.m_centerID == object->m_centerGroupID
            && instance.m_controlID == object->m_controlID) {
            EnterEffectInstance::loadValuesFromObject(&instance, object);
            return;
        }
    }
    int effectID = object->m_effectID;
    if (effectID > 0) {
        for (auto it = instances->begin(); it != instances->end(); ++it) {
            if (it->m_gameObject->m_effectID == effectID) {
                instances->erase(it);
                break;
            }
        }
    }
    int specialKey = getSpecialKey(object->m_targetGroupID, object->m_ignoreGroupParent, object->m_ignoreLinkedObjects);
    int targetGroupIndex = 0;
    int controlIDCount = 0;
    auto it = m_targetGroups.find(specialKey);
    if (it != m_targetGroups.end()) {
        targetGroupIndex = it->second.first;
        controlIDCount = it->second.second;
    }
    int unkUint7 = m_gameState.m_unkUint7 + 1;
    m_gameState.m_unkUint7 = unkUint7;

    EnterEffectInstance instance{};
    instance.m_gameObject = object;
    instance.m_targetID = object->m_targetGroupID;
    instance.m_centerID = object->m_centerGroupID;
    instance.m_unkFloat3 = (float)unkUint7;
    instance.m_targetGroupIndex = targetGroupIndex;
    instance.m_controlID = object->m_controlID;
    if (controlIDCount > 0) instance.m_easeIndices.resize(controlIDCount);
    EnterEffectInstance::loadValuesFromObject(&instance, object);
    instances->push_back(std::move(instance));

    m_gameState.m_unsortedAreaEffects.insert((int)type);
}

void GJBaseGameLayer::addProximityVolumeEffect(int channelID, int targetType, SFXTriggerGameObject* object) {
    std::pair<int,int> key(channelID, targetType);
    m_gameState.m_proximityVolumeRelated.erase(key);

    SFXTriggerInstance instance{};
    instance.m_groupID1 = object->m_targetGroupID;
    instance.m_groupID2 = object->m_centerGroupID;
    instance.m_controlID = object->m_controlID;
    instance.m_sfxTriggerGameObject = object;

    m_gameState.m_proximityVolumeRelated[key] = instance;

    float volumeMod = volumeForProximityEffect(instance);
    FMODAudioEngine::sharedEngine()->setChannelVolumeMod(channelID, (AudioTargetType)targetType, volumeMod);
}

void GJBaseGameLayer::animateInDualGroundNew(GameObject* object, float height, bool instant, float duration) {
    float targetY = getTargetFlyCameraY(object);
    float halfHeight = height * 0.5f;
    float v = targetY - halfHeight;
    if (!m_gameState.m_unkBool9) v = floorf(v / 30.0f) * 30.0f;
    if (v <= 90.0f) v = 90.0f;
    m_gameState.m_unkInt14 = v + halfHeight;

    float tweenRate = 0.0f;
    bool instantHandled = false;

    if (!m_gameState.m_isDualMode) {
        if (!m_gameState.m_unkBool7) {
            if (instant) instantHandled = true;
            else tweenRate = 0.5f;
        } else {
            if (instant) instantHandled = true;
            else { tweenRate = 0.4f; if (height == *(float*)&m_gameState.m_unkInt11) tweenRate = 0.0f; }
        }
    } else {
        if (instant) instantHandled = true;
        else if (m_gameState.m_unkBool7 && height == *(float*)&m_gameState.m_unkInt11) tweenRate = 0.0f;
        else tweenRate = 0.4f;
    }

    if (instantHandled) {
        GJGameState::stopTweenAction(&m_gameState, 25);
        m_gameState.m_unkFloat9 = 1.0f;
    } else if (tweenRate > 0.0f) {
        float cur = m_gameState.m_unkFloat9;
        if (cur >= 0.0f) {
            if (cur > 0.0f) {
                cocos2d::CCSize ws = cocos2d::CCDirector::sharedDirector()->getWinSize();
                float numerator = (ws.height * 0.5f + 1.0f - *(float*)&m_gameState.m_unkInt11 * 0.5f) * cur;
                float denom = ws.height * 0.5f + 1.0f - halfHeight;
                m_gameState.m_unkFloat9 = numerator / denom;
            }
        } else {
            m_gameState.m_unkFloat9 = 0.0f;
        }
        GJGameState::tweenValue(&m_gameState, m_gameState.m_unkFloat9, 1.0f, 25, tweenRate, 1, 2.0f, -1, -1);
    }

    if (m_gameState.m_unkPoint17.y == 0.0f || m_gameState.m_unkBool1) {
        cocos2d::CCPoint pos(0.0f, *(float*)&m_gameState.m_unkInt14);
        updateStaticCameraPos(pos, true, false, tweenRate != 0.0f, 1, 2.0f, tweenRate);
        m_gameState.m_unkBool1 = true;
    }
    m_gameState.m_unkBool7 = true;
    m_gameState.m_unkInt11 = *(int*)&height;
    m_gameState.m_unkInt10 = (int)targetY;
}

void GJBaseGameLayer::applySFXEditTrigger(int channelID, int targetType, SFXTriggerGameObject* object) {
    if (!(m_practiceMusicSync || !m_isPracticeMode || m_isPlatformer)) return;

    if (object->m_stop || object->m_stopLoop) {
        FMODAudioEngine::sharedEngine()->stopChannel(channelID, (AudioTargetType)targetType, object->m_stopLoop, 0.0f);
    }
    if (object->m_changeVolume) {
        float pitch = object->m_pitch;
        if (pitch <= 0.0f) pitch = FMODAudioEngine::pitchForIdx(object->m_speed);
        FMODAudioEngine::sharedEngine()->updateChannel(channelID, (AudioTargetType)targetType, Pitch, object->m_duration, pitch);
    }
    if (object->m_changeSpeed) {
        FMODAudioEngine::sharedEngine()->updateChannel(channelID, (AudioTargetType)targetType, Volume, object->m_duration, object->m_volume);
    }
}

void GJBaseGameLayer::applyRemap(EffectGameObject* object, std::vector<int>* remapKeys, std::unordered_map<int,int>* remap) {
    if (remapKeys->size() <= 1) return;

    for (size_t i = 0; i + 1 < remapKeys->size(); ++i) {
        auto& trigger = m_spawnRemapTriggers[(*remapKeys)[i]];

        auto processInt = [&](int& field, int sentinel) {
            auto it = trigger.find(field);
            if (it == trigger.end()) return;
            if (remap->find(sentinel) == remap->end()) (*remap)[sentinel] = field;
            field = it->second;
        };
        auto processFloat = [&](float& field, int sentinel) {
            int bits = *reinterpret_cast<int*>(&field);
            auto it = trigger.find(bits);
            if (it == trigger.end()) return;
            if (remap->find(sentinel) == remap->end()) (*remap)[sentinel] = bits;
            int newBits = it->second;
            field = *reinterpret_cast<float*>(&newBits);
        };

        processInt(object->m_targetGroupID, 51);
        processInt(object->m_centerGroupID, 71);
        processInt(object->m_itemID, 80);
        processInt(object->m_itemID2, 95);
        processInt(object->m_controlID, 534);
        processInt(object->m_animationID, 76);

        int id = object->m_objectID;
        if (id > 3021) {
            if (id == 3604) {
                processInt(object->m_eObjType, 447);
                processInt(object->m_uIndexInArray, 525);
            } else if (id == 3607) {
                ChanceTriggerGameObject::remapChanceObjects((ChanceTriggerGameObject*)object, trigger);
            } else if (id == 3605) {
                processInt(*(int*)&object->m_sTransform.a, 392);
                processFloat(object->m_sTransform.c, 455);
                processFloat(object->m_sTransform.d, 457);
                processInt(object->m_uOrderOfArrival, 416);
                processFloat(object->m_sAdditionalTransform.ty, 432);
            } else if ((unsigned)(id - 3660) <= 1) {
                processInt(*(int*)&object->m_bRunning, 366);
                processInt(object->m_nTag, 560);
                processInt(object->m_eObjType, 565);
            }
        } else if (id >= 3017 || id == 3023) {
            processFloat(object->m_sTransform.d, 225);
            processInt((int&)object->m_pGrid, 344);
            processInt(object->m_enterChannel, 343);
        } else if (id == 2068) {
            ChanceTriggerGameObject::remapChanceObjects((ChanceTriggerGameObject*)object, trigger);
        } else if (id == 2903) {
            processInt(object->m_nLuaID, 203);
            processInt(object->m_nTag, 204);
            processInt(object->m_uReference, 205);
            processInt(object->m_uAutoReleaseCount, 206);
            processInt(object->m_uID, 209);
        } else if (id == 901) {
            processInt(object->m_targetModCenterID, 395);
        } else if (id == 1346) {
            processInt(object->m_rotationTargetID, 401);
            processInt(*(int*)((char*)&object->m_canSpawnTriggers + 1), 516);
            processInt(object->m_uID, 517);
            processInt(object->m_nLuaID, 518);
            processInt(object->m_uReference, 519);
        } else if (id == 1934) {
            processInt(object->m_uID, 392);
            processFloat(object->m_sTransform.c, 455);
            processFloat(object->m_sTransform.d, 457);
            processInt(object->m_uOrderOfArrival, 416);
            processFloat(object->m_sAdditionalTransform.ty, 432);
        } else if (id == 3016 || id == 3006) {
            processInt(*(int*)&object->m_bRunning, 366);
            processInt(object->m_nTag, 560);
            processInt(object->m_eObjType, 565);
        }
    }
}

void GJBaseGameLayer::bumpPlayer(PlayerObject* player, EffectGameObject* object) {
    if (!canBeActivatedByPlayer(player, object)) return;

    cocos2d::CCPoint objectPos = object->getPosition();
    player->m_lastPortalPos = cocos2d::CCPoint(objectPos.x, objectPos.y - 10.0f);
    object->activatedByPlayer(player);

    GameObjectType type = object->getType();
    float bumpMod = getBumpMod(player, type);
    player->m_lastActivatedPortal = object;
    if (object->m_isReverse) PlayerObject::reversePlayer(player, object);
    PlayerObject::bumpPlayer(player, bumpMod, object->getType(), object->m_hasNoEffects, object);

    GJGameEvent event = objectTypeToGameEvent(object->getType());
    gameEventTriggered(event, 0, 0);
    gameEventTriggered(PadActivated, 0, 0);
}

int GJBaseGameLayer::buttonIDToButton(int id) {
    switch (id) {
        case 2: case 7: return 2;
        case 3: case 8: return 3;
        case 4: case 9: return 4;
        case 5: case 10: return 5;
        case 98: case 99: return id;
        default: return 1;
    }
}

bool GJBaseGameLayer::buttonIsRelevant(const PlayerButtonCommand* button) {
    if (button->m_button != Jump) return false;

    bool isPlayer2 = button->m_isPlayer2;
    int twoPlayerMode = m_levelSettings->m_twoPlayerMode;
    if (!m_isPlatformer && !m_useReplay) {
        if (GameManager::sharedState()->getGameVariable("0010")) isPlayer2 = !isPlayer2;
    }

    int hasActiveDualTouch = GJEffectManager::hasActiveDualTouch(m_effectManager);

    if (button->m_isPush) {
        if (isPlayer2 && !m_dualTouchTrigger) return false;
        if (hasActiveDualTouch > (int)!isPlayer2) return false;
    } else if (m_dualTouchTrigger) {
        if (isPlayer2) return true;
        if (hasActiveDualTouch) return false;
    }

    bool checkP1 = !isPlayer2 ? (twoPlayerMode == 0) : (twoPlayerMode > (int)isPlayer2);
    bool checkP2 = false;

    if (!m_dualTouchTrigger && m_gameState.m_isDualMode
        && !(isPlayer2 && twoPlayerMode)) {
        if (!checkP1) return isRelevantForPlayer(m_player2, button);
        checkP2 = true;
    } else {
        if (!checkP1) return false;
        checkP2 = false;
    }

    if (isRelevantForPlayer(m_player1, button)) return true;
    if (!checkP2) return false;
    return isRelevantForPlayer(m_player2, button);
}

bool GJBaseGameLayer::canBeActivatedByPlayer(PlayerObject* player, EffectGameObject* object) {
    if (object->canMultiActivate(player->m_isPlatformer) && playerWasTouchingObject(player, object)) {
        playerTouchedObject(player, object);
        return false;
    }
    playerTouchedObject(player, object);
    return !object->hasBeenActivatedByPlayer(player);
}

bool GJBaseGameLayer::canTouchObject(GameObject* object) {
    if (!m_gameState.m_currentChannel) return true;
    if (object->m_classType != Effect) return true;
    int channel = object->m_currentChannel;  // offset m_sQuad.tr.vertices.y, ver nota abajo
    return channel == 0 || channel == m_gameState.m_currentChannel;
}

void GJBaseGameLayer::checkCameraLimitAfterTeleport(PlayerObject* player, float yOffset) {
    if (m_gameState.m_unkPoint17.y != 0.0f || player->m_isSideways) return;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float playerY = player->getPosition()->y;
    float camY = m_gameState.m_cameraPosition.y;
    if (playerY > camY + winSize.height + yOffset || playerY < camY - yOffset) {
        m_gameState.m_unkBool29 = true;
        updateCamera(0.0f);
    }
}

bool GJBaseGameLayer::checkCollision(int blockAID, int blockBID) {
    return GJEffectManager::checkCollision(m_effectManager, &blockAID, &blockBID);
}

int GJBaseGameLayer::checkCollisions(PlayerObject* player, float dt, bool ignoreDamage) {
    bool wasTeleported = player->m_wasTeleported;
    player->m_wasTeleported = false;
    PlayerObject::preCollision(player);

    float vehicleSize = player->m_vehicleSize;
    float angle1 = player->m_unkAngle1;
    float offset = 0.0f;
    if (vehicleSize != 1.0f) offset = angle1 * (1.0f - vehicleSize) * 0.5f;
    float halfAngle = angle1 * 0.5f;
    float groundY = (halfAngle + 90.0f) - offset;

    bool isFlyingMode = false;
    if (!m_gameState.m_isFreeMode) {
        isFlyingMode = PlayerObject::isFlying(player) || player->m_isBall || player->m_isSpider || m_gameState.m_isDualMode;
    }

    bool wasOutOfBounds = player->m_isOutOfBounds;
    player->m_isOutOfBounds = false;
    bool isOutOfBounds = false;

    if (m_isPlatformer) {
        if (player->getPosition()->x < -30.0f) {
            player->m_platformerXVelocity = 0.0f;
            player->setPosition(cocos2d::CCPoint(-30.0f, player->getPosition()->y));
        }
    } else if (player->m_isGoingLeft) {
        isOutOfBounds = player->getPosition()->x < -30.0f;
    }

    if (player->getPosition()->y < groundY
        && (!isFlyingMode || (!PlayerObject::isFlying(player) && !player->m_isBall && !m_gameState.m_isDualMode && !player->m_isSpider))) {
        if (player->m_isUpsideDown) {
            if (PlayerObject::isSafeFlip(player, 0.1f)) {
                player->setPosition(cocos2d::CCPoint(player->getPosition()->x, groundY));
                PlayerObject::hitGround(player, nullptr, true);
                PlayerObject::updateCollide(player, (PlayerCollisionDirection)0, nullptr);
                player->m_isOnGround2 = false;
                return 0;
            }
            isOutOfBounds = true;
        } else if (!player->m_maybeIsBoosted) {
            player->setPosition(cocos2d::CCPoint(player->getPosition()->x, groundY));
            PlayerObject::hitGround(player, nullptr, false);
            PlayerObject::updateCollide(player, (PlayerCollisionDirection)1, nullptr);
        }
    } else if (player->getPosition()->y > offset + m_maxGameplayY) {
        isOutOfBounds = true;
    }

    bool shouldDestroy = false;

    if (isFlyingMode && !wasTeleported) {
        bool exitPortal = false;

        float maxPortalY = getMaxPortalY();
        float minPortalY = getMinPortalY();
        float rangeY = maxPortalY - m_gameState.m_unkUint17;
        bool jumpBuffered = player->m_isShip && player->m_jumpBuffered;
        float topY = (maxPortalY - halfAngle) + offset;

        if (player->getPosition()->y > topY) {
            if (!player->m_wasOnSlope || player->getPosition()->y <= topY + halfAngle) {
                bool pushed = false;
                if (PlayerObject::destroyFromHitHead(player) && !player->m_isUpsideDown) {
                    if (!PlayerObject::isSafeHeadTest(player)) {
                        if (wasOutOfBounds) {
                            shouldDestroy = true;
                        } else {
                            isOutOfBounds = true;
                        }
                        exitPortal = true;
                    } else {
                        pushed = true;
                    }
                }
                if (!exitPortal) {
                    float oldYVel = PlayerObject::getYVelocity(player);
                    bool stickDown = player->m_isUpsideDown && PlayerObject::canStickToGround(player) && rangeY != 0.0f;
                    player->setPosition(cocos2d::CCPoint(player->getPosition()->x, topY));
                    if (pushed) {
                        PlayerObject::pushDown(player);
                    } else if (PlayerObject::getYVelocity(player) > 0.0f) {
                        PlayerObject::hitGround(player, nullptr, !player->m_isUpsideDown);
                    }
                    PlayerObject::updateCollide(player, (PlayerCollisionDirection)player->m_isUpsideDown, nullptr);
                    if ((stickDown && oldYVel > PlayerObject::getYVelocity(player))
                        || (!player->m_isUpsideDown && jumpBuffered && rangeY > 0.0f)) {
                        PlayerObject::setYVelocity(player, oldYVel, false);
                    }
                    player->m_wasOnSlope = false;
                }
            } else {
                if (wasOutOfBounds) {
                    shouldDestroy = true;
                } else {
                    isOutOfBounds = true;
                }
                exitPortal = true;
            }
        } else {
            float bottomY = (minPortalY + halfAngle) - offset;
            if (player->getPosition()->y < bottomY) {
                if (player->m_wasOnSlope && player->getPosition()->y < bottomY - halfAngle) {
                    if (wasOutOfBounds) {
                        shouldDestroy = true;
                    } else {
                        isOutOfBounds = true;
                    }
                    exitPortal = true;
                }
                if (!exitPortal) {
                    bool safe = false;
                    if (PlayerObject::destroyFromHitHead(player)) {
                        if (player->m_isUpsideDown) {
                            if (!PlayerObject::isSafeHeadTest(player)) {
                                if (wasOutOfBounds) {
                                    shouldDestroy = true;
                                } else {
                                    isOutOfBounds = true;
                                }
                                exitPortal = true;
                            } else {
                                safe = true;
                            }
                        }
                    }
                    if (!exitPortal) {
                        float oldYVel = PlayerObject::getYVelocity(player);
                        bool stickDown = !player->m_isUpsideDown && PlayerObject::canStickToGround(player) && rangeY != 0.0f;
                        float yOffset = (safe || PlayerObject::canStickToGround(player)
                            || m_gameState.m_cameraZoom == m_gameState.m_targetCameraZoom) ? 0.0f : 1.0f;
                        player->setPosition(cocos2d::CCPoint(player->getPosition()->x, bottomY + yOffset));
                        if (safe) {
                            PlayerObject::pushDown(player);
                        } else if ((!player->m_isShip && !player->m_isSwing) || PlayerObject::getYVelocity(player) < 0.0f) {
                            PlayerObject::hitGround(player, nullptr, player->m_isUpsideDown);
                        }
                        PlayerObject::updateCollide(player, (PlayerCollisionDirection)!player->m_isUpsideDown, nullptr);
                        if ((stickDown && oldYVel < PlayerObject::getYVelocity(player))
                            || (player->m_isUpsideDown && jumpBuffered && rangeY < 0.0f)) {
                            PlayerObject::setYVelocity(player, oldYVel, false);
                        }
                        player->m_wasOnSlope = false;
                    }
                }
            }
        }
    }

    if (shouldDestroy) {
        if (!ignoreDamage) destroyPlayer(player, nullptr);
        return 1;
    }

    if (wasOutOfBounds && isOutOfBounds) {
        if (!ignoreDamage) destroyPlayer(player, nullptr);
        return 1;
    }

    player->m_isOutOfBounds = isOutOfBounds;

    int outerMin = (player->getPosition()->x > 0.0f)
        ? (player->getPosition()->x < 10000000.0f ? (int)(player->getPosition()->x * m_sectionXFactor) : (int)(m_sectionXFactor * 10000000.0f))
        : 0;
    int outerMax = (player->getPosition()->y > 0.0f)
        ? (player->getPosition()->y < 10000000.0f ? (int)(player->getPosition()->y * m_sectionYFactor) : (int)(m_sectionYFactor * 10000000.0f))
        : 0;

    int xStart = outerMin > 1 ? outerMin - 1 : 0;
    int xEnd = (outerMin + 1);
    if (xEnd >= (int)m_nonEffectObjects.size() - 1) xEnd = (int)m_nonEffectObjects.size() - 1;
    int yStart = outerMax > 1 ? outerMax - 1 : 0;
    int yEnd = outerMax + 1;

    m_solidCollisionObjectsCount = 0;
    m_hazardCollisionObjectsCount = 0;

    for (int sx = xStart; sx <= xEnd; ++sx) {
        auto* xVec = m_nonEffectObjects[sx];
        if (!xVec) continue;
        int maxY = (int)xVec->size() - 1;
        if (yEnd < maxY) maxY = yEnd;
        for (int sy = yStart; sy <= maxY; ++sy) {
            auto* vec = (*xVec)[sy];
            if (!vec) continue;
            int count = (*m_nonEffectObjectsSizes[sx])[sy];
            if (((*m_nonEffectObjectsFlags[sx])[sy])) {
                std::sort(vec->begin(), vec->end(), [](GameObject* a, GameObject* b) {
                    return a->m_uniqueID < b->m_uniqueID;
                });
                for (int i = 0; i < count; ++i) (*vec)[i]->m_innerSectionIndex = i;
                (*m_nonEffectObjectsFlags[sx])[sy] = false;
            }
            collisionCheckObjects(player, vec, count, dt);
        }
    }

    if (m_calcNonEffectObjectsSize > 0) {
        collisionCheckObjects(player, &m_calcNonEffectObjects, m_calcNonEffectObjectsSize, dt);
    }

    bool wasCollidingWithSlope = player->m_lastGroundObject ? player->m_isCollidingWithSlope : false;
    cocos2d::CCRect playerRect = *player->getObjectRect();
    float maxX = playerRect.getMaxX();
    float minX = playerRect.getMinX();
    float maxY = playerRect.getMaxY();
    float minY = playerRect.getMinY();

    for (int i = 0; i < m_solidCollisionObjectsCount; ++i) {
        GameObject* obj = m_solidCollisionObjects[m_solidCollisionObjectsCount - 1 - i];
        if (obj->getGroupDisabled()) continue;
        cocos2d::CCRect* objRect = obj->getObjectRectPointer();
        if (player->m_lastGroundObject && obj->m_groupCount > 0) {
            wasCollidingWithSlope = !PlayerObject::testForMoving(player, dt, obj);
        }
        if (objRect->getMinX() <= maxX && objRect->getMaxX() >= minX
            && objRect->getMinY() <= maxY && objRect->getMaxY() >= minY) {
            PlayerObject::collidedWithObject(player, dt, obj);
            playerRect = *player->getObjectRect();
            maxX = playerRect.getMaxX();
            minX = playerRect.getMinX();
            maxY = playerRect.getMaxY();
            minY = playerRect.getMinY();
        }
    }

    if (m_player1->m_isDead) PlayerObject::removePlacedCheckpoint(m_player1);

    playerRect = *player->getObjectRect();
    maxX = playerRect.getMaxX();
    minX = playerRect.getMinX();
    maxY = playerRect.getMaxY();
    minY = playerRect.getMinY();
    player->updateOrientedBox();

    for (int i = 0; i < m_hazardCollisionObjectsCount; ++i) {
        GameObject* obj = m_hazardCollisionObjects[i];
        if (obj->getGroupDisabled()) continue;
        cocos2d::CCRect* objRect = obj->getObjectRectPointer();

        if (obj->m_objectRadius <= 0.0f) {
            if (maxX < objRect->getMinX() || objRect->getMaxX() < minX
                || maxY < objRect->getMinY() || objRect->getMaxY() < minY) continue;
        } else if (!playerCircleCollision(player, obj)) {
            continue;
        }

        if (obj->m_shouldUseOuterOb
            && (!m_levelSettings->m_fixRadiusCollision || obj->m_objectRadius <= 0.0f)) {
            OBB2D* objOBB = obj->getOrientedBox();
            OBB2D* playerOBB = player->getOrientedBox();
            if (!OBB2D::overlaps1Way(objOBB) || !OBB2D::overlaps1Way(playerOBB)) continue;
        }

        if (!ignoreDamage) destroyPlayer(player, obj);
        return 1;
    }

    PlayerObject::postCollision(player, dt, m_isBetweenSteps);
    return 0;
}

void GJBaseGameLayer::checkForEnd() {}

void GJBaseGameLayer::checkRepellPlayer() {
    if (!m_gameState.m_isDualMode) return;
    if (!m_player1->m_isBall || !m_player2->m_isBall) return;
    if (m_levelSettings->m_twoPlayerMode) return;
    if (m_player1->m_isUpsideDown != m_player2->m_isUpsideDown) return;

    float r1 = m_player1->getScaleX() * 30.0f * 0.5f;
    float r2 = m_player2->getScaleX() * 30.0f * 0.5f;
    float y1 = m_player1->getPosition()->y;
    float y2 = m_player2->getPosition()->y;
    if (fabsf(y1 - y2) >= r1 + r2 + 5.0f) return;

    PlayerObject* flying;
    PlayerObject* grounded;
    float radius;
    if (m_player1->m_isOnGround2 && !m_player2->m_isOnGround2) {
        flying = m_player1; grounded = m_player2; radius = r1;
    } else if (m_player2->m_isOnGround2 && !m_player1->m_isOnGround2) {
        flying = m_player2; grounded = m_player1; radius = r2;
    } else {
        return;
    }

    PlayerObject::hardFlipGravity(grounded);
    if (m_isEditor) return;

    GLubyte r = grounded->m_playerColor1.r;
    GLubyte g = grounded->m_playerColor1.g;
    GLubyte b = grounded->m_playerColor1.b;
    cocos2d::CCPoint pos = grounded->getPosition();
    CCCircleWave* wave = CCCircleWave::create(0, radius + 2.0f, radius * 4.0f, 0.3f, false, false);
    wave->m_color.r = r;
    wave->m_color.g = g;
    wave->m_color.b = b;
    wave->m_lineWidth = 4;
    wave->setPosition(&pos);
    m_objectLayer->addChild(wave, 0);
    wave->m_circleMode = Outline;
    CCCircleWave::followObject(wave, grounded, false);
}

void GJBaseGameLayer::checkSnapshot() {}

bool GJBaseGameLayer::checkSpawnAbuse() {
    return m_spawnAbuse;
}

void GJBaseGameLayer::checkSpawnObjects() {
    cocos2d::CCPoint playerPos;
    if (m_player1->m_isPlatformer) {
        playerPos = posForTime(m_gameState.m_levelTime);
    } else {
        playerPos = *m_player1->getPosition();
    }

    bool anyProcessed;
    do {
        cocos2d::CCArray* orderObjects = getActiveOrderSpawnObjects();
        int channel = m_gameState.m_currentChannel;
        unsigned int idx = m_gameState.m_spawnChannelRelated0[channel];
        bool dirFlag = m_gameState.m_spawnChannelRelated1[channel];

        if (cocos2d::CCArray::count(orderObjects) <= idx) break;

        SpawnTriggerGameObject* spawn = (SpawnTriggerGameObject*)cocos2d::CCArray::objectAtIndex(orderObjects, idx);
        bool pass = false;
        bool skipTrigger = false;

        if (spawn->m_isTouchTriggered) {
            if (!spawn->m_activated) break;
            skipTrigger = true;
        } else {
            float checkValue;
            float compareValue;
            if (!m_isPlatformer && m_player1->m_isSideways) {
                compareValue = dirFlag ? spawn->m_speedStart.y : spawn->m_speedStart.x;
                checkValue = playerPos.y;
            } else {
                compareValue = dirFlag ? spawn->m_speedStart.x : spawn->m_speedStart.y;
                checkValue = m_isPlatformer ? playerPos.x : playerPos.x;
            }
            pass = dirFlag ? (compareValue >= checkValue) : (compareValue <= checkValue);
            if (!pass) break;
        }

        if (!spawn->m_isGroupDisabled && !skipTrigger) {
            if (spawn->m_objectID == 1268) {
                std::vector<int> remap;
                spawn->updateRemapKeys(&remap);
            }
            spawn->triggerObject(this, 0, nullptr);
        }
        ++m_gameState.m_spawnChannelRelated0[channel];
        anyProcessed = true;
    } while (!m_spawnAbuse && anyProcessed);

    applyTimeWarp(m_gameState.m_timeWarp);
}

void GJBaseGameLayer::checkpointActivated(CheckpointGameObject* object) {
    if (object->m_hasNoEffects) return;

    cocos2d::CCPoint realPos = object->getRealPosition();
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    float randX = randRange(realPos.x - 50.0f, realPos.x + 100.0f);
    float camY = m_gameState.m_cameraPosition.y;
    float yOffset = winSize.height + 50.0f;
    float scale = m_objectLayer->getScale();

    cocos2d::CCPoint from(randX, camY + yOffset / scale);
    cocos2d::ccColor3B color = object->m_colorSprite->getColor();
    lightningFlash(from, realPos, color, 5.0f, 0.5f, 1.0f, 0);
}

void GJBaseGameLayer::checkCollisionBlocks(EffectGameObject* object, std::vector<EffectGameObject*>* blocks, int blockCount) {
    int uniqueID = object->m_uniqueID;
    cocos2d::CCRect objRect = *object->getObjectRect();
    float maxX = objRect.getMaxX();
    float minX = objRect.getMinX();
    float maxY = objRect.getMaxY();
    float minY = objRect.getMinY();
    OBB2D* objOBB = object->getOrientedBox();

    for (int i = 0; i < blockCount; ++i) {
        EffectGameObject* block = (*blocks)[i];
        if (block->m_isGroupDisabled) continue;

        cocos2d::CCRect* blockRect = block->getObjectRectPointer();
        if (maxX < blockRect->origin.x) continue;
        if (blockRect->origin.x + blockRect->size.width < minX) continue;
        if (maxY < blockRect->origin.y) continue;
        if (blockRect->origin.y + blockRect->size.height < minY) continue;
        if (block->m_uniqueID == uniqueID) continue;

        if (object->m_shouldUseOuterOb || block->m_shouldUseOuterOb) {
            OBB2D* blockOBB = block->getOrientedBox();
            if (!OBB2D::overlaps1Way(objOBB) || !OBB2D::overlaps1Way(blockOBB)) continue;
        }
        objectsCollided(object->m_itemID, block->m_itemID);
    }
}

void GJBaseGameLayer::collisionCheckObjects(PlayerObject* player, std::vector<GameObject*>* objects, int objectCount, float dt) {
    cocos2d::CCRect playerRect = *player->getObjectRect();
    float maxX = playerRect.getMaxX();
    float minX = playerRect.getMinX();
    float maxY = playerRect.getMaxY();
    float minY = playerRect.getMinY();

    for (int i = 0; i < objectCount; ++i) {
        GameObject* obj = (*objects)[i];
        GameObjectType type = obj->m_objectType;

        if (type == Decoration || obj->m_isGroupDisabled || obj->m_isDisabled) continue;

        switch (type) {
            case Solid:
            case Breakable: {
                int idx = m_solidCollisionObjectsIndex;
                if (idx > m_solidCollisionObjectsCount) {
                    m_solidCollisionObjects[idx] = obj;
                } else {
                    m_solidCollisionObjects.push_back(obj);
                }
                m_solidCollisionObjectsIndex = idx + 1;
                ++m_solidCollisionObjectsCount;
                continue;
            }
            case Hazard:
            case AnimatedHazard: {
                int idx = m_hazardCollisionObjectsIndex;
                if (idx > m_hazardCollisionObjectsCount) {
                    m_hazardCollisionObjects[idx] = obj;
                } else {
                    m_hazardCollisionObjects.push_back(obj);
                }
                m_hazardCollisionObjectsIndex = idx + 1;
                ++m_hazardCollisionObjectsCount;
                continue;
            }
            case CollisionObject:
                continue;
            default:
                break;
        }

        if (obj->hasBeenActivatedByPlayer(player)) continue;

        cocos2d::CCRect objRect;
        if (obj->getType() == Slope) {
            objRect = *obj->getObjectRect_0(2.0f, 2.0f);
        } else {
            objRect = *obj->getObjectRect();
        }

        if (obj->m_objectRadius <= 0.0f) {
            if (objRect.getMinX() > maxX || objRect.getMaxX() < minX
                || objRect.getMinY() > maxY || objRect.getMaxY() < minY) continue;
        } else if (!playerCircleCollision(player, obj)) {
            continue;
        }

        if (obj->m_shouldUseOuterOb && (!m_levelSettings->m_fixRadiusCollision || obj->m_objectRadius <= 0.0f)) {
            OBB2D* objOBB = obj->getOrientedBox();
            player->updateOrientedBox();
            OBB2D* playerOBB = player->getOrientedBox();
            if (!OBB2D::overlaps1Way(objOBB) || !OBB2D::overlaps1Way(playerOBB)) {
                if (obj->getType() == Slope) objRect = *obj->getObjectRect();
                continue;
            }
        }

        if (obj->getType() == Slope) objRect = *obj->getObjectRect();

        if (!canTouchObject(obj)) continue;

        switch (obj->m_objectType) {
            case InverseGravityPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    if (!player->m_isUpsideDown && !obj->m_hasNoEffects) playGravityEffect(true);
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    flipGravity(player, true, obj->m_hasNoEffects ? true : obj->m_isHide);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                    gameEventTriggered(PortalGravityFlip, 0, 0);
                }
                break;
            case NormalGravityPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    if (player->m_isUpsideDown && !obj->m_hasNoEffects) playGravityEffect(false);
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    flipGravity(player, false, obj->m_hasNoEffects ? false : obj->m_isHide);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                    gameEventTriggered(PortalGravityNormal, 0, 0);
                }
                break;
            case ShipPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    playerWillSwitchMode(player, obj);
                    switchToFlyMode(player, obj, false, 5);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                }
                break;
            case CubePortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    playerWillSwitchMode(player, obj);
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    PlayerObject::switchedToMode(player, CubePortal);
                    PlayerObject::modeDidChange(player);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                }
                break;
            case YellowJumpPad:
            case PinkJumpPad:
            case RedJumpPad:
            case CustomRing|YellowJumpPad:
                bumpPlayer(player, (EffectGameObject*)obj);
                break;
            case YellowJumpPad|Hazard:
                gravBumpPlayer(player, (EffectGameObject*)obj);
                break;
            case YellowJumpRing:
            case PinkJumpRing:
            case GravityRing:
            case GreenRing:
            case DropRing:
            case RedJumpRing:
            case CustomRing:
            case DashRing:
            case GravityDashRing:
            case GravityTogglePortal|0x1:
                playerTouchedRing(player, (RingObject*)obj);
                break;
            case InverseMirrorPortal:
                if (!m_isEditor && canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    toggleFlipped(true, obj->m_hasNoEffects);
                    obj->playShineEffect();
                    obj->triggerActivated(0.0f);
                    gameEventTriggered(PortalFlip, 0, 0);
                }
                break;
            case NormalMirrorPortal:
                if (!m_isEditor && canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    toggleFlipped(false, obj->m_hasNoEffects);
                    obj->playShineEffect();
                    obj->triggerActivated(0.0f);
                    gameEventTriggered(PortalUnFlip, 0, 0);
                }
                break;
            case BallPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    playerWillSwitchMode(player, obj);
                    switchToRollMode(player, obj, false);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                }
                break;
            case RegularSizePortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    PlayerObject::togglePlayerScale(player, false, obj->m_hasNoEffects);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                    gameEventTriggered(PortalNormalScale, 0, 0);
                }
                break;
            case MiniSizePortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    PlayerObject::togglePlayerScale(player, true, obj->m_hasNoEffects);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                    gameEventTriggered(PortalMiniScale, 0, 0);
                }
                break;
            case UfoPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    playerWillSwitchMode(player, obj);
                    switchToFlyMode(player, obj, false, 19);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                }
                break;
            case Modifier:
            case EnterEffectObject:
                playerTouchedTrigger(player, (EffectGameObject*)obj);
                break;
            case SecretCoin:
                if (!m_isPracticeMode) {
                    obj->triggerObject(this, player->m_uniqueID, nullptr);
                    obj->triggerActivated(0.0f);
                    destroyObject(obj);
                    if (!hasUniqueCoin((EffectGameObject*)obj)) pickupItem((EffectGameObject*)obj);
                    gameEventTriggered(UserCoin, 0, 0);
                }
                break;
            case DualPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    m_player2->m_isBeingSpawnedByDualPortal = true;
                    toggleDualMode(obj, true, player, false);
                    m_player2->m_isBeingSpawnedByDualPortal = false;
                    obj->playShineEffect();
                    obj->triggerActivated(0.0f);
                    gameEventTriggered(PortalDualOn, 0, 0);
                }
                break;
            case SoloPortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    toggleDualMode(obj, false, player, false);
                    obj->playShineEffect();
                    obj->triggerActivated(0.0f);
                    gameEventTriggered(PortalDualOff, 0, 0);
                }
                break;
            case Slope:
                PlayerObject::collidedWithSlope(player, dt, obj, false);
                playerRect = *player->getObjectRect();
                maxX = playerRect.getMaxX();
                minX = playerRect.getMinX();
                maxY = playerRect.getMaxY();
                minY = playerRect.getMinY();
                break;
            case WavePortal:
                if (m_isPlatformer) break;
                if (!canBeActivatedByPlayer(player, (EffectGameObject*)obj)) break;
                playerWillSwitchMode(player, obj);
                switchToFlyMode(player, obj, false, 26);
                obj->playShineEffect();
                obj->activatedByPlayer(player);
                break;
            case RobotPortal:
                if (!canBeActivatedByPlayer(player, (EffectGameObject*)obj)) break;
                playerWillSwitchMode(player, obj);
                switchToRobotMode(player, obj, false);
                obj->playShineEffect();
                obj->activatedByPlayer(player);
                break;
            case TeleportPortal:
                if (!canBeActivatedByPlayer(player, (EffectGameObject*)obj)) break;
                gameEventTriggered(PortalTeleport, 0, 0);
                teleportPlayer((TeleportPortalObject*)obj, player);
                playerRect = *player->getObjectRect();
                maxX = playerRect.getMaxX();
                minX = playerRect.getMinX();
                maxY = playerRect.getMaxY();
                minY = playerRect.getMinY();
                break;
            case Collectible:
                if (!GJEffectManager::hasBeenTriggered(m_effectManager, obj->m_uniqueID, 0)) {
                    GJEffectManager::storeTriggeredID(m_effectManager, obj->m_uniqueID, 0);
                    if (obj->m_objectID == 1268) {
                        std::vector<int> remap;
                        ((SpawnTriggerGameObject*)obj)->updateRemapKeys(&remap);
                    }
                    obj->triggerObject(this, player->m_uniqueID, nullptr);
                    gameEventTriggered(PickupItem, 0, 0);
                }
                obj->triggerActivated(0.0f);
                destroyObject(obj);
                break;
            case Collectible|0x1:
                if (m_isPracticeMode) break;
                if (!GJEffectManager::hasBeenTriggered(m_effectManager, obj->m_uniqueID, 0)) {
                    GJEffectManager::storeTriggeredID(m_effectManager, obj->m_uniqueID, 0);
                    obj->triggerObject(this, player->m_uniqueID, nullptr);
                    if (!hasUniqueCoin((EffectGameObject*)obj)) pickupItem((EffectGameObject*)obj);
                    gameEventTriggered(UserCoin, 0, 0);
                }
                obj->triggerActivated(0.0f);
                destroyObject(obj);
                break;
            case SpiderPortal:
                if (!canBeActivatedByPlayer(player, (EffectGameObject*)obj)) break;
                playerWillSwitchMode(player, obj);
                switchToSpiderMode(player, obj, false);
                obj->playShineEffect();
                obj->activatedByPlayer(player);
                break;
            case DropRing|YellowJumpPad:
                PlayerObject::touchedObject(player, obj);
                break;
            case SwingPortal:
                if (m_isPlatformer) break;
                if (!canBeActivatedByPlayer(player, (EffectGameObject*)obj)) break;
                playerWillSwitchMode(player, obj);
                switchToFlyMode(player, obj, false, 41);
                obj->playShineEffect();
                obj->activatedByPlayer(player);
                break;
            case GravityTogglePortal:
                if (canBeActivatedByPlayer(player, (EffectGameObject*)obj)) {
                    if (!obj->m_hasNoEffects) playGravityEffect(!player->m_isUpsideDown);
                    player->m_lastPortalPos = *obj->getPosition();
                    player->m_lastActivatedPortal = obj;
                    flipGravity(player, !player->m_isUpsideDown, obj->m_hasNoEffects ? true : obj->m_isHide);
                    obj->playShineEffect();
                    obj->activatedByPlayer(player);
                    gameEventTriggered(PortalGravityInvert, 0, 0);
                }
                break;
            case GravityTogglePortal|NormalGravityPortal:
                playerTouchedRing(player, (RingObject*)obj);
                playerRect = *player->getObjectRect();
                maxX = playerRect.getMaxX();
                minX = playerRect.getMinX();
                maxY = playerRect.getMaxY();
                minY = playerRect.getMinY();
                break;
            default:
                break;
        }
    }
}

void GJBaseGameLayer::calculateColorGroups() {
    std::set<int> targets;
    for (unsigned int i = 0; i < cocos2d::CCArray::count(m_objects); ++i) {
        GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(m_objects, i);
        obj->resetColorGroups();
        if ((unsigned int)(obj->m_objectID - 1006) <= 1) targets.insert(obj->m_colorGroupID);
    }
    addRemapTargets(&targets);
    for (int groupID : targets) {
        cocos2d::CCArray* group = getGroup(groupID);
        for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
            GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
            obj->addToColorGroup(groupID);
        }
    }
}

void GJBaseGameLayer::cameraMoveX(float value, float duration, float rate, bool unused) {
    m_gameState.m_unkBool4 = true;
    m_gameState.tweenValue(m_gameState.m_cameraPosition.x, value, 1, duration, 1, rate, -1, -1);
}

void GJBaseGameLayer::cameraMoveY(float value, float duration, float rate, bool force) {
    if (!m_gameState.m_unkBool5 || m_gameState.m_unkInt13 != value || force) {
        m_gameState.m_unkBool5 = true;
        m_gameState.m_unkInt13 = value;
        m_gameState.tweenValue(m_gameState.m_cameraPosition.y, value, 2, duration, 1, rate, -1, -1);
    }
}

bool GJBaseGameLayer::canProcessSFX(SFXTriggerState* state, std::unordered_map<int,int>* stateIndices,
                                    std::unordered_map<int,float>* times,
                                    std::vector<SFXTriggerState>* stateIndicesVec) {
    int uniqueID = SFXTriggerGameObject::getUniqueSFXID(state->m_sfxTriggerGameObject);
    SFXTriggerGameObject* obj = state->m_sfxTriggerGameObject;
    int key = uniqueID ? uniqueID : -obj->m_uniqueID;

    if (obj->m_minInterval <= 0.0f) {
        if (!uniqueID) {
            (*stateIndices)[key] = state->m_unkInt2;
            return true;
        }
    } else {
        auto it = times->find(key);
        if (it != times->end()) {
            if (state->m_unkDouble1 - it->second < obj->m_minInterval) return false;
        }
    }

    auto idxIt = stateIndices->find(key);
    if (idxIt != stateIndices->end()) {
        SFXTriggerState* existing = &(*stateIndicesVec)[idxIt->second];
        if (obj->m_override) {
            existing->m_processed = true;
        } else if (!existing->m_processed) {
            processSFXState(existing, nullptr, 0, state->m_unkDouble1);
            if (!existing->m_processed) return false;
        }
    }

    (*stateIndices)[key] = state->m_unkInt2;
    (*times)[key] = state->m_unkDouble1;
    return true;
}

cocos2d::CCParticleSystemQuad* GJBaseGameLayer::claimCustomParticle(std::string key, cocos2d::ParticleStruct* particleStruct,
                                                                     int zLayer, int zOrder, bool uiObject, bool dontAdd) {
    if (m_particleCount > 99999) return nullptr;

    cocos2d::CCArray* customParticles = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_customParticles, &key);
    cocos2d::CCParticleSystemQuad* result = nullptr;

    if (!customParticles || !cocos2d::CCArray::count(customParticles) || dontAdd) {
        result = createCustomParticle(key, particleStruct, 0, dontAdd);
        if (!customParticles) {
            customParticles = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_customParticles, &key);
        }
    }

    if (!dontAdd) {
        result = (cocos2d::CCParticleSystemQuad*)cocos2d::CCArray::lastObject(customParticles);
        cocos2d::CCArray* claimed = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_claimedParticles, &key);
        cocos2d::CCArray::addObject(claimed, result);
        cocos2d::CCArray::removeLastObject(customParticles);
    }

    if (!result) return nullptr;

    cocos2d::CCNode* parent = parentForZLayer(zLayer, false, 4, uiObject);
    if (result->getParent() == parent) {
        if (result->getZOrder() != zOrder) parent->reorderChild(result, zOrder);
    } else {
        if (result->getParent()) result->removeFromParentAndCleanup(false);
        parent->addChild(result, zOrder);
    }

    unsigned int uid = result->getID();
    if (m_customParticlesUIDs.insert(uid).second) {
        cocos2d::CCArray::addObject(m_temporaryParticles, result);
    }
    result->resetSystem();
    result->setVisible(true);
    result->setAutoRemoveOnFinish(dontAdd);
    m_particleCount += result->getTotalParticles();
    return result;
}

cocos2d::CCPoint GJBaseGameLayer::claimMoveAction(int groupID, bool ignoreStaticGroups) {
    auto& map = m_effectManager->m_unkMap618;
    auto it = map.find(groupID);
    if (it == map.end()) return cocos2d::CCPoint(0.0f, 0.0f);

    CCMoveCNode* node = it->second;
    if (!node) return cocos2d::CCPoint(0.0f, 0.0f);

    if (node->m_unk0d0) return cocos2d::CCPoint(0.0f, 0.0f);
    node->m_unk0d0 = 1;

    if (ignoreStaticGroups) {
        return cocos2d::CCPoint(node->m_unk090, node->m_unk098);
    }

    if (node->m_groupObjects) {
        unsigned int count = node->m_groupObjects->count();
        for (unsigned int i = 0; i < count; ++i) {
            GroupCommandObject2* cmd = static_cast<GroupCommandObject2*>(
                node->m_groupObjects->objectAtIndex(i));
            if (cmd) {
                cmd->m_currentXOffset = 0.0;
                cmd->m_currentYOffset = 0.0;
            }
        }
    }

    return cocos2d::CCPoint(node->m_unk038, node->m_unk040);
}

cocos2d::CCParticleSystemQuad* GJBaseGameLayer::claimParticle(std::string key, int zLayer) {
    cocos2d::CCArray* available = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_particlesDict, &key);
    if (!available || !cocos2d::CCArray::count(available)) return nullptr;

    cocos2d::CCParticleSystemQuad* result = (cocos2d::CCParticleSystemQuad*)cocos2d::CCArray::lastObject(available);
    std::string key2 = getParticleKey2(key);
    cocos2d::CCArray* claimed = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_particlesDict, &key2);
    cocos2d::CCArray::addObject(claimed, result);
    cocos2d::CCArray::removeLastObject(available);

    if (!result->getParent()) {
        cocos2d::CCLayer* parent = m_objectLayer;
        if (m_shaderLayer && m_shaderLayer->m_state.m_usesShaders
            && (unsigned int)(m_shaderLayer->m_state.m_minBlendingLayer - 2) <= 5) {
            parent = m_inShaderObjectLayer;
        }
        parent->addChild(result, result->getTag());
    }
    cocos2d::CCArray::addObject(m_temporaryParticles, result);
    result->setVisible(true);
    return result;
}

void GJBaseGameLayer::claimRotationAction(int targetID, int centerID, float* rotation, float* offset,
                                           bool ignoreStaticGroups, bool ignoreMove) {
    auto& map = m_effectManager->m_unkMap770;
    auto it = map.find(std::make_pair(targetID, centerID));
    if (it == map.end()) return;

    for (GroupCommandObject2* cmd : it->second) {
        if (!cmd) continue;
        if (cmd->m_someInterpValue1RelatedFalse) continue;
        cmd->m_someInterpValue1RelatedFalse = true;

        double v25 = ignoreStaticGroups ? cmd->m_unkA8 : cmd->m_unkA0;
        double v26 = ignoreStaticGroups ? cmd->m_unk118 : cmd->m_unk110;
        float delta = (float)(v25 - v26);
        *rotation += delta;
        if (!cmd->m_unkB8) *offset += delta;
    }

    map.erase(it);
}

void GJBaseGameLayer::applyRemap(EffectGameObject* object, std::vector<int>* remapKeys,
                                  std::unordered_map<int,int>* remap) {
    if (remapKeys->size() <= 1) return;

    for (size_t i = 0; i + 1 < remapKeys->size(); ++i) {
        auto& trigger = m_spawnRemapTriggers[(*remapKeys)[i]];

        auto processInt = [&](int& field, int sentinel) {
            auto it = trigger.find(field);
            if (it == trigger.end()) return;
            if (remap->find(sentinel) == remap->end()) (*remap)[sentinel] = field;
            field = it->second;
        };
        auto processFloat = [&](float& field, int sentinel) {
            int bits = *reinterpret_cast<int*>(&field);
            auto it = trigger.find(bits);
            if (it == trigger.end()) return;
            if (remap->find(sentinel) == remap->end()) (*remap)[sentinel] = bits;
            int newBits = it->second;
            field = *reinterpret_cast<float*>(&newBits);
        };

        processInt(object->m_targetGroupID, 51);
        processInt(object->m_centerGroupID, 71);
        processInt(object->m_itemID, 80);
        processInt(object->m_itemID2, 95);
        processInt(object->m_controlID, 534);
        processInt(object->m_animationID, 76);

        int id = object->m_objectID;
        if (id > 3021) {
            if (id == 3604) {
                processInt(object->m_eObjType, 447);
                processInt(object->m_uIndexInArray, 525);
            } else if (id == 3607) {
                ChanceTriggerGameObject::remapChanceObjects((ChanceTriggerGameObject*)object, trigger);
            } else if (id == 3605) {
                processInt(*(int*)&object->m_sTransform.a, 392);
                processFloat(object->m_sTransform.c, 455);
                processFloat(object->m_sTransform.d, 457);
                processInt(object->m_uOrderOfArrival, 416);
                processFloat(object->m_sAdditionalTransform.ty, 432);
            } else if ((unsigned)(id - 3660) <= 1) {
                processInt(*(int*)&object->m_bRunning, 366);
                processInt(object->m_nTag, 560);
                processInt(object->m_eObjType, 565);
            }
        } else if (id >= 3017 || id == 3023) {
            processFloat(object->m_sTransform.d, 225);
            processInt((int&)object->m_pGrid, 344);
            processInt(object->m_enterChannel, 343);
        } else if (id == 2068) {
            ChanceTriggerGameObject::remapChanceObjects((ChanceTriggerGameObject*)object, trigger);
        } else if (id == 2903) {
            processInt(object->m_nLuaID, 203);
            processInt(object->m_nTag, 204);
            processInt(object->m_uReference, 205);
            processInt(object->m_uAutoReleaseCount, 206);
            processInt(object->m_uID, 209);
        } else if (id == 901) {
            processInt(object->m_targetModCenterID, 395);
        } else if (id == 1346) {
            processInt(object->m_rotationTargetID, 401);
            processInt(*(int*)((char*)&object->m_canSpawnTriggers + 1), 516);
            processInt(object->m_uID, 517);
            processInt(object->m_nLuaID, 518);
            processInt(object->m_uReference, 519);
        } else if (id == 1934) {
            processInt(object->m_uID, 392);
            processFloat(object->m_sTransform.c, 455);
            processFloat(object->m_sTransform.d, 457);
            processInt(object->m_uOrderOfArrival, 416);
            processFloat(object->m_sAdditionalTransform.ty, 432);
        } else if (id == 3016 || id == 3006) {
            processInt(*(int*)&object->m_bRunning, 366);
            processInt(object->m_nTag, 560);
            processInt(object->m_eObjType, 565);
        }
    }
}

void GJBaseGameLayer::applySFXEditTrigger(int channelID, int targetType, SFXTriggerGameObject* object) {
    if (!(m_practiceMusicSync || !m_isPracticeMode || m_isPlatformer)) return;

    if (object->m_stop || object->m_stopLoop) {
        FMODAudioEngine::sharedEngine()->stopChannel(channelID, (AudioTargetType)targetType, object->m_stopLoop, 0.0f);
    }
    if (object->m_changeVolume) {
        float pitch = object->m_pitch;
        if (pitch <= 0.0f) pitch = FMODAudioEngine::pitchForIdx(object->m_speed);
        FMODAudioEngine::sharedEngine()->updateChannel(channelID, (AudioTargetType)targetType, Pitch, object->m_duration, pitch);
    }
    if (object->m_changeSpeed) {
        FMODAudioEngine::sharedEngine()->updateChannel(channelID, (AudioTargetType)targetType, Volume, object->m_duration, object->m_volume);
    }
}

float GJBaseGameLayer::convertToClosestDirection(float angle, float bound) {
    if (angle < -bound) {
        return angle + ceilf(floorf(fabsf(angle) / bound) * 0.5f) * (bound + bound);
    }
    if (angle > bound) {
        return angle - ceilf(floorf(fabsf(angle) / bound) * 0.5f) * (bound + bound);
    }
    return angle;
}

int GJBaseGameLayer::countCollectedUserCoins() {
    if (!m_collectedItems) return 0;
    cocos2d::CCDictElement* element = m_collectedItems->m_pElements;
    if (!element) return 0;

    int count = 0;
    while (element) {
        GameObject* obj = (GameObject*)element->m_pObject;
        if (obj->getType() == (Collectible|0x1)) count++;
        element = (cocos2d::CCDictElement*)element->hh.next;
    }
    return count;
}

void GJBaseGameLayer::createBackground(int background) {
    if (m_background && m_background->getTag() == background) return;
    if (m_background) removeBackground();
    if (background > 59) background = 59;

    bool useMirroredWrap = true;
    if (background > 51) {
        if ((unsigned int)(background - 53) <= 6) useMirroredWrap = false;
    } else if (background >= 40 || background == 35 || background == 37 || background == 16) {
        useMirroredWrap = false;
    }

    GameManager* gm = GameManager::sharedState();
    cocos2d::CCTexture2D* texture = gm->getBGTexture(background);
    m_background = cocos2d::CCSprite::create(texture);
    m_background->setTag(background);

    cocos2d::ccTexParams texParams;
    texParams.minFilter = GL_LINEAR;
    texParams.magFilter = GL_LINEAR;
    texParams.wrapS = GL_CLAMP_TO_EDGE;
    texParams.wrapT = useMirroredWrap ? GL_MIRRORED_REPEAT : GL_CLAMP_TO_EDGE;
    m_background->getTexture()->setTexParameters(&texParams);

    m_objectParent->addChild(m_background, -60);
    m_background->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    m_background->setScale(AppDelegate::get()->bgScale());
    m_background->setBlendFunc({GL_ONE, GL_ZERO});

    cocos2d::ccColor3B color;
    color.r = 40;
    color.g = 125;
    color.b = 255;
    m_background->setColor(color);

    m_gameState.m_unkFloat10 = m_background->m_obRect.size.width * m_background->getScale();
}

cocos2d::CCParticleSystemQuad* GJBaseGameLayer::createCustomParticle(std::string key, cocos2d::ParticleStruct* particleStruct, int minimum, bool dontAdd) {
    cocos2d::CCArray* array = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_customParticles, &key);

    if (!array) {
        if (dontAdd && minimum <= 0) {
            cocos2d::CCParticleSystemQuad* particle = GameToolbox::particleFromStruct(particleStruct, nullptr, true);
            particle->setTag(0);
            cocos2d::CCParticleSystem::stopSystem(particle);
            particle->setVisible(false);
            return particle;
        }
        if (!dontAdd) {
            array = cocos2d::CCArray::create();
            m_customParticleCount++;
            array->setTag(m_customParticleCount);
            m_particleCountToParticleString[array->getTag()] = key;
            cocos2d::CCDictionary::setObject(m_customParticles, array, &key);
            cocos2d::CCDictionary::setObject(m_claimedParticles, cocos2d::CCArray::create(), &key);
        }
    }

    if (minimum > 0 && array && cocos2d::CCArray::count(array) >= (unsigned int)minimum) {
        return nullptr;
    }

    cocos2d::CCParticleSystemQuad* particle = GameToolbox::particleFromStruct(particleStruct, nullptr, true);
    particle->setTag(0);
    cocos2d::CCParticleSystem::stopSystem(particle);
    particle->setVisible(false);
    if (!dontAdd && array) {
        cocos2d::CCArray::addObject(array, particle);
        particle->m_nCustomParticleIndex = array->getTag();
    }
    return particle;
}

cocos2d::CCParticleSystemQuad* GJBaseGameLayer::createParticle(int objectType, char* plistName, int tag, int positionType) {
    if (GameManager::sharedState()->m_performanceMode) return nullptr;

    std::string key = getParticleKey(objectType, plistName, tag, positionType);

    cocos2d::CCArray* array = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_particlesDict, &key);
    if (!array) {
        array = cocos2d::CCArray::create();
        cocos2d::CCDictionary::setObject(m_particlesDict, array, &key);

        std::string key2 = getParticleKey2(key);
        cocos2d::CCDictionary::setObject(m_particlesDict, cocos2d::CCArray::create(), &key2);
    }

    cocos2d::CCArray* particleArray = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(m_particlesDict, &key);
    if (cocos2d::CCArray::count(particleArray) >= m_particleSystemLimit) {
        return nullptr;
    }

    cocos2d::CCParticleSystemQuad* particle = cocos2d::CCParticleSystemQuad::create(plistName);
    particle->setTag(tag);
    particle->setPositionType((cocos2d::tCCPositionType)positionType);
    cocos2d::CCParticleSystem::stopSystem(particle);
    particle->setVisible(false);
    cocos2d::CCArray::addObject(particleArray, particle);
    return particle;
}

void GJBaseGameLayer::createGroundLayer(int ground, int line) {
    if (m_groundLayer) removeGroundLayer();
    if (ground >= 22) ground = 22;

    m_groundLayer = GJGroundLayer::create(ground, line);
    m_groundLayer->setTag(ground);
    m_objectParent->addChild(m_groundLayer, 40);

    m_groundLayer2 = GJGroundLayer::create(ground, line);
    m_objectParent->addChild(m_groundLayer2, 40);
    m_groundLayer2->setScaleY(-1.0f);
}

void GJBaseGameLayer::createMiddleground(int middleground) {
    if (m_middleground) removeMiddleground();
    if (middleground > 0) {
        if (middleground >= 3) middleground = 3;
        m_middleground = GJMGLayer::create(middleground);
        m_middleground->setTag(middleground);
        m_objectParent->addChild(m_middleground, -39);
    }
}

cocos2d::CCArray* GJBaseGameLayer::createNewKeyframeAnim() {
    cocos2d::CCArray* anim = cocos2d::CCArray::create();
    anim->setTag(m_keyframeGroup);
    m_keyframeGroups->setObject(anim, (intptr_t)m_keyframeGroup);
    m_keyframeGroup++;
    return anim;
}

void GJBaseGameLayer::createPlayer() {
    GameManager* gm = GameManager::sharedState();

    m_player1 = PlayerObject::create(
        gm->m_playerFrame.random - gm->m_playerFrame.seed,
        gm->m_playerShip.random - gm->m_playerShip.seed,
        this, m_objectLayer, !m_isEditor);

    m_player1->setColor(gm->colorForIdx(gm->m_playerColor.random - gm->m_playerColor.seed));
    m_player1->setSecondColor(gm->colorForIdx(gm->m_playerColor2.random - gm->m_playerColor2.seed));

    if (gm->m_playerGlowColor.random - gm->m_playerGlowColor.seed >= 0) {
        m_player1->enableCustomGlowColor(gm->colorForIdx(gm->m_playerGlowColor.random - gm->m_playerGlowColor.seed));
    }
    m_player1->updateGlowColor();
    m_objectLayer->addChild(m_player1, 59);
    m_player1->m_uniqueID = 1;

    if (!m_isEditor) m_player1->addAllParticles();

    m_player1->updateCheckpointMode(gm->getGameVariable("0068"));
    m_player1->setVisible(!m_isEditor);
    m_player1->m_ignoreDamage = m_isEditor ? m_ignoreDamage : false;

    gm = GameManager::sharedState();
    m_player2 = PlayerObject::create(
        gm->m_playerFrame.random - gm->m_playerFrame.seed,
        gm->m_playerShip.random - gm->m_playerShip.seed,
        this, m_objectLayer, !m_isEditor);

    m_player2->setColor(gm->colorForIdx(gm->m_playerColor2.random - gm->m_playerColor2.seed));
    m_player2->setSecondColor(gm->colorForIdx(gm->m_playerColor.random - gm->m_playerColor.seed));

    if (gm->m_playerGlowColor.random - gm->m_playerGlowColor.seed >= 0) {
        m_player2->enableCustomGlowColor(gm->colorForIdx(gm->m_playerGlowColor.random - gm->m_playerGlowColor.seed));
    }
    m_player2->updateGlowColor();
    cocos2d::CCObject::retain(m_player2);
    m_player2->m_isSecondPlayer = true;
    m_player2->m_uniqueID = 2;
    m_player2->setVisible(!m_isEditor);
    m_player2->m_ignoreDamage = m_isEditor ? m_ignoreDamage : false;
}

void GJBaseGameLayer::createPlayerCollisionBlock() {
    if (m_player1CollisionBlock) return;

    for (int i = 0; i < 2; ++i) {
        bool isPlayer2 = (i == 1);
        GameObject* block = GameObject::createWithKey(328);
        cocos2d::CCObject::retain(block);
        block->customSetup();
        block->addColorSprite("edit_eCollisionBlock01_001.png");
        block->setupCustomSprites("edit_eCollisionBlock01_001.png");

        PlayerObject* player;
        if (isPlayer2) {
            m_player2CollisionBlock = block;
            player = m_player2;
        } else {
            m_player1CollisionBlock = block;
            player = m_player1;
        }

        block->m_collisionBlockID = isPlayer2 ? 10001 : 10000;
        block->setStartPos(*player->getPosition());
        block->m_bDontDraw = true;
        block->m_unk507 = true;
        addToSection(block);
        cocos2d::CCArray::addObject(m_collisionBlocks, block);
    }
}

void GJBaseGameLayer::createTextLayers() {
    GameManager* gm = GameManager::sharedState();
    const char* fontTexture = gm->getFontTexture(gm->m_loadedFont);

    auto loadTexture = [&]() {
        return cocos2d::CCTextureCache::sharedTextureCache()->addImage(fontTexture);
    };

    auto createBatch = [&](cocos2d::CCTexture2D* tex, int zOrder, bool blending) {
        cocos2d::CCSpriteBatchNode* node = cocos2d::CCSpriteBatchNode::createWithTexture(tex, 100);
        if (blending) {
            cocos2d::ccBlendFunc blend = {GL_SRC_ALPHA, GL_ONE};
            node->setBlendFunc(blend);
        }
        m_objectLayer->addChild(node, zOrder);
        return node;
    };

    cocos2d::CCTexture2D* baseTex = loadTexture();

    m_textLayerT4 = createBatch(baseTex, 1260, false);
    m_textBlendingLayerT4 = createBatch(baseTex, 1280, true);
    m_textLayerT3 = createBatch(baseTex, 880, false);
    m_textBlendingLayerT3 = createBatch(baseTex, 900, true);
    m_textLayerT2 = createBatch(baseTex, 580, false);
    m_textBlendingLayerT2 = createBatch(baseTex, 600, true);
    m_textLayerT1 = createBatch(baseTex, 280, false);
    m_textBlendingLayerT1 = createBatch(baseTex, 300, true);

    m_textLayerB1 = createBatch(loadTexture(), -100, false);
    m_textBlendingLayerB1 = createBatch(loadTexture(), -80, true);
    m_textLayerB2 = createBatch(baseTex, -400, false);
    m_textBlendingLayerB2 = createBatch(baseTex, -380, true);
    m_textLayerB3 = createBatch(baseTex, -700, false);
    m_textBlendingLayerB3 = createBatch(baseTex, -680, true);
    m_textLayerB4 = createBatch(baseTex, -1000, false);
    m_textBlendingLayerB4 = createBatch(baseTex, -980, true);
    m_textLayerB5 = createBatch(baseTex, -1300, false);
    m_textBlendingLayerB5 = createBatch(baseTex, -1280, true);
}

void GJBaseGameLayer::destroyObject(GameObject* object) {
    if (object->getType() == Collectible && GameObject::shouldShowPickupEffects(object)) {
        int pickupID = object->m_pickupParticleID;
        if (pickupID <= 0) {
            GameObject::spawnDefaultPickupParticle(object, this);
        } else {
            cocos2d::CCPoint pos = *object->getPosition();
            spawnParticleTrigger(pickupID, pos, 0.0f, 1.0f);
        }
    }
    GameObject::playDestroyObjectAnim(object, this);
    GameObject::destroyObject(object);
    if (object->getType() == Breakable) {
        gameEventTriggered(DestroyBlock, 0, 0);
    }
}

void GJBaseGameLayer::destroyPlayer(PlayerObject* player, GameObject* object) {
}

void GJBaseGameLayer::didRotateGameplay() {
}

cocos2d::CCArray* GJBaseGameLayer::damagingObjectsInRect(cocos2d::CCRect rect, bool enabledGroups) {
    updateOBB2(rect);
    cocos2d::CCArray* result = cocos2d::CCArray::create();

    int outerMin = 0;
    if (rect.origin.x > 0.0f) {
        float scaledX = rect.origin.x < 10000000.0f
            ? rect.origin.x * m_sectionXFactor
            : m_sectionXFactor * 10000000.0f;
        outerMin = (int)scaledX - 1;
        if (outerMin < 0) outerMin = -1;
    }

    int outerMax = 0;
    float rightEdge = rect.origin.x + rect.size.width;
    if (rightEdge > 0.0f) {
        float scaledR = rightEdge < 10000000.0f
            ? rightEdge * m_sectionXFactor
            : m_sectionXFactor * 10000000.0f;
        outerMax = (int)(scaledR + 1.0f);
        int maxSection = (int)m_nonEffectObjects.size() - 1;
        if (outerMax > maxSection) outerMax = maxSection;
    } else {
        outerMax = 1;
    }

    int middleMin = 0;
    if (rect.origin.y > 0.0f) {
        float scaledY = rect.origin.y < 10000000.0f
            ? rect.origin.y * m_sectionYFactor
            : m_sectionYFactor * 10000000.0f;
        middleMin = (int)scaledY - 1;
        if (middleMin < 0) middleMin = -1;
    }

    int middleMax = 1;
    float topEdge = rect.origin.y + rect.size.height;
    if (topEdge > 0.0f) {
        float scaledT = topEdge < 10000000.0f
            ? topEdge * m_sectionYFactor
            : m_sectionYFactor * 10000000.0f;
        middleMax = (int)(scaledT + 1.0f);
    }

    for (int sx = outerMin; sx <= outerMax; ++sx) {
        if (sx >= (int)m_nonEffectObjects.size()) break;
        auto* xVec = m_nonEffectObjects[sx];
        if (!xVec) continue;

        int yEnd = (int)xVec->size() - 1;
        if (middleMax < yEnd) yEnd = middleMax;

        for (int sy = middleMin; sy <= yEnd; ++sy) {
            auto* vec = (*xVec)[sy];
            if (!vec) continue;
            int count = (*m_nonEffectObjectsSizes[sx])[sy];

            for (int i = 0; i < count; ++i) {
                GameObject* obj = (*vec)[i];
                GameObjectType type = obj->getType();
                if (type != Hazard && type != AnimatedHazard) continue;
                if (!obj->m_isActivated && !(enabledGroups && !obj->getGroupDisabled())) continue;

                cocos2d::CCRect objRect = *obj->getObjectRect();
                if (obj->m_objectRadius <= 0.0f) {
                    if (!rect.intersectsRect(objRect)) continue;
                    if (obj->m_shouldUseOuterOb) {
                        OBB2D* objObb = obj->getOrientedBox();
                        if (!OBB2D::overlaps1Way(objObb) || !OBB2D::overlaps1Way(m_obb2)) continue;
                    }
                } else {
                    float radius = obj->getObjectRadius();
                    cocos2d::CCPoint pos = obj->getRealPosition();
                    if (!rectIntersectsCircle(&rect, &pos, radius)) continue;
                }
                cocos2d::CCArray::addObject(result, obj);
            }
        }
    }
    return result;
}


void GJBaseGameLayer::exitStaticCamera(bool exitX, bool exitY, float time, int easingType, float easingRate,
                                        bool smoothVelocity, float smoothVelocityMod, bool exitInstant) {
    if (exitInstant) {
        if (exitX) m_gameState.m_unkBool28 = true;
        if (exitY) m_gameState.m_unkBool29 = true;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint halfCam(
        (winSize.width * 0.5f) / m_gameState.m_cameraZoom,
        (winSize.height * 0.5f) / m_gameState.m_cameraZoom);

    cocos2d::CCPoint camPos = m_gameState.m_cameraPosition2 + halfCam;
    camPos -= m_gameState.m_cameraOffset;

    if (exitX && m_gameState.m_unkPoint17.x != 0.0f) {
        m_gameState.m_unkPoint17.x = 0.0f;
        m_gameState.m_unkPoint18.x = 1.0f;
        GJGameState::stopTweenAction(&m_gameState, 10);
        m_gameState.m_unkPoint19.x = time;
        m_gameState.m_unkPoint20.x = easingRate;
        m_gameState.m_unkPoint21.x = (float)easingType;
        m_gameState.m_unkPoint22.x = (float)smoothVelocity;
        m_gameState.m_unkPoint23.x = smoothVelocityMod;

        float clamped = cocos2d::clampf(m_gameState.m_unkPoint4.x, -10000.0f, 10000.0f);
        m_gameState.m_unkPoint5.x = m_gameState.m_unkPoint6.x;
        m_gameState.m_unkPoint10.x = clamped;
    }

    if (exitY && m_gameState.m_unkPoint17.y != 0.0f) {
        m_gameState.m_unkPoint17.y = 0.0f;
        m_gameState.m_unkPoint18.y = 1.0f;
        GJGameState::stopTweenAction(&m_gameState, 11);
        m_gameState.m_unkPoint19.y = time;
        m_gameState.m_unkPoint20.y = easingRate;
        m_gameState.m_unkPoint21.y = (float)easingType;
        m_gameState.m_unkPoint22.y = (float)smoothVelocity;
        m_gameState.m_unkPoint23.y = smoothVelocityMod;

        float clamped = cocos2d::clampf(m_gameState.m_unkPoint4.y, -10000.0f, 10000.0f);
        m_gameState.m_unkPoint5.y = m_gameState.m_unkPoint6.y;
        m_gameState.m_unkPoint10.y = clamped;
    }

    if (m_gameState.m_unkBool7 && m_gameState.m_unkPoint17.y == 0.0f) {
        updateDualGround(m_player1, m_gameState.m_dualRelated, false, 0.0f);
    }
}

void GJBaseGameLayer::enterDualMode(GameObject* object, bool unused) {
    if (!object) return;
    m_player1->m_lastPortalPos = *object->getPosition();
    m_player1->m_lastActivatedPortal = object;
    m_gameState.m_lastActivatedPortal1 = object;
}

void GJBaseGameLayer::flipArt(bool flip) {}

void GJBaseGameLayer::flipFinished() {
    PlayerObject::levelFlipFinished(m_player1);
    if (m_gameState.m_isDualMode) PlayerObject::levelFlipFinished(m_player2);
    m_gameState.m_unkBool11 = false;
    m_gameState.m_unkBool12 = false;
}

void GJBaseGameLayer::flipGravity(PlayerObject* player, bool flip, bool noEffects) {
    if (player->m_isUpsideDown == flip) return;
    PlayerObject::flipGravity(player, flip, noEffects);

    if (m_gameState.m_unkBool31 || !m_gameState.m_isDualMode || m_levelSettings->m_twoPlayerMode) return;

    if (m_player1->m_isShip != m_player2->m_isShip) return;
    if (m_player1->m_isBall != m_player2->m_isBall) return;
    if (m_player1->m_isBird != m_player2->m_isBird) return;
    if (m_player1->m_isSpider != m_player2->m_isSpider) return;
    if (m_player1->m_isRobot != m_player2->m_isRobot) return;
    if (m_player1->m_isSwing != m_player2->m_isSwing) return;

    PlayerObject* other = (player->m_uniqueID == m_player1->m_uniqueID) ? m_player2 : m_player1;
    PlayerObject::flipGravity(other, !flip, noEffects);
}

void GJBaseGameLayer::flipObjects() {
    m_player1->setScaleX(-m_player1->getScaleX());
    m_player1->setRotation(-m_player1->getRotation());
    m_player2->setScaleX(-m_player2->getScaleX());
    m_player2->setRotation(-m_player2->getRotation());
}

std::string GJBaseGameLayer::gameEventToString(GJGameEvent event) {
    switch (event) {
        case GJGameEvent::None: return "None";
        case GJGameEvent::TinyLanding: return "Tiny Landing";
        case GJGameEvent::FeatherLanding: return "Feather Landing";
        case GJGameEvent::SoftLanding: return "Soft Landing";
        case GJGameEvent::NormalLanding: return "Normal Landing";
        case GJGameEvent::HardLanding: return "Hard Landing";
        case GJGameEvent::HitHead: return "Hit Head";
        case GJGameEvent::OrbTouched: return "Orb Touched";
        case GJGameEvent::OrbActivated: return "Orb Activated";
        case GJGameEvent::PadActivated: return "Pad Activated";
        case GJGameEvent::GravityInverted: return "Gravity Inverted";
        case GJGameEvent::GravityRestored: return "Gravity Restored";
        case GJGameEvent::NormalJump: return "Normal Jump";
        case GJGameEvent::RobotBoostStart: return "Robot Boost Start";
        case GJGameEvent::RobotBoostStop: return "Robot Boost Stop";
        case GJGameEvent::UFOJump: return "UFO Jump";
        case GJGameEvent::ShipBoostStart: return "Ship Boost Start";
        case GJGameEvent::ShipBoostEnd: return "Ship Boost End";
        case GJGameEvent::SpiderTeleport: return "Spider Teleport";
        case GJGameEvent::BallSwitch: return "Ball Switch";
        case GJGameEvent::SwingSwitch: return "Swing Switch";
        case GJGameEvent::WavePush: return "Wave Push";
        case GJGameEvent::WaveRelease: return "Wave Release";
        case GJGameEvent::DashStart: return "Dash Start";
        case GJGameEvent::DashStop: return "Dash Stop";
        case GJGameEvent::Teleported: return "Teleported";
        case GJGameEvent::PortalNormal: return "Portal: Normal";
        case GJGameEvent::PortalShip: return "Portal: Ship";
        case GJGameEvent::PortalBall: return "Portal: Ball";
        case GJGameEvent::PortalUFO: return "Portal: UFO";
        case GJGameEvent::PortalWave: return "Portal: Wave";
        case GJGameEvent::PortalRobot: return "Portal: Robot";
        case GJGameEvent::PortalSpider: return "Portal: Spider";
        case GJGameEvent::PortalSwing: return "Portal: Swing";
        case GJGameEvent::YellowOrb: return "Yellow Orb";
        case GJGameEvent::PinkOrb: return "Pink Orb";
        case GJGameEvent::RedOrb: return "Red Orb";
        case GJGameEvent::GravityOrb: return "Gravity Orb";
        case GJGameEvent::GreenOrb: return "Green Orb";
        case GJGameEvent::DropOrb: return "Drop Orb";
        case GJGameEvent::CustomOrb: return "Custom Orb";
        case GJGameEvent::DashOrb: return "Dash Orb";
        case GJGameEvent::GravityDashOrb: return "Gravity Dash Orb";
        case GJGameEvent::SpiderOrb: return "Spider Orb";
        case GJGameEvent::TeleportOrb: return "Teleport Orb";
        case GJGameEvent::YellowPad: return "Yellow Pad";
        case GJGameEvent::PinkPad: return "Pink Pad";
        case GJGameEvent::RedPad: return "Red Pad";
        case GJGameEvent::GravityPad: return "Gravity Pad";
        case GJGameEvent::SpiderPad: return "Spider Pad";
        case GJGameEvent::PortalGravityFlip: return "Portal: Gravity Flip";
        case GJGameEvent::PortalGravityNormal: return "Portal: Gravity Normal";
        case GJGameEvent::PortalGravityInvert: return "Portal: Gravity Invert";
        case GJGameEvent::PortalFlip: return "Portal: Flip";
        case GJGameEvent::PortalUnFlip: return "Portal: UnFlip";
        case GJGameEvent::PortalNormalScale: return "Portal: Normal Scale";
        case GJGameEvent::PortalMiniScale: return "Portal: Mini Scale";
        case GJGameEvent::PortalDualOn: return "Portal: Dual On";
        case GJGameEvent::PortalDualOff: return "Portal: Dual Off";
        case GJGameEvent::PortalTeleport: return "Portal: Teleport";
        case GJGameEvent::Checkpoint: return "Checkpoint";
        case GJGameEvent::DestroyBlock: return "Destroy Block";
        case GJGameEvent::UserCoin: return "User Coin";
        case GJGameEvent::PickupItem: return "Pickup Item";
        case GJGameEvent::CheckpointRespawn: return "Checkpoint Respawn";
        case GJGameEvent::FallLow: return "Fall Low";
        case GJGameEvent::FallMed: return "Fall Med";
        case GJGameEvent::FallHigh: return "Fall High";
        case GJGameEvent::FallVHigh: return "Fall VHigh";
        case GJGameEvent::JumpPush: return "Jump Push";
        case GJGameEvent::JumpRelease: return "Jump Release";
        case GJGameEvent::LeftPush: return "Left Push";
        case GJGameEvent::LeftRelease: return "Left Release";
        case GJGameEvent::RightPush: return "Right Push";
        case GJGameEvent::RightRelease: return "Right Release";
        case GJGameEvent::PlayerReversed: return "Player Reversed";
        case GJGameEvent::FallSpeedLow: return "Fall Speed Low";
        case GJGameEvent::FallSpeedMed: return "Fall Speed Med";
        case GJGameEvent::FallSpeedHigh: return "Fall Speed High";
        default: return "NA";
    }
}

void GJBaseGameLayer::gameEventTriggered(GJGameEvent event, int material, int playerID) {
    auto key = std::make_pair(event, getEventKey(material, playerID));

    auto& commandIndexMap = m_gameState.m_unkMapPairGJGameEventIntInt;
    if (commandIndexMap[key] != m_gameState.m_commandIndex) {
        commandIndexMap[key] = m_gameState.m_commandIndex;

        auto& instances = m_gameState.m_unkMapPairGJGameEventIntVectorEventTriggerInstance[key];
        for (auto& instance : instances) {
            if (instance.m_processed) continue;
            spawnGroup(instance.m_targetGroupID, instance.m_ordered, 0.0,
                       &instance.m_remapKeys, instance.m_uniqueID, instance.m_controlID);
        }
    }

    if (playerID) {
        gameEventTriggered(event, material, 0);
    }
}

int GJBaseGameLayer::generateEnterEasingBuffer(int easingType, float easingRate) {
    int key = getEnterEasingKey(easingType, easingRate);
    if (key < 0) return key;

    auto it = m_enterEasingIndices.find(key);
    if (it != m_enterEasingIndices.end()) {
        return it->second;
    }

    int startIndex = m_enterEasingValuesIndex;
    m_enterEasingIndices[key] = startIndex;

    for (int i = 0; i <= 100; ++i) {
        float value = (float)(i - startIndex) / 100.0f;
        m_enterEasingValues.push_back(GameToolbox::getEasedValue(value, easingType, easingRate));
        m_enterEasingValuesIndex++;
    }

    return startIndex;
}

void GJBaseGameLayer::generateEnterEasingBuffers(EnterEffectObject* object) {
    object->m_easingInBuffer = generateEnterEasingBuffer(object->m_easingInType, object->m_easingInRate);
    object->m_easingOutBuffer = generateEnterEasingBuffer(object->m_easingOutType, object->m_easingOutRate);
}

void GJBaseGameLayer::generatePickupAnimRandVal(GameObject* object, float* randomValue1, float* randomValue2) {
    cocos2d::CCPoint pos = *object->getPosition();
    std::pair<int, int> key((int)pos.x, (int)pos.y);

    auto& map = m_destroyObjectValues;
    auto it = map.find(key);
    if (it == map.end()) {
        float rand1 = ((float)rand() * 4.6566e-10f) * 2.0f - 1.0f;
        float rand2 = (float)rand() * 4.6566e-10f;
        map[key] = std::make_pair(rand1, rand2);
        it = map.find(key);
    }

    *randomValue1 = it->second.first;
    *randomValue2 = it->second.second;
}

void GJBaseGameLayer::generateSpawnRemap() {
    m_spawnRemapTriggers.clear();
    m_spawnRemapTriggers.emplace_back();

    if (!m_objects) return;
    cocos2d::ccArray* data = m_objects->data;
    if (!data->num) return;

    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = (GameObject*)data->arr[i];
        if (!obj) break;
        if (obj->m_objectID != 1268) continue;

        SpawnTriggerGameObject* spawn = (SpawnTriggerGameObject*)obj;
        if (spawn->m_chanceObjects.m_start == spawn->m_chanceObjects.m_finish) {
            spawn->m_spawnRemapIndex = 0;
        } else {
            spawn->m_spawnRemapIndex = registerSpawnRemap(&spawn->m_chanceObjects);
        }
    }
}

void GJBaseGameLayer::generateVisibilityGroups() {
    m_visibilityGroups.clear();

    std::unordered_set<int> targetIDs;

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        if (data->num) {
            for (unsigned int i = 0; i < data->num; ++i) {
                GameObject* obj = (GameObject*)data->arr[i];
                if (!obj) break;
                obj->m_visibilityGroupID = 0;
                if (obj->m_objectID == 3662) {
                    targetIDs.insert(obj->m_unkGroupID);
                }
            }
        }
    }

    int groupIndex = 0;
    std::unordered_map<int, std::unordered_set<int>> relatedGroups;

    std::unordered_set<int> toProcess = targetIDs;
    for (int groupID : targetIDs) {
        auto* group = getGroup(groupID);
        groupIndex++;
        relatedGroups[groupIndex].insert(groupID);

        std::unordered_set<int> subGroups;
        if (group) {
            for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
                GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
                if (obj->m_unkVisibilityGroup > 0) subGroups.insert(obj->m_unkVisibilityGroup);
                obj->m_unkVisibilityGroup = groupIndex;
            }
        }

        std::vector<int> subList(subGroups.begin(), subGroups.end());
        for (int subID : subList) {
            relatedGroups[groupIndex].insert(subID);
            auto* subGroup = getGroup(subID);
            if (subGroup) {
                for (unsigned int i = 0; i < cocos2d::CCArray::count(subGroup); ++i) {
                    GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(subGroup, i);
                    obj->m_unkVisibilityGroup = groupIndex;
                }
            }
        }
    }

    for (auto& [idx, subs] : relatedGroups) {
        auto& group = m_visibilityGroups[idx];
        std::unordered_set<int> seen;
        for (int subID : subs) {
            auto* subGroup = getGroup(subID);
            if (!subGroup) continue;
            for (unsigned int i = 0; i < cocos2d::CCArray::count(subGroup); ++i) {
                GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(subGroup, i);
                if (seen.insert(obj->m_uniqueID).second) {
                    group.push_back(obj);
                }
            }
        }
    }
}

cocos2d::CCArray* GJBaseGameLayer::getActiveOrderSpawnObjects() {
    cocos2d::CCArray* result = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(
        m_spawnObjects, m_gameState.m_currentChannel);
    if (!result) return cocos2d::CCArray::create();
    return result;
}

void GJBaseGameLayer::generateTargetGroups() {
    if (m_targetGroupsArray) {
        m_targetGroupsArray->removeAllObjects();
    } else {
        m_targetGroupsArray = cocos2d::CCArray::create();
        m_targetGroupsArray->retain();
    }
    m_targetGroups.clear();

    struct TargetGroupVariations {
        bool m_bits[4] = { false, false, false, false };
    };

    std::unordered_map<int, TargetGroupVariations> variations;
    std::unordered_set<int> worklist;
    std::unordered_map<int, std::unordered_set<int>> relatedGroups;

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = (GameObject*)data->arr[i];
            if (!obj) break;

            if (obj->m_objectID == 3016 || obj->m_objectType == 45) {
                int groupID = obj->m_groupID;
                auto& v = variations[groupID];
                if (obj->m_hasAreaParent && obj->m_hasGroupParent) {
                    v.m_bits[3] = true;
                } else if (obj->m_hasAreaParent) {
                    v.m_bits[2] = true;
                } else if (obj->m_hasGroupParent) {
                    v.m_bits[1] = true;
                } else {
                    v.m_bits[0] = true;
                }
                worklist.insert(groupID);
            } else if (obj->m_objectID == 1268) {
                SpawnTriggerGameObject* spawn = (SpawnTriggerGameObject*)obj;
                for (auto& chanceObj : spawn->m_chanceObjects) {
                    relatedGroups[chanceObj.m_groupID].insert(chanceObj.m_chanceGroupID);
                }
            }
        }
    }

    std::unordered_set<int> processed;
    std::vector<int> initialList(worklist.begin(), worklist.end());
    for (int groupID : initialList) {
        if (processed.count(groupID)) continue;
        processed.insert(groupID);

        auto it = relatedGroups.find(groupID);
        if (it == relatedGroups.end()) continue;

        auto sourceBits = variations[groupID];
        for (int relatedID : it->second) {
            if (processed.count(relatedID)) continue;
            auto& rel = variations[relatedID];
            for (int b = 0; b < 4; ++b) {
                if (sourceBits.m_bits[b]) rel.m_bits[b] = true;
            }
        }
        relatedGroups.erase(it);
    }

    struct Combo { bool m_ignoreGroupParent; bool m_ignoreLinkedObjects; int m_bitIndex; };
    static const Combo s_combos[4] = {
        { false, false, 0 },
        { false, true,  1 },
        { true,  true,  3 },
        { true,  false, 2 }
    };

    for (int groupID : worklist) {
        auto& variationsEntry = variations[groupID];
        GameObject* groupParent = getGroupParent(groupID);
        cocos2d::CCArray* group = getGroup(groupID);

        for (int comboIdx = 0; comboIdx < 4; ++comboIdx) {
            const Combo& combo = s_combos[comboIdx];
            if (!variationsEntry.m_bits[combo.m_bitIndex]) continue;

            int specialKey = getSpecialKey(groupID, combo.m_ignoreGroupParent, combo.m_ignoreLinkedObjects);

            cocos2d::CCDictionary* areaDict  = cocos2d::CCDictionary::create();
            cocos2d::CCDictionary* groupDict = cocos2d::CCDictionary::create();
            std::unordered_map<int, int> targetMap;

            if (group) {
                for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
                    GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
                    targetMap[obj->m_uniqueID] = obj->m_groupID;

                    if (!combo.m_ignoreGroupParent && obj->m_hasGroupParentsString) {
                        cocos2d::CCArray* parentIDs = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(
                            m_parentGroupIDs, obj->m_uniqueID);
                        if (!parentIDs) continue;

                        for (unsigned int j = 0; j < cocos2d::CCArray::count(parentIDs); ++j) {
                            GameObject* parentEntry = (GameObject*)cocos2d::CCArray::objectAtIndex(parentIDs, j);
                            int parentGroupID = parentEntry->m_groupID;
                            if (groupParent && (obj != groupParent || parentGroupID != groupID)) continue;

                            cocos2d::CCArray* parentObjs = getGroup(parentGroupID);
                            if (!parentObjs) continue;

                            for (unsigned int k = 0; k < cocos2d::CCArray::count(parentObjs); ++k) {
                                GameObject* pObj = (GameObject*)cocos2d::CCArray::objectAtIndex(parentObjs, k);
                                targetMap.erase(pObj->m_uniqueID);

                                if (!(pObj->m_hasGroupParent || pObj->m_hasAreaParent || pObj == obj)) continue;

                                auto* existingA = (GameObject*)cocos2d::CCDictionary::objectForKey(areaDict, -parentGroupID);
                                bool pickA = !existingA
                                    || (!existingA->m_hasAreaParent && pObj->m_hasAreaParent)
                                    || (pObj == obj && pObj->m_hasAreaParent);
                                if (pickA) cocos2d::CCDictionary::setObject(areaDict, pObj, -parentGroupID);

                                auto* existingB = (GameObject*)cocos2d::CCDictionary::objectForKey(groupDict, -parentGroupID);
                                bool pickB = !existingB
                                    || (!existingB->m_hasGroupParent && pObj->m_hasGroupParent)
                                    || (pObj == obj && pObj->m_hasGroupParent);
                                if (pickB) cocos2d::CCDictionary::setObject(groupDict, pObj, -parentGroupID);
                            }

                            auto* areaParent = (GameObject*)cocos2d::CCDictionary::objectForKey(areaDict, -parentGroupID);
                            if (areaParent) targetMap[areaParent->m_uniqueID] = -parentGroupID;
                        }
                    }

                    if (!combo.m_ignoreLinkedObjects && obj->m_linkedGroup) {
                        auto linkedIt = targetMap.find(obj->m_linkedGroup);
                        if (linkedIt != targetMap.end()) {
                            int linkedGroupID = linkedIt->second;
                            if (linkedGroupID > 0 && (obj->m_hasAreaParent || obj->m_hasGroupParent)) {
                                auto* existingA = (GameObject*)cocos2d::CCDictionary::objectForKey(areaDict, linkedGroupID);
                                if (!existingA || (!existingA->m_hasAreaParent && obj->m_hasAreaParent)) {
                                    cocos2d::CCDictionary::setObject(areaDict, obj, linkedGroupID);
                                }
                                auto* existingB = (GameObject*)cocos2d::CCDictionary::objectForKey(groupDict, linkedGroupID);
                                if (!existingB || (!existingB->m_hasGroupParent && obj->m_hasGroupParent)) {
                                    cocos2d::CCDictionary::setObject(groupDict, obj, linkedGroupID);
                                }
                            }
                        }
                    }
                }
            }

            if (cocos2d::CCDictionary::count(areaDict) > 0) {
                cocos2d::CCArray* targetArray1 = cocos2d::CCArray::create();
                cocos2d::CCArray* targetArray2 = cocos2d::CCArray::create();
                cocos2d::CCDictionary* stickyDict = cocos2d::CCDictionary::create();
                cocos2d::CCDictionary* originDict = cocos2d::CCDictionary::create();

                if (group) {
                    for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
                        GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
                        auto it = targetMap.find(obj->m_uniqueID);
                        if (it == targetMap.end()) continue;
                        int groupForObj = it->second;
                        if (!groupForObj) continue;

                        auto* areaEntry = (GameObject*)cocos2d::CCDictionary::objectForKey(areaDict, groupForObj);
                        if (obj == areaEntry) {
                            cocos2d::CCArray::addObject(targetArray1, obj);
                            cocos2d::CCArray* stickyGroup = (groupForObj >= 0)
                                ? getStickyGroup(groupForObj)
                                : getGroup(-groupForObj);
                            cocos2d::CCDictionary::setObject(stickyDict, stickyGroup, groupForObj);
                            cocos2d::CCDictionary::setObject(originDict,
                                cocos2d::CCDictionary::objectForKey(groupDict, groupForObj), groupForObj);
                        } else if (!areaEntry) {
                            cocos2d::CCArray::addObject(targetArray2, obj);
                        }
                    }
                }

                int index = (int)cocos2d::CCArray::count(m_targetGroupsArray) + 1;
                int totalSize = (int)cocos2d::CCArray::count(targetArray2) + (int)cocos2d::CCArray::count(targetArray1);
                m_targetGroups[specialKey] = std::make_pair(index, totalSize);

                cocos2d::CCArray::addObject(m_targetGroupsArray, targetArray1);
                cocos2d::CCArray::addObject(m_targetGroupsArray, targetArray2);
                cocos2d::CCArray::addObject(m_targetGroupsArray, stickyDict);
                cocos2d::CCArray::addObject(m_targetGroupsArray, originDict);
            } else {
                int groupSize = group ? (int)cocos2d::CCArray::count(group) : 0;
                m_targetGroups[specialKey] = std::make_pair(0, groupSize);
            }
        }
    }
}

float GJBaseGameLayer::getCameraEdgeValue(int type) {
    int edgeValueID;
    switch (type) {
        case 1: edgeValueID = m_gameState.m_cameraEdgeValue0; break;
        case 2: edgeValueID = m_gameState.m_cameraEdgeValue1; break;
        case 3: edgeValueID = m_gameState.m_cameraEdgeValue2; break;
        case 4: edgeValueID = m_gameState.m_cameraEdgeValue3; break;
        default: edgeValueID = 0; break;
    }

    cocos2d::CCPoint pos(0.0f, 0.0f);
    GameObject* mainObject = tryGetMainObject(edgeValueID);
    if (mainObject) pos = mainObject->getRealPosition();

    switch (type - 1) {
        case 0:
        case 1: return pos.x;
        case 2:
        case 3: return pos.y;
        default: return 0.0f;
    }
}

std::string GJBaseGameLayer::getRecordString(bool compress) {
    if (!m_recordInputs) return "";

    std::string buffer;
    bool first = true;

    for (size_t i = 0; i < m_queuedRecordedButtons.size(); ++i) {
        const RecordButtonCommand& cmd = m_queuedRecordedButtons[i];

        if (!first) buffer += ";";
        if (!cmd.m_isPush) buffer += "-";
        buffer += std::to_string(cmd.m_step);

        int buttonID = getPlayerButtonID(cmd.m_button, cmd.m_isPlayer2);
        if (buttonID != 1) {
            buffer += ",";
            buffer += std::to_string(buttonID);
        }

        if (buttonID == 98 || buttonID == 99) {
            auto it = m_unk3330.find(cmd.m_unk00c);
            if (it != m_unk3330.end()) {
                const RecordCheckpoint& cp = it->second;

                buffer += ":";
                buffer += "O";   
                buffer += ",";
                buffer += std::to_string(reinterpret_cast<const uint64_t&>(cp.m_field24));
                buffer += ",";
                buffer += "1";    
                buffer += ",";
                buffer += std::to_string(cp.m_field16);
                buffer += ",";
                buffer += "i";   
                buffer += ",";
                buffer += std::to_string(cp.m_field32);
                buffer += ",";
                buffer += "2";   
                buffer += ",";
                buffer += std::to_string(cp.m_field36);
                buffer += ",";
                buffer += "6";  
                buffer += ",";
                buffer += std::to_string(cp.m_field48);
                buffer += ",";
                buffer += "r";   
                buffer += ",";
                buffer += std::to_string(reinterpret_cast<const uint64_t&>(cp.m_field40));
                buffer += ",";
                buffer += "3";   
                buffer += ",";
                buffer += cp.m_persistentStateString;
                buffer += ",";
            }
        }

        first = false;
    }

    std::string result = buffer + getRecordExtra();

    if (compress && !result.empty()) {
        result = cocos2d::ZipUtils::compressString(result, false, 11);
    }
    return result;
}
void GJBaseGameLayer::handleButton(bool down, int button, bool isPlayer1) {
    unsigned int playerIndex = isPlayer1;
    unsigned int twoPlayerMode = m_levelSettings->m_twoPlayerMode;

    if (!m_isPlatformer && !m_useReplay) {
        if (GameManager::sharedState()->getGameVariable("0010")) {
            playerIndex ^= 1u;
        }
    }

    bool dualTouch = GJEffectManager::hasActiveDualTouch(m_effectManager);

    if (down) {
        m_dualTouchTrigger = dualTouch;
    }

    bool skipToEnd = false;
    bool tryPlayer2 = false;

    if (down || m_dualTouchTrigger) {
        if (playerIndex < (unsigned int)dualTouch) {
            skipToEnd = true;
        }
    }

    if (!skipToEnd) {
        if (playerIndex < twoPlayerMode && isButtonAllowed(down, button, true)) {
            bool success = down ? m_player1->pushButton((PlayerButton)button)
                                : m_player1->releaseButton((PlayerButton)button);

            bool goToDual = !m_dualTouchTrigger
                          && m_gameState.m_isDualMode
                          && playerIndex <= (twoPlayerMode ^ 1)
                          && isButtonAllowed(down, button, false);

            if (goToDual) {
                tryPlayer2 = true;
            } else if (!success) {
                skipToEnd = true;
            } else {
                if ((unsigned int)(button - 1) <= 2) {
                    GJGameEvent event = GJGameEvent::None;
                    if (down) {
                        if (button == 1) event = GJGameEvent::JumpPush;
                        else if (button == 2) event = GJGameEvent::LeftPush;
                        else event = GJGameEvent::RightPush;
                    } else {
                        if (button == 1) event = GJGameEvent::JumpRelease;
                        else if (button == 2) event = GJGameEvent::LeftRelease;
                        else event = GJGameEvent::RightRelease;
                    }
                    if (event != GJGameEvent::None) {
                        gameEventTriggered(event, 0, twoPlayerMode ? 2 : 1);
                    }
                }
            }
        } else {
            if (m_dualTouchTrigger
                || !m_gameState.m_isDualMode
                || playerIndex > (twoPlayerMode ^ 1)
                || !isButtonAllowed(down, button, false)) {
                skipToEnd = true;
            } else {
                tryPlayer2 = true;
            }
        }
    }

    if (!skipToEnd && tryPlayer2) {
        bool success = down ? m_player2->pushButton((PlayerButton)button)
                            : m_player2->releaseButton((PlayerButton)button);
        if (success && (unsigned int)(button - 1) <= 2) {
            GJGameEvent event = GJGameEvent::None;
            if (down) {
                if (button == 1) event = GJGameEvent::JumpPush;
                else if (button == 2) event = GJGameEvent::LeftPush;
                else event = GJGameEvent::RightPush;
            } else {
                if (button == 1) event = GJGameEvent::JumpRelease;
                else if (button == 2) event = GJGameEvent::LeftRelease;
                else event = GJGameEvent::RightRelease;
            }
            if (event != GJGameEvent::None) {
                gameEventTriggered(event, 0, twoPlayerMode ? 2 : 1);
            }
        }
    }

    if (!m_player1->m_isDead) {
        GJEffectManager::playerButton(m_effectManager, down, playerIndex);
    }
    if (!m_useReplay) {
        recordAction(button, down, twoPlayerMode & (playerIndex ^ 1));
    }
    if (down) {
        int clickIndex = m_clickIndex;
        ++m_clicks;
        m_clickIndex = clickIndex + 1;
        if (button == 1) {
            m_jumping = true;
        }
    }
}

bool GJBaseGameLayer::hasItem(int id) {
    std::string key = cocos2d::CCString::createWithFormat("%i", id)->getCString();
    return m_collectedItems->objectForKey(key) != nullptr;
}

bool GJBaseGameLayer::hasUniqueCoin(EffectGameObject* object) {
    if (m_level->m_levelType == GJLevelType::Editor) {
        if (object->getType() != GameObjectType::UserCoin) return false;
        int coinID = object->m_secretCoinID;
        switch (coinID) {
            case 1: return m_level->m_firstCoinVerified.random - m_level->m_firstCoinVerified.seed == 1;
            case 2: return m_level->m_secondCoinVerified.random - m_level->m_secondCoinVerified.seed == 1;
            case 3: return m_level->m_thirdCoinVerified.random - m_level->m_thirdCoinVerified.seed == 1;
            default: return false;
        }
    }

    const char* coinKey = GJGameLevel::getCoinKey(m_level, object->m_secretCoinID);
    if (m_level->m_levelType == GJLevelType::Main) {
        return GameStatsManager::sharedState()->hasSecretCoin(coinKey);
    }
    GameStatsManager* stats = GameStatsManager::sharedState();
    return stats->hasUserCoin(coinKey) || stats->hasPendingUserCoin(coinKey);
}

void GJBaseGameLayer::increaseBatchNodeCapacity() {
    int capacity = GameManager::sharedState()->getGameVariable("0066") ? 1000 : 200;
    if (!m_batchNodes) return;

    cocos2d::ccArray* data = m_batchNodes->data;
    if (!data->num) return;

    for (unsigned int i = 0; i < data->num; ++i) {
        cocos2d::CCSpriteBatchNode* node = static_cast<cocos2d::CCSpriteBatchNode*>(data->arr[i]);
        if (!node) break;
        if (capacity > node->getAtlasCapacity()) {
            node->increaseAtlasCapacity(capacity);
        }
    }
}

bool GJBaseGameLayer::isButtonAllowed(bool down, int button, bool isPlayer1) {
    int key = 10 * isPlayer1 + 1000 * button + down;
    if (m_allowedButtons.find(key) != m_allowedButtons.end()) return false;
    m_allowedButtons.insert(key);
    return true;
}

bool GJBaseGameLayer::isFlipping() {
    return m_gameState.m_levelFlipping != 0.0f && m_gameState.m_levelFlipping != 1.0f;
}

bool GJBaseGameLayer::isPlayer2Button(int button) {
    return button > 5;
}

void GJBaseGameLayer::lightningFlash(cocos2d::CCPoint to, cocos2d::ccColor3B color) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float x = randRange(to.x - 50.0f, to.x + 100.0f);
    float y = m_gameState.m_cameraPosition.y + (winSize.height + 50.0f) / m_objectLayer->getScale();

    cocos2d::CCPoint from(x, y);
    lightningFlash(from, to, color, 5.0f, 0.5f, 0, true, 1.0f);
}

void GJBaseGameLayer::lightningFlash(cocos2d::CCPoint from, cocos2d::CCPoint to,
                                     cocos2d::ccColor3B color, float lineWidth,
                                     float duration, int displacement, bool flash,
                                     float opacity) {

    if (flash) {
        playFlashEffect(duration, 2, opacity);
    }
    (void)from;
    (void)to;
    (void)color;
    (void)lineWidth;
    (void)displacement;
}

void GJBaseGameLayer::loadGroupParentsFromString(GameObject* object, std::string groupList) {
    if (groupList.empty()) return;

    size_t start = 0;
    size_t end = groupList.find('.');
    while (true) {
        std::string token = groupList.substr(start, end - start);
        int groupID = atoi(token.c_str());
        setGroupParent(object, groupID);

        if (end == std::string::npos) break;
        start = end + 1;
        end = groupList.find('.', start);
    }
}

void GJBaseGameLayer::loadLevelSettings() {
    m_isPlatformer = m_levelSettings->m_platformerMode;

    m_player1->togglePlatformerMode(m_isPlatformer);
    m_player2->togglePlatformerMode(m_isPlatformer);

    if (m_isPlatformer) {
        m_level->m_levelLength = 5;
    }

    m_player1->m_fixRobotJump = !m_levelSettings->m_fixRobotJump;
    m_player2->m_fixRobotJump = !m_levelSettings->m_fixRobotJump;
    m_player1->m_decreaseBoostSlide = !m_levelSettings->m_decreaseBoostSlide;
    m_player2->m_decreaseBoostSlide = !m_levelSettings->m_decreaseBoostSlide;

    if (m_level->m_timestamp <= 0) {
        m_level->m_timestamp = m_levelSettings->m_propertykA44;
    }

    m_sectionYFactor = LevelSettingsObject::shouldUseYSection(m_levelSettings) ? 0.01f : 0.0f;

    if (!m_objects) return;
    cocos2d::ccArray* data = m_objects->data;
    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = static_cast<GameObject*>(data->arr[i]);
        if (!obj) break;
        applyLevelSettings(obj);
    }
}

void GJBaseGameLayer::loadStartPosObject() {
    m_startOptimization = GameManager::sharedState()->getGameVariable("0151");

    m_loadingStartPosition = true;
    m_player1->doReversePlayer(false);
    m_player1->rotateGameplayOnly(0);

    g_loadUpToPositionGate = true;

    cocos2d::CCPoint pos = m_player1->getPosition();
    loadUpToPosition(pos.x,
                     m_startPosObject->m_startSettings->m_targetOrder,
                     m_startPosObject->m_startSettings->m_targetChannel);

    g_loadUpToPositionGate = false;

    m_loadingStartPosition = false;

    if (m_startPosObject->m_startSettings->m_resetCamera) {
        m_gameState.m_cameraEdgeValue0 = 0;
        m_gameState.m_cameraEdgeValue1 = 0;
        m_gameState.m_cameraEdgeValue2 = 0;
        m_gameState.m_cameraEdgeValue3 = 0;

        updateScreenRotation(0.0f, false, false, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f);

        m_cameraWidthOffset = 0.0f;
        m_cameraHeightOffset = 0.0f;
        m_cameraUnzoomedHeightOffset = 0.0f;
        m_targetCameraHeightOffset = 0.0f;
        m_calculateTargetHeightOffset = true;

        m_gameState.m_cameraStepDiff = cocos2d::CCPoint(0.0f, 0.0f);
        m_gameState.m_unkPoint10 = cocos2d::CCPoint(0.0f, 0.0f);

        resetStaticCamera(true, true);
    }

    setupLevelStart(m_startPosObject->m_startSettings);
    m_startOptimization = false;
}

void GJBaseGameLayer::loadUpToPosition(float position, int order, int channel) {
    resetSPTriggered();
    resetSongTriggerValues();

    cocos2d::CCPoint targetPos(position, 0.0f);
    float time = timeForPos(targetPos, order, channel, false, 0);

    float totalTime;
    if (time > 3600.0f) {
        totalTime = 216000.0f;
        time = 3600.0f;
    } else {
        totalTime = time / 0.016667f;
    }

    int steps = (int)(ceilf(totalTime) + 1.0f);

    cocos2d::CCPoint startPos = m_player1->getPosition();
    m_player1->setPosition(cocos2d::CCPoint(0.0f, 0.0f));

    if (steps > 0) {
        float elapsed = 0.0f;
        int stepIndex = 0;
        do {
            float dt;
            if (time >= elapsed + 0.016667f) {
                dt = 0.016667f;
                elapsed += 0.016667f;
            } else {
                dt = time - elapsed;
                elapsed = (time - elapsed) + elapsed;
            }

            double newTotalTime = m_gameState.m_totalTime + dt;
            m_gameState.m_levelTime += dt;
            m_gameState.m_totalTime = newTotalTime;

            cocos2d::CCPoint newPos = posForTime(elapsed);
            float deltaX = newPos.x - m_player1->getPosition()->x;
            float deltaY = newPos.y - m_player1->getPosition()->y;
            m_player1->setPosition(newPos);

            GJEffectManager::updateEffects(m_effectManager, dt);
            GJEffectManager::preCollisionCheck(m_effectManager);

            m_spawnTuples.clear();
            GJEffectManager::updateSpawnTriggers(m_effectManager, dt);
            m_spawnTuples.clear();

            m_effectManager->m_unk780 = deltaX;
            m_effectManager->m_unk784 = deltaY;
            m_effectManager->m_unk788 = deltaX;
            m_effectManager->m_unk78C = deltaY;

            GJEffectManager::updateTimers(m_effectManager, dt, 1.0f);
            GJEffectManager::prepareMoveActions(m_effectManager, dt, false);

            bool isLast = (steps - 1 == stepIndex);
            stepIndex++;
            processMoveActionsStep(dt, isLast);
            GJEffectManager::postMoveActions(m_effectManager);

            updateCollisionBlocks();
            checkSpawnObjects();

            m_gameState.m_currentChannel = m_gameState.m_rotateChannel;
            checkSpawnObjects();

            GJEffectManager::postCollisionCheck(m_effectManager);
            GJEffectManager::processColors(m_effectManager);
        } while (stepIndex != steps);
    }

    m_player1->m_maybeReverseSpeed = 0.0;
    m_player1->m_maybeReverseAcceleration = 0.0;
    m_player1->setYVelocity(0.0f, false);

    toggleLockPlayer(false, true);
    toggleLockPlayer(false, false);

    m_effectManager->m_unk780 = 0.0f;
    m_effectManager->m_unk784 = 0.0f;
    m_effectManager->m_unk788 = 0.0f;
    m_effectManager->m_unk78C = 0.0f;

    m_player1->setPosition(startPos);
}

int GJBaseGameLayer::maxZOrderForShaderZ(int zLayer) {
    switch (zLayer) {
        case 1:
        case 2:
            return -1600;
        case 3: case 4: case 5: case 6: case 7:
        case 9: case 10: case 11: case 12: case 13: case 14:
            return minZOrderForShaderZ(zLayer + 1) - 1;
        case 8:
            return 61;
        case 15:
            return 1400;
        default:
            return 0;
    }
}

int GJBaseGameLayer::minZOrderForShaderZ(int zLayer) {
    static const int s_zOrderMap[] = {
        -1600, -1400, -1200, -1000, -800, -600, -400, -200,
        200, 400, 600, 800, 1000, 1200, 1400
    };
    int idx = zLayer - 1;
    if (idx >= 0 && idx < 15) {
        return s_zOrderMap[idx];
    }
    return 0;
}

void GJBaseGameLayer::modifyGroupPhysics(AdvancedFollowEditObject* object, cocos2d::CCArray* group) {
    if (!group) return;

    cocos2d::ccArray* data = group->data;
    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = static_cast<GameObject*>(data->arr[i]);
        if (!obj) break;

        auto it = m_gameState.m_gameObjectPhysics.find(obj->m_uniqueID);
        if (it != m_gameState.m_gameObjectPhysics.end()) {
            modifyObjectPhysics(object, it->second);
        }
    }
}

void GJBaseGameLayer::modifyObjectPhysics(AdvancedFollowEditObject* object, GameObjectPhysics& physics) {
    float modX = object->m_modX + object->m_modXVariance * GameToolbox::fast_rand_0_1();
    float modY = object->m_modY + object->m_modYVariance * GameToolbox::fast_rand_0_1();

    physics.m_unkPoint1.x *= modX;
    physics.m_unkPoint1.y *= modY;

    float speed = object->m_startSpeed + object->m_startSpeedVariance * GameToolbox::fast_rand_0_1();
    if (speed != 0.0f) {
        float direction = object->m_startDirection
                        + object->m_startDirectionVariance * GameToolbox::fast_rand_0_1();

        cocos2d::CCPoint result = getFollowSpeedVal(physics.m_gameObject,
                                                    object->m_startSpeedReference,
                                                    object->m_startDirectionReference,
                                                    direction, speed);

        if (!object->m_xOnly) {
            physics.m_unkPoint1.y += result.y;
        }
        if (!object->m_yOnly) {
            physics.m_unkPoint1.x += result.x;
        }
    }
}

void GJBaseGameLayer::moveAreaObject(GameObject* object, float dx, float dy) {
    bool valuesReset = resetAreaObjectValues(object, true);

    if (dx == 0.0f && dy == 0.0f) {
        if (valuesReset) updateObjectSection(object);
        return;
    }

    bool applyX = (dx != 0.0f);

    if (applyX && dy == 0.0f) {
        GameObject::dirtifyObjectPos(object);
        GameObject::dirtifyObjectRect(object);
    }

    if (dy != 0.0f) {
        object->m_positionY += dy;
        object->m_positionYOffset += dy;
        GameObject::dirtifyObjectPos(object);
        GameObject::dirtifyObjectRect(object);
    }

    if (applyX && !object->m_tempOffsetXRelated) {
        double posX = object->m_positionX;
        object->m_positionXOffset += dx;
        object->m_positionX = posX + dx;
    }

    updateObjectSection(object);
}

void GJBaseGameLayer::moveCameraToPos(cocos2d::CCPoint pos) {
    cameraMoveX(pos.x, 1.2f, 1.8f, false);
    cameraMoveY(pos.y, 1.2f, 1.8f, false);
}

void GJBaseGameLayer::moveObject(GameObject* object, double dx, double dy, bool lockPlayerY) {
    m_objectsToMove->addObject(object);
    moveObjects(m_objectsToMove, dx, dy, lockPlayerY);
    m_objectsToMove->removeObjectAtIndex(0);
}

void GJBaseGameLayer::moveObjects(cocos2d::CCArray* objects, double dx, double dy, bool lockPlayerY) {
    if (!objects) return;

    int count = cocos2d::CCArray::count(objects);
    m_movedCount += count;

    cocos2d::ccArray* data = objects->data;
    if (data->num) {
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;

            if (!obj->m_unk3ee) {
                unsigned int commandIndex = m_gameState.m_commandIndex;
                if (obj->m_unk4C4 != commandIndex) {
                    obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                    obj->m_unk4C4 = commandIndex;
                    GameObject::dirtifyObjectRect(obj);
                }
            }

            if (dx != 0.0 && !obj->m_tempOffsetXRelated) {
                obj->m_positionX += dx;
            }
            if (dy != 0.0) {
                obj->m_positionY += dy;
            }

            GameObject::dirtifyObjectPos(obj);
            updateObjectSection(obj);
        }
    }

    updateDisabledObjectsLastPos(objects);
}

void GJBaseGameLayer::moveObjectsSilent(int groupId, double dx, double dy) {
    cocos2d::CCArray* group = getGroup(groupId);
    if (!group) return;

    cocos2d::ccArray* data = group->data;
    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = static_cast<GameObject*>(data->arr[i]);
        if (!obj) break;

        if (!obj->m_tempOffsetXRelated) {
            obj->m_positionX += dx;
        }
        obj->m_positionY += dy;

        GameObject::dirtifyObjectRect(obj);
        GameObject::dirtifyObjectPos(obj);
        updateObjectSection(obj);

        obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
    }

    GJEffectManager::saveCompletedMove(m_effectManager, groupId, dx, dy);
}

void GJBaseGameLayer::moveObjectToStaticGroup(GameObject* object) {
    if (!object->m_isDecoration2) return;

    int groupCount = object->m_groupCount;
    object->m_isDecoration2 = false;

    for (int i = 0; i < groupCount; ++i) {
        int groupID = GameObject::getGroupID(object, i);
        cocos2d::CCArray* staticGroup = getStaticGroup(groupID);
        cocos2d::CCArray* optimizedGroup = getOptimizedGroup(groupID);

        if (!cocos2d::CCArray::containsObject(staticGroup, object)) {
            cocos2d::CCArray::addObject(staticGroup, object);
        }
        cocos2d::CCArray::removeObject(optimizedGroup, object, true);
    }
}

bool GJBaseGameLayer::objectIntersectsCircle(GameObject* object, GameObject* circle) {
    cocos2d::CCRect rect = *object->getObjectRect();
    float radius = GameObject::getObjectRadius(circle);
    cocos2d::CCPoint center = circle->getRealPosition();

    float maxX = rect.getMaxX();
    float minX = rect.getMinX();
    float maxY = rect.getMaxY();
    float minY = rect.getMinY();

    if (rect.containsPoint(center)) return true;

    cocos2d::CCPoint corner;

    corner = cocos2d::CCPoint(maxX, maxY);
    if (radius > fabsf(cocos2d::ccpDistance(corner, center))) return true;

    corner = cocos2d::CCPoint(maxX, minY);
    if (radius > fabsf(cocos2d::ccpDistance(corner, center))) return true;

    corner = cocos2d::CCPoint(minX, minY);
    if (radius > fabsf(cocos2d::ccpDistance(corner, center))) return true;

    corner = cocos2d::CCPoint(minX, maxY);
    return radius > fabsf(cocos2d::ccpDistance(corner, center));
}

GJGameEvent GJBaseGameLayer::objectTypeToGameEvent(int type) {
    static const GJGameEvent s_typeToEvent[] = {
        GJGameEvent::PortalNormal,     
        GJGameEvent::PortalGravityFlip,  
        GJGameEvent::PortalShip,       
        GJGameEvent::PortalGravityNormal, 
        GJGameEvent::PortalGravityInvert,  
        GJGameEvent::YellowPad,         
        GJGameEvent::PinkPad,   
        GJGameEvent::RedPad,           
        GJGameEvent::YellowOrb,     
        GJGameEvent::PinkOrb,        
        GJGameEvent::GravityOrb,    
        GJGameEvent::PortalFlip,       
        GJGameEvent::PortalUnFlip,    
        GJGameEvent::PortalBall,       
        GJGameEvent::PortalNormalScale,   
        GJGameEvent::PortalMiniScale,    
        GJGameEvent::PortalUFO,     
        GJGameEvent::None,             
        GJGameEvent::None,         
        GJGameEvent::PortalDualOn,   
        GJGameEvent::PortalDualOff,      
        GJGameEvent::None,           
        GJGameEvent::None,              
        GJGameEvent::PortalWave,      
        GJGameEvent::PortalRobot,      
        GJGameEvent::None,               
        GJGameEvent::GreenOrb,           
        GJGameEvent::None,          
        GJGameEvent::None,              
        GJGameEvent::DropOrb,         
        GJGameEvent::PortalSpider,      
        GJGameEvent::RedPad,            
        GJGameEvent::RedOrb,          
        GJGameEvent::CustomOrb,        
        GJGameEvent::DashOrb,          
        GJGameEvent::GravityDashOrb,      
        GJGameEvent::None,             
        GJGameEvent::None,             
        GJGameEvent::PortalSwing,     
        GJGameEvent::None,             
        GJGameEvent::SpiderOrb,          
        GJGameEvent::SpiderPad,      
        GJGameEvent::None,               
        GJGameEvent::TeleportOrb,        
    };

    unsigned int idx = (unsigned int)(type - 3);
    if (idx > 0x2B) return GJGameEvent::None;
    return s_typeToEvent[idx];
}

void GJBaseGameLayer::optimizeMoveGroups() {
    std::set<int> targets;
    std::set<int> transformGroups;

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;

            int objectID = obj->m_objectID;
            if (objectID == 2915) {
                targets.insert(obj->m_uOrderOfArrival);
            } else if (objectID == 1346) {
                transformGroups.insert(obj->m_uOrderOfArrival);
                targets.insert(obj->m_unknown5);
                targets.insert(obj->m_eObjType);
            } else if (objectID == 901) {
                targets.insert(obj->m_unknown5);
                targets.insert((int)obj->m_eObjType);
            } else if (objectID == 2067) {
                targets.insert(obj->m_unknown5);
                transformGroups.insert(obj->m_uOrderOfArrival);
            } else if (objectID == 1347 || objectID == 3016 || objectID == 3661) {
                targets.insert(obj->m_unknown5);
            } else if (objectID >= 3006 && objectID <= 3008) {
                int groupID = obj->m_nLuaID;
                if (groupID == 0) groupID = obj->m_unknown5;
                if (groupID > 0) targets.insert(groupID);
            }
        }
    }

    addRemapTargets(&targets);
    for (int groupID : targets) {
        GameObject* mainObject = tryGetMainObject(groupID);
        if (mainObject) moveObjectToStaticGroup(mainObject);
    }

    addRemapTargets(&transformGroups);
    for (int groupID : transformGroups) {
        cocos2d::CCArray* group = getGroup(groupID);
        if (!group) continue;
        for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
            GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
            moveObjectToStaticGroup(obj);
        }
    }
}

void GJBaseGameLayer::orderSpawnObjects() {
    m_spawnObjects->removeAllObjects();

    cocos2d::CCArray* spawnArray = m_spawnObjectsArray;
    if (spawnArray) {
        cocos2d::ccArray* data = spawnArray->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;
            obj->update();
        }
    }

    LevelTools::sortChannelOrderObjects(spawnArray, m_spawnObjects, 0);
}

cocos2d::CCNode* GJBaseGameLayer::parentForZLayer(int zLayer, bool blending, int parentMode, int uiObject) {
    if (uiObject) {
        if (parentMode != 5) {
            int uiKey = 100000 * parentMode + 1000000 + 100 * zLayer + (int)blending;
            cocos2d::CCNode* existing = (cocos2d::CCNode*)cocos2d::CCDictionary::objectForKey(m_uiObjectLayers, uiKey);
            if (existing) return existing;

            cocos2d::CCNode* baseParent = parentForZLayer(zLayer, blending, parentMode, 0);
            cocos2d::CCNode* result;
            if (parentMode == 4) {
                result = CCNodeContainer::create();
            } else {
                cocos2d::CCTexture2D* tex = ((cocos2d::CCSpriteBatchNode*)baseParent)->getTexture();
                result = cocos2d::CCSpriteBatchNode::createWithTexture(tex, 100);
                if (blending) {
                    ((cocos2d::CCSpriteBatchNode*)result)->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                }
            }
            cocos2d::CCDictionary::setObject(m_uiObjectLayers, result, uiKey);
            m_uiTriggerUI->addChild(result, baseParent->getZOrder());
            return result;
        }

        int uiKey = 100 * zLayer + 1500001;
        cocos2d::CCNode* existing = (cocos2d::CCNode*)cocos2d::CCDictionary::objectForKey(m_uiObjectLayers, uiKey);
        if (existing) return existing;

        cocos2d::CCNode* baseParent = parentForZLayer(zLayer, true, 5, 0);
        cocos2d::CCTexture2D* tex = ((cocos2d::CCSpriteBatchNode*)baseParent)->getTexture();
        cocos2d::CCNode* result = cocos2d::CCSpriteBatchNode::createWithTexture(tex, 100);
        ((cocos2d::CCSpriteBatchNode*)result)->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        cocos2d::CCDictionary::setObject(m_uiObjectLayers, result, uiKey);
        m_uiTriggerUI->addChild(result, baseParent->getZOrder());
        return result;
    }

    if (parentMode == 5) {
        if (zLayer < -4) return m_glowLayerB5;
        if (zLayer < -2) return m_glowLayerB4;
        if (zLayer < 0)  return m_glowLayerB3;
        if (zLayer <= 1) return m_glowLayerB2;
        if (zLayer <= 4) return m_glowLayerB1;
        if (zLayer == 5) return m_glowLayerT1;
        if (zLayer <= 7) return m_glowLayerT2;
        if (zLayer > 9)  return m_glowLayerT4;
        return m_glowLayerT3;
    }

    if (parentMode == 4) {
        if (zLayer < -4) return m_specialLayerB5;
        if (zLayer < -2) return m_specialLayerB4;
        if (zLayer < 0)  return m_specialLayerB3;
        if (zLayer <= 1) return m_specialLayerB2;
        if (zLayer <= 3) return m_specialLayerB1;
        if (zLayer <= 5) return m_specialLayerT1;
        if (zLayer <= 7) return m_specialLayerT2;
        if (zLayer > 9)  return m_specialLayerT4;
        return m_specialLayerT3;
    }

    if (blending) {
        switch (parentMode) {
            case 1:
                if (zLayer < -4) return m_game2BlendingLayerB5;
                if (zLayer < -2) return m_game2BlendingLayerB4;
                if (zLayer < 0)  return m_game2BlendingLayerB3;
                if (zLayer <= 1) return m_game2BlendingLayerB2;
                if (zLayer <= 3) return m_game2BlendingLayerB1;
                if (zLayer == 4) return m_game2BlendingLayerB0;
                if (zLayer == 5) return m_game2BlendingLayerT1;
                if (zLayer <= 7) return m_game2BlendingLayerT2;
                if (zLayer <= 9) return m_game2BlendingLayerT3;
                if (zLayer > 11) return m_game2BlendingLayerT5;
                return m_game2BlendingLayerT4;
            case 2:
                if (m_increasedLayerCapacity) return m_textBlendingLayerB1;
                if (zLayer < -4) return m_textBlendingLayerB5;
                if (zLayer < -2) return m_textBlendingLayerB4;
                if (zLayer < 0)  return m_textBlendingLayerB3;
                if (zLayer <= 1) return m_textBlendingLayerB2;
                if (zLayer <= 3) return m_textBlendingLayerB1;
                if (zLayer <= 5) return m_textBlendingLayerT1;
                if (zLayer <= 7) return m_textBlendingLayerT2;
                if (zLayer > 9)  return m_textBlendingLayerT4;
                return m_textBlendingLayerT3;
            case 3:
                if (m_increasedLayerCapacity) return m_fireBlendingLayerB1;
                if (zLayer < -4) return m_fireBlendingLayerB5;
                if (zLayer < -2) return m_fireBlendingLayerB4;
                if (zLayer < 0)  return m_fireBlendingLayerB3;
                if (zLayer <= 1) return m_fireBlendingLayerB2;
                if (zLayer <= 3) return m_fireBlendingLayerB1;
                if (zLayer == 4) return m_fireBlendingLayerB0;
                if (zLayer == 5) return m_fireBlendingLayerT1;
                if (zLayer <= 7) return m_fireBlendingLayerT2;
                if (zLayer <= 9) return m_fireBlendingLayerT3;
                if (zLayer > 11) return m_fireBlendingLayerT5;
                return m_fireBlendingLayerT4;
            case 6:
                if (zLayer < -4) return m_pixelBlendingLayerB5;
                if (zLayer < -2) return m_pixelBlendingLayerB4;
                if (zLayer < 0)  return m_pixelBlendingLayerB3;
                if (zLayer <= 1) return m_pixelBlendingLayerB2;
                if (zLayer <= 3) return m_pixelBlendingLayerB1;
                if (zLayer == 4) return m_pixelBlendingLayerB0;
                if (zLayer == 5) return m_pixelBlendingLayerT1;
                if (zLayer <= 7) return m_pixelBlendingLayerT2;
                if (zLayer <= 9) return m_pixelBlendingLayerT3;
                if (zLayer > 11) return m_pixelBlendingLayerT5;
                return m_pixelBlendingLayerT4;
            case 8:
                if (zLayer < -4) return m_particleBlendingLayerB5;
                if (zLayer < -2) return m_particleBlendingLayerB4;
                if (zLayer < 0)  return m_particleBlendingLayerB3;
                if (zLayer <= 1) return m_particleBlendingLayerB2;
                if (zLayer <= 3) return m_particleBlendingLayerB1;
                if (zLayer == 4) return m_particleBlendingLayerB0;
                if (zLayer == 5) return m_particleBlendingLayerT1;
                if (zLayer <= 7) return m_particleBlendingLayerT2;
                if (zLayer <= 9) return m_particleBlendingLayerT3;
                if (zLayer > 11) return m_particleBlendingLayerT5;
                return m_particleBlendingLayerT4;
            default:
                if (zLayer < -4) return m_gameBlendingLayerB5;
                if (zLayer < -2) return m_gameBlendingLayerB4;
                if (zLayer < 0)  return m_gameBlendingLayerB3;
                if (zLayer <= 1) return m_gameBlendingLayerB2;
                if (zLayer <= 3) return m_gameBlendingLayerB1;
                if (zLayer == 4) return m_gameBlendingLayerB0;
                if (zLayer == 5) return m_gameBlendingLayerT1;
                if (zLayer <= 7) return m_gameBlendingLayerT2;
                if (zLayer <= 9) return m_gameBlendingLayerT3;
                if (zLayer > 11) return m_gameBlendingLayerT5;
                return m_gameBlendingLayerT4;
        }
    }

    switch (parentMode) {
        case 1:
            if (zLayer < -4) return m_game2LayerB5;
            if (zLayer < -2) return m_game2LayerB4;
            if (zLayer < 0)  return m_game2LayerB3;
            if (zLayer <= 1) return m_game2LayerB2;
            if (zLayer <= 3) return m_game2LayerB1;
            if (zLayer == 4) return m_game2LayerB0;
            if (zLayer == 5) return m_game2LayerT1;
            if (zLayer <= 7) return m_game2LayerT2;
            if (zLayer > 9)  return m_game2LayerT4;
            return m_game2LayerT3;
        case 2:
            if (m_increasedLayerCapacity) return m_textLayerB1;
            if (zLayer < -4) return m_textLayerB5;
            if (zLayer < -2) return m_textLayerB4;
            if (zLayer < 0)  return m_textLayerB3;
            if (zLayer <= 1) return m_textLayerB2;
            if (zLayer <= 3) return m_textLayerB1;
            if (zLayer <= 5) return m_textLayerT1;
            if (zLayer <= 7) return m_textLayerT2;
            if (zLayer > 9)  return m_textLayerT4;
            return m_textLayerT3;
        case 3:
            if (m_increasedLayerCapacity) return m_fireLayerB1;
            if (zLayer < -4) return m_fireLayerB5;
            if (zLayer < -2) return m_fireLayerB4;
            if (zLayer < 0)  return m_fireLayerB3;
            if (zLayer <= 1) return m_fireLayerB2;
            if (zLayer <= 3) return m_fireLayerB1;
            if (zLayer <= 5) return m_fireLayerT1;
            if (zLayer <= 7) return m_fireLayerT2;
            if (zLayer > 9)  return m_fireLayerT4;
            return m_fireLayerT3;
        case 6:
            if (zLayer < -4) return m_pixelLayerB5;
            if (zLayer < -2) return m_pixelLayerB4;
            if (zLayer < 0)  return m_pixelLayerB3;
            if (zLayer <= 1) return m_pixelLayerB2;
            if (zLayer <= 3) return m_pixelLayerB1;
            if (zLayer <= 5) return m_pixelLayerT1;
            if (zLayer <= 7) return m_pixelLayerT2;
            if (zLayer > 9)  return m_pixelLayerT4;
            return m_pixelLayerT3;
        case 8:
            if (zLayer < -4) return m_particleLayerB5;
            if (zLayer < -2) return m_particleLayerB4;
            if (zLayer < 0)  return m_particleLayerB3;
            if (zLayer <= 1) return m_particleLayerB2;
            if (zLayer <= 3) return m_particleLayerB1;
            if (zLayer <= 5) return m_particleLayerT1;
            if (zLayer <= 7) return m_particleLayerT2;
            if (zLayer > 9)  return m_particleLayerT4;
            return m_particleLayerT3;
        default:
            if (zLayer < -4) return m_gameLayerB5;
            if (zLayer < -2) return m_gameLayerB4;
            if (zLayer < 0)  return m_gameLayerB3;
            if (zLayer <= 1) return m_gameLayerB2;
            if (zLayer <= 3) return m_gameLayerB1;
            if (zLayer <= 5) return m_gameLayerT1;
            if (zLayer <= 7) return m_gameLayerT2;
            if (zLayer <= 9) return m_gameLayerT3;
            return m_gameLayerT4;
    }
}

void GJBaseGameLayer::pauseAudio() {
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    FMODAudioEngine::pauseAllAudio(engine);
    FMODAudioEngine::pauseAllMusic(engine, true);
    engine->m_system->update();
}

double GJBaseGameLayer::performMathOperation(double operand1, double operand2, int operation) {
    switch (operation) {
        case 1: return operand1 + operand2;
        case 2: return operand1 - operand2;
        case 3: return operand1 * operand2;
        case 4: return operand2 == 0.0 ? 0.0 : operand1 / operand2;
        default: return 0.0;
    }
}

double GJBaseGameLayer::performMathRounding(double value, int type) {
    switch (type) {
        case 1: return round(value);
        case 2: return floor(value);
        case 3: return ceil(value);
        default: return value;
    }
}

void GJBaseGameLayer::pickupItem(EffectGameObject* object) {
    std::string key = cocos2d::CCString::createWithFormat("%i", object->m_secretCoinID)->getCString();
    cocos2d::CCDictionary::setObject(m_collectedItems, object, key);
}

void GJBaseGameLayer::playAnimationCommand(int id, int groupID) {
    if (groupID <= 0) return;

    cocos2d::CCArray* group = getGroup(groupID);
    unsigned int count = cocos2d::CCArray::count(group);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
        if (obj->m_objectID == 2) {
            ((AnimatedGameObject*)obj)->playAnimation(id);
        } else {
            obj->getTag();
        }
    }
}

void GJBaseGameLayer::playExitDualEffect(PlayerObject* player) {
    GameManager* gm = GameManager::sharedState();
    SimplePlayer* icon = SimplePlayer::create(gm->m_playerFrame.random - gm->m_playerFrame.seed);

    if (player->m_isBall) {
        SimplePlayer::updatePlayerFrame(icon, gm->m_playerBall.random - gm->m_playerBall.seed, IconType::Ball);
    } else if (player->m_isShip) {
        SimplePlayer::updatePlayerFrame(icon, gm->m_playerShip.random - gm->m_playerShip.seed, IconType::Ship);
    } else if (player->m_isBird) {
        SimplePlayer::updatePlayerFrame(icon, gm->m_playerBird.random - gm->m_playerBird.seed, IconType::Ufo);
    } else if (player->m_isDart) {
        SimplePlayer::updatePlayerFrame(icon, gm->m_playerDart.random - gm->m_playerDart.seed, IconType::Wave);
    } else if (player->m_isRobot) {
        SimplePlayer::updatePlayerFrame(icon, gm->m_playerRobot.random - gm->m_playerRobot.seed, IconType::Robot);
    } else if (player->m_isSpider) {
        SimplePlayer::updatePlayerFrame(icon, gm->m_playerSpider.random - gm->m_playerSpider.seed, IconType::Spider);
    }

    icon->setScaleX(player->getScaleX() * m_gameState.m_cameraZoom);
    icon->setScaleY(player->getScaleY() * m_gameState.m_cameraZoom);

    cocos2d::CCPoint pos = *player->getPosition();
    float rotation = player->getRotation();
    int direction = 1;

    if (m_gameState.m_unkBool10) {
        rotation = -rotation;
        direction = -1;
        icon->setScaleX(-icon->getScaleX());
        pos += cocos2d::CCPoint(150.0f, 0.0f);
    }

    addChild(icon, 100);
    icon->setColor(player->getColor());
    icon->setSecondColor(player->getSecondColor());
    icon->updateColors();

    pos -= m_gameState.m_cameraPosition;
    pos *= m_gameState.m_cameraZoom;
    icon->setPosition(pos);
    icon->setRotation(rotation);

    cocos2d::CCFadeOut* fade = cocos2d::CCFadeOut::create(0.4f);
    cocos2d::CCPoint targetPos = icon->getPosition() + cocos2d::CCPoint((float)direction * -124.63f, 0.0f);
    cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.4f, targetPos);

    float targetScale = icon->getScaleX() <= 0.0f ? -0.75f : 0.75f;
    cocos2d::CCScaleTo* scale = cocos2d::CCScaleTo::create(0.4f, targetScale);
    cocos2d::CCRotateBy* rotate = cocos2d::CCRotateBy::create(0.4f, 180.0f);
    cocos2d::CCCallFunc* remove = cocos2d::CCCallFunc::create(icon, 520, 1);
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(scale, remove, nullptr);

    icon->runAction(fade);
    icon->runAction(move);
    icon->runAction(seq);
    icon->runAction(rotate);

    GLubyte r = player->m_playerColor1.r;
    GLubyte g = player->m_playerColor1.g;
    GLubyte b = player->m_playerColor1.b;

    CCCircleWave* wave = CCCircleWave::create(10.0f, 30.0f, 0.4f, 0);
    wave->m_color.r = r;
    wave->m_color.g = g;
    wave->m_color.b = b;
    wave->setPosition(&pos);
    addChild(wave, 0);
    wave->m_circleMode = Outline;
    CCCircleWave::followObject(wave, icon, 0);
}

void GJBaseGameLayer::playFlashEffect(float duration, int flashes, float unknown) {
    m_unk31f8 = unknown;
    m_flashNode->setVisible(true);

    cocos2d::CCHide* hide = cocos2d::CCHide::create();
    cocos2d::CCSequence* seq;
    if (flashes <= 1) {
        seq = cocos2d::CCSequence::create(cocos2d::CCDelayTime::create(duration), hide, nullptr);
    } else {
        seq = cocos2d::CCSequence::create(cocos2d::CCBlink::create(duration, flashes - 1), hide, nullptr);
    }

    seq->setTag(15);
    m_flashNode->runAction(seq);
}

void GJBaseGameLayer::playGravityEffect(bool flip) {
}

void GJBaseGameLayer::playKeyframeAnimation(KeyframeAnimTriggerObject* object, std::vector<int> const& remapKeys) {
    cocos2d::CCArray* group = getGroup(object->m_animationID);

    std::unordered_set<int> processedKeyframeGroups;

    if (group) {
        unsigned int count = cocos2d::CCArray::count(group);
        for (unsigned int i = 0; i < count; ++i) {
            GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
            if (obj->m_objectID != 3032) continue;

            int keyframeGroupID = obj->m_keyframeGroup;
            if (processedKeyframeGroups.find(keyframeGroupID) != processedKeyframeGroups.end()) continue;
            processedKeyframeGroups.insert(keyframeGroupID);

            cocos2d::CCArray* keyframeGroup = (cocos2d::CCArray*)cocos2d::CCDictionary::objectForKey(
                m_keyframeGroups, keyframeGroupID);
            GameObject* centerObject = tryGetMainObject(object->m_centerGroupID);

            int targetGroupID = object->m_targetGroupID;
            if (targetGroupID == 0) targetGroupID = obj->m_uOrderOfArrival;

            GJEffectManager::createKeyframeCommand(
                m_effectManager,
                targetGroupID,
                keyframeGroup,
                centerObject,
                object->m_uniqueID,
                object->m_controlID,
                0,
                object->m_positionXMod,
                object->m_positionYMod,
                object->m_rotationMod,
                object->m_scaleXMod,
                object->m_scaleYMod,
                object->m_timeMod,
                remapKeys);
        }
    }
}

void GJBaseGameLayer::playSpeedParticle(float timeMod) {
    if (GameManager::sharedState()->m_performanceMode || m_skipArtReload) return;

    const char* plist;
    if (timeMod == 0.7f)       plist = "speedEffect_slow.plist";
    else if (timeMod == 1.1f)  plist = "speedEffect_fast.plist";
    else if (timeMod == 1.3f)  plist = "speedEffect_vfast.plist";
    else if (timeMod == 1.6f)  plist = "speedEffect_vvfast.plist";
    else                       plist = "speedEffect_normal.plist";

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCParticleSystemQuad* particles = cocos2d::CCParticleSystemQuad::create(plist);
    particles->setPositionType(kCCPositionTypeGrouped);
    particles->setAutoRemoveOnFinish(true);
    addChild(particles, 100);

    float screenRight = cocos2d::CCDirector::sharedDirector()->getScreenRight();
    particles->setPosition(cocos2d::CCPoint(screenRight + 10.0f, winSize.height * 0.5f));

    float posVarY = cocos2d::CCDirector::sharedDirector()->getScreenScaleFactorH() * 320.0f * 0.5f;
    particles->setPosVar(cocos2d::CCPoint(10.0f, posVarY));
    particles->resetSystem();
}

bool GJBaseGameLayer::playerCircleCollision(PlayerObject* player, GameObject* object) {
    if (m_levelSettings->m_fixRadiusCollision) {
        return playerIntersectsCircle(player, object);
    }
    return objectIntersectsCircle(player, object);
}

bool GJBaseGameLayer::playerIntersectsCircle(PlayerObject* player, GameObject* object) {
    cocos2d::CCPoint playerPos = *player->getPosition();
    cocos2d::CCPoint objectPos = object->getRealPosition();
    float distance = cocos2d::ccpDistance(playerPos, objectPos);
    float radius = GameObject::getObjectRadius(object);
    float playerRadius = player->getObjectRect()->size.width * 0.5f;

    return distance <= radius + playerRadius;
}

void GJBaseGameLayer::playerTookDamage(PlayerObject* player) {
}

void GJBaseGameLayer::playerTouchedObject(PlayerObject* player, GameObject* object) {
    auto key = std::make_pair(object->m_uniqueID, player->m_uniqueID);
    m_gameState.m_activatedObjectIDs[key] = m_gameState.m_commandIndex;
}

void GJBaseGameLayer::playerTouchedRing(PlayerObject* player, RingObject* object) {
    if (!object->m_isRingPoweredOn) {
        registerStateObject(object);
        gameEventTriggered(GJGameEvent::OrbTouched, 0, 0);
    }
    PlayerObject::addToTouchedRings(player, object);
    object->powerOnObject(m_gameState.m_commandIndex);
    if (!PlayerObject::isFlying(player) && !object->m_claimTouch) {
        PlayerObject::ringJump(player, object, 0);
    }
}

void GJBaseGameLayer::playerTouchedTrigger(PlayerObject* player, EffectGameObject* object) {
    if (object->m_objectID == 3640) {
        object->powerOnObject(m_gameState.m_commandIndex);
    }

    if (!object->m_isTouchTriggered) return;

    int playerKey = object->m_isSinglePTouch ? 0 : player->m_uniqueID;
    int key[2] = { object->m_uniqueID, playerKey };

    bool exists = m_gameState.m_activatedObjectIDs.find(std::make_pair(key[0], key[1]))
                  != m_gameState.m_activatedObjectIDs.end();

    if (!exists) {
        if (object->m_isMultiTriggered) {
            GJEffectManager::removeTriggeredID(m_effectManager, key[0], key[1]);
        }
        m_gameState.m_activatedObjectIDs[std::make_pair(key[0], key[1])] = 0;

        if (!GJEffectManager::hasBeenTriggered(m_effectManager, key[0], key[1])) {
            GJEffectManager::storeTriggeredID(m_effectManager, key[0], key[1]);

            if (object->m_objectID == 1268) {
                std::vector<int> remap;
                SpawnTriggerGameObject::updateRemapKeys((SpawnTriggerGameObject*)object, &remap);
            }
            object->triggerObject(this, player->m_uniqueID, nullptr);
        }
    } else if (object->m_isMultiTriggered) {
        GJEffectManager::removeTriggeredID(m_effectManager, key[0], key[1]);
        m_gameState.m_activatedObjectIDs[std::make_pair(key[0], key[1])] = m_gameState.m_commandIndex;

        if (!GJEffectManager::hasBeenTriggered(m_effectManager, key[0], key[1])) {
            GJEffectManager::storeTriggeredID(m_effectManager, key[0], key[1]);
            if (object->m_objectID == 1268) {
                std::vector<int> remap;
                SpawnTriggerGameObject::updateRemapKeys((SpawnTriggerGameObject*)object, &remap);
            }
            object->triggerObject(this, player->m_uniqueID, nullptr);
        }
    }

    m_gameState.m_activatedObjectIDs[std::make_pair(key[0], key[1])] = m_gameState.m_commandIndex;
    object->triggerActivated(0.0f);
}

bool GJBaseGameLayer::playerWasTouchingObject(PlayerObject* player, GameObject* object) {
    auto key = std::make_pair(object->m_uniqueID, player->m_uniqueID);
    return m_gameState.m_activatedObjectIDs.find(key) != m_gameState.m_activatedObjectIDs.end();
}

void GJBaseGameLayer::playerWillSwitchMode(PlayerObject* player, GameObject* object) {
    if (!object) return;

    m_gameState.m_lastActivatedPortal1 = object;

    if (object->m_classType == GameObjectClassType::Effect) {
        updateCameraMode((EffectGameObject*)object, false);
    }

    GameObjectType type = object->getType();
    updateDualGround(player, type, false, 0.0f);
    gameEventTriggered(objectTypeToGameEvent(type), 0, 0);

    bool isFlyingPortal = (type == GameObjectType::ShipPortal
                       || type == GameObjectType::UfoPortal
                       || type == GameObjectType::WavePortal
                       || type == GameObjectType::SwingPortal);

    if (!isFlyingPortal) {
        toggleGlitter(false);
    }

    if (!m_gameState.m_isDualMode) return;

    if (m_levelSettings->m_twoPlayerMode) return;

    PlayerObject* other = getOtherPlayer(player);
    int typeOffset = type - 5;

    switch (typeOffset) {
        case 0: 
            if (other->m_isShip) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        case 1:
            if (!PlayerObject::isFlying(other)
                && !other->m_isBall
                && !other->m_isRobot
                && !other->m_isSpider
                && !m_gameState.m_unkBool31) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        case 11: 
            if (other->m_isBall) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        case 14: 
            if (other->m_isBird) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        case 22: 
            if (other->m_isRobot) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        case 28: 
            if (other->m_isSpider) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        case 36: 
            if (other->m_isSwing) {
                PlayerObject::flipGravity(player, !other->m_isUpsideDown, true);
            }
            break;
        default:
            break;
    }
}

cocos2d::CCPoint GJBaseGameLayer::positionForShaderTarget(int groupID) {
    if (groupID == -1) return *m_player1->getPosition();
    if (groupID == -2) {
        if (m_gameState.m_isDualMode) return *m_player2->getPosition();
        return *m_player1->getPosition();
    }
    GameObject* main = tryGetMainObject(groupID);
    if (main) return main->getRealPosition();
    return cocos2d::CCPoint(0.0f, 0.0f);
}

void GJBaseGameLayer::postUpdate(float dt) {}

void GJBaseGameLayer::preResumeGame() {
    m_resumeTimer = 2;
}

void GJBaseGameLayer::prepareTransformParent(bool rotateNode) {
    cocos2d::CCObject::retain(m_areaTransformNode2);
    m_areaTransformNode2->removeFromParentAndCleanup(false);
    cocos2d::CCNode* target = rotateNode ? m_areaRotateNode : m_areaTransformNode;
    target->addChild(m_areaTransformNode2);
    cocos2d::CCObject::release(m_areaTransformNode2);
}

GameObject* GJBaseGameLayer::processCameraObject(GameObject* object, PlayerObject* player) {
    GameObject* portal = object;
    if (object) {
        player->m_lastPortalPos = *object->getPosition();
        player->m_lastActivatedPortal = object;
        if (m_gameState.m_isDualMode && m_gameState.m_lastActivatedPortal2) {
            portal = m_gameState.m_lastActivatedPortal2;
        }
        m_gameState.m_lastActivatedPortal1 = portal;
    } else if (m_gameState.m_isDualMode) {
        portal = m_gameState.m_lastActivatedPortal2;
        if (portal) {
            m_gameState.m_lastActivatedPortal1 = portal;
        }
    }
    return portal;
}

void GJBaseGameLayer::processCheckpoints() {}

void GJBaseGameLayer::processReplayCheckpoint(int id) {}

void GJBaseGameLayer::processMoveActionsStep(float dt, bool visibleFrame) {
    m_unked0 = 0;
    m_disabledObjectsCount = 0;
    m_unked8 = 0;
    m_areaObjectsCount = 0;

    for (DynamicObjectAction& action : m_gameState.m_dynamicMoveActions) {
        CCMoveCNode* node = GJEffectManager::tryGetMoveCommandNode(
            m_effectManager, action.m_gameObject1->m_targetGroupID);
        if (node) node->m_unk0d1 = 1;
    }

    processDynamicObjectActions(1, dt);
    processTransformActions(visibleFrame);
    processRotationActions();
    processDynamicObjectActions(0, dt);
    processMoveActions();
    processPlayerFollowActions(dt);
    processAdvancedFollowActions(dt);
    processFollowActions();
    processAreaActions(dt, visibleFrame);

    if (m_isEditor && m_disabledObjectsCount > 0) {
        for (int i = 0; i < m_disabledObjectsCount; ++i) {
            m_disabledObjects[m_disabledObjectsIndex++]->quickUpdatePosition();
        }
    }
}

void GJBaseGameLayer::processQueuedButtons(float dt, bool clearInputQueue) {
    while (!m_queuedButtons.empty()) {
        const PlayerButtonCommand& cmd = m_queuedButtons.front();
        if (!clearInputQueue && cmd.m_timestamp >= dt + m_timestamp) break;
        handleButton(cmd.m_isPush, cmd.m_button, !cmd.m_isPlayer2);
        m_queuedButtons.erase(m_queuedButtons.begin());
    }
}

void GJBaseGameLayer::processQueuedAudioTriggers() {
    m_startingFromBeginning = false;

    for (SFXTriggerGameObject* trigger : m_activeSfxTriggers) {
        int objectID = trigger->m_objectID;
        if (objectID == 3602) {
            activateSFXTrigger(trigger);
        } else if (objectID == 3603) {
            activateSFXEditTrigger(trigger);
        } else if (objectID == 3605) {
            activateSongEditTrigger((SongTriggerGameObject*)trigger);
        } else if (objectID == 1934) {
            activateSongTrigger((SongTriggerGameObject*)trigger);
        }
    }
    m_activeSfxTriggers.clear();
}

void GJBaseGameLayer::processItems() {
    cocos2d::CCArray* keys = cocos2d::CCDictionary::allKeys(m_collectedItems);
    unsigned int count = cocos2d::CCArray::count(keys);
    if (count == 0) return;

    for (unsigned int i = 0; i < count; ++i) {
        cocos2d::CCString* key = (cocos2d::CCString*)cocos2d::CCArray::objectAtIndex(keys, i);
        GameObject* obj = (GameObject*)cocos2d::CCDictionary::objectForKey(
            m_collectedItems, key->getCString());
        if (!obj) continue;

        int type = obj->getType();
        GameStatsManager* stats = GameStatsManager::sharedState();

        if (type == 22) {
            const char* coinKey = GJGameLevel::getCoinKey(m_level, obj->m_secretCoinID);
            if (!stats->hasSecretCoin(coinKey)) {
                stats->storeSecretCoin(coinKey);
                stats->incrementStat("8");
            }
        } else if (type == 31) {
            const char* coinKey = GJGameLevel::getCoinKey(m_level, obj->m_secretCoinID);
            if (m_level->m_coinsVerified.random == m_level->m_coinsVerified.seed) {
                if (!stats->hasPendingUserCoin(coinKey)) {
                    stats->storePendingUserCoin(coinKey);
                }
            } else {
                if (!stats->hasUserCoin(coinKey)) {
                    stats->storeUserCoin(coinKey);
                    stats->incrementStat("12");
                }
            }
        }
    }
}

void GJBaseGameLayer::processOptionsTrigger(GameOptionsTrigger* object) {
    if (object->m_streakAdditive != GameOptionsSetting::Disabled) {
        togglePlayerStreakBlend(object->m_streakAdditive == GameOptionsSetting::On, false);
    }
    if (object->m_unlinkDualGravity != GameOptionsSetting::Disabled) {
        m_gameState.m_unkBool31 = (object->m_unlinkDualGravity == GameOptionsSetting::On);
    }
    if (object->m_hideGround != GameOptionsSetting::Disabled) {
        toggleGroundVisibility(object->m_hideGround != GameOptionsSetting::On);
    }
    if (object->m_hideMG != GameOptionsSetting::Disabled) {
        toggleMGVisibility(object->m_hideMG != GameOptionsSetting::On);
    }
    if (object->m_hideP1 != GameOptionsSetting::Disabled) {
        togglePlayerVisibility(object->m_hideP1 != GameOptionsSetting::On, true);
    }
    if (object->m_hideP2 != GameOptionsSetting::Disabled) {
        togglePlayerVisibility(object->m_hideP2 != GameOptionsSetting::On, false);
    }
    if (object->m_disableP1Controls != GameOptionsSetting::Disabled) {
        toggleLockPlayer(object->m_disableP1Controls == GameOptionsSetting::On, true);
    }
    if (object->m_disableP2Controls != GameOptionsSetting::Disabled) {
        toggleLockPlayer(object->m_disableP2Controls == GameOptionsSetting::On, false);
    }
    if (object->m_hideAttempts != GameOptionsSetting::Disabled) {
        toggleHideAttempts(object->m_hideAttempts == GameOptionsSetting::On);
    }
    if (object->m_audioOnDeath != GameOptionsSetting::Disabled) {
        m_gameState.m_unkBool22 = (object->m_audioOnDeath == GameOptionsSetting::On);
    }
    if (object->m_noDeathSFX != GameOptionsSetting::Disabled) {
        m_gameState.m_unkBool26 = (object->m_noDeathSFX == GameOptionsSetting::On);
    }
    if (object->m_boostSlide != GameOptionsSetting::Disabled) {
        bool enabled = (object->m_boostSlide == GameOptionsSetting::On);
        m_player1->m_decreaseBoostSlide = enabled;
        m_player2->m_decreaseBoostSlide = enabled;
    }
    if (object->m_editRespawnTime != GameOptionsSetting::Disabled) {
        float respawn = 0.0f;
        if (object->m_editRespawnTime == GameOptionsSetting::On) {
            respawn = std::max(1.0f, std::min(10.0f, object->m_respawnTime));
        }
        m_gameState.m_unkUint14 = respawn;
    }
}

void GJBaseGameLayer::processStateObjects() {
    auto& stateObjects = m_gameState.m_stateObjects;
    for (auto it = stateObjects.begin(); it != stateObjects.end();) {
        EnhancedGameObject* obj = it->second;
        EnhancedGameObject::updateState(obj, m_gameState.m_commandIndex);
        if (obj->m_isRingPoweredOn) {
            ++it;
            continue;
        }
        obj->stateSensitiveOff(this);
        it = stateObjects.erase(it);
    }
}

void GJBaseGameLayer::processSFXObjects() {
    std::unordered_set<int> sfxIDs;
    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;
            if (obj->m_objectID == 3602 && obj->m_preload && obj->m_sfxID > 0) {
                sfxIDs.insert(obj->m_sfxID);
            }
        }
    }

    int totalLength = 0;
    for (int sfxID : sfxIDs) {
        std::string path = MusicDownloadManager::sharedState()->pathForSFX(sfxID);
        FMODSound* sound = FMODAudioEngine::sharedEngine()->preloadEffect(path);
        if (!sound) continue;
        totalLength += sound->m_length;
        if (totalLength > 120000) break;
    }
}

void GJBaseGameLayer::processMoveActions() {
    GJEffectManager::processMoveCalculations(m_effectManager);

    for (CCMoveCNode* node : m_effectManager->m_unkVector6c0) {
        if (node->m_unk0d0) continue;

        int groupID = node->getTag();
        bool lockPlayerY = (node->m_eObjType == GameObjectObjectType);

        cocos2d::CCArray* staticGroup = getStaticGroup(groupID);
        float dx = node->m_unk038;
        float dy = node->m_unk040;
        if (staticGroup && (dx != 0.0f || dy != 0.0f || node->m_unk078)) {
            moveObjects(staticGroup, dx, dy, lockPlayerY);
        }

        cocos2d::CCArray* optimizedGroup = getOptimizedGroup(groupID);
        dx = node->m_unk090;
        dy = node->m_unk098;
        if (optimizedGroup && (dx != 0.0f || dy != 0.0f || node->m_unk078)) {
            moveObjects(optimizedGroup, dx, dy, lockPlayerY);
        }
    }
}

void GJBaseGameLayer::processPlayerFollowActions(float dt) {
    float speed = dt * 60.0f;

    for (CCMoveCNode* node : m_effectManager->m_unkVector6f0) {
        int groupID = node->getTag();
        cocos2d::CCArray* group = getGroup(groupID);
        if (!group) continue;

        float factor = speed * node->m_unk0a8;
        factor = std::max(0.0f, std::min(1.0f, factor));
        float maxDelta = speed * node->m_unk0c0;

        cocos2d::ccArray* data = group->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;

            if (!obj->m_unk3ee) {
                unsigned int cmdIdx = m_gameState.m_commandIndex;
                if (obj->m_unk4C4 != cmdIdx) {
                    obj->m_unk4C4 = cmdIdx;
                    obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                    GameObject::dirtifyObjectRect(obj);
                }
            }

            float oldPos = PlayerObject::getOldPosition(m_player1, node->m_unk0b0);
            if (oldPos == 0.0f) continue;

            float target = oldPos + node->m_unk0b8;
            float delta = factor * (target - obj->m_positionY);
            if (maxDelta > 0.0f) {
                if (maxDelta < delta) delta = maxDelta;
                else if (delta < -maxDelta) delta = -maxDelta;
            }
            if (delta == 0.0f) continue;

            obj->m_positionY += delta;
            GameObject::dirtifyObjectPos(obj);
        }

        updateDisabledObjectsLastPos(group);
    }
}

void GJBaseGameLayer::processFollowActions() {
    auto& followNodes = m_effectManager->m_unkVector6d8;

    for (CCMoveCNode* node : followNodes) {
        int groupID = node->getTag();
        cocos2d::CCArray* group = getGroup(groupID);
        GameObject* target = tryGetMainObject(node->m_unk074);
        if (!target) continue;

        unsigned int cmdIdx = m_gameState.m_commandIndex;
        if (target->m_unk4C4 != cmdIdx) {
            target->m_unk4C4 = cmdIdx;
            target->m_lastPosition = cocos2d::CCPoint(target->m_positionX, target->m_positionY);
            GameObject::dirtifyObjectRect(target);
        }

        float dx = (float)((target->m_positionX - target->m_lastPosition.x) * node->m_unk080);
        float dy = (float)((target->m_positionY - target->m_lastPosition.y) * node->m_unk088);
        node->m_unk038 = dx;
        node->m_unk040 = dy;

        bool hasDelta = (dx != 0.0f) || (dy != 0.0f);

        if (!hasDelta && !node->m_unk078) {
            if (target->m_unk4fb && GJEffectManager::wasFollowing(m_effectManager, groupID, node->m_unk074))
                continue;
            if (!target->m_unk4fb)
                continue;
        }

        if (node->m_groupObject) {
            node->m_groupObject->m_currentXOffset += dx;
            node->m_groupObject->m_currentYOffset += dy;
        }

        GJEffectManager::setFollowing(m_effectManager, groupID, node->m_unk074, hasDelta);
        m_followedCount += cocos2d::CCArray::count(group);

        if (group) {
            cocos2d::ccArray* data = group->data;
            for (unsigned int i = 0; i < data->num; ++i) {
                GameObject* obj = static_cast<GameObject*>(data->arr[i]);
                if (!obj) break;

                if (!obj->m_unk3ee) {
                    unsigned int cIdx = m_gameState.m_commandIndex;
                    if (obj->m_unk4C4 != cIdx) {
                        obj->m_unk4C4 = cIdx;
                        obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                        GameObject::dirtifyObjectRect(obj);
                    }
                }
                if (dy != 0.0f) obj->m_positionY += dy;
                GameObject::dirtifyObjectPos(obj);
                if (dx != 0.0f && !obj->m_tempOffsetXRelated) obj->m_positionX += dx;
                updateObjectSection(obj);
            }
        }

        updateDisabledObjectsLastPos(group);
    }
}


void GJBaseGameLayer::processAdvancedFollowActions(float dt) {
    float speed = dt * 240.0f;
    auto& instances = m_gameState.m_advanceFollowInstances;

    if (m_gameState.m_unkBool27) {
        std::sort(instances.begin(), instances.end(),
            [](const AdvancedFollowInstance& a, const AdvancedFollowInstance& b) {
                return a.m_group < b.m_group;
            });
        m_gameState.m_unkBool27 = false;
    }

    for (AdvancedFollowInstance& instance : instances) {
        if (instance.m_finished) {
            if (instance.m_doStart) {
                instance.m_doStart = false;
                processAdvancedFollowAction(&instance, true, speed);
            }
        } else {
            processAdvancedFollowAction(&instance, instance.m_started, speed);
        }
    }
}

void GJBaseGameLayer::processAreaVisualActions(float dt) {
    processAreaEffects(&m_gameState.m_fadeEffectInstances, GJAreaActionType::Fade, dt, true);
    processAreaEffects(&m_gameState.m_tintEffectInstances, GJAreaActionType::Tint, dt, true);
}

void GJBaseGameLayer::processAdvancedFollowAction(AdvancedFollowInstance& instance,
                                                  bool started, float dt) {
    AdvancedFollowTriggerObject* gameObject = instance.m_gameObject;

    int specialKey = getSpecialKey(instance.m_group,
                                   gameObject->m_ignoreGroupParent,
                                   gameObject->m_ignoreLinkedObjects);

    int index = 0;
    int groupID = instance.m_objectKey;
    int iterations = 1;

    auto tgIt = m_targetGroups.find(specialKey);
    if (tgIt != m_targetGroups.end()) {
        index = tgIt->second.first;
        iterations = index > 0 ? 2 : 1;
    }

    for (int i = 0; i < iterations; ++i) {
        cocos2d::CCArray* group;
        if (index > 0) {
            group = (cocos2d::CCArray*)cocos2d::CCArray::objectAtIndex(
                m_targetGroupsArray, index - 1 + i);
        } else {
            group = getGroup(instance.m_group);
        }
        if (!group) continue;

        cocos2d::ccArray* data = group->data;
        for (unsigned int j = 0; j < data->num; ++j) {
            GameObject* obj = static_cast<GameObject*>(data->arr[j]);
            if (!obj) break;

            if (started) {
                GameObjectPhysics* physics = GJGameState::getGameObjectPhysics(
                    &m_gameState, obj);
                int cmdIdx = m_gameState.m_commandIndex;
                if (physics->m_unkInt2 < cmdIdx) {
                    physics->m_unkInt2 = cmdIdx;
                    if (i == 1) {
                        cocos2d::CCArray* target = getTargetGroup(index, obj->m_uniqueID);
                        moveObjects(target, 0.0, 0.0, false);
                    } else {
                        moveObject(obj, 0.0, 0.0, false);
                    }
                }
                continue;
            }

            GameObjectPhysics* physics = GJGameState::getGameObjectPhysics(&m_gameState, obj);
            if (physics->m_unkInt3 >= (int)m_gameState.m_commandIndex) continue;

            float delayVariance = gameObject->m_delayVariance;
            float delayOffset = delayVariance != 0.0f
                ? delayVariance * m_varianceValues[obj->m_varianceIndex] : 0.0f;

            cocos2d::CCPoint savedPos = getSavedPosition(groupID,
                gameObject->m_delay + delayOffset);

            if (gameObject->m_ignoreDisabled && obj->m_isGroupDisabled) continue;
            if (savedPos.x == 0.0f && savedPos.y == 0.0f) continue;

            cocos2d::CCPoint realPos = obj->getRealPosition();
            cocos2d::CCPoint delta = savedPos - realPos;
            float distance = sqrtf(delta.x * delta.x + delta.y * delta.y);

            float maxSpeedVar = 0.0f;
            if (gameObject->m_maxSpeedVariance != 0.0f) {
                maxSpeedVar = gameObject->m_maxSpeedVariance
                    * m_varianceValues[obj->m_varianceIndex + 1];
            }
            float maxSpeed = gameObject->m_maxSpeed + maxSpeedVar;

            float accelVar = 0.0f;
            if (gameObject->m_accelerationVariance != 0.0f) {
                accelVar = gameObject->m_accelerationVariance
                    * m_varianceValues[obj->m_varianceIndex + 2];
            }
            float accel = gameObject->m_acceleration + accelVar;

            float rangeVar = 0.0f;
            if (gameObject->m_maxRangeVariance != 0.0f) {
                rangeVar = gameObject->m_maxRangeVariance
                    * m_varianceValues[obj->m_varianceIndex + 3];
            }
            float maxRange = gameObject->m_maxRange + rangeVar;

            float checkDist = distance;
            if (instance.m_otherObjectKey > 0) {
                GameObject* other = tryGetMainObject(instance.m_otherObjectKey);
                if (other) {
                    cocos2d::CCPoint otherPos = other->getRealPosition();
                    cocos2d::CCPoint d2 = savedPos - otherPos;
                    checkDist = sqrtf(d2.x * d2.x + d2.y * d2.y);
                }
            }

            if (gameObject->m_xOnly) delta.y = 0.0f;
            else if (gameObject->m_yOnly) delta.x = 0.0f;

            if (maxRange != 0.0f && checkDist > maxRange) continue;

            if (!instance.m_processed) {
                float startSpeed = gameObject->m_startSpeed
                    + gameObject->m_startSpeedVariance * GameToolbox::fast_rand_0_1();
                if (startSpeed != 0.0f) {
                    float startDir = gameObject->m_startDirection
                        + gameObject->m_startDirectionVariance * GameToolbox::fast_rand_0_1();
                    cocos2d::CCPoint speedVal = getFollowSpeedVal(
                        obj, gameObject->m_startSpeedReference,
                        gameObject->m_startDirectionReference, startDir, startSpeed);
                    int startMode = gameObject->m_startMode;
                    if (!physics->m_unkInt2 || startMode == 1) {
                        if (!gameObject->m_xOnly) physics->m_unkPoint1.y = speedVal.y;
                        if (!gameObject->m_yOnly) physics->m_unkPoint1.x = speedVal.x;
                    } else if (startMode == 2) {
                        if (!gameObject->m_xOnly) physics->m_unkPoint1.y += speedVal.y;
                        if (!gameObject->m_yOnly) physics->m_unkPoint1.x += speedVal.x;
                    }
                }
            }

            unsigned int cmdIdx = m_gameState.m_commandIndex;
            instance.m_processed = true;
            physics->m_unkInt2 = cmdIdx;
            if (gameObject->m_exclusive) physics->m_unkInt3 = cmdIdx;

            int followMode = gameObject->m_followMode;
            float accelScaled = accel * 0.01f;
            float steerScale = (gameObject->m_steerForce + gameObject->m_steerForceVariance
                * GameToolbox::fast_rand_0_1()) * 0.01f;
            float steerForceLow = (gameObject->m_steerForceLow + gameObject->m_steerForceLowVariance
                * GameToolbox::fast_rand_0_1()) * 0.01f;
            float steerForceHigh = (gameObject->m_steerFroceHigh + gameObject->m_steerFroceHighVariance
                * GameToolbox::fast_rand_0_1()) * 0.01f;
            float speedRangeLow = gameObject->m_speedRangeLow + gameObject->m_speedRangeLowVariance
                * GameToolbox::fast_rand_0_1();
            float speedRangeHigh = gameObject->m_speedRangeHigh + gameObject->m_speedRangeHighVariance
                * GameToolbox::fast_rand_0_1();
            float breakForce = (gameObject->m_breakForce + gameObject->m_breakForceVariance
                * GameToolbox::fast_rand_0_1()) * 0.01f;
            float breakAngle = (gameObject->m_breakAngle + gameObject->m_breakAngleVariance
                * GameToolbox::fast_rand_0_1()) * 0.017453f;
            float breakSteerForce = (gameObject->m_breakSteerForce + gameObject->m_breakSteerForceVariance
                * GameToolbox::fast_rand_0_1()) * 0.01f;
            float breakSteerSpeedLimit = gameObject->m_breakSteerSpeedLimit
                + gameObject->m_breakSteerSpeedLimitVariance * GameToolbox::fast_rand_0_1();

            cocos2d::CCPoint newVel = delta;
            cocos2d::CCPoint targetVel;

            switch (followMode) {
                case 1: {
                    float nearDist = gameObject->m_nearDistance + gameObject->m_nearDistanceVariance
                        * m_varianceValues[obj->m_varianceIndex + 4];
                    float nearAccel = gameObject->m_nearAcceleration
                        + gameObject->m_nearAccelerationVariance
                        * m_varianceValues[obj->m_varianceIndex + 5];
                    float friction = gameObject->m_friction + gameObject->m_frictionVariance
                        * m_varianceValues[obj->m_varianceIndex + 6];
                    float nearFriction = gameObject->m_nearFriction
                        + gameObject->m_nearFrictionVariance
                        * m_varianceValues[obj->m_varianceIndex + 7];

                    float norm = sqrtf(delta.x * delta.x + delta.y * delta.y);
                    cocos2d::CCPoint dir = norm > 0.0f
                        ? cocos2d::CCPoint(delta.x / norm, delta.y / norm)
                        : cocos2d::CCPoint(0.0f, 0.0f);

                    float accelFinal = accelScaled * 0.01f;
                    float frictionFinal = friction * 0.01f;
                    if (nearDist > 0.0f && nearDist > distance) {
                        float t = 1.0f - (distance / nearDist);
                        accelFinal += (nearAccel * 0.01f - accelFinal) * t;
                        frictionFinal += (nearFriction * 0.01f - frictionFinal) * t;
                    }

                    if (frictionFinal != 0.0f) {
                        newVel.x = (physics->m_unkPoint1.x * (1.0f - frictionFinal)) + (dir.x * accelFinal * dt);
                        newVel.y = (physics->m_unkPoint1.y * (1.0f - frictionFinal)) + (dir.y * accelFinal * dt);
                        if (!gameObject->m_xOnly) physics->m_unkPoint1.y = newVel.y;
                        if (!gameObject->m_yOnly) physics->m_unkPoint1.x = newVel.x;
                    } else {
                        if (!gameObject->m_xOnly) physics->m_unkPoint1.y += dir.y * accelFinal * dt;
                        if (!gameObject->m_yOnly) physics->m_unkPoint1.x += dir.x * accelFinal * dt;
                    }
                    break;
                }
                case 2: {
                    float speed = sqrtf(physics->m_unkPoint1.x * physics->m_unkPoint1.x
                        + physics->m_unkPoint1.y * physics->m_unkPoint1.y);
                    float angle = atan2f(physics->m_unkPoint1.y, physics->m_unkPoint1.x);
                    float targetAngle = angle;

                    if (gameObject->m_targetDirection) {
                        targetAngle = atan2f(delta.y, delta.x);
                    }
                    float angleDiff = convertToClosestDirection(targetAngle - angle, 3.1416f);
                    float appliedSteer = steerScale;
                    if (steerForceLow > 0.0f && speed <= speedRangeLow) appliedSteer = steerForceLow;
                    else if (steerForceHigh > 0.0f && speedRangeHigh > 0.0f
                             && speed >= speedRangeHigh) appliedSteer = steerForceHigh;

                    if (breakForce > 0.0f && fabsf(angleDiff) > breakAngle && speed > breakSteerSpeedLimit)
                        appliedSteer = breakSteerForce;

                    float currentSteer = physics->m_unkFloat2;
                    currentSteer += (appliedSteer - currentSteer) / 10.0f;
                    physics->m_unkFloat2 = currentSteer;

                    float applied = currentSteer;
                    if (fabsf(angleDiff) < applied) applied = angleDiff;
                    angle += applied * dt;
                    if (!gameObject->m_xOnly) physics->m_unkPoint1.y = sinf(angle) * speed;
                    if (!gameObject->m_yOnly) physics->m_unkPoint1.x = cosf(angle) * speed;
                    break;
                }
                default: {
                    float targetSpeed = gameObject->m_easing;
                    float norm = sqrtf(delta.x * delta.x + delta.y * delta.y);
                    if (norm > 0.0f) {
                        float invEasing = targetSpeed > 1.0f ? 1.0f / targetSpeed : 1.0f;
                        cocos2d::CCPoint target = cocos2d::CCPoint(delta.x * invEasing, delta.y * invEasing);
                        if (!gameObject->m_xOnly) physics->m_unkPoint1.y = target.y;
                        if (!gameObject->m_yOnly) physics->m_unkPoint1.x = target.x;
                    }
                    break;
                }
            }

            if (maxSpeed < 0.0f || maxSpeed > 0.0f) {
                float speed = sqrtf(physics->m_unkPoint1.x * physics->m_unkPoint1.x
                    + physics->m_unkPoint1.y * physics->m_unkPoint1.y);
                if (maxSpeed > 0.0f && speed > maxSpeed) {
                    physics->m_unkPoint1.x *= maxSpeed / speed;
                    physics->m_unkPoint1.y *= maxSpeed / speed;
                }
            }

            float stepX = physics->m_unkPoint1.x * dt;
            float stepY = physics->m_unkPoint1.y * dt;
            if (gameObject->m_xOnly) stepY = 0.0f;
            else if (gameObject->m_yOnly) stepX = 0.0f;

            if (stepX != 0.0f || stepY != 0.0f) {
                if (i == 1) {
                    cocos2d::CCArray* target = getTargetGroup(index, obj->m_uniqueID);
                    moveObjects(target, stepX, stepY, false);
                } else {
                    moveObject(obj, stepX, stepY, false);
                }
            }

            bool shouldRotate = gameObject->m_rotateDirection;
            if (shouldRotate) {
                float targetRot = (90.0f - atan2f(physics->m_unkPoint1.y, physics->m_unkPoint1.x)
                    * 57.296f) + gameObject->m_rotationOffset;
                float curRot = obj->getRotation();
                float rotDelta = convertToClosestDirection(targetRot - curRot, 180.0f) * 0.25f;
                if (rotDelta != 0.0f) {
                    if (i == 1) {
                        cocos2d::CCArray* target = getTargetGroup(index, obj->m_uniqueID);
                        cocos2d::CCPoint origin = obj->getUnmodifiedPosition();
                        rotateObjects(target, rotDelta, origin, origin, false, false);
                    } else {
                        rotateObject(obj, rotDelta);
                    }
                }
            }
        }
    }
}

void GJBaseGameLayer::processCommands(float dt, bool isHalfTick, bool isLastTick) {
    unsigned int commandIndex = m_gameState.m_commandIndex;
    m_gameState.m_unkUint4 = commandIndex;

    if (dt <= 0.0f) {
        if (!m_playerDied) {
            if (isHalfTick) {
                ++m_gameState.m_currentProgress;
            } else {
                m_gameState.m_currentProgress += 2;
            }
        }
    } else {
        int timeWarpMs = llroundf(m_gameState.m_timeWarp * 1000.0f);
        if (isHalfTick) {
            unsigned int prev = m_gameState.m_unkUint6;
            m_gameState.m_commandIndex = commandIndex + 1;
            m_gameState.m_unkUint6 = (unsigned int)((float)prev + (float)timeWarpMs * 0.5f);
            if (!m_playerDied) {
                ++m_gameState.m_currentProgress;
            }
        } else {
            int prev = m_gameState.m_unkUint6;
            m_gameState.m_commandIndex = commandIndex + 2;
            m_gameState.m_unkUint6 = prev + timeWarpMs;
            if (!m_playerDied) {
                m_gameState.m_currentProgress += 2;
            }
        }
    }

    if (!m_playerDied) {
        if (!m_clickBetweenSteps) {
            if (!m_clickOnSteps) {
                isLastTick = true;
            }
        }

        processQueuedButtons(dt, isLastTick);

        int recordedSize = m_queuedRecordedButtonsSize;
        unsigned int totalSize = (unsigned int)(m_queuedRecordedButtons.m_finish - m_queuedRecordedButtons.m_start);
        if (recordedSize < (int)totalSize) {
            unsigned int unkUint4 = m_gameState.m_unkUint4;
            RecordButtonCommand* iter = &m_queuedRecordedButtons.m_start[recordedSize];
            int prevStep = m_currentStep;
            m_currentStep = unkUint4;
            int stepDelta = unkUint4 - prevStep;
            do {
                iter->m_step = stepDelta;
                ++iter;
                stepDelta = 0;
            } while (iter != m_queuedRecordedButtons.m_finish);
            m_queuedRecordedButtonsSize = recordedSize + 1 + totalSize + ~recordedSize;
        }

        updateReplay();
    }

    if (m_gameState.m_pauseCounter > 0) {
        int buf = m_gameState.m_pauseBufferTimer - 1;
        if (buf < 0) {
            m_gameState.m_pauseCounter--;
            m_gameState.m_pauseBufferTimer = 120;
        } else {
            m_gameState.m_pauseBufferTimer = buf;
        }
    }
}

void GJBaseGameLayer::processTransformActions(bool visibleFrame) {
    float savedScaleX = m_objectLayer->getScaleX();
    float savedScaleY = m_objectLayer->getScaleY();
    cocos2d::CCPoint savedPos = *m_objectLayer->getPosition();

    m_objectLayer->setScale(1.0f);
    m_objectLayer->setPosition(cocos2d::CCPoint(0.0f, 0.0f));

    std::vector<GroupCommandObject2*> snapshot = m_effectManager->m_unkVector600;

    for (GroupCommandObject2* cmd : snapshot) {
        GameObject* target = tryGetMainObject(cmd->m_centerGroupID);
        cocos2d::CCArray* staticGroup = getStaticGroup(cmd->m_targetGroupID);
        cocos2d::CCArray* optimizedGroup = getOptimizedGroup(cmd->m_targetGroupID);

        bool ignoreStatic = false;
        if (cocos2d::CCArray::count(staticGroup)) {
            optimizedGroup = getGroup(cmd->m_targetGroupID);
        } else {
            ignoreStatic = true;
        }

        m_scaledCount += cocos2d::CCArray::count(optimizedGroup);

        double objectScaleX;
        double objectScaleY;

        if (ignoreStatic) {
            objectScaleX = cmd->m_unkFloat_528;
            objectScaleY = cmd->m_unkFloat_568;
        } else {
            objectScaleX = cmd->m_unkFloat_520;
            objectScaleY = cmd->m_unkFloat_560;
        }

        bool forceNoScale = cmd->m_someByte_522;
        float skewX = cmd->m_unkFloat_530;
        float skewY = cmd->m_unkFloat_538;

        if (forceNoScale) {
            continue;
        }

        if (objectScaleX == objectScaleY) {
            continue;
        }

        double scaleAdjustX = objectScaleX;
        double scaleAdjustY = objectScaleY;

        if (scaleAdjustX == 0.0) scaleAdjustX = objectScaleY;
        if (scaleAdjustY == 0.0) scaleAdjustY = objectScaleX;

        double texScaleX = cmd->m_unkDouble_548;
        double texScaleY = cmd->m_unkDouble_550;

        double effX = scaleAdjustX * (double)(cmd->m_unkFloat_518 - 1.0f) + 1.0;
        double effY = scaleAdjustY * (double)(cmd->m_unkFloat_514 - 1.0f) + 1.0;

        if (texScaleX == 0.0) {
            texScaleX = scaleAdjustX * (double)(cmd->m_unkFloat_520 - 1.0f) + 1.0;
            texScaleY = scaleAdjustY * (double)(cmd->m_unkFloat_514 - 1.0f) + 1.0;
        }

        if (fabs(effX) < 0.01) effX = effX < 0.0 ? -0.01 : 0.01;
        if (fabs(effY) < 0.01) effY = effY < 0.0 ? -0.01 : 0.01;
        if (fabs(texScaleX) < 0.01) texScaleX = texScaleX < 0.0 ? -0.01 : 0.01;
        if (fabs(texScaleY) < 0.01) texScaleY = texScaleY < 0.0 ? -0.01 : 0.01;

        if (!target) {
            if (optimizedGroup && cocos2d::CCArray::count(optimizedGroup)) {
                cocos2d::ccArray* data = optimizedGroup->data;
                for (unsigned int i = 0; i < data->num; ++i) {
                    GameObject* obj = (GameObject*)data->arr[i];
                    if (!obj) break;

                    if (!obj->m_unk3ee) {
                        unsigned int cmdIdx = m_gameState.m_commandIndex;
                        if (obj->m_unk4C4 != cmdIdx) {
                            obj->m_unk4C4 = cmdIdx;
                            obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                            GameObject::dirtifyObjectRect(obj);
                        }
                    }

                    obj->m_isDirty = true;
                    float newScaleX = -((float)cmd->m_unkFloat_528 - (float)cmd->m_unkFloat_528 * (float)(effX / texScaleX));
                    GameObject::addToCustomScaleX(obj, newScaleX);
                    float newScaleY = -((float)cmd->m_unkFloat_568 - (float)cmd->m_unkFloat_568 * (float)(effY / texScaleY));
                    GameObject::addToCustomScaleY(obj, newScaleY);
                    if (visibleFrame) obj->updateTransform();
                }
            }
            continue;
        }

        bool targetFlipped = cmd->m_someByte_50A;
        float targetRotation = targetFlipped ? target->getRotationX() : 0.0f;

        float rotationOffset = 0.0f;
        float positionOffset = 0.0f;
        claimRotationAction(cmd->m_targetGroupID, cmd->m_centerGroupID, &rotationOffset, &positionOffset, ignoreStatic, true);
        cocos2d::CCPoint moveAction = claimMoveAction(cmd->m_targetGroupID, ignoreStatic);

        m_areaTransformNode->setPosition(moveAction);
        m_areaTransformNode->setScaleX((float)effX);
        m_areaTransformNode->setScaleY((float)effY);
        m_areaScaleNode->setScaleX(1.0f / (float)texScaleX);
        m_areaScaleNode->setScaleY(1.0f / (float)texScaleY);
        m_areaTransformNode->setSkewX((float)skewX * (float)objectScaleX);
        m_areaTransformNode->setSkewY((float)skewY * (float)objectScaleX);
        m_areaSkewNode->setSkewX((float)-skewX * (float)objectScaleY);
        m_areaSkewNode->setSkewY((float)-skewY * (float)objectScaleY);
        m_areaTransformNode->setRotation(targetRotation + rotationOffset);
        m_areaRotateNode->setRotation(-targetRotation);
        prepareTransformParent(true);

        m_areaTransformNode->nodeToWorldTransform();
        m_areaSkewNode->nodeToWorldTransform();
        m_areaScaleNode->nodeToWorldTransform();
        cocos2d::CCAffineTransform baseTransform = m_areaRotateNode->nodeToWorldTransform();

        if (optimizedGroup) {
            cocos2d::ccArray* data = optimizedGroup->data;
            for (unsigned int i = 0; i < data->num; ++i) {
                GameObject* obj = (GameObject*)data->arr[i];
                if (!obj) break;

                float objOffsetX = obj->m_positionXOffset;
                float objOffsetY = obj->m_positionYOffset;
                bool wasNotInView = obj->m_unk3ee;

                if (!wasNotInView) {
                    unsigned int cmdIdx = m_gameState.m_commandIndex;
                    if (obj->m_unk4C4 != cmdIdx) {
                        obj->m_unk4C4 = cmdIdx;
                        obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                        GameObject::dirtifyObjectRect(obj);
                    }
                }

                float relX = (float)(obj->m_positionX - objOffsetX);
                float relY = (float)(obj->m_positionY - objOffsetY);

                m_areaTransformNode2->setPosition(cocos2d::CCPoint(relX - moveAction.x, relY - moveAction.y));
                m_areaTransformNode2->setScaleX((float)cmd->m_unkFloat_528 - (float)obj->m_unkFloat_528);
                m_areaTransformNode2->setScaleY((float)cmd->m_unkFloat_568 - (float)obj->m_unkFloat_568);
                m_areaTransformNode2->setRotationX((float)cmd->m_unkFloat_588 + obj->m_rotationXOffset - obj->m_unkFloat_590);
                m_areaTransformNode2->setRotationY((float)cmd->m_unkFloat_590 + obj->m_rotationYOffset - obj->m_unkFloat_592);

                m_areaTransformNode2->nodeToParentTransform();
                cocos2d::CCAffineTransform objTransform = m_areaTransformNode2->nodeToParentTransform();
                cocos2d::CCAffineTransform result = cocos2d::CCAffineTransformConcat(&baseTransform, &objTransform);

                float newX = result.tx + moveAction.x + objOffsetX;
                float newY = result.ty + moveAction.y + objOffsetY;

                if (newX > 10000000.0f) newX = 10000000.0f;
                else if (newX < -10000000.0f) newX = -10000000.0f;
                if (newY > 10000000.0f) newY = 10000000.0f;
                else if (newY < -10000000.0f) newY = -10000000.0f;

                obj->m_positionX = newX;
                obj->m_positionY = newY;

                if (wasNotInView) {
                    if (positionOffset != 0.0f && obj->m_canRotateFree) {
                        float rotDelta = positionOffset;
                        obj->m_rotationXOffset += rotDelta;
                        obj->m_rotationYOffset += rotDelta;
                        obj->m_isObjectRectDirty = true;
                        GameObject::addRotation(obj, rotDelta);
                        if (obj->m_objectType != GameObjectType::Decoration) {
                            obj->m_isOrientedBoxDirty = true;
                            if (!obj->m_shouldUseOuterOb) {
                                GameObject::calculateOrientedBox(obj);
                            }
                        }
                    }
                } else {
                    float scaleXResult = sqrtf(result.b * result.b + result.a * result.a);
                    float scaleYResult = sqrtf(result.d * result.d + result.c * result.c);
                    float angleY = atan2f(result.d, result.c) * 57.2957795f - 90.0f;
                    float angleX = atan2f(result.b, result.a) * 57.2957795f;

                    float rotX = obj->m_rotationXOffset;
                    float rotY = obj->m_rotationYOffset;
                    obj->m_rotationXOffset = rotX - angleY - cmd->m_unkFloat_588;
                    obj->m_rotationYOffset = rotY - angleX - cmd->m_unkFloat_590;
                    obj->m_isObjectRectDirty = true;
                    obj->m_isOrientedBoxDirty = true;
                    if (obj->m_objectType != GameObjectType::Decoration && !obj->m_shouldUseOuterOb) {
                        GameObject::calculateOrientedBox(obj);
                    }
                    GameObject::addToCustomScaleX(obj,
                        (float)(scaleXResult - (float)cmd->m_unkFloat_528) + (float)obj->m_unkFloat_530);
                    GameObject::addToCustomScaleY(obj,
                        (float)(scaleYResult - (float)cmd->m_unkFloat_568) + (float)obj->m_unkFloat_538);
                }

                obj->m_unk3ee = forceNoScale;
                if (visibleFrame) obj->updateTransform();
                updateObjectSection(obj);
            }
        }

        updateDisabledObjectsLastPos(optimizedGroup);
    }

    m_objectLayer->setScaleX(savedScaleX);
    m_objectLayer->setScaleY(savedScaleY);
    m_objectLayer->setPosition(savedPos);
}

void GJBaseGameLayer::processAreaActions(float dt, bool visibleFrame) {
    m_gameState.m_unkUint3 = m_gameState.m_commandIndex - 1;

    processAreaEffects(&m_gameState.m_scaleEffectInstances, GJAreaActionType::Scale, dt, visibleFrame);
    processAreaEffects(&m_gameState.m_rotateEffectInstances, GJAreaActionType::Rotate, dt, visibleFrame);
    processAreaEffects(&m_gameState.m_moveEffectInstances, GJAreaActionType::Move, dt, visibleFrame);

    int processed = m_processedAreaObjectsCount;
    int i = 0;
    while (i < processed) {
        GameObject* obj = m_processedAreaObjects[i];
        bool doFullReset = obj->m_unk4C8 < (int)m_gameState.m_commandIndex;
        bool doFinalize = doFullReset || visibleFrame;

        if (doFullReset) {
            obj->m_isDirty = true;
            obj->m_isObjectRectDirty = true;
            obj->m_isOrientedBoxDirty = true;
            int wasDirty = 0;

            if (obj->m_positionXOffset != 0.0f) {
                obj->m_isUnmodifiedPosDirty = true;
                float offsetX = obj->m_positionXOffset;
                wasDirty = 1;
                obj->m_positionXOffset = 0.0f;
                obj->m_positionX -= offsetX;
            }

            if (obj->m_positionYOffset != 0.0f) {
                obj->m_isUnmodifiedPosDirty = true;
                float offsetY = obj->m_positionYOffset;
                wasDirty = 1;
                obj->m_positionYOffset = 0.0f;
                obj->m_positionY -= offsetY;
            }

            float scaleOffsetX = obj->m_unk2BC;
            float scaleOffsetY = obj->m_unk2C0;
            if (scaleOffsetX != 0.0f || scaleOffsetY != 0.0f) {
                float newScaleX = obj->m_scaleX - scaleOffsetX;
                float newScaleYOffset = obj->m_scaleYOffset - scaleOffsetY;
                float newScaleY = obj->m_scaleY - scaleOffsetY;
                obj->m_scaleXOffset -= scaleOffsetX;
                obj->m_scaleX = newScaleX;
                obj->m_unk2BC = 0.0f;
                obj->m_scaleYOffset = newScaleYOffset;
                obj->m_scaleY = newScaleY;
                obj->m_unk2C0 = 0.0f;
                obj->setRScale(1.0f);
            }

            float rotOffsetX = obj->m_unk2A8;
            float rotOffsetY = obj->m_unk2B0;
            if (rotOffsetX != 0.0f || rotOffsetY != 0.0f) {
                float newRotY = obj->m_rotationYOffset - rotOffsetY;
                float newRotX = obj->m_rotationXOffset - rotOffsetX;
                obj->m_unk2A8 = 0.0f;
                obj->m_rotationYOffset = newRotY;
                obj->m_rotationXOffset = newRotX;
                obj->m_unk2B0 = 0.0f;
                obj->setRRotation(0.0f);
            }

            obj->m_unk4C8 = m_gameState.m_commandIndex;

            if (!obj->m_isDecoration2) {
                unsigned int cmdIdx = m_gameState.m_commandIndex;
                if (obj->m_unk4C4 != cmdIdx) {
                    obj->m_unk4C4 = cmdIdx;
                    obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                    GameObject::dirtifyObjectRect(obj);
                }
                if (obj->m_unk4CC != cmdIdx) {
                    obj->m_unk4CC = cmdIdx;
                    obj->m_customScaleX = obj->m_scaleX;
                    obj->m_customScaleY = obj->m_scaleY;
                    GameObject::dirtifyObjectRect(obj);
                }
            }

            if (wasDirty) updateObjectSection(obj);
        }

        if (doFinalize) {
            obj->setRRotation(0.0f);
            obj->setRScale(1.0f);
            processed = m_processedAreaObjectsCount;
        }

        ++i;
    }

    int areaCount = m_areaObjectsCount;
    m_processedAreaObjectsCount = 0;

    if (areaCount > 0) {
        for (int j = 0; j < areaCount; ++j) {
            GameObject* obj = m_areaObjects[j];
            if ((int)m_processedAreaObjects.size() <= m_processedAreaObjectsCount) {
                m_processedAreaObjects.push_back(obj);
            } else {
                m_processedAreaObjects[m_processedAreaObjectsCount] = obj;
            }
            m_processedAreaObjectsCount++;
            m_processedAreaObjectsIndex++;
        }

        if (m_areaObjectsUpdated && m_areaObjectsCount > 0) {
            for (int j = 0; j < m_areaObjectsCount; ++j) {
                GameObject* obj = m_areaObjects[j];
                obj->m_isObjectRectDirty = true;
                obj->m_isOrientedBoxDirty = true;
                obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                obj->m_customScaleX = obj->m_scaleX;
                obj->m_customScaleY = obj->m_scaleY;
            }
        }
    }
}

void GJBaseGameLayer::processAreaFadeGroupAction(cocos2d::CCArray* objects,
                                                  EnterEffectInstance* instance,
                                                  cocos2d::CCPoint position,
                                                  cocos2d::CCPoint* targetPos) {
    float fromOpacity = instance->m_fromOpacity;
    float toOpacity = instance->m_toOpacity;
    bool applyToGroups = targetPos != nullptr;

    m_areaColorCount += cocos2d::CCArray::count(objects);

    if (!objects) {
        m_areaColorCountTotal += 0;
        return;
    }

    int processedCount = 0;
    int targetGroupTotal = 0;
    EnterEffectObject* gameObject = instance->m_gameObject;

    cocos2d::ccArray* data = objects->data;
    if (!data->num) {
        m_areaColorCountTotal += 0;
        return;
    }

    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = (GameObject*)data->arr[i];
        if (!obj) break;
        if (!obj->m_isActivated) continue;

        ++processedCount;

        cocos2d::CCPoint objPos = obj->getRealPosition();
        float distance;

        int directionType = gameObject->m_directionType;
        if (directionType == 1) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.x - targetPos->x;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
        } else if (directionType == 2) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.y - targetPos->y;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
        } else {
            float offset = 0.0f;
            if (instance->m_offsetVariance != 0.0f)
                offset = instance->m_offsetVariance * m_varianceValues[obj->m_varianceIndex + 1];
            float offsetY = 0.0f;
            if (instance->m_offsetYVariance != 0.0f)
                offsetY = instance->m_offsetYVariance * m_varianceValues[obj->m_varianceIndex];
            cocos2d::CCPoint ref(targetPos->x + offset, targetPos->y + offsetY);
            distance = cocos2d::ccpDistance(objPos, ref);
        }

        float lengthVar = 0.0f;
        if (instance->m_lengthVariance != 0.0f)
            lengthVar = instance->m_lengthVariance * m_varianceValues[obj->m_varianceIndex];
        int length = (int)(instance->m_length + lengthVar);

        float ratio;
        if (instance->m_deadzone == 0.0f) {
            ratio = distance / (float)length;
        } else {
            ratio = (distance / (float)length - instance->m_deadzone) / (1.0f - instance->m_deadzone);
        }

        if (ratio >= 1.0f) ratio = 1.0f;
        else if (ratio <= 0.0f) ratio = 0.0f;

        if (gameObject->m_inbound) ratio = 1.0f - ratio;

        float opacity = fromOpacity + (toOpacity - fromOpacity) * ratio;

        if (applyToGroups) {
            cocos2d::CCArray* target = getTargetGroup(instance->m_targetGroupIndex, obj->m_uniqueID);
            targetGroupTotal = targetGroupTotal - 1 + cocos2d::CCArray::count(target);

            if (target) {
                cocos2d::ccArray* tdata = target->data;
                for (unsigned int j = 0; j < tdata->num; ++j) {
                    GameObject* tobj = (GameObject*)tdata->arr[j];
                    if (!tobj) break;
                    GameObject::setAreaOpacity(tobj, opacity, ratio, m_gameState.m_commandIndex);
                }
            }
        } else {
            GameObject::setAreaOpacity(obj, opacity, ratio, m_gameState.m_commandIndex);
        }
    }

    m_areaColorCountTotal += processedCount + targetGroupTotal;
    m_areaColorCount += targetGroupTotal;
}

void GJBaseGameLayer::processAreaTintGroupAction(cocos2d::CCArray* objects,
                                                  EnterEffectInstance* instance,
                                                  cocos2d::CCPoint position,
                                                  cocos2d::CCPoint* targetPos) {
    EnterEffectObject* gameObject = instance->m_gameObject;
    cocos2d::ccColor3B baseColor = GJEffectManager::activeColorForIndex(m_effectManager, gameObject->m_tintChannelID);

    bool pulseDetailOnly = gameObject->m_pulseDetailOnly;
    bool pulseMainOnly = gameObject->m_pulseMainOnly;
    bool hsvEnabled = gameObject->m_hsvEnabled;

    m_areaColorCount += cocos2d::CCArray::count(objects);

    if (!objects) {
        m_areaColorCountTotal += 0;
        return;
    }

    int processedCount = 0;
    int targetGroupTotal = 0;
    bool applyToGroups = targetPos != nullptr;

    cocos2d::ccArray* data = objects->data;
    if (!data->num) {
        m_areaColorCountTotal += 0;
        return;
    }

    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = (GameObject*)data->arr[i];
        if (!obj) break;
        if (!obj->m_isActivated) continue;

        ++processedCount;

        cocos2d::CCPoint objPos = obj->getRealPosition();
        float distance;

        int directionType = gameObject->m_directionType;
        if (directionType == 1) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.x - targetPos->x;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
        } else if (directionType == 2) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.y - targetPos->y;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
        } else {
            float offset = 0.0f;
            if (instance->m_offsetVariance != 0.0f)
                offset = instance->m_offsetVariance * m_varianceValues[obj->m_varianceIndex + 1];
            float offsetY = 0.0f;
            if (instance->m_offsetYVariance != 0.0f)
                offsetY = instance->m_offsetYVariance * m_varianceValues[obj->m_varianceIndex];
            cocos2d::CCPoint ref(targetPos->x + offset, targetPos->y + offsetY);
            distance = cocos2d::ccpDistance(objPos, ref);
        }

        float lengthVar = 0.0f;
        if (instance->m_lengthVariance != 0.0f)
            lengthVar = instance->m_lengthVariance * m_varianceValues[obj->m_varianceIndex];
        int length = (int)(instance->m_length + lengthVar);

        float ratio;
        if (instance->m_deadzone == 0.0f) {
            ratio = distance / (float)length;
        } else {
            ratio = (distance / (float)length - instance->m_deadzone) / (1.0f - instance->m_deadzone);
        }

        if (ratio >= 1.0f) ratio = 1.0f;
        else if (ratio <= 0.0f) ratio = 0.0f;

        if (gameObject->m_inbound) ratio = 1.0f - ratio;

        float tintAmount = ratio;
        if (hsvEnabled) {
            cocos2d::ccHSVValue multiplied = GameToolbox::getMultipliedHSV(instance->m_hsvValue, 1.0f - tintAmount);

            if (applyToGroups) {
                cocos2d::CCArray* target = getTargetGroup(instance->m_targetGroupIndex, obj->m_uniqueID);
                targetGroupTotal = targetGroupTotal - 1 + cocos2d::CCArray::count(target);

                if (target) {
                    cocos2d::ccArray* tdata = target->data;
                    for (unsigned int j = 0; j < tdata->num; ++j) {
                        GameObject* tobj = (GameObject*)tdata->arr[j];
                        if (!tobj) break;

                        if (!pulseMainOnly) {
                            cocos2d::ccColor3B current = tobj->getColor();
                            cocos2d::ccColor3B tinted = GameToolbox::transformColor(current, multiplied);
                            tobj->updateMainColor(&tinted);
                        }
                        if (!pulseDetailOnly && GameObject::hasSecondaryColor(tobj)) {
                            cocos2d::ccColor3B current = tobj->m_colorSprite->getColor();
                            cocos2d::ccColor3B tinted = GameToolbox::transformColor(current, multiplied);
                            tobj->updateSecondaryColor(&tinted);
                        }
                    }
                }
            } else {
                if (!pulseMainOnly) {
                    cocos2d::ccColor3B current = obj->getColor();
                    cocos2d::ccColor3B tinted = GameToolbox::transformColor(current, multiplied);
                    obj->updateMainColor(&tinted);
                }
                if (!pulseDetailOnly && GameObject::hasSecondaryColor(obj)) {
                    cocos2d::ccColor3B current = obj->m_colorSprite->getColor();
                    cocos2d::ccColor3B tinted = GameToolbox::transformColor(current, multiplied);
                    obj->updateSecondaryColor(&tinted);
                }
            }
        } else {
            float blend = (1.0f - instance->m_tint) + instance->m_tint * tintAmount;

            if (applyToGroups) {
                cocos2d::CCArray* target = getTargetGroup(instance->m_targetGroupIndex, obj->m_uniqueID);
                targetGroupTotal = targetGroupTotal - 1 + cocos2d::CCArray::count(target);

                if (target) {
                    cocos2d::ccArray* tdata = target->data;
                    for (unsigned int j = 0; j < tdata->num; ++j) {
                        GameObject* tobj = (GameObject*)tdata->arr[j];
                        if (!tobj) break;

                        if (!pulseMainOnly) {
                            cocos2d::ccColor3B current = tobj->getColor();
                            cocos2d::ccColor3B tinted = GameToolbox::multipliedColorValue(baseColor, current, blend);
                            tobj->updateMainColor(&tinted);
                        }
                        if (!pulseDetailOnly && GameObject::hasSecondaryColor(tobj)) {
                            cocos2d::ccColor3B current = tobj->m_colorSprite->getColor();
                            cocos2d::ccColor3B tinted = GameToolbox::multipliedColorValue(baseColor, current, blend);
                            tobj->updateSecondaryColor(&tinted);
                        }
                    }
                }
            } else {
                if (!pulseMainOnly) {
                    cocos2d::ccColor3B current = obj->getColor();
                    cocos2d::ccColor3B tinted = GameToolbox::multipliedColorValue(baseColor, current, blend);
                    obj->updateMainColor(&tinted);
                }
                if (!pulseDetailOnly && GameObject::hasSecondaryColor(obj)) {
                    cocos2d::ccColor3B current = obj->m_colorSprite->getColor();
                    cocos2d::ccColor3B tinted = GameToolbox::multipliedColorValue(baseColor, current, blend);
                    obj->updateSecondaryColor(&tinted);
                }
            }
        }
    }

    m_areaColorCountTotal += processedCount + targetGroupTotal;
    m_areaColorCount += targetGroupTotal;
}

void GJBaseGameLayer::processAreaMoveGroupAction(cocos2d::CCArray* objects,
                                                  EnterEffectInstance* instance,
                                                  cocos2d::CCPoint position,
                                                  cocos2d::CCPoint* targetPos,
                                                  int outerMax, int middleMin,
                                                  int middleMax, int startIndex,
                                                  bool targetGroups, bool reset) {
    EnterEffectObject* gameObject = instance->m_gameObject;
    float relativeFade = gameObject->m_relativeFade;
    if (relativeFade <= 0.0f) relativeFade = 0.0f;

    m_areaMovedCount += cocos2d::CCArray::count(objects);

    if (!objects) {
        m_areaMovedCountTotal += 0;
        return;
    }

    cocos2d::ccArray* data = objects->data;
    if (!data->num) {
        m_areaMovedCountTotal += 0;
        return;
    }

    int processed = 0;
    int targetGroupTotal = 0;
    int targetIndex = 0;

    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = (GameObject*)data->arr[i];
        if (!obj) break;

        bool wasDirty = false;

        if (obj->m_unk3ee) {
            unsigned int cmdIdx = m_gameState.m_commandIndex;
            ++processed;
            if (obj->m_unk4C4 < cmdIdx) {
                wasDirty = false;
            } else {
                wasDirty = true;
            }
        } else {
            int sectionX = obj->m_outerSectionIndex;
            if (sectionX > middleMin || sectionX < outerMax) continue;
            int sectionY = obj->m_middleSectionIndex;
            if (sectionY > startIndex || sectionY < middleMax) continue;

            unsigned int cmdIdx = m_gameState.m_commandIndex;
            ++processed;
            if (obj->m_unk4C4 < cmdIdx) {
                wasDirty = false;
            } else {
                wasDirty = true;
            }
        }

        if (!obj->m_isGroupDisabled && !wasDirty) {
            unsigned int cmdIdx = m_gameState.m_commandIndex;
            if (obj->m_unk4C4 != cmdIdx) {
                obj->m_unk4C4 = cmdIdx;
                obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                GameObject::dirtifyObjectRect(obj);
            }
        }

        int offX1 = 0;
        if (obj->m_positionXOffset != 0.0f) {
            obj->m_isUnmodifiedPosDirty = true;
            float off = obj->m_positionXOffset;
            obj->m_positionXOffset = 0.0f;
            obj->m_positionX -= off;
            offX1 = 1;
        }
        if (obj->m_positionYOffset != 0.0f) {
            obj->m_isUnmodifiedPosDirty = true;
            float off = obj->m_positionYOffset;
            obj->m_positionYOffset = 0.0f;
            obj->m_positionY -= off;
            offX1 = 1;
        }
        if (obj->m_unk2BC != 0.0f || obj->m_unk2C0 != 0.0f) {
            float s = obj->m_unk2BC;
            float t = obj->m_unk2C0;
            float nsx = obj->m_scaleXOffset - s;
            float nsy = obj->m_scaleYOffset - t;
            float nx = obj->m_scaleX - s;
            float ny = obj->m_scaleY - t;
            obj->m_scaleXOffset = nsx;
            obj->m_scaleX = nx;
            obj->m_unk2BC = 0.0f;
            obj->m_scaleYOffset = nsy;
            obj->m_scaleY = ny;
            obj->m_unk2C0 = 0.0f;
        }
        if (obj->m_unk2A8 != 0.0f || obj->m_unk2B0 != 0.0f) {
            float rx = obj->m_rotationXOffset - obj->m_unk2A8;
            float ry = obj->m_rotationYOffset - obj->m_unk2B0;
            obj->m_unk2A8 = 0.0f;
            obj->m_rotationXOffset = rx;
            obj->m_rotationYOffset = ry;
            obj->m_unk2B0 = 0.0f;
        }

        obj->m_unk4C8 = m_gameState.m_commandIndex;

        if (m_areaObjectsIndex > m_areaObjectsCount) {
            m_areaObjects[m_areaObjectsCount] = obj;
        } else {
            m_areaObjects.push_back(obj);
        }
        m_areaObjectsIndex++;
        m_areaObjectsCount++;

        if (offX1) updateObjectSection(obj);

        bool outOfView = !wasDirty;

        cocos2d::CCPoint objPos = obj->getRealPosition();
        float distance;
        float signCheck;

        int directionType = gameObject->m_directionType;
        if (directionType == 1) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.x - targetPos->x;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
            signCheck = adj;
        } else if (directionType == 2) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.y - targetPos->y;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
            signCheck = adj;
        } else {
            float offset = 0.0f;
            if (instance->m_offsetVariance != 0.0f)
                offset = instance->m_offsetVariance * m_varianceValues[obj->m_varianceIndex + 1];
            float offsetY = 0.0f;
            if (instance->m_offsetYVariance != 0.0f)
                offsetY = instance->m_offsetYVariance * m_varianceValues[obj->m_varianceIndex];
            cocos2d::CCPoint ref(targetPos->x + offset, targetPos->y + offsetY);
            distance = cocos2d::ccpDistance(objPos, ref);
            signCheck = objPos.y - targetPos->y;
        }

        float lengthVar = 0.0f;
        if (instance->m_lengthVariance != 0.0f)
            lengthVar = instance->m_lengthVariance * m_varianceValues[obj->m_varianceIndex];
        float length = (float)(int)(instance->m_length + lengthVar);

        float ratio;
        if (instance->m_deadzone == 0.0f) {
            ratio = distance / length;
        } else {
            ratio = (distance / length - instance->m_deadzone) / (1.0f - instance->m_deadzone);
        }

        bool reverse = instance->m_reversed;
        bool pastTarget = reverse ? (signCheck >= 0.0f) : (signCheck <= 0.0f);

        if (ratio >= 1.0f) ratio = 1.0f;
        else if (ratio <= 0.0f) ratio = 0.0f;

        if (gameObject->m_inbound) ratio = 1.0f - ratio;

        float moveOffsetX = 0.0f;
        float moveOffsetY = 0.0f;

        if (ratio < 1.0f) {
            if (!gameObject->m_xyMode) {
                float moveDistVar = 0.0f;
                if (instance->m_moveDistanceVariance != 0.0f)
                    moveDistVar = instance->m_moveDistanceVariance * m_varianceValues[obj->m_varianceIndex];
                float moveDist = instance->m_moveDistance + moveDistVar;

                if (moveDist != 0.0f) {
                    bool useOutEase = gameObject->m_easeOutEnabled && (ratio > 0.01f && ratio < 0.99f && !pastTarget);
                    if (useOutEase) {
                        int& easeIndex = instance->m_easeIndices[targetIndex];
                        int currentEaseIdx = instance->m_easeIndex;
                        if (easeIndex == currentEaseIdx - 2) {
                            easeIndex = currentEaseIdx;
                        } else {
                            if (easeIndex == currentEaseIdx - 1) pastTarget = true;
                            easeIndex = currentEaseIdx + (pastTarget ? 1 : 0);
                        }
                    }

                    float eased = pastTarget
                        ? getEnterEasingValue(ratio, gameObject->m_easingInType,
                                              gameObject->m_easingInRate, gameObject->m_easingInBuffer)
                        : getEnterEasingValue(ratio, gameObject->m_easingOutType,
                                              gameObject->m_easingOutRate, gameObject->m_easingOutBuffer);

                    cocos2d::CCPoint dir;

                    if (gameObject->m_relative) {
                        cocos2d::CCPoint diff = objPos - *targetPos;
                        float mag = sqrtf(diff.x * diff.x + diff.y * diff.y);
                        if (relativeFade > mag) moveDist *= (1.0f / relativeFade) * mag;
                        dir = mag > 0.0f ? cocos2d::CCPoint(diff.x / mag, diff.y / mag) : cocos2d::CCPoint(0.0f, 0.0f);
                    } else if (gameObject->m_startAngle) {
                        dir = gameObject->m_anglePosition;
                    } else {
                        float angleRad = (instance->m_moveAngle + instance->m_moveAngleVariance
                            * m_varianceValues[obj->m_varianceIndex]) - 90.0f;
                        angleRad *= 0.017453f;
                        dir = cocos2d::CCPoint(cosf(angleRad), sinf(angleRad));
                    }

                    float scale = (1.0f - eased) * moveDist;
                    moveOffsetX = scale * dir.x;
                    moveOffsetY = scale * dir.y;
                }
            } else {
                float moveX = gameObject->m_moveX + gameObject->m_moveXVariance
                    * m_varianceValues[obj->m_varianceIndex];
                float moveY = gameObject->m_moveY + gameObject->m_moveYVariance
                    * m_varianceValues[obj->m_varianceIndex];

                if (moveX != 0.0f || moveY != 0.0f) {
                    bool useOutEase = gameObject->m_easeOutEnabled && (ratio > 0.01f && ratio < 0.99f && !pastTarget);
                    if (useOutEase) {
                        int& easeIndex = instance->m_easeIndices[targetIndex];
                        int currentEaseIdx = instance->m_easeIndex;
                        if (easeIndex == currentEaseIdx - 2) {
                            easeIndex = currentEaseIdx;
                        } else {
                            if (easeIndex == currentEaseIdx - 1) pastTarget = true;
                            easeIndex = currentEaseIdx + (pastTarget ? 1 : 0);
                        }
                    }

                    float eased = pastTarget
                        ? getEnterEasingValue(ratio, gameObject->m_easingInType,
                                              gameObject->m_easingInRate, gameObject->m_easingInBuffer)
                        : getEnterEasingValue(ratio, gameObject->m_easingOutType,
                                              gameObject->m_easingOutRate, gameObject->m_easingOutBuffer);

                    moveOffsetX = (1.0f - eased) * moveX;
                    moveOffsetY = moveY * (1.0f - eased);
                }
            }
        }

        if (reset) {
            cocos2d::CCArray* target = getTargetGroup(instance->m_targetGroupIndex, obj->m_uniqueID);
            targetGroupTotal = targetGroupTotal - 1 + cocos2d::CCArray::count(target);

            if (gameObject->m_dontEditAreaParent) obj->m_unk3ee = true;

            if (target) {
                cocos2d::ccArray* tdata = target->data;
                for (unsigned int j = 0; j < tdata->num; ++j) {
                    GameObject* tobj = (GameObject*)tdata->arr[j];
                    if (!tobj) break;

                    unsigned int cmdIdx = m_gameState.m_commandIndex;
                    if (tobj->m_unk4C4 != cmdIdx) {
                        tobj->m_unk4C4 = cmdIdx;
                        tobj->m_lastPosition = cocos2d::CCPoint(tobj->m_positionX, tobj->m_positionY);
                        GameObject::dirtifyObjectRect(tobj);
                    }

                    if (moveOffsetX != 0.0f || moveOffsetY != 0.0f) {
                        if (moveOffsetX != 0.0f) {
                            tobj->m_positionX += moveOffsetX;
                            tobj->m_positionYOffset += moveOffsetX;
                        }
                        GameObject::dirtifyObjectPos(tobj);
                        GameObject::dirtifyObjectRect(tobj);
                        if (moveOffsetY != 0.0f && !tobj->m_tempOffsetXRelated) {
                            tobj->m_positionY += moveOffsetY;
                        }
                    }
                    updateObjectSection(tobj);
                }
            }

            obj->m_unk3ee = false;
        } else {
            unsigned int cmdIdx = m_gameState.m_commandIndex;
            if (obj->m_unk4C4 != cmdIdx) {
                obj->m_unk4C4 = cmdIdx;
                obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                GameObject::dirtifyObjectRect(obj);
            }

            if (moveOffsetX != 0.0f || moveOffsetY != 0.0f) {
                if (moveOffsetX != 0.0f) {
                    obj->m_positionX += moveOffsetX;
                    obj->m_positionXOffset += moveOffsetX;
                }
                if (moveOffsetY != 0.0f) {
                    obj->m_positionY += moveOffsetY;
                }
                GameObject::dirtifyObjectPos(obj);
                GameObject::dirtifyObjectRect(obj);
                updateObjectSection(obj);
            }
        }

        ++targetIndex;
    }

    m_areaMovedCountTotal += processed + targetGroupTotal;
    m_areaMovedCount += targetGroupTotal;
}

void GJBaseGameLayer::processAreaRotateGroupAction(cocos2d::CCArray* objects,
                                                    EnterEffectInstance* instance,
                                                    cocos2d::CCPoint position,
                                                    cocos2d::CCPoint* targetPos,
                                                    int outerMax, int middleMin,
                                                    int middleMax, int startIndex,
                                                    bool targetGroups, bool reset) {
    EnterEffectObject* gameObject = instance->m_gameObject;

    m_areaRotatedCount += cocos2d::CCArray::count(objects);

    if (!objects) {
        m_areaRotatedCountTotal += 0;
        return;
    }

    cocos2d::ccArray* data = objects->data;
    if (!data->num) {
        m_areaRotatedCountTotal += 0;
        return;
    }

    int processed = 0;
    int targetGroupTotal = 0;
    int targetIndex = 0;

    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = (GameObject*)data->arr[i];
        if (!obj) break;

        bool wasDirty = false;

        if (obj->m_unk3ee) {
            unsigned int cmdIdx = m_gameState.m_commandIndex;
            ++processed;
            if (obj->m_unk4C4 < cmdIdx) {
                wasDirty = false;
            } else {
                wasDirty = true;
            }
        } else {
            int sectionX = obj->m_outerSectionIndex;
            if (sectionX > middleMin || sectionX < outerMax) continue;
            int sectionY = obj->m_middleSectionIndex;
            if (sectionY > startIndex || sectionY < middleMax) continue;

            unsigned int cmdIdx = m_gameState.m_commandIndex;
            ++processed;
            if (obj->m_unk4C4 < cmdIdx) {
                wasDirty = false;
            } else {
                wasDirty = true;
            }
        }

        if (!obj->m_isGroupDisabled && !wasDirty) {
            unsigned int cmdIdx = m_gameState.m_commandIndex;
            if (obj->m_unk4C4 != cmdIdx) {
                obj->m_unk4C4 = cmdIdx;
                obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                GameObject::dirtifyObjectRect(obj);
            }
        }

        int offX1 = 0;
        if (obj->m_positionXOffset != 0.0f) {
            obj->m_isUnmodifiedPosDirty = true;
            float off = obj->m_positionXOffset;
            obj->m_positionXOffset = 0.0f;
            obj->m_positionX -= off;
            offX1 = 1;
        }
        if (obj->m_positionYOffset != 0.0f) {
            obj->m_isUnmodifiedPosDirty = true;
            float off = obj->m_positionYOffset;
            obj->m_positionYOffset = 0.0f;
            obj->m_positionY -= off;
            offX1 = 1;
        }
        if (obj->m_unk2BC != 0.0f || obj->m_unk2C0 != 0.0f) {
            float s = obj->m_unk2BC;
            float t = obj->m_unk2C0;
            obj->m_scaleX -= s;
            obj->m_scaleY -= t;
            obj->m_scaleXOffset -= s;
            obj->m_scaleYOffset -= t;
            obj->m_unk2BC = 0.0f;
            obj->m_unk2C0 = 0.0f;
        }
        if (obj->m_unk2A8 != 0.0f || obj->m_unk2B0 != 0.0f) {
            obj->m_rotationXOffset -= obj->m_unk2A8;
            obj->m_rotationYOffset -= obj->m_unk2B0;
            obj->m_unk2A8 = 0.0f;
            obj->m_unk2B0 = 0.0f;
        }

        obj->m_unk4C8 = m_gameState.m_commandIndex;

        if (m_areaObjectsIndex > m_areaObjectsCount) {
            m_areaObjects[m_areaObjectsCount] = obj;
        } else {
            m_areaObjects.push_back(obj);
        }
        m_areaObjectsIndex++;
        m_areaObjectsCount++;

        if (offX1) updateObjectSection(obj);

        bool outOfView = !wasDirty;

        cocos2d::CCPoint objPos = obj->getRealPosition();
        float distance;
        float signCheck;

        int directionType = gameObject->m_directionType;
        if (directionType == 1) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.x - targetPos->x;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
            signCheck = adj;
        } else if (directionType == 2) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.y - targetPos->y;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
            signCheck = adj;
        } else {
            float offset = 0.0f;
            if (instance->m_offsetVariance != 0.0f)
                offset = instance->m_offsetVariance * m_varianceValues[obj->m_varianceIndex + 1];
            float offsetY = 0.0f;
            if (instance->m_offsetYVariance != 0.0f)
                offsetY = instance->m_offsetYVariance * m_varianceValues[obj->m_varianceIndex];
            cocos2d::CCPoint ref(targetPos->x + offset, targetPos->y + offsetY);
            distance = cocos2d::ccpDistance(objPos, ref);
            signCheck = objPos.y - targetPos->y;
        }

        float lengthVar = 0.0f;
        if (instance->m_lengthVariance != 0.0f)
            lengthVar = instance->m_lengthVariance * m_varianceValues[obj->m_varianceIndex];
        float length = (float)(int)(instance->m_length + lengthVar);

        float ratio;
        if (instance->m_deadzone == 0.0f) {
            ratio = distance / length;
        } else {
            ratio = (distance / length - instance->m_deadzone) / (1.0f - instance->m_deadzone);
        }

        bool reverse = instance->m_reversed;
        bool pastTarget = reverse ? (signCheck >= 0.0f) : (signCheck <= 0.0f);

        if (ratio >= 1.0f) ratio = 1.0f;
        else if (ratio <= 0.0f) ratio = 0.0f;

        if (gameObject->m_inbound) ratio = 1.0f - ratio;

        float rotationAmount = 0.0f;

        if (ratio < 1.0f) {
            float rotVar = 0.0f;
            if (instance->m_rotationVariance != 0.0f)
                rotVar = instance->m_rotationVariance * m_varianceValues[obj->m_varianceIndex];
            float rotation = instance->m_rotation + rotVar;

            if (rotation != 0.0f) {
                bool useOutEase = gameObject->m_easeOutEnabled && (ratio > 0.01f && ratio < 0.99f && !pastTarget);
                if (useOutEase) {
                    int& easeIndex = instance->m_easeIndices[targetIndex];
                    int currentEaseIdx = instance->m_easeIndex;
                    if (easeIndex == currentEaseIdx - 2) {
                        easeIndex = currentEaseIdx;
                    } else {
                        if (easeIndex == currentEaseIdx - 1) pastTarget = true;
                        easeIndex = currentEaseIdx + (pastTarget ? 1 : 0);
                    }
                }

                float eased = pastTarget
                    ? getEnterEasingValue(ratio, gameObject->m_easingInType,
                                          gameObject->m_easingInRate, gameObject->m_easingInBuffer)
                    : getEnterEasingValue(ratio, gameObject->m_easingOutType,
                                          gameObject->m_easingOutRate, gameObject->m_easingOutBuffer);

                rotationAmount = (1.0f - eased) * rotation;
            }
        }

        if (reset) {
            cocos2d::CCArray* target = getTargetGroup(instance->m_targetGroupIndex, obj->m_uniqueID);
            cocos2d::CCArray* origin = getTargetGroupOrigin(instance->m_targetGroupIndex, obj->m_uniqueID);
            targetGroupTotal = targetGroupTotal - 1 + cocos2d::CCArray::count(target);

            if (gameObject->m_dontEditAreaParent) obj->m_unk3ee = true;

            rotateAreaObjects(obj, target, rotationAmount, outOfView);
            obj->m_unk3ee = false;
        } else if (rotationAmount != 0.0f && obj->m_canRotateFree) {
            obj->m_rotationXOffset += rotationAmount;
            obj->m_rotationYOffset += rotationAmount;
            GameObject::addRotation(obj, rotationAmount);
            if (obj->m_objectType != GameObjectType::Decoration) {
                obj->m_isObjectRectDirty = true;
                obj->m_isOrientedBoxDirty = true;
                if (!obj->m_shouldUseOuterOb) {
                    GameObject::calculateOrientedBox(obj);
                }
            }
        }

        if (outOfView) updateObjectSection(obj);
        ++targetIndex;
    }

    m_areaRotatedCountTotal += processed + targetGroupTotal;
    m_areaRotatedCount += targetGroupTotal;
}

void GJBaseGameLayer::processAreaTransformGroupAction(cocos2d::CCArray* objects,
                                                       EnterEffectInstance* instance,
                                                       cocos2d::CCPoint position,
                                                       cocos2d::CCPoint* targetPos,
                                                       int outerMax, int middleMin,
                                                       int middleMax, int startIndex,
                                                       bool targetGroups, bool reset) {
    EnterEffectObject* gameObject = instance->m_gameObject;

    float savedScaleX = m_objectLayer->getScaleX();
    float savedScaleY = m_objectLayer->getScaleY();
    cocos2d::CCPoint savedPos = *m_objectLayer->getPosition();

    m_objectLayer->setScale(1.0f);
    m_objectLayer->setPosition(cocos2d::CCPoint(0.0f, 0.0f));

    m_areaScaledCount += cocos2d::CCArray::count(objects);

    if (!objects) {
        m_areaScaledCountTotal += 0;
        m_objectLayer->setScaleX(savedScaleX);
        m_objectLayer->setScaleY(savedScaleY);
        m_objectLayer->setPosition(savedPos);
        return;
    }

    cocos2d::ccArray* data = objects->data;
    if (!data->num) {
        m_areaScaledCountTotal += 0;
        m_objectLayer->setScaleX(savedScaleX);
        m_objectLayer->setScaleY(savedScaleY);
        m_objectLayer->setPosition(savedPos);
        return;
    }

    int processed = 0;
    int targetGroupTotal = 0;
    int targetIndex = 0;

    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = (GameObject*)data->arr[i];
        if (!obj) break;

        bool wasDirty = false;

        if (obj->m_unk3ee) {
            ++processed;
            wasDirty = obj->m_unk4C4 >= m_gameState.m_commandIndex;
        } else {
            int sectionX = obj->m_outerSectionIndex;
            if (sectionX > middleMin || sectionX < outerMax) continue;
            int sectionY = obj->m_middleSectionIndex;
            if (sectionY > startIndex || sectionY < middleMax) continue;

            ++processed;
            wasDirty = obj->m_unk4C4 >= m_gameState.m_commandIndex;
        }

        if (!obj->m_isGroupDisabled && !wasDirty) {
            unsigned int cmdIdx = m_gameState.m_commandIndex;
            if (obj->m_unk4C4 != cmdIdx) {
                obj->m_unk4C4 = cmdIdx;
                obj->m_lastPosition = cocos2d::CCPoint(obj->m_positionX, obj->m_positionY);
                GameObject::dirtifyObjectRect(obj);
            }
        }

        int offX1 = 0;
        if (obj->m_positionXOffset != 0.0f) {
            obj->m_isUnmodifiedPosDirty = true;
            float off = obj->m_positionXOffset;
            obj->m_positionXOffset = 0.0f;
            obj->m_positionX -= off;
            offX1 = 1;
        }
        if (obj->m_positionYOffset != 0.0f) {
            obj->m_isUnmodifiedPosDirty = true;
            float off = obj->m_positionYOffset;
            obj->m_positionYOffset = 0.0f;
            obj->m_positionY -= off;
            offX1 = 1;
        }
        if (obj->m_unk2BC != 0.0f || obj->m_unk2C0 != 0.0f) {
            obj->m_scaleX -= obj->m_unk2BC;
            obj->m_scaleY -= obj->m_unk2C0;
            obj->m_scaleXOffset -= obj->m_unk2BC;
            obj->m_scaleYOffset -= obj->m_unk2C0;
            obj->m_unk2BC = 0.0f;
            obj->m_unk2C0 = 0.0f;
        }
        if (obj->m_unk2A8 != 0.0f || obj->m_unk2B0 != 0.0f) {
            obj->m_rotationXOffset -= obj->m_unk2A8;
            obj->m_rotationYOffset -= obj->m_unk2B0;
            obj->m_unk2A8 = 0.0f;
            obj->m_unk2B0 = 0.0f;
        }

        obj->m_unk4C8 = m_gameState.m_commandIndex;

        if (m_areaObjectsIndex > m_areaObjectsCount) {
            m_areaObjects[m_areaObjectsCount] = obj;
        } else {
            m_areaObjects.push_back(obj);
        }
        m_areaObjectsIndex++;
        m_areaObjectsCount++;

        if (offX1) updateObjectSection(obj);

        bool outOfView = !wasDirty;

        cocos2d::CCPoint objPos = obj->getRealPosition();
        float distance;
        float signCheck;

        int directionType = gameObject->m_directionType;
        if (directionType == 1) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.x - targetPos->x;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
            signCheck = adj;
        } else if (directionType == 2) {
            float offVar = instance->m_offsetVariance;
            float offset = 0.0f;
            float delta = objPos.y - targetPos->y;
            if (offVar != 0.0f) offset = offVar * m_varianceValues[obj->m_varianceIndex + 1];
            float adj = delta + offset;
            distance = adj >= 0.0f ? adj * instance->m_modBack : adj * instance->m_modFront;
            signCheck = adj;
        } else {
            float offset = 0.0f;
            if (instance->m_offsetVariance != 0.0f)
                offset = instance->m_offsetVariance * m_varianceValues[obj->m_varianceIndex + 1];
            float offsetY = 0.0f;
            if (instance->m_offsetYVariance != 0.0f)
                offsetY = instance->m_offsetYVariance * m_varianceValues[obj->m_varianceIndex];
            cocos2d::CCPoint ref(targetPos->x + offset, targetPos->y + offsetY);
            distance = cocos2d::ccpDistance(objPos, ref);
            signCheck = objPos.y - targetPos->y;
        }

        float lengthVar = 0.0f;
        if (instance->m_lengthVariance != 0.0f)
            lengthVar = instance->m_lengthVariance * m_varianceValues[obj->m_varianceIndex];
        float length = (float)(int)(instance->m_length + lengthVar);

        float ratio;
        if (instance->m_deadzone == 0.0f) {
            ratio = distance / length;
        } else {
            ratio = (distance / length - instance->m_deadzone) / (1.0f - instance->m_deadzone);
        }

        bool reverse = instance->m_reversed;
        bool pastTarget = reverse ? (signCheck >= 0.0f) : (signCheck <= 0.0f);

        if (ratio >= 1.0f) ratio = 1.0f;
        else if (ratio <= 0.0f) ratio = 0.0f;

        if (gameObject->m_inbound) ratio = 1.0f - ratio;

        float scaleX = 1.0f;
        float scaleY = 1.0f;

        if (ratio < 1.0f) {
            float sx = gameObject->m_scaleX + gameObject->m_scaleXVariance
                * m_varianceValues[obj->m_varianceIndex];
            float sy = gameObject->m_scaleY + gameObject->m_scaleYVariance
                * m_varianceValues[obj->m_varianceIndex];

            if (sx != 1.0f || sy != 1.0f) {
                bool useOutEase = gameObject->m_easeOutEnabled && (ratio > 0.01f && ratio < 0.99f && !pastTarget);
                if (useOutEase) {
                    int& easeIndex = instance->m_easeIndices[targetIndex];
                    int currentEaseIdx = instance->m_easeIndex;
                    if (easeIndex == currentEaseIdx - 2) {
                        easeIndex = currentEaseIdx;
                    } else {
                        if (easeIndex == currentEaseIdx - 1) pastTarget = true;
                        easeIndex = currentEaseIdx + (pastTarget ? 1 : 0);
                    }
                }

                float eased = pastTarget
                    ? getEnterEasingValue(ratio, gameObject->m_easingInType,
                                          gameObject->m_easingInRate, gameObject->m_easingInBuffer)
                    : getEnterEasingValue(ratio, gameObject->m_easingOutType,
                                          gameObject->m_easingOutRate, gameObject->m_easingOutBuffer);

                scaleX = (sx - 1.0f) * (1.0f - eased) + 1.0f;
                scaleY = (sy - 1.0f) * (1.0f - eased) + 1.0f;
            }
        }

        if (reset) {
            cocos2d::CCArray* target = getTargetGroup(instance->m_targetGroupIndex, obj->m_uniqueID);
            cocos2d::CCArray* origin = getTargetGroupOrigin(instance->m_targetGroupIndex, obj->m_uniqueID);
            targetGroupTotal = targetGroupTotal - 1 + cocos2d::CCArray::count(target);

            if (gameObject->m_dontEditAreaParent) obj->m_unk3ee = true;

            transformAreaObjects(obj, target, scaleX, scaleY, outOfView);
            obj->m_unk3ee = false;
        } else {
            float objScaleX = obj->m_scaleX;
            float objScaleY = obj->m_scaleY;
            float deltaX = objScaleX * (scaleX - 1.0f);
            float deltaY = objScaleY * (scaleY - 1.0f);

            if (deltaX != 0.0f || deltaY != 0.0f) {
                LOBYTE(obj->m_unknown2) = 1;
                LOBYTE(obj->m_uID) = 1;
                obj->m_scaleX += deltaX;
                obj->m_scaleY += deltaY;
                obj->m_scaleXOffset += deltaY;
                obj->m_scaleYOffset += deltaX;
                obj->m_unk2BC += deltaX;
                obj->m_unk2C0 += deltaY;
                if (outOfView) obj->updateTransform();
            }
        }

        if (outOfView) updateObjectSection(obj);
        ++targetIndex;
    }

    m_objectLayer->setScaleX(savedScaleX);
    m_objectLayer->setScaleY(savedScaleY);
    m_objectLayer->setPosition(savedPos);

    m_areaScaledCountTotal += processed + targetGroupTotal;
    m_areaScaledCount += targetGroupTotal;
}

void GJBaseGameLayer::processActivatedAudioTriggers(float levelTime) {
    m_processingAudioTriggers = true;

    std::unordered_set<int> activeMusicChannels;
    bool hasDefaultSong = false;

    for (auto& pair : m_gameState.m_songChannelStates) {
        int channelID = pair.first;
        SongChannelState& state = pair.second;

        if (state.m_activatedTrigger) {
            activateSongTrigger(state.m_activatedTrigger);
        }

        if (state.m_songTrigger) {
            SongTriggerGameObject* trigger = state.m_songTrigger;
            trigger->m_spawnTriggerDelay = state.m_spawnTriggerDelay;

            float pitch = FMODAudioEngine::pitchForIdx(trigger->m_speed);
            float volume = trigger->m_volume;
            std::string songPath = MusicDownloadManager::pathForSong(trigger->m_soundID);
            bool dontReset = trigger->m_dontReset;
            bool loop = trigger->m_loop;

            FMODAudioEngine::sharedEngine()->loadMusic(
                songPath, pitch, 0.0f, volume, loop, channelID, 0, dontReset);

            if (loop && (trigger->m_start > 0 || trigger->m_end > 0)) {
                FMODAudioEngine::sharedEngine()->startMusic(
                    trigger->m_start, trigger->m_end, 0, 0, 1, channelID, 1, dontReset);
            }

            std::vector<SongTriggerState>* stateVector = nullptr;
            auto it = m_gameState.m_songTriggerStateVectors.find(channelID);
            if (it != m_gameState.m_songTriggerStateVectors.end()) {
                stateVector = &it->second;
            }

            if (processSongState(channelID, trigger->m_spawnTriggerDelay, levelTime,
                                  trigger->m_start, volume, pitch, stateVector, trigger)) {
                activeMusicChannels.insert(channelID);
            }

            if (channelID == 0) hasDefaultSong = true;
        } else if (channelID == 0 && state.m_spawnTriggerDelay > 0.0f) {
            hasDefaultSong = true;
        }
    }

    if (!hasDefaultSong && !m_gameState.m_unkBool30) {
        std::string audioFileName = m_level->getAudioFileName();
        bool loop = m_isPlatformer;
        bool dontReset = m_levelSettings->m_dontReset;

        FMODAudioEngine::sharedEngine()->loadMusic(
            audioFileName, 1.0f, 0.0f, 1.0f, loop, 0, 0, dontReset);

        int songOffsetMs = (int)(m_levelSettings->m_songOffset * 1000.0f);

        std::vector<SongTriggerState>* stateVector = nullptr;
        auto it = m_gameState.m_songTriggerStateVectors.find(0);
        if (it != m_gameState.m_songTriggerStateVectors.end()) {
            stateVector = &it->second;
        }

        if (processSongState(0, 0.0f, levelTime, songOffsetMs, 1.0f, 1.0f, stateVector, nullptr)) {
            activeMusicChannels.insert(0);
        }
    }

    FMODAudioEngine::sharedEngine()->stopMusicNotInSet(activeMusicChannels);

    std::unordered_map<int, std::vector<int>> triggeredSfx;
    std::unordered_map<int, std::vector<int>> triggeredGroups;
    std::unordered_map<int, int> stateIndices;
    std::unordered_map<int, float> times;
    std::unordered_map<int, int> stateIndexMap;

    for (SFXTriggerState& state : m_gameState.m_sfxTriggerStates) {
        SFXTriggerGameObject* trigger = state.m_sfxTriggerGameObject;
        int objectID = trigger->m_objectID;

        if (objectID == 3602) {
            if (canProcessSFX(&state, &stateIndices, &times, &m_gameState.m_sfxTriggerStates)) {
                int uniqueID = trigger->m_uniqueID;
                triggeredSfx[uniqueID].push_back(state.m_unkInt1);
                stateIndexMap[state.m_unkInt1] = state.m_unkInt2;

                if (trigger->m_sfxGroup > 0) {
                    triggeredGroups[trigger->m_sfxGroup].push_back(state.m_unkInt1);
                }
            } else {
                state.m_processed = true;
            }
        } else if (objectID == 3603) {
            std::unordered_set<int> triggerSet;

            if (trigger->m_groupID > 0) {
                cocos2d::CCArray* group = getGroup(trigger->m_groupID);
                if (group && cocos2d::CCArray::count(group)) {
                    for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
                        GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
                        if (!obj) break;
                        if (obj->m_objectID == 3602) {
                            int uid = obj->m_uniqueID;
                            auto it = triggeredSfx.find(uid);
                            if (it != triggeredSfx.end()) {
                                for (int idx : it->second) triggerSet.insert(idx);
                            }
                        }
                    }
                }
            }

            if (trigger->m_sfxUniqueID > 0 && stateIndices.count(trigger->m_sfxUniqueID)) {
                int resolvedIdx = stateIndices[trigger->m_sfxUniqueID];
                int resolvedUID = m_gameState.m_sfxTriggerStates[resolvedIdx].m_sfxTriggerGameObject->m_uniqueID;
                auto it = triggeredSfx.find(resolvedUID);
                if (it != triggeredSfx.end()) {
                    for (int idx : it->second) triggerSet.insert(idx);
                }
            }

            for (auto& pair : stateIndexMap) {
                if (triggerSet.count(pair.first)) {
                    processSFXState(&m_gameState.m_sfxTriggerStates[pair.second], &state, 0, levelTime);
                }
            }

            if (trigger->m_sfxGroup > 0) {
                auto it = triggeredGroups.find(trigger->m_sfxGroup);
                if (it != triggeredGroups.end()) {
                    for (int idx : it->second) {
                        if (stateIndexMap.count(idx)) {
                            processSFXState(&m_gameState.m_sfxTriggerStates[stateIndexMap[idx]], &state, 1, levelTime);
                        }
                    }
                }
            }

            if (trigger->m_changeVolume) processSFXGroupEdit(&state, &stateIndices, state.m_unkFloat1);
            if (trigger->m_changeSpeed)  processSFXGroupEdit(&state, &times, state.m_unkFloat3);
        }
    }

    for (int i = 0; i < 2; ++i) {
        auto& map = (i == 0) ? times : stateIndices;
        for (auto& pair : map) {
            FMODAudioEngine::sharedEngine()->updateChannel(
                pair.first, SFXGroup, i == 0 ? Volume : Pitch, 0.0f, pair.second);
        }
    }

    for (size_t i = 0; i < m_gameState.m_sfxTriggerStates.size(); ++i) {
        SFXTriggerState* state = &m_gameState.m_sfxTriggerStates[i];
        processSFXState(state, nullptr, 0, levelTime);

        SFXTriggerGameObject* trigger = state->m_sfxTriggerGameObject;
        int objectID = trigger->m_objectID;

        if (objectID == 3602) {
            if (state->m_processed) continue;

            FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
            int savedEnd = trigger->m_end;
            bool savedPreload = trigger->m_preload;
            bool loop = trigger->m_loop;

            engine->m_audioState.m_elapsed = state->m_unkDouble1;
            trigger->m_preload = true;

            if (!loop && trigger->m_fadeOut) {
                int end = savedEnd;
                if (end <= 0) {
                    float pitch = trigger->m_speed ? FMODAudioEngine::pitchForIdx(trigger->m_speed) : 1.0f;
                    end = (int)(pitch * (trigger->m_soundDuration * 1000.0f));
                }
                trigger->m_end = (int)((double)end - state->m_unkDouble2);
            }

            activateSFXTrigger(trigger);
            trigger->m_preload = savedPreload;
            trigger->m_end = savedEnd;
            continue;
        } else if (objectID == 3603) {
            float savedDuration = trigger->m_duration;
            int targetGroupID = trigger->m_targetGroupID;
            float remaining = (savedDuration + trigger->m_spawnTriggerDelay) - levelTime;
            trigger->m_duration = remaining;

            bool shouldActivate = false;
            if (remaining < 0.0f) {
                trigger->m_applyDisabled = true;
                if (targetGroupID > 0) shouldActivate = true;
            } else if (targetGroupID > 0 || !trigger->m_applyDisabled) {
                shouldActivate = true;
            }

            if (shouldActivate) activateSFXEditTrigger(trigger);

            trigger->m_duration = savedDuration;
            trigger->m_applyDisabled = false;
        }
    }

    m_gameState.m_sfxTriggerStates.clear();
    FMODAudioEngine::sharedEngine()->m_audioState.m_elapsed = levelTime;
    m_varianceValues[1975 + 1861] = 0;
    m_processingAudioTriggers = false;
}

void GJBaseGameLayer::processSFXState(SFXTriggerState* state1, SFXTriggerState* state2,
                                      int type, float levelTime) {
    if (state1->m_processed) return;

    SFXTriggerGameObject* trigger2 = nullptr;
    if (state2) {
        trigger2 = state2->m_sfxTriggerGameObject;
        if (trigger2->m_stop) { state1->m_processed = true; return; }
        levelTime = (float)state2->m_unkDouble1;
    }

    bool hasVolumeChange = state2 && trigger2->m_changeVolume;
    bool hasStopLoop = state2 && trigger2->m_stopLoop;

    if (!state2 || hasVolumeChange || hasStopLoop) {
        double lastTime = state1->m_unkDouble3;
        if (lastTime < (double)levelTime) {
            double accumulated = state1->m_unkDouble2;
            float volumeF = state1->m_unkFloat1;
            float pitchF = state1->m_unkFloat2;

            while (true) {
                auto& c0 = state1->m_sfxStateContainers[0];
                auto& c1 = state1->m_sfxStateContainers[1];
                double sampleT;

                if (c0.m_unkBool) {
                    double start = c0.m_unkDouble1;
                    double duration = c0.m_unkDouble2;
                    double end = start + duration;
                    sampleT = std::min((double)levelTime, end);
                    if (c1.m_unkBool) {
                        double pEnd = c1.m_unkDouble1 + c1.m_unkDouble2;
                        if (pEnd < sampleT) sampleT = pEnd;
                    }
                    float target = c0.m_unkFloat1;
                    if (sampleT >= end) { c0.m_unkBool = false; volumeF = target; }
                    else {
                        float ratio = (float)((sampleT - start) / duration);
                        volumeF = volumeF + (target - volumeF) * ratio;
                    }
                    state1->m_unkFloat1 = volumeF;
                    volumeF = (volumeF + state1->m_unkFloat1) * 0.5f;
                } else {
                    sampleT = (double)levelTime;
                    accumulated += ((double)levelTime - lastTime) * volumeF * pitchF * 1000.0;
                }

                if (c1.m_unkBool) {
                    double start = c1.m_unkDouble1;
                    double duration = c1.m_unkDouble2;
                    double end = start + duration;
                    float target = c1.m_unkFloat1;
                    if (sampleT >= end) { c1.m_unkBool = false; pitchF = target; }
                    else {
                        float ratio = (float)((sampleT - start) / duration);
                        pitchF = pitchF + (target - pitchF) * ratio;
                    }
                    state1->m_unkFloat2 = pitchF;
                    pitchF = (pitchF + state1->m_unkFloat2) * 0.5f;
                    accumulated += (sampleT - lastTime) * volumeF * pitchF * 1000.0;
                    lastTime = sampleT;
                    if (sampleT >= (double)levelTime) break;
                    continue;
                }

                accumulated += ((double)levelTime - lastTime) * volumeF * pitchF * 1000.0;
                lastTime = (double)levelTime;
                break;
            }

            state1->m_unkDouble2 = accumulated;
            state1->m_unkDouble3 = lastTime;
        }

        int containerIdx = type ? 1 : 0;
        auto& target = state1->m_sfxStateContainers[containerIdx];

        if (hasVolumeChange) {
            target.m_unkBool = true;
            target.m_unkDouble1 = (double)levelTime;
            target.m_unkFloat1 = state2->m_unkFloat1;
            target.m_unkDouble2 = trigger2->m_duration;
            target.m_unkFloat2 = 0.0f;
        } else if (target.m_unkBool) {
            double start = target.m_unkDouble1;
            double duration = target.m_unkDouble2;
            if (start + duration >= (double)levelTime) {
                target.m_unkDouble1 = (double)levelTime;
                target.m_unkDouble2 = duration - ((double)levelTime - start);
            } else {
                target.m_unkBool = false;
            }
        }

        if (!type) {
            if (state2 && trigger2->m_changeSpeed) {
                auto& speedC = state1->m_sfxStateContainers[2];
                if (!speedC.m_unkBool) {
                    speedC.m_unkBool = true;
                    speedC.m_unkDouble1 = (double)levelTime;
                    speedC.m_unkFloat1 = state2->m_unkFloat3;
                    speedC.m_unkDouble2 = trigger2->m_duration;
                    speedC.m_unkFloat2 = 0.0f;
                    state1->m_unkDouble4 = (double)levelTime;
                } else {
                    double start = speedC.m_unkDouble1;
                    double duration = speedC.m_unkDouble2;
                    float target = speedC.m_unkFloat1;
                    double elapsed = (double)levelTime - start;
                    if (duration > elapsed && duration > 0.0)
                        target = state1->m_unkFloat3 + (target - state1->m_unkFloat3) * (float)(elapsed / duration);
                    state1->m_unkFloat3 = target;
                    if (start + duration >= (double)levelTime) {
                        speedC.m_unkDouble1 = (double)levelTime;
                        state1->m_unkDouble4 = (double)levelTime;
                        speedC.m_unkDouble2 = duration - ((double)levelTime - start);
                    } else {
                        speedC.m_unkBool = false;
                        state1->m_unkDouble4 = (double)levelTime;
                    }
                }
            } else if (!state2) {
                auto& speedC = state1->m_sfxStateContainers[2];
                if (!speedC.m_unkBool) {
                    state1->m_unkDouble4 = (double)levelTime;
                } else {
                    double start = speedC.m_unkDouble1;
                    double duration = speedC.m_unkDouble2;
                    float target = speedC.m_unkFloat1;
                    double elapsed = (double)levelTime - start;
                    if (duration > elapsed && duration > 0.0)
                        target = state1->m_unkFloat3 + (target - state1->m_unkFloat3) * (float)(elapsed / duration);
                    state1->m_unkFloat3 = target;
                    if (start + duration >= (double)levelTime) {
                        speedC.m_unkDouble1 = (double)levelTime;
                        state1->m_unkDouble4 = (double)levelTime;
                        speedC.m_unkDouble2 = duration - ((double)levelTime - start);
                    } else {
                        speedC.m_unkBool = false;
                        state1->m_unkDouble4 = (double)levelTime;
                    }
                }
            }
        }
    }

    if (state2 && trigger2->m_stopLoop) {
        if (state1->m_unkBool1) {
            float interval = state1->m_unkFloat4;
            state1->m_unkBool1 = false;
            state1->m_unkDouble2 -= interval * std::floor(state1->m_unkDouble2 / interval);
        } else if (state1->m_sfxTriggerGameObject->m_objectID == 3602) {
            if (state1->m_unkDouble2 > (double)state1->m_unkFloat4)
                state1->m_processed = true;
        }
        return;
    }

    if (state1->m_sfxTriggerGameObject->m_objectID != 3602 || state1->m_unkBool1) return;

    if (state1->m_unkDouble2 > (double)state1->m_unkFloat4)
        state1->m_processed = true;
}

bool GJBaseGameLayer::processSongState(int musicID, float spawnDelay, float levelTime,
                                        int songOffset, float volume, float speed,
                                        std::vector<SongTriggerState>* states,
                                        SongTriggerGameObject* object) {
    SongTriggerGameObject* prevVolumeTrigger = nullptr;
    SongTriggerGameObject* prevSpeedTrigger = nullptr;

    if (states && !states->empty()) {
        SongTriggerState* iter = states->data();
        SongTriggerState* end = iter + states->size();

        while (true) {
            SongTriggerGameObject* trigger = iter->m_songTriggerGameObject;
            double triggerTime = iter->m_unkDouble;
            trigger->m_spawnTriggerDelay = (float)triggerTime;

            if (triggerTime >= (double)spawnDelay) {
                if (trigger->m_stop) {
                    FMODAudioEngine::sharedEngine()->stopMusic(musicID);
                    return false;
                }

                if (trigger->m_changeVolume) {
                    if (prevVolumeTrigger) {
                        float duration = prevVolumeTrigger->m_duration;
                        float elapsed = (float)(triggerTime - prevVolumeTrigger->m_spawnTriggerDelay);
                        float newPitch = FMODAudioEngine::pitchForIdx(prevVolumeTrigger->m_speed);
                        float remaining = elapsed - duration;

                        if (duration <= 0.0f) {
                            speed = newPitch;
                        } else {
                            if (duration > elapsed) {
                                float ratio = elapsed / duration;
                                duration = elapsed;
                                remaining = 0.0f;
                                newPitch = speed + (newPitch - speed) * ratio;
                            }
                            float avg = speed + newPitch;
                            speed = newPitch;
                            songOffset = (int)((float)songOffset + duration * (avg * 0.5f) * 1000.0f);
                        }
                        prevVolumeTrigger = trigger;
                        if (remaining > 0.0f)
                            songOffset = (int)((float)songOffset + remaining * newPitch * 1000.0f);
                    } else {
                        prevVolumeTrigger = trigger;
                        songOffset = (int)((float)songOffset + (float)(triggerTime - spawnDelay) * speed * 1000.0f);
                    }
                }

                if (trigger->m_changeSpeed) {
                    if (!prevSpeedTrigger) {
                        prevSpeedTrigger = trigger;
                    } else {
                        float duration = prevSpeedTrigger->m_duration;
                        float elapsed = trigger->m_spawnTriggerDelay - prevSpeedTrigger->m_spawnTriggerDelay;
                        float targetVol = trigger->m_volume;
                        if (duration > elapsed && duration > 0.0f) {
                            volume = volume + (targetVol - volume) * (elapsed / duration);
                        } else {
                            volume = targetVol;
                        }
                        prevSpeedTrigger = trigger;
                    }
                }
            }

            ++iter;
            if (iter == end) break;
        }
    }

    if (prevVolumeTrigger) {
        float duration = prevVolumeTrigger->m_duration;
        float elapsed = levelTime - prevVolumeTrigger->m_spawnTriggerDelay;
        float newPitch = FMODAudioEngine::pitchForIdx(prevVolumeTrigger->m_speed);
        float remaining = elapsed - duration;

        if (duration <= 0.0f) {
            if (remaining > 0.0f) {
                speed = newPitch;
                songOffset = (int)((float)songOffset + newPitch * remaining * 1000.0f);
            } else {
                FMODAudioEngine::sharedEngine()->updateChannel(musicID, MusicChannel, Pitch, 0.0f, newPitch);
                speed = newPitch;
            }
        } else {
            float fadeTime = 0.0f;
            if (duration > elapsed) {
                float ratio = elapsed / duration;
                fadeTime = duration - elapsed;
                speed = speed + (newPitch - speed) * ratio;
                songOffset = (int)((float)songOffset + elapsed * (speed + speed) * 0.5f * 1000.0f);
            } else {
                songOffset = (int)((float)songOffset + duration * (newPitch + speed) * 0.5f * 1000.0f);
                if (remaining > 0.0f) {
                    speed = newPitch;
                    songOffset = (int)((float)songOffset + newPitch * remaining * 1000.0f);
                } else {
                    speed = newPitch;
                }
            }

            FMODAudioEngine::sharedEngine()->updateChannel(musicID, MusicChannel, Pitch, 0.0f, speed);
            if (fadeTime > 0.0f)
                FMODAudioEngine::sharedEngine()->updateChannel(musicID, MusicChannel, Pitch, fadeTime, newPitch);
        }
    }

    if (prevSpeedTrigger) {
        float duration = prevSpeedTrigger->m_duration;
        float elapsed = levelTime - prevSpeedTrigger->m_spawnTriggerDelay;
        float targetVol = prevSpeedTrigger->m_volume;

        if (duration <= elapsed || duration <= 0.0f) {
            FMODAudioEngine::sharedEngine()->updateChannel(musicID, MusicChannel, Volume, 0.0f, targetVol);
        } else {
            float ratio = elapsed / duration;
            float fadeTime = duration - elapsed;
            float currentVol = volume + (targetVol - volume) * ratio;
            FMODAudioEngine::sharedEngine()->updateChannel(musicID, MusicChannel, Volume, 0.0f, currentVol);
            if (fadeTime > 0.0f)
                FMODAudioEngine::sharedEngine()->updateChannel(musicID, MusicChannel, Volume, fadeTime, targetVol);
        }
    }

    songOffset = (int)((float)songOffset + (float)(levelTime - spawnDelay) * speed * 1000.0f);

    if (!object) {
        if (m_levelSettings->m_dontReset) return true;
    } else {
        if (object->m_loop && object->m_end > 0) {
            FMOD::Channel* channel = FMODAudioEngine::sharedEngine()->getActiveMusicChannel(musicID);
            if (channel) {
                unsigned int loopStart = 0;
                unsigned int loopEnd = 0;
                channel->getLoopPoints(&loopStart, 1, &loopEnd, 1);
                songOffset = (songOffset - (int)loopStart) % ((int)loopEnd - (int)loopStart) + (int)loopStart;
            }
        }
        if (object->m_dontReset) return true;
    }

    FMODAudioEngine::sharedEngine()->setMusicTimeMS(songOffset, 0, musicID);
    return true;
}

void GJBaseGameLayer::preUpdateVisibility(float dt) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint cameraPos = m_gameState.m_cameraPosition2;
    float cameraZoom = m_gameState.m_cameraZoom;

    if (m_isEditor && !m_started) {
        cameraZoom = m_objectLayer->getScale();
        cocos2d::CCPoint layerPos = *m_objectLayer->getPosition();
        cameraPos = layerPos / cameraZoom;
        if (m_gameState.m_cameraShakeEnabled) {
            cameraPos -= m_gameState.m_unkPoint34;
        }
    }

    m_activeObjectsCount = 0;
    m_activeObjectsIndex = 0;
    m_visibleObjectsCount = 0;
    m_visibleObjectsIndex = 0;
    m_visibleObjects2Count = 0;
    m_visibleObjects2Index = 0;

    float width = winSize.width / cameraZoom;
    float height = winSize.height / cameraZoom;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

    m_cameraUnzoomedX = cameraPos.x + winSize.width * 0.5f;
    m_cameraWidth = width;
    m_halfCameraWidth = halfWidth;
    m_cameraHeight = height;

    cocos2d::CCRect cameraRect(cameraPos.x, cameraPos.y, width, height);

    if (m_gameState.m_cameraAngle == 0.0f) {
        m_cameraWidthOffset = 0.0f;
        m_cameraHeightOffset = 0.0f;
        m_cameraUnzoomedHeightOffset = 0.0f;
        m_targetCameraHeightOffset = 0.0f;
        bool groundWasRotated = m_groundLayer->m_cameraRotated;
        if (groundWasRotated) {
            m_groundLayer->m_cameraRotated = false;
            m_groundLayer2->m_cameraRotated = false;
            GJGroundLayer::scaleGround(m_groundLayer, m_groundLayer->getScaleX());
            GJGroundLayer::scaleGround(m_groundLayer2, m_groundLayer2->getScaleX());
        }
    } else {
        if (m_calculateTargetHeightOffset) {
            m_cameraObb2->calculateWithCenter(winSize / cameraZoom,
                                              m_gameState.m_targetCameraAngle * 0.017453f,
                                              m_gameState.m_targetCameraAngle, 0.0f);
            cocos2d::CCRect targetBounds = m_cameraObb2->getBoundingRect();
            m_targetCameraHeightOffset = targetBounds.size.height - height;
            m_calculateTargetHeightOffset = false;
        }

        m_cameraObb2->calculateWithCenter(winSize / cameraZoom,
                                          m_gameState.m_cameraAngle * 0.017453f,
                                          m_gameState.m_cameraAngle, 0.0f);
        cocos2d::CCRect obbBounds = m_cameraObb2->getBoundingRect();

        bool groundRotated = (m_gameState.m_cameraAngle != 0.0f);
        if (m_groundLayer->m_cameraRotated != groundRotated) {
            m_groundLayer->m_cameraRotated = groundRotated;
            m_groundLayer2->m_cameraRotated = groundRotated;
            GJGroundLayer::scaleGround(m_groundLayer, m_groundLayer->getScaleX());
            GJGroundLayer::scaleGround(m_groundLayer2, m_groundLayer2->getScaleX());
        }

        m_cameraUnzoomedHeightOffset = obbBounds.size.height - height;
        m_cameraHeightOffset = cameraZoom * m_cameraUnzoomedHeightOffset;
        m_cameraWidthOffset = (obbBounds.size.width - width) * cameraZoom;

        if (m_cameraWidthOffset != 0.0f) {
            updateGroundShadows();
            m_updateGroundShadows = true;
            float widthDiff = m_cameraWidthOffset / cameraZoom;
            float heightDiff = m_cameraHeightOffset / cameraZoom;
            cameraRect.size.width += widthDiff;
            cameraRect.size.height += heightDiff;
            cameraRect.origin.x -= widthDiff * 0.5f;
            cameraRect.origin.y -= heightDiff * 0.5f;
        }
    }

    if (m_cameraWidthOffset == 0.0f && m_updateGroundShadows) {
        GJGroundLayer::updateShadowXPos(m_groundLayer, 0.0f, 0.0f);
        GJGroundLayer::updateShadows(m_groundLayer);
        GJGroundLayer::updateShadowXPos(m_groundLayer2, 0.0f, 0.0f);
        GJGroundLayer::updateShadows(m_groundLayer2);
        m_updateGroundShadows = false;
    }

    float maxX = cameraRect.getMaxX();
    float minX = cameraRect.getMinX();
    float maxY = cameraRect.getMaxY();
    float minY = cameraRect.getMinY();

    int leftSection = 0;
    if (std::min(minX, 10000000.0f) * m_sectionXFactor - 1.0f > 0.0f) {
        leftSection = (int)(floorf(std::min(minX, 10000000.0f) * m_sectionXFactor) - 1.0f);
    }

    int rightSection = 0;
    if (std::min(maxX, 10000000.0f) * m_sectionXFactor + 1.0f > 0.0f) {
        rightSection = (int)(ceilf(std::min(maxX, 10000000.0f) * m_sectionXFactor) + 1.0f);
    }

    int bottomSection = 0;
    if (std::min(minY, 10000000.0f) * m_sectionYFactor - 1.0f > 0.0f) {
        bottomSection = (int)(floorf(std::min(minY, 10000000.0f) * m_sectionYFactor) - 1.0f);
    }

    int topSection = 0;
    if (std::min(maxY, 10000000.0f) * m_sectionYFactor + 1.0f > 0.0f) {
        topSection = (int)(ceilf(std::min(maxY, 10000000.0f) * m_sectionYFactor) + 1.0f);
    }

    cocos2d::ccColor3B color2 = GJEffectManager::activeColorForIndex(m_effectManager, 1007);
    cocos2d::ccColor3B color1 = GJEffectManager::activeColorForIndex(m_effectManager, 1000);
    cocos2d::ccHSVValue hsv = { 1.0f, 1.0f, 1.0f, false, false };
    m_lightBGColor = GameToolbox::transformColor(color1, hsv);

    int totalSections = (int)(m_sections.m_finish - m_sections.m_start);

    for (int x = m_leftSectionIndex; x <= m_rightSectionIndex && x < totalSections; x++) {
        auto* xVec = m_sections.m_start[x];
        if (!xVec) continue;
        int maxMiddle = (int)xVec->size() - 1;
        if (maxMiddle > m_topSectionIndex) maxMiddle = m_topSectionIndex;
        for (int y = m_bottomSectionIndex; y <= maxMiddle; y++) {
            auto* vec = (*xVec)[y];
            if (!vec) continue;
            int count = (*m_sectionSizes.m_start[x])[y];
            for (int i = 0; i < count; i++) {
                (*vec)[i]->m_unk3ee = 1;
            }
        }
    }

    for (int i = 0; i < m_visibleObjects2Count; i++) {
        m_visibleObjects2.m_start[i]->m_unk3ee = 1;
    }

    std::unordered_set<int> affectedVisibilityGroups;

    for (int x = leftSection; x <= rightSection && x < totalSections; x++) {
        auto* xVec = m_sections.m_start[x];
        if (!xVec) continue;
        int maxMiddle = (int)xVec->size() - 1;
        if (maxMiddle > topSection) maxMiddle = topSection;

        bool forceMark = (rightSection >= totalSections - 1) || (leftSection + 1 >= totalSections);

        for (int y = bottomSection; y <= maxMiddle; y++) {
            auto* vec = (*xVec)[y];
            if (!vec) continue;
            int count = (*m_sectionSizes.m_start[x])[y];

            bool forceProcess = forceMark || maxMiddle <= bottomSection + 1 || bottomSection == topSection;

            for (int i = 0; i < count; i++) {
                GameObject* obj = (*vec)[i];
                if (obj->m_isGroupDisabled) {
                    obj->m_unk3ee = 1;
                    continue;
                }
                if (obj->m_isActivated) {
                    obj->m_unk3ee = 0;
                    if (m_activeObjectsIndex > m_activeObjectsCount) {
                        m_activeObjects.m_start[m_activeObjectsCount] = obj;
                    } else {
                        m_activeObjects.push_back(obj);
                    }
                    m_activeObjectsIndex++;
                    m_activeObjectsCount++;
                    if (obj->m_unk35C > 0) {
                        affectedVisibilityGroups.insert(obj->m_unk35C);
                    }
                    continue;
                }
                cocos2d::CCRect* objRect = obj->getObjectTextureRect();
                bool visible = maxX >= objRect->getMinX() && minX <= objRect->getMaxX()
                            && maxY >= objRect->getMinY() && minY <= objRect->getMaxY();
                if (visible || obj->m_unk370) {
                    obj->m_unk3ee = 0;
                    if (m_activeObjectsIndex > m_activeObjectsCount) {
                        m_activeObjects.m_start[m_activeObjectsCount] = obj;
                    } else {
                        m_activeObjects.push_back(obj);
                    }
                    m_activeObjectsIndex++;
                    m_activeObjectsCount++;
                    if (obj->m_unk35C > 0) {
                        affectedVisibilityGroups.insert(obj->m_unk35C);
                    }
                } else {
                    obj->m_unk3ee = 1;
                }
            }
        }
    }

    if (m_uiObjects) {
        cocos2d::ccArray* data = m_uiObjects->data;
        for (unsigned int i = 0; i < data->num; i++) {
            GameObject* obj = (GameObject*)data->arr[i];
            if (!obj) break;
            if (!obj->m_isGroupDisabled) {
                cocos2d::CCRect* objRect = obj->getObjectTextureRect();
                bool visible = winSize.width >= objRect->getMinX() && objRect->getMaxX() >= 0.0f
                            && winSize.height >= objRect->getMinY() && objRect->getMaxY() >= 0.0f;
                if (visible || obj->m_unk370) {
                    obj->m_unk3ee = 0;
                    if (m_activeObjectsIndex > m_activeObjectsCount) {
                        m_activeObjects.m_start[m_activeObjectsCount] = obj;
                    } else {
                        m_activeObjects.push_back(obj);
                    }
                    m_activeObjectsIndex++;
                    m_activeObjectsCount++;
                    if (obj->m_unk35C > 0) {
                        affectedVisibilityGroups.insert(obj->m_unk35C);
                    }
                    continue;
                }
            }
            obj->m_unk3ee = 1;
        }
    }

    m_visibleObjectsCount = 0;
    for (int groupID : affectedVisibilityGroups) {
        auto it = m_visibilityGroups.find(groupID);
        if (it == m_visibilityGroups.end()) continue;
        for (GameObject* obj : it->second) {
            if (obj->m_isActivated && !obj->m_unk3ee) continue;
            if (obj->m_isGroupDisabled) continue;
            obj->m_unk3ee = 0;
            if (m_activeObjectsIndex > m_activeObjectsCount) {
                m_activeObjects.m_start[m_activeObjectsCount] = obj;
            } else {
                m_activeObjects.push_back(obj);
            }
            m_activeObjectsIndex++;
            m_activeObjectsCount++;
            if (m_visibleObjectsIndex > m_visibleObjectsCount) {
                m_visibleObjects.m_start[m_visibleObjectsCount] = obj;
            } else {
                m_visibleObjects.push_back(obj);
            }
            m_visibleObjectsIndex++;
            m_visibleObjectsCount++;
        }
    }

    for (int i = 0; i < m_visibleObjects2Count; i++) {
        GameObject* obj = m_visibleObjects2.m_start[i];
        if (obj->m_unk3ee) {
            obj->deactivateObject(0);
        }
    }

    m_visibleObjects2Count = 0;
    m_visibleObjects2Index = 0;
    for (int i = 0; i < m_visibleObjectsCount; i++) {
        if (m_visibleObjects2Index > m_visibleObjects2Count) {
            m_visibleObjects2.m_start[m_visibleObjects2Count] = m_visibleObjects.m_start[i];
        } else {
            m_visibleObjects2.push_back(m_visibleObjects.m_start[i]);
        }
        m_visibleObjects2Index++;
        m_visibleObjects2Count++;
    }

    if (!m_objectsDeactivated) {
        int startX = m_leftSectionIndex;
        int endX = m_rightSectionIndex;
        if (startX < totalSections && startX <= endX) {
            for (int x = startX; x <= endX && x < totalSections; x++) {
                auto* xVec = m_sections.m_start[x];
                if (!xVec) continue;
                int maxMiddle = (int)xVec->size() - 1;
                if (maxMiddle > m_topSectionIndex) maxMiddle = m_topSectionIndex;
                for (int y = m_bottomSectionIndex; y <= maxMiddle; y++) {
                    auto* vec = (*xVec)[y];
                    if (!vec) continue;
                    int count = (*m_sectionSizes.m_start[x])[y];
                    for (int i = 0; i < count; i++) {
                        (*vec)[i]->deactivateObject(0);
                    }
                }
            }
        }
    }

    if (m_uiObjects) {
        cocos2d::ccArray* data = m_uiObjects->data;
        for (unsigned int i = 0; i < data->num; i++) {
            GameObject* obj = (GameObject*)data->arr[i];
            if (!obj) break;
            obj->deactivateObject(0);
        }
    }

    if (m_objectsToDeactivate && m_objectsToDeactivate->m_pElements) {
        cocos2d::CCDictElement* element = m_objectsToDeactivate->m_pElements;
        while (element) {
            GameObject* obj = (GameObject*)element->m_pObject;
            cocos2d::CCDictElement* next = (cocos2d::CCDictElement*)element->hh.next;
            int sectionX = obj->m_nLuaID;
            int sectionY = obj->m_nTag;
            bool inView = sectionX <= rightSection && sectionX >= leftSection
                       && sectionY <= topSection && sectionY >= bottomSection;
            if (!inView) {
                obj->deactivateObject(1);
            }
            element = next;
        }
    }
    m_objectsToDeactivate->removeAllObjects();

    if (m_batchNodes) {
        cocos2d::ccArray* data = m_batchNodes->data;
        for (unsigned int i = 0; i < data->num; i++) {
            cocos2d::CCNode* node = (cocos2d::CCNode*)data->arr[i];
            if (!node) break;
            node->updateChildIndexes();
        }
    }

    if (m_glitterParticles) {
        float posVarX = (cocos2d::CCDirector::sharedDirector()->getScreenScaleFactorW() * 480.0f / 1.8f) / cameraZoom;
        float posVarY = cocos2d::CCDirector::sharedDirector()->getScreenScaleFactorH() * 320.0f * 0.5f / cameraZoom;
        m_glitterParticles->setPosVar(cocos2d::CCPoint(posVarX, posVarY));
    }

    m_objectsDeactivated = false;
    m_leftSectionIndex = leftSection;
    m_rightSectionIndex = rightSection;
    m_bottomSectionIndex = bottomSection;
    m_topSectionIndex = topSection;
}

void GJBaseGameLayer::prepareSavePositionObjects() {
    m_savePositionObjects.clear();
    m_savePositionValues.clear();
    m_gameState.m_unkVecFloat1.clear();
    m_gameState.m_unkUint12 = -1;
    m_player1->m_unk52C = -1;
    m_player2->m_unk52C = -1;

    std::vector<AdvancedFollowTriggerObject*> advancedFollowObjects;
    std::vector<AdvancedFollowData> advancedFollowData;
    std::unordered_set<int> targetGroupSet;

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        for (unsigned int i = 0; i < data->num; i++) {
            GameObject* obj = (GameObject*)data->arr[i];
            if (!obj) break;
            if (obj->m_objectID == 3016) {
                saveAdvFollowSetup((AdvancedFollowTriggerObject*)obj, &advancedFollowData, &targetGroupSet);
                advancedFollowObjects.push_back((AdvancedFollowTriggerObject*)obj);
            } else if (obj->m_objectID == 3661) {
                advancedFollowObjects.push_back((AdvancedFollowTriggerObject*)obj);
            }
        }
    }

    std::unordered_map<int, std::unordered_set<int>> groupParentMap;
    for (auto& remap : m_spawnRemapTriggers) {
        for (auto& pair : remap) {
            groupParentMap[pair.first].insert(pair.second);
        }
    }

    for (auto& pair : groupParentMap) {
        if (targetGroupSet.count(pair.first)) {
            for (int linked : pair.second) {
                if (!targetGroupSet.count(linked)) {
                    targetGroupSet.insert(linked);
                }
            }
        }
    }

    for (auto& fd : advancedFollowData) {
        for (int idx : fd.m_targetGroupIndices) {
            if (fd.m_hasTargetControlID) {
                fd.m_followData.insert(idx);
            } else if (fd.m_targetGroupID == idx) {
                fd.m_followData.insert(idx);
            }
        }
    }

    std::unordered_set<int> mainObjectGroups;
    for (auto& pair : groupParentMap) {
        if (targetGroupSet.count(pair.first)) {
            for (int linked : pair.second) {
                if (targetGroupSet.count(linked)) {
                    mainObjectGroups.insert(linked);
                }
            }
        }
    }
    for (int groupID : targetGroupSet) {
        mainObjectGroups.insert(groupID);
    }

    for (int groupID : mainObjectGroups) {
        GameObject* mainObj = tryGetMainObject(groupID);
        if (mainObj) {
            m_savePositionObjects[groupID] = mainObj;
        }
        int posValue = 0;
        if (groupID > 0) posValue = groupID;
        if (posValue != 0) {
            auto it = m_savePositionValues.find(posValue);
            if (it != m_savePositionValues.end()) {
                if (groupID < it->second) it->second = groupID;
            } else {
                m_savePositionValues[posValue] = groupID;
            }
        }
    }

    for (auto& pair : m_savePositionValues) {
        int groupID = pair.first;
        int value = pair.second;
        if (groupID > 0) {
            GameObject* mainObj = tryGetMainObject(groupID);
            if (mainObj) {
                m_savePositionObjects[groupID] = mainObj;
            }
        }
        if (groupID < 0) {
            switch (groupID) {
                case -1: m_player1->m_unk52C = value; break;
                case -2: m_player2->m_unk52C = value; break;
                case -3: m_gameState.m_unkUint12 = value; break;
                default: break;
            }
        }
    }
}

void GJBaseGameLayer::positionUIObjects() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);
    float scaleX = (winSize.width * 0.5f) / 240.0f;
    float scaleY = (winSize.height * 0.5f) / 160.0f;

    if (!m_objects) return;
    cocos2d::ccArray* data = m_objects->data;
    for (unsigned int idx = 0; idx < data->num; idx++) {
        GameObject* obj = (GameObject*)data->arr[idx];
        if (!obj) break;
        if (obj->m_objectID != 3613) continue;

        int groupID = obj->m_uOrderOfArrival;
        cocos2d::CCArray* group = getGroup(groupID);
        GameObject* parent = getGroupParent(groupID);
        GameObject* mainObj = tryGetMainObject(obj->m_unknown5);

        cocos2d::CCArray* targetGroup = group;
        if (parent) {
            targetGroup = cocos2d::CCArray::create();
            cocos2d::CCArray::addObject(targetGroup, parent);
            if (mainObj) mainObj->getRealPosition();
        }

        if (!targetGroup) continue;

        cocos2d::ccArray* tdata = targetGroup->data;
        for (unsigned int j = 0; j < tdata->num; j++) {
            GameObject* member = (GameObject*)tdata->arr[j];
            if (!member) break;
            if (member->m_isUIObject) continue;

            addUIObject(member);

            cocos2d::CCPoint startPos = member->getStartPos();
            cocos2d::CCPoint newPos = startPos;

            int anchor = obj->m_eObjType;
            int position = obj->m_uIndexInArray;
            bool scaled = obj->m_unkBool;

            if (anchor == 0 || anchor == 5) {
                if (startPos.y >= center.y) anchor = 8;
                else anchor = 7;
            }

            switch (anchor) {
                case 1:
                    newPos.x = startPos.x - (center.x - 240.0f);
                    if (scaled) newPos.x *= scaleX;
                    break;
                case 2:
                    newPos.x = startPos.x + (center.x - 240.0f);
                    if (scaled) newPos.x *= scaleX;
                    break;
                case 3:
                    newPos.x = startPos.x - center.x;
                    if (scaled) newPos.x *= scaleX;
                    newPos.x += winSize.width * 0.5f;
                    break;
                case 4:
                    if (startPos.x >= center.x) {
                        newPos.x = startPos.x - (center.x + 240.0f);
                        if (scaled) newPos.x *= scaleX;
                        newPos.x += winSize.width;
                    } else {
                        newPos.x = startPos.x - (center.x - 240.0f);
                        if (scaled) newPos.x *= scaleX;
                    }
                    break;
                default:
                    break;
            }

            switch (position) {
                case 6:
                    newPos.y = startPos.y - center.y;
                    if (scaled) newPos.y *= scaleY;
                    newPos.y += winSize.height * 0.5f;
                    break;
                case 7:
                    newPos.y = startPos.y - (center.y - 160.0f);
                    if (scaled) newPos.y *= scaleY;
                    break;
                case 8:
                    newPos.y = startPos.y - (center.y + 160.0f);
                    if (scaled) newPos.y *= scaleY;
                    newPos.y += winSize.height;
                    break;
                default:
                    break;
            }

            member->setStartPos(newPos);

            if (member == parent && group) {
                cocos2d::CCPoint delta = newPos - startPos;
                cocos2d::ccArray* gdata = group->data;
                for (unsigned int k = 0; k < gdata->num; k++) {
                    GameObject* other = (GameObject*)gdata->arr[k];
                    if (!other) break;
                    if (!other->m_isUIObject) {
                        addUIObject(other);
                        other->setStartPos(other->getStartPos() + delta);
                    }
                }
            }
        }
    }
}

void GJBaseGameLayer::removeAllCheckpoints() {
}

void GJBaseGameLayer::removeBackground() {
    if (this->m_background) {
        this->m_background->removeFromParent();
        this->m_background = nullptr;
    }
}

void GJBaseGameLayer::removeCustomEnterEffects(int id, bool enter) {
    if ((unsigned int)id > 100) return;
    auto& vectors = enter ? this->m_gameState.m_enterEffectInstanceVectors : this->m_gameState.m_exitEffectInstanceVectors;
    auto it = vectors.find(id);
    if (it != vectors.end()) {
        vectors.erase(it);
    }
}

void GJBaseGameLayer::removeFromGroup(GameObject* object, int groupID) {
    cocos2d::CCArray* group = getGroup(groupID);
    cocos2d::CCArray::removeObject(group, object, true);
    if (object->m_isDecoration2) {
        cocos2d::CCArray* optimized = getOptimizedGroup(groupID);
        cocos2d::CCArray::removeObject(optimized, object, true);
        if (!object->m_hasGroupParentsString) return;
    } else {
        cocos2d::CCArray* staticGroup = getStaticGroup(groupID);
        cocos2d::CCArray::removeObject(staticGroup, object, true);
        if (!object->m_hasGroupParentsString) return;
    }
    if (object == getGroupParent(groupID)) {
        removeGroupParent(groupID);
    }
}

void GJBaseGameLayer::removeFromGroupParents(GameObject* object) {
    if (!object->m_hasGroupParentsString) return;
    cocos2d::CCArray* parentIDs = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_parentGroupIDs, object->m_uniqueID));
    if (!parentIDs) return;
    unsigned int count = cocos2d::CCArray::count(parentIDs);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* entry = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(parentIDs, i));
        cocos2d::CCDictionary::removeObjectForKey(this->m_parentGroupsDict, entry->m_groupID);
    }
    cocos2d::CCDictionary::setObject(this->m_removedParentGroupIDs, parentIDs, object->m_uniqueID);
    cocos2d::CCDictionary::removeObjectForKey(this->m_parentGroupIDs, object->m_uniqueID);
}

void GJBaseGameLayer::removeFromGroups(GameObject* object) {
    for (int i = 0; i < object->m_groupCount; ++i) {
        int groupID = object->getGroupID(i);
        removeFromGroup(object, groupID);
    }
}

void GJBaseGameLayer::removeFromStickyGroup(GameObject* object) {
    int linkedGroup = object->m_linkedGroup;
    if (linkedGroup == 0) return;
    cocos2d::CCArray* stickyGroup = getStickyGroup(linkedGroup);
    if (stickyGroup) {
        cocos2d::CCArray::removeObject(stickyGroup, object, true);
    }
    this->m_stickyGroups[object->m_uniqueID] = object->m_linkedGroup;
    object->m_linkedGroup = 0;
}

void GJBaseGameLayer::removeGroundLayer() {
    if (!this->m_groundLayer) return;
    this->m_groundLayer->removeFromParent();
    this->m_groundLayer = nullptr;
    this->m_groundLayer2->removeFromParent();
    this->m_groundLayer2 = nullptr;
}

void GJBaseGameLayer::removeGroupParent(int groupID) {
    GameObject* groupParent = getGroupParent(groupID);
    if (!groupParent) return;
    cocos2d::CCArray* parentIDs = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_parentGroupIDs, groupParent->m_uniqueID));
    if (parentIDs) {
        unsigned int count = cocos2d::CCArray::count(parentIDs);
        unsigned int i = 0;
        while (i < count) {
            GameObject* entry = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(parentIDs, i));
            if (entry->m_groupID == groupID) {
                cocos2d::CCArray::removeObjectAtIndex(parentIDs, i, true);
                break;
            }
            ++i;
        }
        if (cocos2d::CCArray::count(parentIDs) == 0) {
            groupParent->m_hasGroupParentsString = false;
        }
    } else {
        groupParent->m_hasGroupParentsString = false;
    }
    cocos2d::CCDictionary::removeObjectForKey(this->m_parentGroupsDict, groupID);
}

void GJBaseGameLayer::removeKeyframe(KeyframeGameObject* object) {
    cocos2d::CCArray* group = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_keyframeGroups, object->m_keyframeGroup));
    if (!group) return;
    cocos2d::CCArray::removeObject(group, object, true);
    updateKeyframeOrder(object->m_keyframeGroup);
}

void GJBaseGameLayer::removeMiddleground() {
    if (!this->m_middleground) return;
    this->m_middleground->removeFromParent();
    this->m_middleground = nullptr;
}

void GJBaseGameLayer::removeObjectFromSection(GameObject* object) {
    int outerIndex = object->m_outerSectionIndex;
    if (outerIndex < 0) return;

    int middleIndex = object->m_middleSectionIndex;
    int innerIndex = object->m_someOtherIndex;

    std::vector<int>* sectionSizes = this->m_sectionSizes[outerIndex];
    std::vector<GameObject*>* section = (*this->m_sections[outerIndex])[middleIndex];

    int newCount = (*sectionSizes)[middleIndex] - 1;
    (*sectionSizes)[middleIndex] = newCount;
    (*section)[innerIndex] = (*section)[newCount];
    (*section)[object->m_someOtherIndex]->m_someOtherIndex = object->m_someOtherIndex;

    if (object->m_objectID == 1816) {
        if (object->m_hasExtendedCollision) {
            int newSize = this->m_calcCollisionBlockObjectsSize - 1;
            this->m_calcCollisionBlockObjectsSize = newSize;
            this->m_calcCollisionBlockObjects[object->m_innerSectionIndex] = this->m_calcCollisionBlockObjects[newSize];
            this->m_calcCollisionBlockObjects[object->m_innerSectionIndex]->m_innerSectionIndex = object->m_innerSectionIndex;
        } else {
            std::vector<int>* blockSectionSizes = this->m_collisionBlockSectionSizes[outerIndex];
            std::vector<GameObject*>* blockSection = (*this->m_collisionBlockSections[outerIndex])[middleIndex];
            int newBlockCount = (*blockSectionSizes)[middleIndex] - 1;
            (*blockSectionSizes)[middleIndex] = newBlockCount;
            (*blockSection)[object->m_innerSectionIndex] = (*blockSection)[newBlockCount];
            (*blockSection)[object->m_innerSectionIndex]->m_innerSectionIndex = object->m_innerSectionIndex;
        }
    } else if (object->m_objectType != GameObjectType::Decoration) {
        if (object->m_hasExtendedCollision) {
            int newSize = this->m_calcNonEffectObjectsSize - 1;
            this->m_calcNonEffectObjectsSize = newSize;
            this->m_calcNonEffectObjects[object->m_innerSectionIndex] = this->m_calcNonEffectObjects[newSize];
            this->m_calcNonEffectObjects[object->m_innerSectionIndex]->m_innerSectionIndex = object->m_innerSectionIndex;
        } else {
            std::vector<int>* nonEffectSizes = this->m_nonEffectObjectsSizes[outerIndex];
            std::vector<GameObject*>* nonEffectSection = (*this->m_nonEffectObjects[outerIndex])[middleIndex];
            int newCount = (*nonEffectSizes)[middleIndex] - 1;
            (*nonEffectSizes)[middleIndex] = newCount;
            (*nonEffectSection)[object->m_innerSectionIndex] = (*nonEffectSection)[newCount];
            (*nonEffectSection)[object->m_innerSectionIndex]->m_innerSectionIndex = object->m_innerSectionIndex;
            std::vector<bool>* flags = this->m_nonEffectObjectsFlags[outerIndex];
            (*flags)[middleIndex] = true;
        }
    }
}

void GJBaseGameLayer::removePlayer2() {
    this->m_player2->removeFromParentAndCleanup(true);
    this->m_player2->setOpacity(0);
    this->m_player2->releaseAllButtons();
    this->m_player2->removeAllParticles();
    this->m_player2->deactivateStreak(true);
}

void GJBaseGameLayer::removeReleasedButtons() {
    auto& buttons = this->m_queuedButtons;
    unsigned int count = (unsigned int)buttons.size();

    for (unsigned int i = 0; i < count; ) {
        PlayerButtonCommand& push = buttons[i];
        if (!push.m_isPush || i + 1 >= count) {
            ++i;
            continue;
        }

        unsigned int matchIndex = count;
        for (unsigned int j = i + 1; j < count; ++j) {
            if (buttons[j].m_button == push.m_button && !buttons[j].m_isPush) {
                matchIndex = j;
                break;
            }
        }

        if (matchIndex == count) {
            ++i;
            continue;
        }

        buttons.erase(buttons.begin() + matchIndex);
        buttons.erase(buttons.begin() + i);
        count = (unsigned int)buttons.size();
    }
}

void GJBaseGameLayer::removeTemporaryParticles() {
    unsigned int i = 0;
    while (i < cocos2d::CCArray::count(this->m_temporaryParticles)) {
        cocos2d::CCParticleSystemQuad* particle = static_cast<cocos2d::CCParticleSystemQuad*>(cocos2d::CCArray::objectAtIndex(this->m_temporaryParticles, i));
        if (particle->isFull()) {
            particle->removeFromParent();
            this->m_customParticlesUIDs.erase(particle->getID());
            cocos2d::CCArray::fastRemoveObjectAtIndex(this->m_temporaryParticles, i);
            continue;
        }
        ++i;
    }
}

void GJBaseGameLayer::reorderObjectSection(GameObject* object) {
    removeObjectFromSection(object);
    addToSection(object);
}

void GJBaseGameLayer::reparentObject(cocos2d::CCNode* node, cocos2d::CCNode* parent) {
    if (parent == node->getParent()) return;
    if (node->getTag() == -9999) return;
    cocos2d::CCObject::retain(node);
    node->removeFromParentAndCleanup(false);
    if (parent) {
        parent->addChild(node, node->getZOrder());
    }
    cocos2d::CCObject::release(node);
}

void GJBaseGameLayer::restoreAllUIObjects() {
    if (this->m_uiObjects) {
        unsigned int count = cocos2d::CCArray::count(this->m_uiObjects);
        for (unsigned int i = 0; i < count; ++i) {
            GameObject* object = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(this->m_uiObjects, i));
            if (!object) break;
            auto it = this->m_uiObjectPositions.find(object->m_uniqueID);
            if (it != this->m_uiObjectPositions.end()) {
                cocos2d::CCPoint pos = it->second;
                object->setStartPos(pos);
            }
            object->deactivateObject(true);
            object->m_outerSectionIndex = 0;
            object->m_isUIObject = false;
            addToSection(object);
            cocos2d::CCDictionary::setObject(this->m_objectsToDeactivate, object, object->m_uniqueID);
            object->m_unk3ee = true;
        }
        cocos2d::CCArray::removeAllObjects(this->m_uiObjects);
    }
    this->m_uiObjectPositions.clear();
    this->m_calcCollisionBlockObjects2.clear();
    this->m_calcCollisionBlockObjects2Size = 0;
}

void GJBaseGameLayer::restoreDefaultGameplayOffsetX() {
    updateGameplayOffsetX(75, false);
}

void GJBaseGameLayer::restoreDefaultGameplayOffsetY() {
    updateGameplayOffsetY(75, false);
}

void GJBaseGameLayer::restoreRemap(EffectGameObject* object, std::unordered_map<int, int>& remap) {
    int defaultValue;

    defaultValue = 51;
    auto itTargetGroup = remap.find(defaultValue);
    if (itTargetGroup != remap.end()) {
        object->m_targetGroupID = itTargetGroup->second;
    }

    defaultValue = 71;
    auto itCenterGroup = remap.find(defaultValue);
    if (itCenterGroup != remap.end()) {
        object->m_centerGroupID = itCenterGroup->second;
    }

    defaultValue = 80;
    auto itItemID = remap.find(defaultValue);
    if (itItemID != remap.end()) {
        object->m_itemID = itItemID->second;
    }

    defaultValue = 95;
    auto itItemID2 = remap.find(defaultValue);
    if (itItemID2 != remap.end()) {
        object->m_itemID2 = itItemID2->second;
    }

    defaultValue = 534;
    auto itControlID = remap.find(defaultValue);
    if (itControlID != remap.end()) {
        object->m_controlID = itControlID->second;
    }

    defaultValue = 76;
    auto itAnimationID = remap.find(defaultValue);
    if (itAnimationID != remap.end()) {
        object->m_animationID = itAnimationID->second;
    }

    int objectID = object->m_objectID;
    if (objectID > 3021) {
        if (objectID == 3604) {
            defaultValue = 447;
            auto it = remap.find(defaultValue);
            if (it != remap.end()) {
                object->m_unknown5 = it->second;
            }
            defaultValue = 525;
            auto it2 = remap.find(defaultValue);
            if (it2 != remap.end()) {
                object->m_fRotationX = *reinterpret_cast<float*>(&it2->second);
            }
            return;
        }
        if (objectID > 3604) {
            if (objectID == 3607) {
                ChanceTriggerGameObject::revertChanceRemap(static_cast<ChanceTriggerGameObject*>(object));
                return;
            }
            if (objectID > 3607) {
                if ((unsigned int)(objectID - 3660) > 1) return;
                goto label_3660_3661;
            }
            if (objectID != 3605) return;

            defaultValue = 392;
            auto it392 = remap.find(defaultValue);
            if (it392 != remap.end()) {
                object->m_uID = it392->second;
            }
            defaultValue = 455;
            auto it455 = remap.find(defaultValue);
            if (it455 != remap.end()) {
                object->m_obAnchorPointInPoints.x = *reinterpret_cast<float*>(&it455->second);
            }
            defaultValue = 457;
            auto it457 = remap.find(defaultValue);
            if (it457 != remap.end()) {
                object->m_obAnchorPoint.x = *reinterpret_cast<float*>(&it457->second);
            }
            defaultValue = 416;
            auto it416 = remap.find(defaultValue);
            if (it416 != remap.end()) {
                object->m_unknown5 = it416->second;
            }
            defaultValue = 432;
            auto it432 = remap.find(defaultValue);
            if (it432 != remap.end()) {
                object->m_sTransform.a = *reinterpret_cast<float*>(&it432->second);
            }
            return;
        }
        if (objectID == 3023) goto label_area;
        if (objectID < 3602) return;

        defaultValue = 392;
        auto it392 = remap.find(defaultValue);
        if (it392 != remap.end()) {
            object->m_uID = it392->second;
        }
        defaultValue = 455;
        auto it455 = remap.find(defaultValue);
        if (it455 != remap.end()) {
            object->m_obAnchorPointInPoints.x = *reinterpret_cast<float*>(&it455->second);
        }
        defaultValue = 457;
        auto it457 = remap.find(defaultValue);
        if (it457 != remap.end()) {
            object->m_obAnchorPoint.x = *reinterpret_cast<float*>(&it457->second);
        }
        defaultValue = 416;
        auto it416 = remap.find(defaultValue);
        if (it416 != remap.end()) {
            object->m_unknown5 = it416->second;
        }
        return;
    }

    if (objectID >= 3017) goto label_area;
    if (objectID == 2068) {
        ChanceTriggerGameObject::revertChanceRemap(static_cast<ChanceTriggerGameObject*>(object));
        return;
    }
    if (objectID > 2068) {
        if (objectID <= 3010) {
            if (objectID < 3006) {
                if (objectID == 2903) {
                    defaultValue = 203;
                    auto it203 = remap.find(defaultValue);
                    if (it203 != remap.end()) object->m_uID = it203->second;
                    defaultValue = 204;
                    auto it204 = remap.find(defaultValue);
                    if (it204 != remap.end()) object->m_nLuaID = it204->second;
                    defaultValue = 205;
                    auto it205 = remap.find(defaultValue);
                    if (it205 != remap.end()) object->m_nTag = it205->second;
                    defaultValue = 206;
                    auto it206 = remap.find(defaultValue);
                    if (it206 != remap.end()) object->m_uReference = it206->second;
                    defaultValue = 209;
                    auto it209 = remap.find(defaultValue);
                    if (it209 != remap.end()) object->m_uAutoReleaseCount = it209->second;
                }
                return;
            }
            goto label_area;
        }
        if (objectID != 3016) return;
        goto label_3660_3661;
    }
    if (objectID == 1346) {
        defaultValue = 401;
        auto it401 = remap.find(defaultValue);
        if (it401 != remap.end()) object->m_rotationTargetID = it401->second;
        defaultValue = 516;
        auto it516 = remap.find(defaultValue);
        if (it516 != remap.end()) object->m_canSpawnTriggers = it516->second;
        defaultValue = 517;
        auto it517 = remap.find(defaultValue);
        if (it517 != remap.end()) object->m_uID = it517->second;
        defaultValue = 518;
        auto it518 = remap.find(defaultValue);
        if (it518 != remap.end()) object->m_nLuaID = it518->second;
        defaultValue = 519;
        auto it519 = remap.find(defaultValue);
        if (it519 != remap.end()) object->m_uReference = it519->second;
        return;
    }
    if (objectID == 1934) {
        defaultValue = 392;
        auto it392 = remap.find(defaultValue);
        if (it392 != remap.end()) object->m_uID = it392->second;
        defaultValue = 455;
        auto it455 = remap.find(defaultValue);
        if (it455 != remap.end()) object->m_obAnchorPointInPoints.x = *reinterpret_cast<float*>(&it455->second);
        defaultValue = 457;
        auto it457 = remap.find(defaultValue);
        if (it457 != remap.end()) object->m_obAnchorPoint.x = *reinterpret_cast<float*>(&it457->second);
        defaultValue = 416;
        auto it416 = remap.find(defaultValue);
        if (it416 != remap.end()) object->m_unknown5 = it416->second;
        defaultValue = 432;
        auto it432 = remap.find(defaultValue);
        if (it432 != remap.end()) object->m_sTransform.a = *reinterpret_cast<float*>(&it432->second);
        return;
    }
    if (objectID == 901) {
        defaultValue = 395;
        auto it395 = remap.find(defaultValue);
        if (it395 != remap.end()) object->m_targetModCenterID = it395->second;
        return;
    }
    return;

label_3660_3661:
    defaultValue = 366;
    auto it366 = remap.find(defaultValue);
    if (it366 != remap.end()) object->m_pUserData = it366->second;
    defaultValue = 560;
    auto it560 = remap.find(defaultValue);
    if (it560 != remap.end()) object->m_nLuaID = it560->second;
    defaultValue = 565;
    auto it565 = remap.find(defaultValue);
    if (it565 != remap.end()) object->m_uAutoReleaseCount = it565->second;
    return;

label_area:
    defaultValue = 225;
    auto it225 = remap.find(defaultValue);
    if (it225 != remap.end()) object->m_sTransform.c = *reinterpret_cast<float*>(&it225->second);
    defaultValue = 344;
    auto it344 = remap.find(defaultValue);
    if (it344 != remap.end()) object->m_sInverse = *reinterpret_cast<cocos2d::CCAffineTransform*>(&it344->second);
    defaultValue = 343;
    auto it343 = remap.find(defaultValue);
    if (it343 != remap.end()) object->m_enterChannel = it343->second;
}

void GJBaseGameLayer::resumeAudio() {
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    FMODAudioEngine::resumeAllAudio(engine);
    FMODAudioEngine::resumeAllMusic(engine);
    engine->m_system->update();
}

void GJBaseGameLayer::reverseDirection(EffectGameObject* object) {
    PlayerObject::reversePlayer(this->m_player1);
    if (this->m_gameState.m_isDualMode) {
        PlayerObject::reversePlayer(this->m_player2);
    }
}

void GJBaseGameLayer::rotateAreaObjects(GameObject* object, cocos2d::CCArray* objects, float rotation, bool reset) {
    int commandIndex = this->m_gameState.m_commandIndex;
    if (object->m_unk4C8 < commandIndex) {
        if (!object->m_isDecoration2) {
            int lastCommand = object->m_unk4C4;
            if (commandIndex != lastCommand) {
                object->m_unk4C4 = commandIndex;
                object->m_lastPosition.x = object->m_positionX;
                object->m_lastPosition.y = object->m_positionY;
                object->dirtifyObjectRect();
                lastCommand = this->m_gameState.m_commandIndex;
            }
            commandIndex = lastCommand;
            if (object->m_unk4CC != lastCommand) {
                object->m_unk4CC = lastCommand;
                object->m_customScaleX = object->m_scaleX;
                object->m_customScaleY = object->m_scaleY;
                object->dirtifyObjectRect();
                commandIndex = this->m_gameState.m_commandIndex;
            }
        }

        int dirty = 0;
        if (object->m_positionXOffset != 0.0f) {
            object->m_isUnmodifiedPosDirty = true;
            float offset = object->m_positionXOffset;
            dirty = 1;
            object->m_positionXOffset = 0.0f;
            object->m_positionX -= offset;
        }
        if (object->m_positionYOffset != 0.0f) {
            object->m_isUnmodifiedPosDirty = true;
            float offset = object->m_positionYOffset;
            dirty = 1;
            object->m_positionYOffset = 0.0f;
            object->m_positionY -= offset;
        }
        if (object->m_unk2BC != 0.0f || object->m_unk2C0 != 0.0f) {
            float sx = object->m_unk2BC;
            float sy = object->m_unk2C0;
            float newScaleXOffset = object->m_scaleXOffset - sx;
            float newScaleYOffset = object->m_scaleYOffset - sy;
            float newScaleX = object->m_scaleX - sx;
            float newScaleY = object->m_scaleY - sy;
            object->m_scaleXOffset = newScaleXOffset;
            object->m_unk2BC = 0.0f;
            object->m_scaleX = newScaleX;
            object->m_scaleYOffset = newScaleYOffset;
            object->m_scaleY = newScaleY;
            object->m_unk2C0 = 0.0f;
        }
        if (object->m_unk2A8 != 0.0f || object->m_unk2B0 != 0.0f) {
            float newRotXOffset = object->m_rotationXOffset - object->m_unk2A8;
            float newRotYOffset = object->m_rotationYOffset - object->m_unk2B0;
            object->m_unk2A8 = 0.0f;
            object->m_rotationXOffset = newRotXOffset;
            object->m_rotationYOffset = newRotYOffset;
            object->m_unk2B0 = 0.0f;
        }
        object->m_unk4C8 = commandIndex;
        if (this->m_areaObjectsIndex > this->m_areaObjectsCount) {
            this->m_areaObjects[this->m_areaObjectsCount] = object;
        } else {
            this->m_areaObjects.push_back(object);
        }
        this->m_areaObjectsIndex++;
        this->m_areaObjectsCount++;
        if (dirty) updateObjectSection(object);
    }

    cocos2d::CCPoint realPos = object->getRealPosition();
    this->m_areaTransformNode->setPosition(&realPos);
    this->m_areaTransformNode->setScaleX(1.0f);
    this->m_areaTransformNode->setScaleY(1.0f);
    this->m_areaTransformNode->setSkewX(0.0f);
    this->m_areaTransformNode->setSkewY(0.0f);
    this->m_areaTransformNode->setRotation(rotation);
    prepareTransformParent(false);

    cocos2d::CCAffineTransform worldTransform = this->m_areaTransformNode->nodeToWorldTransform();

    this->m_areaTransformNode2->setScaleX(1.0f);
    this->m_areaTransformNode2->setScaleY(1.0f);
    this->m_areaTransformNode2->setSkewX(0.0f);
    this->m_areaTransformNode2->setSkewY(0.0f);
    this->m_areaTransformNode2->setRotation(0.0f);

    if (!objects) return;

    cocos2d::ccArray* data = objects->data;
    if (!data->num) return;

    cocos2d::CCObject** arr = data->arr;
    cocos2d::CCObject** end = &arr[data->num - 1];
    if (arr > end) return;

    for (; arr <= end; ++arr) {
        GameObject* target = static_cast<GameObject*>(*arr);
        if (!target) return;

        if (target->m_unk3ee) continue;

        int dirty = 0;
        int commandIndex2 = this->m_gameState.m_commandIndex;
        if (target->m_unk4C4 < commandIndex2) {
            if (!target->m_unk3ee) {
                target->m_unk4C4 = commandIndex2;
                target->m_lastPosition.x = target->m_positionX;
                target->m_lastPosition.y = target->m_positionY;
                target->dirtifyObjectRect();
            }
            target->m_unk4C4 = this->m_gameState.m_commandIndex;
        }

        if (target->m_positionXOffset != 0.0f) {
            target->m_isUnmodifiedPosDirty = true;
            float offset = target->m_positionXOffset;
            dirty = 1;
            target->m_positionXOffset = 0.0f;
            target->m_positionX -= offset;
        }
        if (target->m_positionYOffset != 0.0f) {
            target->m_isUnmodifiedPosDirty = true;
            float offset = target->m_positionYOffset;
            dirty = 1;
            target->m_positionYOffset = 0.0f;
            target->m_positionY -= offset;
        }
        if (target->m_unk2BC != 0.0f || target->m_unk2C0 != 0.0f) {
            target->m_scaleX -= target->m_unk2BC;
            target->m_scaleY -= target->m_unk2C0;
            target->m_scaleXOffset -= target->m_unk2BC;
            target->m_scaleYOffset -= target->m_unk2C0;
            target->m_unk2BC = 0.0f;
            target->m_unk2C0 = 0.0f;
        }
        if (target->m_unk2A8 != 0.0f || target->m_unk2B0 != 0.0f) {
            target->m_rotationXOffset -= target->m_unk2A8;
            target->m_rotationYOffset -= target->m_unk2B0;
            target->m_unk2A8 = 0.0f;
            target->m_unk2B0 = 0.0f;
        }
        target->m_unk4C8 = this->m_gameState.m_commandIndex;
        if (this->m_areaObjectsIndex > this->m_areaObjectsCount) {
            this->m_areaObjects[this->m_areaObjectsCount] = target;
        } else {
            this->m_areaObjects.push_back(target);
        }
        this->m_areaObjectsIndex++;
        this->m_areaObjectsCount++;
        if (dirty) updateObjectSection(target);

        cocos2d::CCPoint realPos2 = target->getRealPosition();
        cocos2d::CCPoint relative = realPos2 - realPos;
        this->m_areaTransformNode2->setPosition(&relative);
        cocos2d::CCAffineTransform localTransform = this->m_areaTransformNode2->nodeToParentTransform();
        cocos2d::CCAffineTransform result = cocos2d::CCAffineTransformConcat(&localTransform, &worldTransform);

        target->m_positionX = result.tx;
        target->m_positionY = result.ty;
        target->dirtifyObjectPos();
        target->dirtifyObjectRect();
        if (rotation != 0.0f && target->m_canRotateFree) {
            target->m_rotationXOffset += rotation;
            target->m_rotationYOffset += rotation;
            target->addRotation(rotation);
            if (target->m_objectType != GameObjectType::Decoration) {
                target->m_isObjectRectDirty = true;
                target->m_isOrientedBoxDirty = true;
                if (!target->m_shouldUseOuterOb) {
                    target->calculateOrientedBox();
                }
            }
        }
        updateObjectSection(target);
    }
    updateDisabledObjectsLastPos(objects);
}

void GJBaseGameLayer::stopAllGroundActions() {
    GJGroundLayer::deactivateGround(this->m_groundLayer);
    GJGroundLayer::deactivateGround(this->m_groundLayer2);
}

void GJBaseGameLayer::stopCameraShake() {
    this->m_gameState.m_cameraShakeEnabled = false;
    this->m_gameState.m_cameraShakeFactor = 0.0f;
}

void GJBaseGameLayer::stopCustomEnterEffect(EnterEffectObject* object) {
    unsigned int enterType = object->m_enterType;
    if (enterType <= 1) {
        stopCustomEnterEffect(object, true);
        enterType = object->m_enterType;
    }
    if ((enterType & 0xFFFFFFFD) == 0) {
        stopCustomEnterEffect(object, false);
    }
}

void GJBaseGameLayer::stopSFXTrigger(SFXTriggerGameObject* object) {
    if (object->m_objectID != 3602) return;
    auto it = this->m_gameState.m_unkUMap8.find(object->m_uniqueID);
    if (it == this->m_gameState.m_unkUMap8.end()) return;
    for (int channelID : it->second) {
        FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
        FMODAudioEngine::stopChannel(engine, channelID);
        this->m_gameState.m_proximityVolumeRelated.erase(std::make_pair(channelID, 0));
    }
    it->second.clear();
}

void GJBaseGameLayer::swapBackground(int background) {
    this->m_gameState.m_background = background;
    if (this->m_skipArtReload) return;
    GameManager* gm = GameManager::sharedState();
    if (background == gm->m_loadedBgID) {
        asyncBGLoaded(background);
    } else {
        GameManager::loadBackgroundAsync(gm, background);
    }
}

void GJBaseGameLayer::swapGround(int ground) {
    this->m_gameState.m_ground = ground;
    if (this->m_skipArtReload) return;
    GameManager* gm = GameManager::sharedState();
    if (ground != gm->m_loadedGroundID) {
        GameManager::loadGroundAsync(gm, ground);
    }
}

void GJBaseGameLayer::swapMiddleground(int middleground) {
    this->m_gameState.m_middleground = middleground;
    if (this->m_skipArtReload) return;
    GameManager* gm = GameManager::sharedState();
    if (middleground != gm->m_loadedMG || (middleground > 0 && !this->m_middleground)) {
        if (middleground == 0) {
            createMiddleground(0);
        } else if (middleground == gm->m_loadedMG) {
            asyncMGLoaded(middleground);
        } else {
            GameManager::loadMiddlegroundAsync(gm, middleground);
        }
    }
}

void GJBaseGameLayer::switchToFlyMode(PlayerObject* player, GameObject* object, bool noPortal, int type) {
    PlayerObject::switchedToMode(player, (GameObjectType)type);
    GameObject* portal = processCameraObject(object, player);
    bool noEffects = portal ? portal->m_hasNoEffects : false;
    switch (type) {
        case 5:
            PlayerObject::toggleFlyMode(player, true, noEffects);
            toggleGlitter(true);
            break;
        case 19:
            PlayerObject::toggleBirdMode(player, true, noEffects);
            toggleGlitter(true);
            break;
        case 41:
            PlayerObject::toggleSwingMode(player, true, noEffects);
            toggleGlitter(true);
            break;
        default:
            PlayerObject::toggleDartMode(player, true, noEffects);
            toggleGlitter(true);
            break;
    }
}

void GJBaseGameLayer::switchToRobotMode(PlayerObject* player, GameObject* object, bool noPortal) {
    PlayerObject::switchedToMode(player, GameObjectType::RobotPortal);
    GameObject* portal = processCameraObject(object, player);
    bool noEffects = portal ? portal->m_hasNoEffects : false;
    PlayerObject::toggleRobotMode(player, true, noEffects);
}

void GJBaseGameLayer::switchToRollMode(PlayerObject* player, GameObject* object, bool noPortal) {
    PlayerObject::switchedToMode(player, GameObjectType::BallPortal);
    GameObject* portal = processCameraObject(object, player);
    bool noEffects = portal ? portal->m_hasNoEffects : false;
    PlayerObject::toggleRollMode(player, true, noEffects);
}

void GJBaseGameLayer::switchToSpiderMode(PlayerObject* player, GameObject* object, bool noPortal) {
    PlayerObject::switchedToMode(player, GameObjectType::SpiderPortal);
    GameObject* portal = processCameraObject(object, player);
    bool noEffects = portal ? portal->m_hasNoEffects : false;
    PlayerObject::toggleSpiderMode(player, true, noEffects);
}

void GJBaseGameLayer::syncBGTextures() {
    int bgTag = this->m_background->getTag();
    if (bgTag != this->m_gameState.m_background) {
        createBackground(this->m_gameState.m_background);
    }

    int groundTag = this->m_groundLayer->getTag();
    if (groundTag != this->m_gameState.m_ground) {
        createGroundLayer(this->m_gameState.m_ground, this->m_levelSettings->m_groundLineIndex);
    }

    int middleTag = this->m_middleground ? this->m_middleground->getTag() : -1;
    int desiredMiddle = this->m_gameState.m_middleground;
    if (middleTag != desiredMiddle) {
        if (middleTag != -1 || desiredMiddle > 0) {
            createMiddleground(desiredMiddle);
        }
    }
}

void GJBaseGameLayer::teleportPlayer(TeleportPortalObject* object, PlayerObject* player) {
    PlayerObject* target = player ? player : this->m_player1;
    target->m_wasTeleported = true;

    bool isOrange = object->m_objectID == 3022;

    if (object->m_orangePortal) {
        cocos2d::CCPoint portalStart = object->m_orangePortal->getStartPos();
        cocos2d::CCPoint objectStart = object->getStartPos();
        object->m_teleportYOffset = portalStart.y - objectStart.y;
    }

    cocos2d::CCPoint playerPos = target->getPosition();

    TeleportPortalObject* portalTarget = getPortalTarget(object);
    float portalRotation = 0.0f;

    if (portalTarget) {
        cocos2d::CCPoint objectPos = object->getPosition();
        target->m_lastPortalPos = objectPos;
        target->m_lastActivatedPortal = object;
        target->m_fallStartY = 0.0f;

        cocos2d::CCPoint targetPos = getPortalTargetPos(object, portalTarget, target);

        cocos2d::CCPoint savedPos = target->getPosition();
        if (object->m_saveOffset) {
            cocos2d::CCPoint objectReal = object->getRealPosition();
            savedPos = targetPos;
            savedPos.x += objectReal.x - playerPos.x;
            savedPos.y += objectReal.y - playerPos.y;
        } else {
            savedPos = targetPos;
        }

        if (object->m_ignoreX) {
            savedPos.x = target->getPosition().x;
        }
        if (object->m_ignoreY) {
            savedPos.y = target->getPosition().y;
        }
        target->setPosition(savedPos);

        int gravityMode = object->m_gravityMode;
        if (gravityMode == 1) {
            PlayerObject::flipGravity(target, false, true);
        } else if (gravityMode == 2) {
            PlayerObject::flipGravity(target, true, true);
        } else if (gravityMode == 3) {
            PlayerObject::flipGravity(target, !target->m_isUpsideDown, true);
        }
        if (!isOrange) {
            object->playShineEffect();
            object->activatedByPlayer(target);
        }

        int targetObjID = portalTarget->m_objectID;
        if (targetObjID == 749 || targetObjID == 38 || targetObjID == 747 || targetObjID == 2064 || targetObjID == 2902) {
            portalRotation = 180.0f;
        } else {
            portalRotation = 90.0f;
        }
        portalRotation -= portalTarget->getRotationX();
        if (portalTarget->isFlipX()) {
            portalRotation += 180.0f;
        }

        if (!object->m_hasNoEffects) {
            cocos2d::ccColor3B color = { 0, 255, 0 };
            PlayerObject::spawnPortalCircle(target, color, 50.0);
        }

        cocos2d::CCPoint portalPos = portalTarget->getPosition();
        target->m_lastPortalPos = portalPos;
        target->m_lastActivatedPortal = portalTarget;
        portalTarget->playShineEffect();

        if (!object->m_hasNoEffects) {
            cocos2d::ccColor3B color = { 127, 255, 0 };
            PlayerObject::spawnPortalCircle(target, color, 50.0);
        }

        PlayerObject::resetStreak(target);
        if (target->m_isDart) {
            PlayerObject::placeStreakPoint(target);
        }

        if (!object->m_hasNoEffects) {
            cocos2d::CCPoint from;
            cocos2d::CCPoint to;
            if (isOrange) {
                from = playerPos;
                to = target->getPosition();
            } else {
                from = object->getPosition();
                to = portalTarget->getPosition();
            }
            cocos2d::ccColor3B color = target->m_playerColor1;
            lightningFlash(from, to, color, 4.0f, 0.2f, 0, true, 1.0f);
        }
    } else {
        portalRotation = object->isFlipX() ? 180.0f : 0.0f;
        portalRotation -= object->getRotationX();
    }

    if (object->m_teleportEase) {
        this->m_gameState.m_unkUint13 = 0.5f;
    }

    float portalRange = object->m_teleportEase ? 180.0f : 60.0f;

    if (portalTarget) {
        PlayerObject::playerTeleported(target);
    }

    if (object->m_redirectForceEnabled) {
        PlayerObject::redirectPlayerForce(target, portalRotation, object->m_redirectForceMod, object->m_redirectForceMin, object->m_redirectForceMax);
    } else if (object->m_staticForceEnabled) {
        PlayerObject::updateStaticForce(target, portalRotation, object->m_staticForce, object->m_staticForceAdditive);
    }

    if (object->m_instantCamera) {
        this->m_gameState.m_unkBool28 = true;
        this->m_gameState.m_unkBool29 = true;
    }

    if (object->m_snapGround) {
        target->m_lastGroundedPos = target->getPosition();
    }

    if (target->m_isDashing && object->m_redirectDash) {
        PlayerObject::redirectDash(target, portalRotation);
    }

    if (portalTarget) {
        checkCameraLimitAfterTeleport(target, portalRange);
    }

    gameEventTriggered(GJGameEvent::Teleported, 0, 0);
}

void GJBaseGameLayer::toggleAudioVisualizer(bool visible) {
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    engine->m_showAudioVisualizer = visible;

    if (this->m_showAudioVisualizer == visible) return;
    this->m_showAudioVisualizer = visible;

    if (!this->m_audioVisualizerBG) {
        cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        this->m_audioVisualizerBG = FMODLevelVisualizer::create();
        this->m_uiLayer->addChild(this->m_audioVisualizerBG, 100);
        cocos2d::CCPoint pos = cocos2d::CCPoint(20.0f, winSize.height * 0.5f);
        this->m_audioVisualizerBG->setPosition(pos);

        this->m_audioVisualizerSFX = FMODLevelVisualizer::create();
        this->m_uiLayer->addChild(this->m_audioVisualizerSFX, 100);
        cocos2d::CCPoint pos2 = cocos2d::CCPoint(35.0f, winSize.height * 0.5f);
        this->m_audioVisualizerSFX->setPosition(pos2);
    }

    this->m_audioVisualizerBG->setVisible(visible);
    this->m_audioVisualizerSFX->setVisible(visible);
}

void GJBaseGameLayer::toggleDualMode(GameObject* object, bool dual, PlayerObject* player, bool noEffects) {
    if (this->m_gameState.m_isDualMode == dual) return;
    this->m_gameState.m_isDualMode = dual;

    bool twoPlayer = dual && this->m_levelSettings->m_twoPlayerMode;
    UILayer::updateDualMode(this->m_uiLayer, twoPlayer);

    this->m_player1->m_unkA99 = dual;
    this->m_player2->m_unkA99 = dual;

    if (!dual) {
        if (this->m_player1->m_isDart) {
            bool created = false;
            if (player) {
                if (player->m_uniqueID == this->m_player1->m_uniqueID) {
                    if (this->m_isEditor) {
                        if (player->m_uniqueID == this->m_player2->m_uniqueID) {
                            PlayerObject::copyAttributes(this->m_player1);
                            removePlayer2();
                        } else {
                            removePlayer2();
                        }
                    } else {
                        if (player->m_uniqueID != this->m_player2->m_uniqueID) {
                            playExitDualEffect(player->m_uniqueID == this->m_player2->m_uniqueID ? this->m_player1 : player);
                        } else {
                            PlayerObject::copyAttributes(this->m_player1);
                            removePlayer2();
                        }
                    }
                    created = true;
                }
            }
            if (!created) {
                if (this->m_isEditor) {
                    if (player && player->m_uniqueID == this->m_player2->m_uniqueID) {
                        PlayerObject::copyAttributes(this->m_player1);
                    }
                    removePlayer2();
                } else {
                    if (player) {
                        PlayerObject* other = this->m_player1;
                        if (player->m_uniqueID == this->m_player2->m_uniqueID) other = this->m_player1;
                        else if (player->m_uniqueID == this->m_player1->m_uniqueID) other = this->m_player2;
                        playExitDualEffect(other);
                    }
                    removePlayer2();
                }
            }
        } else {
            bool wasP2 = player && player->m_uniqueID == this->m_player2->m_uniqueID;
            if (this->m_isEditor) {
                if (wasP2) {
                    PlayerObject::copyAttributes(this->m_player1);
                }
                removePlayer2();
            } else {
                if (player && wasP2) {
                    playExitDualEffect(this->m_player1);
                    PlayerObject::copyAttributes(this->m_player1);
                } else if (player) {
                    playExitDualEffect(this->m_player2);
                }
                removePlayer2();
            }
        }

        if (!this->m_isEditor) {
            cocos2d::ccColor3B color = { 0, 100, 255 };
            PlayerObject::spawnPortalCircle(this->m_player1, color, 50.0);
        }

        this->m_gameState.m_lastActivatedPortal2 = nullptr;
        if (object && object->m_classType == GameObjectClassType::Effect) {
            updateCameraMode(static_cast<EffectGameObject*>(object), false);
        }

        GameObjectType activeMode = PlayerObject::getActiveMode(this->m_player1);
        updateDualGround(player, (int)activeMode, false, 0.0f);

        if (this->m_player1->m_isDart && (!player || player->m_uniqueID != this->m_player1->m_uniqueID)) {
            PlayerObject::resetStreak(this->m_player1);
            PlayerObject::placeStreakPoint(this->m_player1);
            if (this->m_player2->m_isDart) {
                float rotation = this->m_player2->getRotation();
                this->m_player1->setRotation(rotation);
            }
        }
    } else {
        this->m_player2->resetObject();
        spawnPlayer2();

        if (this->m_levelSettings->m_twoPlayerMode) {
            PlayerObject::releaseButton(this->m_player2, PlayerButton::Jump);
        }

        if (!noEffects) {
            if (!this->m_isEditor) {
                PlayerObject::spawnDualCircle(this->m_player1);
                PlayerObject::spawnDualCircle(this->m_player2);
            }
        }

        if (object) {
            this->m_gameState.m_lastActivatedPortal2 = object;
        }

        playerWillSwitchMode(player, object);
    }
}

void GJBaseGameLayer::toggleFlipped(bool flip, bool noEffects) {
    if (this->m_gameState.m_unkBool10 == flip) return;

    this->m_gameState.m_unkBool10 = flip;
    PlayerObject::deactivateStreak(this->m_player1, true);

    if (noEffects) {
        this->m_gameState.m_unkBool11 = false;
        this->m_gameState.m_levelFlipping = flip ? 1.0f : 0.0f;
        return;
    }

    float targetValue = flip ? 1.0f : 0.0f;

    if (flip) {
        cocos2d::ccColor3B color = { 127, 255, 63 };
        PlayerObject::spawnPortalCircle(this->m_player1, color, 50.0);
        this->m_gameState.m_unkBool11 = false;
        if (!isFlipping()) {
            this->m_gameState.m_levelFlipping = 0.00001f;
        }
    } else {
        cocos2d::ccColor3B color = { 0, 255, 255 };
        PlayerObject::spawnPortalCircle(this->m_player1, color, 50.0);
        this->m_gameState.m_unkBool11 = false;
        if (!isFlipping()) {
            this->m_gameState.m_levelFlipping = 0.99999f;
        }
    }

    float current = this->m_gameState.m_levelFlipping;
    if (this->m_gameState.m_unkBool10) {
        if (current >= 0.5f && flip == false) this->m_gameState.m_unkBool11 = true;
        else if (current < 0.5f && flip == false) {}
    } else {
        if (current <= 0.5f && flip == true) {}
    }

    if (flip) {
        if (current < 0.5f) this->m_gameState.m_unkBool11 = true;
    } else {
        if (current > 0.5f) this->m_gameState.m_unkBool11 = true;
    }

    PlayerObject::levelWillFlip(this->m_player1);
    if (this->m_gameState.m_isDualMode) {
        PlayerObject::levelWillFlip(this->m_player2);
    }

    this->m_gameState.tweenValue(this->m_gameState.m_levelFlipping, targetValue, 7, 0.4f, 1, 1.2f, -1, -1);
    this->m_gameState.m_unkBool12 = true;
}

void GJBaseGameLayer::toggleGlitter(bool visible) {
}

void GJBaseGameLayer::toggleGroundVisibility(bool visible) {
}

void GJBaseGameLayer::toggleGroup(int id, bool activate) {
    if (activate == GJEffectManager::isGroupEnabled(this->m_effectManager, id)) return;

    cocos2d::CCArray* group = getGroup(id);
    if (activate) {
        if (group) {
            unsigned int count = cocos2d::CCArray::count(group);
            for (unsigned int i = 0; i < count; ++i) {
                GameObject* object = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
                if (!object) break;
                object->groupWasEnabled();
            }
        }
    } else {
        if (group) {
            unsigned int count = cocos2d::CCArray::count(group);
            for (unsigned int i = 0; i < count; ++i) {
                GameObject* object = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
                if (!object) break;
                object->groupWasDisabled();
            }
        }
    }

    GJEffectManager::toggleGroup(this->m_effectManager, id, activate);
}

void GJBaseGameLayer::toggleGroupTriggered(int group, bool activate, std::vector<int> const& remapKeys, int triggerID, int controlID) {
    toggleGroup(group, activate);
    if (activate) {
        spawnGroup(group, false, 0.0, remapKeys, triggerID, controlID);
    }
}

void GJBaseGameLayer::toggleHideAttempts(bool hide) {
}

void GJBaseGameLayer::toggleInfoLabel() {
}

void GJBaseGameLayer::toggleLockPlayer(bool disable, bool player2) {
    if (disable) {
        if (player2) {
            PlayerObject::disablePlayerControls(this->m_player1);
        } else {
            PlayerObject::disablePlayerControls(this->m_player2);
        }
    } else {
        if (player2) {
            PlayerObject::enablePlayerControls(this->m_player1);
        } else {
            PlayerObject::enablePlayerControls(this->m_player2);
        }
    }
}

void GJBaseGameLayer::toggleMGVisibility(bool visible) {
}

void GJBaseGameLayer::toggleMusicInPractice() {
}

void GJBaseGameLayer::togglePlayerStreakBlend(bool blend, bool force) {
    if (this->m_gameState.m_playerStreakBlend == blend && !force) return;
    this->m_gameState.m_playerStreakBlend = blend;
    PlayerObject::updateStreakBlend(this->m_player1, blend);
    PlayerObject::updateStreakBlend(this->m_player2, blend);
}

void GJBaseGameLayer::togglePlayerVisibility(bool visible) {
    togglePlayerVisibility(visible, true);
    togglePlayerVisibility(visible, false);
}

void GJBaseGameLayer::togglePlayerVisibility(bool visible, bool player1) {
    PlayerObject* player = player1 ? this->m_player1 : this->m_player2;
    if (!player) return;
    player->setVisible(visible);
}

void GJBaseGameLayer::toggleProgressbar() {
}

void GJBaseGameLayer::triggerAdvancedFollowCommand(AdvancedFollowTriggerObject* object) {
    int centerID = object->m_centerGroupID;
    if (object->m_targetPlayer1) {
        centerID = -1;
    } else if (object->m_targetPlayer2) {
        centerID = -2;
    } else if (object->m_followCPP) {
        centerID = -3;
    }

    bool found = false;
    auto& instances = this->m_gameState.m_advanceFollowInstances;
    for (auto& instance : instances) {
        if (instance.m_gameObject == object
            && instance.m_group == object->m_targetGroupID
            && instance.m_objectKey == centerID
            && instance.m_controlId == object->m_controlID) {
            instance.m_finished = false;
            instance.m_doStart = false;
            this->m_gameState.m_unkUint8++;
            instance.m_relatedToGJGameStateUnkUint7 = this->m_gameState.m_unkUint8;
            found = true;
        }
    }

    if (found) {
        this->m_gameState.m_unkBool27 = true;
        return;
    }

    this->m_gameState.m_unkUint8++;
    int unkUint8 = this->m_gameState.m_unkUint8;

    AdvancedFollowInstance instance{};
    instance.m_gameObject = object;
    instance.m_group = object->m_targetGroupID;
    instance.m_objectKey = centerID;
    instance.m_controlId = object->m_controlID;
    instance.m_otherObjectKey = 0;
    instance.m_relatedToGJGameStateUnkUint7 = unkUint8;
    instance.m_finished = false;
    instance.m_doStart = false;
    instance.m_started = false;
    instance.m_processed = false;
    instances.push_back(instance);

    this->m_gameState.m_unkBool27 = true;
}

void GJBaseGameLayer::triggerAdvancedFollowEditCommand(AdvancedFollowEditObject* object) {
    int objectID = object->m_objectID;
    if (objectID == 3660) {
        if (object->m_targetControlID) {
            auto& instances = this->m_gameState.m_advanceFollowInstances;
            int targetControlID = object->m_targetGroupID;
            for (auto& instance : instances) {
                if (instance.m_controlId == targetControlID) {
                    int specialKey = getSpecialKey(instance.m_group, instance.m_gameObject->m_ignoreGroupParent, instance.m_gameObject->m_ignoreLinkedObjects);
                    int index = 0;
                    int iterations = 1;
                    auto it = this->m_targetGroups.find(specialKey);
                    if (it != this->m_targetGroups.end()) {
                        index = it->second.first;
                        iterations = index > 0 ? 2 : 1;
                    }
                    int start = index - 1;
                    int end = index + iterations - 1;
                    for (int i = start; i != end; ++i) {
                        cocos2d::CCArray* group;
                        if (index > 0) {
                            group = static_cast<cocos2d::CCArray*>(cocos2d::CCArray::objectAtIndex(this->m_targetGroupsArray, i));
                        } else {
                            group = getGroup(instance.m_group);
                        }
                        modifyGroupPhysics(object, group);
                    }
                }
            }
        } else {
            cocos2d::CCArray* group = getGroup(object->m_targetGroupID);
            modifyGroupPhysics(object, group);
        }
        return;
    }

    if (objectID != 3661) return;

    int centerID = object->m_centerGroupID;
    if (object->m_targetPlayer1) {
        centerID = -1;
    } else if (object->m_targetPlayer2) {
        centerID = -2;
    } else if (object->m_followCPP) {
        centerID = -3;
    } else if (!centerID) {
        return;
    }

    if (object->m_targetControlID) {
        auto& instances = this->m_gameState.m_advanceFollowInstances;
        int targetControlID = object->m_targetGroupID;
        for (auto& instance : instances) {
            if (instance.m_controlId == targetControlID) {
                instance.m_objectKey = centerID;
            }
        }
        return;
    }

    cocos2d::CCArray* group = getGroup(object->m_targetGroupID);
    if (!group) return;

    auto& instances = this->m_gameState.m_advanceFollowInstances;
    unsigned int count = cocos2d::CCArray::count(group);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
        if (!obj) break;
        if (obj->m_objectID != 3016) continue;
        for (auto& instance : instances) {
            if (instance.m_gameObject == obj) {
                instance.m_objectKey = centerID;
            }
        }
    }
}

void GJBaseGameLayer::triggerAreaEffect(EnterEffectObject* object) {
    std::vector<EnterEffectInstance>* instances = nullptr;
    GJAreaActionType type;

    switch (object->m_objectID) {
        case 3006:
            instances = &this->m_gameState.m_moveEffectInstances;
            type = GJAreaActionType::Move;
            break;
        case 3007:
            instances = &this->m_gameState.m_rotateEffectInstances;
            type = GJAreaActionType::Rotate;
            break;
        case 3008:
            instances = &this->m_gameState.m_scaleEffectInstances;
            type = GJAreaActionType::Scale;
            break;
        case 3009:
            instances = &this->m_gameState.m_fadeEffectInstances;
            type = GJAreaActionType::Fade;
            break;
        case 3010:
            instances = &this->m_gameState.m_tintEffectInstances;
            type = GJAreaActionType::Tint;
            break;
        case 3011:
        case 3012:
        case 3013:
        case 3014:
        case 3015:
            triggerAreaEffectAnimation(object);
            return;
        default:
            return;
    }

    addAreaEffect(object, instances, type);
}

void GJBaseGameLayer::triggerAreaEffectAnimation(EnterEffectObject* object) {
    int effectID = object->m_targetGroupID;

    if (object->m_useEffectID) {
        std::vector<EnterEffectInstance>* allInstances[5] = {
            &this->m_gameState.m_moveEffectInstances,
            &this->m_gameState.m_rotateEffectInstances,
            &this->m_gameState.m_scaleEffectInstances,
            &this->m_gameState.m_fadeEffectInstances,
            &this->m_gameState.m_tintEffectInstances
        };

        for (int i = 0; i < 5; ++i) {
            auto& instances = *allInstances[i];
            for (auto& instance : instances) {
                if (instance.m_gameObject->m_effectID == effectID) {
                    EnterEffectInstance::loadTransitions(&instance, object, this->m_gameState.m_totalTime);
                    if (effectID > 0) break;
                }
            }
        }
        return;
    }

    cocos2d::CCArray* group = getGroup(effectID);
    if (!group) return;

    unsigned int count = cocos2d::CCArray::count(group);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
        if (!obj) break;
        if (obj->m_classType != GameObjectClassType::Effect) continue;

        EnterEffectObject* enterObj = static_cast<EnterEffectObject*>(obj);

        std::vector<EnterEffectInstance>* instances = nullptr;
        switch (enterObj->m_objectID) {
            case 3006:
                instances = &this->m_gameState.m_moveEffectInstances;
                break;
            case 3007:
                instances = &this->m_gameState.m_rotateEffectInstances;
                break;
            case 3008:
                instances = &this->m_gameState.m_scaleEffectInstances;
                break;
            case 3009:
                instances = &this->m_gameState.m_fadeEffectInstances;
                break;
            case 3010:
                instances = &this->m_gameState.m_tintEffectInstances;
                break;
            default:
                continue;
        }

        for (auto& instance : *instances) {
            if (instance.m_gameObject == enterObj) {
                EnterEffectInstance::loadTransitions(&instance, object, this->m_gameState.m_totalTime);
            }
        }
    }
}

void GJBaseGameLayer::triggerDynamicMoveCommand(EffectGameObject* object) {
    int centerID = object->m_targetModCenterID;
    if (centerID <= 0) centerID = object->m_targetGroupID;

    GameObject* targetObject = tryGetObject(centerID);

    PlayerObject* player;
    if (object->m_targetPlayer1) {
        player = this->m_player1;
    } else if (object->m_targetPlayer2) {
        player = this->m_gameState.m_isDualMode ? this->m_player2 : this->m_player1;
    } else {
        player = static_cast<PlayerObject*>(tryGetObject(object->m_centerGroupID));
    }

    if (!player || !targetObject) return;

    DynamicObjectAction action{};
    action.m_gameObject1 = object;
    action.m_gameObject2 = targetObject;
    action.m_gameObject3 = player;
    action.m_controlID = object->m_controlID;
    action.m_targetGroupID = object->m_targetGroupID;
    action.m_centerGroupID = object->m_centerGroupID;
    action.m_unkBool1 = true;
    action.m_unkBool2 = false;
    action.m_unkBool3 = false;
    action.m_unkBool4 = false;
    action.m_unkFloat1 = 0.0f;
    action.m_unkFloat2 = 0.0f;
    action.m_unkFloat3 = 0.0f;

    this->m_gameState.m_dynamicMoveActions.push_back(action);
}

void GJBaseGameLayer::triggerDynamicRotateCommand(EnhancedTriggerObject* object) {
    GameObject* centerObject = nullptr;
    GameObject* targetObject = nullptr;
    GameObject* rotateObject = nullptr;
    getRotateCommandTargets(object, &centerObject, &targetObject, &rotateObject);

    if (!centerObject || !rotateObject) return;

    if (targetObject) {
        GameObject* minX = tryGetObject(object->m_minXID);
        GameObject* minY = tryGetObject(object->m_minYID);
        GameObject* maxX = tryGetObject(object->m_maxXID);
        GameObject* maxY = tryGetObject(object->m_maxYID);

        DynamicObjectAction action{};
        action.m_gameObject1 = object;
        action.m_gameObject2 = centerObject;
        action.m_gameObject3 = rotateObject;
        action.m_gameObject4 = targetObject;
        action.m_gameObject5 = minX;
        action.m_gameObject6 = minY;
        action.m_gameObject7 = maxX;
        action.m_gameObject8 = maxY;
        action.m_controlID = object->m_controlID;
        action.m_targetGroupID = object->m_targetGroupID;
        action.m_centerGroupID = object->m_centerGroupID;
        action.m_unkBool1 = true;
        action.m_unkBool2 = false;
        action.m_unkBool3 = false;
        action.m_unkBool4 = false;
        action.m_unkFloat1 = 0.0f;
        action.m_unkFloat2 = 0.0f;
        action.m_unkFloat3 = 0.0f;

        this->m_gameState.m_dynamicRotateActions.push_back(action);
    }
}

void GJBaseGameLayer::triggerGradientCommand(GradientTriggerObject* object) {
    if (object->m_disableAll) {
        resetGradientLayers();
        return;
    }

    int gradientID = object->m_gradientID;
    if (gradientID > 999) gradientID = 999;

    GJGradientLayer* layer = static_cast<GJGradientLayer*>(cocos2d::CCDictionary::objectForKey(this->m_gradientLayers, gradientID));
    if (!layer) {
        layer = GJGradientLayer::create();
        layer->m_gradientID = gradientID;
        cocos2d::CCDictionary::setObject(this->m_gradientLayers, layer, gradientID);
    }

    if (object->m_disable) {
        layer->removeFromParentAndCleanup(false);
        layer->m_triggerObject = nullptr;
        return;
    }

    layer->m_triggerObject = object;
    layer->m_baseColor = GameObject::getMainColorMode(object);
    layer->m_detailColor = GameObject::getSecondaryColorMode(object);
    layer->m_blendingMode = object->m_blendingMode;

    int blendFuncSrc;
    int blendFuncDst;
    bool premultiply;
    int mode = object->m_blendingMode;
    if (mode == 2) {
        premultiply = true;
        blendFuncSrc = 0x306;
        blendFuncDst = 0x303;
    } else if (mode == 3) {
        premultiply = true;
        blendFuncSrc = 0x307;
        blendFuncDst = 0x301;
    } else {
        premultiply = false;
        if (mode == 1) {
            blendFuncSrc = 0x302;
            blendFuncDst = 0x1;
        } else {
            blendFuncSrc = 0x302;
            blendFuncDst = 0x303;
        }
    }
    cocos2d::ccBlendFunc blend = { (GLenum)blendFuncSrc, (GLenum)blendFuncDst };
    layer->setBlendFunc(blend);

    layer->m_blendingLayer = object->m_blendingLayer;
    layer->m_bShouldPremultiply = premultiply;

    int maxZOrder = maxZOrderForShaderZ(object->m_blendingLayer);

    cocos2d::CCNode* parent = nullptr;
    switch (object->m_blendingLayer) {
        case 2:
            if (this->m_middleground) {
                parent = this->m_middleground->getParent();
            } else {
                parent = this->m_background->getParent();
            }
            break;
        case 3:
            parent = this->m_specialLayerB5->getParent();
            break;
        case 4:
            parent = this->m_specialLayerB4->getParent();
            break;
        case 5:
            parent = this->m_specialLayerB3->getParent();
            break;
        case 6:
            parent = this->m_specialLayerB2->getParent();
            break;
        case 7:
            parent = this->m_specialLayerB1->getParent();
            break;
        case 8:
            parent = this->m_player1->getParent();
            break;
        case 9:
            parent = this->m_specialLayerT1->getParent();
            break;
        case 10:
            parent = this->m_specialLayerT2->getParent();
            break;
        case 11:
            parent = this->m_specialLayerT3->getParent();
            break;
        case 12:
            parent = this->m_specialLayerT4->getParent();
            break;
        case 13:
            parent = this->m_groundLayer->getParent();
            break;
        case 14:
            parent = this->m_uiTriggerUI->getParent();
            break;
        case 15:
            parent = this->m_aboveShaderParent;
            break;
        default:
            parent = this->m_background->getParent();
            break;
    }

    switch (object->m_blendingLayer) {
        case 1:
            maxZOrder = -54;
            break;
        case 2:
            maxZOrder = -14;
            break;
        case 13:
            maxZOrder = 46;
            break;
        case 14:
            maxZOrder = 56;
            break;
        default:
            break;
    }

    int objectZOrder = GameObject::getObjectZOrder(object);
    if (objectZOrder < -5) objectZOrder = -5;
    if (objectZOrder > 5) objectZOrder = 5;
    int finalZOrder = maxZOrder + objectZOrder;

    layer->m_noScale = true;
    int blendingLayer = object->m_blendingLayer;
    if (blendingLayer == 1 || blendingLayer == 2 || blendingLayer == 13 || blendingLayer == 14 || blendingLayer == 15) {
        layer->m_noScale = false;
    }

    if (layer->getParent() == parent) {
        parent->reorderChild(layer, finalZOrder);
    } else {
        layer->removeFromParentAndCleanup(false);
        parent->addChild(layer, finalZOrder);
    }
}

void GJBaseGameLayer::triggerGravityChange(EffectGameObject* object, int playerID) {
    if (object->m_followCPP) {
        if (playerID == 1) {
            this->m_player1->m_gravityMod = object->m_gravityValue;
        } else if (playerID == 2) {
            this->m_player2->m_gravityMod = object->m_gravityValue;
        }
        return;
    }

    if (!object->m_targetPlayer2) {
        this->m_player1->m_gravityMod = object->m_gravityValue;
    }
    if (!object->m_targetPlayer1) {
        this->m_player2->m_gravityMod = object->m_gravityValue;
    }
}

void GJBaseGameLayer::triggerMoveCommand(EffectGameObject* object) {
    cocos2d::CCPoint moveOffset = object->m_moveOffset;
    bool lockToPlayerX = object->m_lockToPlayerX;
    bool lockToPlayerY = object->m_lockToPlayerY;
    float duration = object->m_duration;
    bool lockToCameraX = object->m_lockToCameraX;
    bool lockToCameraY = object->m_lockToCameraY;
    float moveModX = object->m_moveModX;
    float moveModY = object->m_moveModY;

    if (duration <= 1.0f) duration = 1.0f;

    if ((int)(moveOffset.x / duration) + 999999 > 0x1E847E) return;
    if ((lockToCameraX || lockToPlayerX) && fabsf(moveModX) > 99999.0f) return;
    if ((lockToCameraY || lockToPlayerY) && fabsf(moveModY) > 99999.0f) return;

    if (object->m_isDynamicMode) {
        if (object->m_useMoveTarget || object->m_isDirectionFollowSnap360) {
            triggerDynamicMoveCommand(object);
            return;
        }
    } else if (object->m_useMoveTarget || object->m_isDirectionFollowSnap360) {
        cocos2d::CCPoint delta = getMoveTargetDelta(object, !this->m_enable22Changes);
        moveOffset = delta;

        if (object->m_isDirectionFollowSnap360) {
            float len = sqrtf(moveOffset.x * moveOffset.x + moveOffset.y * moveOffset.y);
            if (len > 0.0f) {
                moveOffset.x /= len;
                moveOffset.y /= len;
            }
            moveOffset.x *= object->m_directionModeDistance;
            moveOffset.y *= object->m_directionModeDistance;
        }

        if (object->m_useMoveTarget) {
            int mode = object->m_moveTargetMode;
            lockToCameraX = false;
            lockToCameraY = false;
            lockToPlayerX = false;
            lockToPlayerY = false;
            if (mode == MoveTargetType::XOnly) {
                moveOffset.y = 0.0f;
            } else if (mode == MoveTargetType::YOnly) {
                moveOffset.x = 0.0f;
            }
        } else {
            lockToCameraX = false;
            lockToCameraY = false;
            lockToPlayerX = false;
            lockToPlayerY = false;
        }
    }

    if (object->m_isSilent) {
        moveObjectsSilent(object->m_targetGroupID, moveOffset.x, moveOffset.y);
        return;
    }

    GJEffectManager::createMoveCommand(
        this->m_effectManager,
        moveOffset,
        moveModX,
        object->m_targetGroupID,
        moveModY,
        object->m_easingType,
        lockToPlayerX,
        lockToPlayerY,
        lockToCameraX,
        lockToCameraY,
        object->m_uniqueID,
        object->m_controlID,
        object->m_duration,
        object->m_easingRate);
}

void GJBaseGameLayer::triggerRotateCommand(EnhancedTriggerObject* object) {
    float rotation = object->m_rotationDegrees + object->m_times360 * 360.0f;

    if (object->m_isDynamicMode && object->m_duration != 0.0f) {
        if (object->m_useMoveTarget || object->m_isDirectionFollowSnap360) {
            triggerDynamicRotateCommand(object);
            return;
        }
    } else if (object->m_useMoveTarget || object->m_isDirectionFollowSnap360) {
        GameObject* centerObject = nullptr;
        GameObject* targetObject = nullptr;
        GameObject* rotateObject = nullptr;
        getRotateCommandTargets(object, &centerObject, &targetObject, &rotateObject);

        if (centerObject && rotateObject && targetObject) {
            GameObject* minX = tryGetObject(object->m_minXID);
            GameObject* minY = tryGetObject(object->m_minYID);
            GameObject* maxX = tryGetObject(object->m_maxXID);
            GameObject* maxY = tryGetObject(object->m_maxYID);
            rotation = getRotateCommandAngleDelta(object, centerObject, targetObject, rotateObject, minX, minY, maxX, maxY);
        }
    }

    GJEffectManager::createRotateCommand(
        this->m_effectManager,
        rotation,
        object->m_duration,
        object->m_targetGroupID,
        object->m_centerGroupID,
        object->m_easingType,
        object->m_easingRate,
        object->m_lockObjectRotation,
        false,
        false,
        object->m_uniqueID,
        object->m_controlID);
}

void GJBaseGameLayer::triggerShaderCommand(ShaderGameObject* object) {
    int target = -1;
    if (!object->m_targetPlayer1) {
        target = -2;
        if (!object->m_targetPlayer2) {
            if (object->m_followCPP) target = -3;
            else target = object->m_targetGroupID;
        }
    }

    if (m_isEditor && target < 0 && m_playbackMode == PlaybackMode::Not) target = 0;

    switch (object->m_objectID) {
        case 2904:
            ShaderLayer::updateZLayer(m_shaderLayer, object->m_zLayerMin, object->m_zLayerMax, object->m_useX);
            if (object->m_disableAll) ShaderLayer::resetAllShaders(m_shaderLayer);
            break;
        case 2905:
            ShaderLayer::triggerShockWave(m_shaderLayer, object->m_speed, object->m_targetX,
                object->m_strength, object->m_waveWidth, object->m_fadeIn, object->m_fadeOut,
                object->m_timeOff, object->m_invert, object->m_inner, object->m_screenOffsetX,
                object->m_screenOffsetY, target, object->m_useX, object->m_useY, object->m_relative,
                object->m_outer, object->m_maxSize, object->m_animate, object->m_duration,
                object->m_easingType, object->m_easingRate);
            break;
        case 2907:
            ShaderLayer::triggerShockLine(m_shaderLayer, object->m_speed, object->m_targetX,
                object->m_rotate, object->m_flip, object->m_dual, object->m_invert,
                object->m_strength, object->m_waveWidth, object->m_fadeIn, object->m_fadeOut,
                object->m_timeOff, object->m_screenOffsetX, target, object->m_useX,
                object->m_useY, object->m_relative, object->m_maxSize, object->m_animate,
                object->m_duration, object->m_easingType, object->m_easingRate);
            break;
        case 2909:
            ShaderLayer::triggerGlitch(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_speed, object->m_waveWidth, object->m_outer, object->m_fadeIn,
                object->m_fadeOut, object->m_relative);
            break;
        case 2910:
            if (object->m_useX)
                ShaderLayer::triggerChromaticX(m_shaderLayer, object->m_duration, object->m_targetX,
                    object->m_easingType, object->m_easingRate, object->m_relative);
            if (object->m_useY)
                ShaderLayer::triggerChromaticY(m_shaderLayer, object->m_duration, object->m_targetY,
                    object->m_easingType, object->m_easingRate, object->m_relative);
            break;
        case 2911:
            ShaderLayer::triggerChromaticGlitch(m_shaderLayer, !object->m_disableAll,
                object->m_duration, object->m_speed, object->m_strength, object->m_targetX,
                object->m_waveWidth, object->m_outer, object->m_targetY, object->m_easingType,
                object->m_easingRate, object->m_relative, object->m_snapGrid);
            break;
        case 2912:
            if (object->m_useX)
                ShaderLayer::triggerPixelateX(m_shaderLayer, object->m_duration, object->m_targetX,
                    object->m_snapGrid, object->m_hardEdges, object->m_easingType,
                    object->m_easingRate, object->m_relative);
            if (object->m_useY)
                ShaderLayer::triggerPixelateY(m_shaderLayer, object->m_duration, object->m_targetY,
                    object->m_snapGrid, object->m_hardEdges, object->m_easingType,
                    object->m_easingRate, object->m_relative);
            break;
        case 2913:
            ShaderLayer::triggerLensCircle(m_shaderLayer, object->m_duration, object->m_waveWidth,
                object->m_fadeIn, object->m_strength, target, object->m_centerGroupID,
                object->m_screenOffsetX, object->m_screenOffsetY, object->m_easingType,
                object->m_easingRate, object->m_relative);
            break;
        case 2914:
            ShaderLayer::triggerRadialBlur(m_shaderLayer, object->m_duration, object->m_waveWidth,
                object->m_strength, object->m_fadeIn, object->m_centerGroupID,
                object->m_screenOffsetX, object->m_screenOffsetY, object->m_useX, target,
                object->m_easingType, object->m_easingRate, object->m_hardEdges);
            break;
        case 2915:
            if (object->m_useX)
                ShaderLayer::triggerMotionBlurX(m_shaderLayer, object->m_duration, object->m_targetX,
                    object->m_strength, object->m_fadeIn, target, object->m_outer,
                    object->m_snapGrid, object->m_centerGroupID, object->m_easingType,
                    object->m_easingRate, object->m_relative, object->m_hardEdges);
            if (object->m_useY)
                ShaderLayer::triggerMotionBlurY(m_shaderLayer, object->m_duration, object->m_targetY,
                    object->m_strength, object->m_fadeIn, target, object->m_outer,
                    object->m_snapGrid, object->m_centerGroupID, object->m_easingType,
                    object->m_easingRate, object->m_relative, object->m_hardEdges);
            break;
        case 2916:
            ShaderLayer::triggerBulge(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_screenOffsetX, object->m_screenOffsetY, object->m_targetX,
                object->m_useX ? target : 0, object->m_easingType, object->m_easingRate,
                object->m_relative);
            break;
        case 2917:
            if (object->m_useY)
                ShaderLayer::triggerPinchX(m_shaderLayer, object->m_duration, object->m_targetX,
                    object->m_screenOffsetX, object->m_maxSize, object->m_waveWidth,
                    object->m_useX, target, object->m_easingType, object->m_easingRate,
                    object->m_relative);
            if (object->m_snapGrid)
                ShaderLayer::triggerPinchY(m_shaderLayer, object->m_duration, object->m_targetY,
                    object->m_screenOffsetY, object->m_maxSize, object->m_waveWidth,
                    object->m_useX, target, object->m_easingType, object->m_easingRate,
                    object->m_relative);
            break;
        case 2919:
            ShaderLayer::triggerGrayscale(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_useX, object->m_useY ? target : -1, object->m_easingType,
                object->m_easingRate);
            break;
        case 2920:
            ShaderLayer::triggerSepia(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_easingType, object->m_easingRate);
            break;
        case 2921:
            ShaderLayer::triggerInvertColor(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_waveWidth, object->m_targetX, object->m_targetY, object->m_useX,
                object->m_useY, object->m_snapGrid, object->m_easingType, object->m_easingRate);
            break;
        case 2922:
            ShaderLayer::triggerHueShift(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_easingType, object->m_easingRate);
            break;
        case 2923:
            ShaderLayer::triggerColorChange(m_shaderLayer, object->m_duration, object->m_strength,
                object->m_outer, object->m_speed, object->m_waveWidth, object->m_targetX,
                object->m_targetY, object->m_easingType, object->m_easingRate);
            break;
        case 2924:
            if (object->m_useX)
                ShaderLayer::triggerSplitScreenCols(m_shaderLayer, object->m_duration,
                    object->m_targetX, object->m_easingType, object->m_easingRate);
            if (object->m_useY)
                ShaderLayer::triggerSplitScreenRows(m_shaderLayer, object->m_duration,
                    object->m_targetY, object->m_easingType, object->m_easingRate);
            break;
        default: break;
    }
}
void GJBaseGameLayer::triggerTransformCommand(TransformTriggerGameObject* object) {
    double scaleX = object->m_objectScaleX;
    double scaleY = object->m_objectScaleY;

    if (scaleX == 0.0) scaleX = 1.0;
    if (scaleY == 0.0) scaleY = 1.0;

    if (object->m_divideX) scaleX = 1.0 / scaleX;
    if (object->m_divideY) scaleY = 1.0 / scaleY;

    if (object->m_relativeScale) {
        GameObject* center = tryGetMainObject(object->m_centerGroupID);
        if (center) {
            double relX = center->m_scaleX / center->m_startScaleX;
            double relY = center->m_scaleY / center->m_startScaleY;
            if (relX != 0.0) scaleX = (scaleX - 1.0) / relX + 1.0;
            if (relY != 0.0) scaleY = (scaleY - 1.0) / relY + 1.0;
        }
    }

    GJEffectManager::createTransformCommand(
        this->m_effectManager,
        scaleX,
        scaleY,
        object->m_property450,
        object->m_property451,
        object->m_onlyMove,
        object->m_duration,
        object->m_targetGroupID,
        object->m_centerGroupID,
        object->m_easingType,
        object->m_easingRate,
        false,
        object->m_relativeRotation,
        object->m_uniqueID,
        object->m_controlID);
}

GameObject* GJBaseGameLayer::tryGetGroupParent(int groupID) {
    GameObject* parent = static_cast<GameObject*>(cocos2d::CCDictionary::objectForKey(this->m_parentGroupsDict, groupID));
    if (parent) return parent;

    cocos2d::CCArray* group = getGroup(groupID);
    if (!group) return nullptr;

    unsigned int count = cocos2d::CCArray::count(group);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* object = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
        if (!object) break;
        if (object->m_hasGroupParent) return object;
    }
    return nullptr;
}

GameObject* GJBaseGameLayer::tryGetMainObject(int groupID) {
    if (groupID <= 0) return nullptr;

    GameObject* parent = static_cast<GameObject*>(cocos2d::CCDictionary::objectForKey(this->m_parentGroupsDict, groupID));
    if (parent) return parent;

    return getSingleGroupObject(groupID);
}

GameObject* GJBaseGameLayer::tryGetObject(int groupID) {
    if (groupID <= 0) return nullptr;

    GameObject* mainObject = tryGetMainObject(groupID);
    if (mainObject) return mainObject;

    cocos2d::CCArray* group = getGroup(groupID);
    if (cocos2d::CCArray::count(group) == 0) return nullptr;

    float randValue = GameToolbox::fast_rand_0_1();
    if (randValue == 1.0f) randValue = 0.0f;

    unsigned int count = cocos2d::CCArray::count(group);
    return static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, (int)((float)count * randValue)));
}

void GJBaseGameLayer::tryResumeAudio() {
    if (this->m_resumeTimer > 0) return;
    if (!this->m_audioPaused) return;

    resumeAudio();
    this->m_audioPaused = false;
}

void GJBaseGameLayer::unclaimCustomParticle(std::string const& key, cocos2d::CCParticleSystemQuad* particle) {
    if (!particle) return;

    cocos2d::CCArray* available = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_customParticles, key));
    if (!available) return;

    cocos2d::CCArray* claimed = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_claimedParticles, key));

    if (particle->getParticleCount() > 0) {
        cocos2d::CCArray::addObject(this->m_unclaimedParticles, particle);
    } else {
        cocos2d::CCArray::addObject(available, particle);
    }

    cocos2d::CCArray::removeObject(claimed, particle, true);
    cocos2d::CCParticleSystem::stopSystem(particle);
    particle->setAutoRemoveOnFinish(true);
}

void GJBaseGameLayer::unclaimParticle(char const* key, cocos2d::CCParticleSystemQuad* particle) {
    if (!particle) return;

    cocos2d::CCArray* available = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_particlesDict, key));
    if (!available) return;

    std::string key2 = getParticleKey2(key);
    cocos2d::CCArray* claimed = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_particlesDict, key2));

    cocos2d::CCArray::addObject(available, particle);
    cocos2d::CCArray::removeObject(claimed, particle, true);
    cocos2d::CCArray::removeObject(this->m_temporaryParticles, particle, true);

    particle->setVisible(false);
    particle->removeFromParentAndCleanup(false);
}

void GJBaseGameLayer::ungroupStickyObjects(cocos2d::CCArray* objects) {
    if (!objects) return;

    unsigned int count = cocos2d::CCArray::count(objects);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* object = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(objects, i));
        if (!object) break;

        cocos2d::CCArray* stickyGroup = getStickyGroup(object->m_linkedGroup);
        if (stickyGroup) {
            cocos2d::CCArray::removeObject(stickyGroup, object, true);
            if (cocos2d::CCArray::count(stickyGroup) == 0) {
                cocos2d::CCDictionary::removeObjectForKey(this->m_linkedGroupDict, object->m_linkedGroup);
                object->m_linkedGroup = 0;
            } else {
                object->m_linkedGroup = 0;
            }
        } else {
            object->m_linkedGroup = 0;
        }
    }
}

void GJBaseGameLayer::unlinkAllEvents() {
    auto& map = this->m_gameState.m_unkMapPairGJGameEventIntVectorEventTriggerInstance;
    map.clear();
}

void GJBaseGameLayer::setStartPosObject(StartPosObject* startPos) {
    if (this->m_startPosObject == startPos) return;
    if (startPos) {
        cocos2d::CCObject::retain(startPos);
    }
    if (this->m_startPosObject) {
        cocos2d::CCObject::release(this->m_startPosObject);
    }
    this->m_startPosObject = startPos;
}

void GJBaseGameLayer::setupLayers() {
    cocos2d::CCTextureCache* cache = cocos2d::CCTextureCache::sharedTextureCache();
    cocos2d::CCTexture2D* texGame = cocos2d::CCTextureCache::addImage(cache, "GJ_GameSheet.png", false);
    cocos2d::CCTexture2D* texGame2 = cocos2d::CCTextureCache::addImage(cache, "GJ_GameSheet02.png", false);
    cocos2d::CCTexture2D* texParticle = cocos2d::CCTextureCache::addImage(cache, "GJ_ParticleSheet.png", false);
    cocos2d::CCTexture2D* texGlow = cocos2d::CCTextureCache::addImage(cache, "GJ_GameSheetGlow.png", false);
    cocos2d::CCTexture2D* texFire = cocos2d::CCTextureCache::addImage(cache, "FireSheet_01.png", false);
    cocos2d::CCTexture2D* texPixel = cocos2d::CCTextureCache::addImage(cache, "PixelSheet_01.png", false);

    this->m_objectLayer = cocos2d::CCLayer::create();
    this->m_objectParent->addChild(this->m_objectLayer, 1);
    this->m_objectLayer->setContentSize(cocos2d::CCSize(10000000.0f, 10000000.0f));

    this->m_inShaderObjectLayer = cocos2d::CCLayer::create();
    this->m_inShaderParent->addChild(this->m_inShaderObjectLayer, 1);
    this->m_inShaderObjectLayer->setContentSize(cocos2d::CCSize(10000000.0f, 10000000.0f));

    this->m_aboveShaderObjectLayer = cocos2d::CCLayer::create();
    this->m_aboveShaderParent->addChild(this->m_aboveShaderObjectLayer, 1);
    this->m_aboveShaderObjectLayer->setContentSize(cocos2d::CCSize(10000000.0f, 10000000.0f));

    this->m_gradientLayers = cocos2d::CCDictionary::create();
    this->m_gradientLayers->retain();

    this->m_uiObjectLayers = cocos2d::CCDictionary::create();
    this->m_uiObjectLayers->retain();

    this->m_uiTriggerUI = cocos2d::CCNode::create();
    this->m_objectParent->addChild(this->m_uiTriggerUI, 50);

    cocos2d::ccBlendFunc blendAdd = { GL_ONE, GL_ONE_MINUS_SRC_COLOR };
    cocos2d::ccBlendFunc blendAdd2 = { GL_ONE, GL_ONE_MINUS_SRC_COLOR };

    auto createBatch = [&](cocos2d::CCTexture2D* tex, bool blending, int zOrder, int tag) -> cocos2d::CCSpriteBatchNode* {
        cocos2d::CCSpriteBatchNode* node = cocos2d::CCSpriteBatchNode::createWithTexture(tex, 150);
        if (blending) {
            node->setBlendFunc(blendAdd);
        }
        this->m_objectLayer->addChild(node, zOrder);
        if (tag != 0) node->setTag(tag);
        if (tag != 0) cocos2d::CCArray::addObject(this->m_batchNodes, node);
        return node;
    };

    this->m_gameBlendingLayerT2 = createBatch(texGame, true, 640, 1001);
    this->m_gameLayerT1 = createBatch(texGame, false, 360, 1002);
    this->m_gameBlendingLayerT1 = createBatch(texGame, true, 340, 1004);
    this->m_gameBlendingLayerB0 = createBatch(texGame, true, -10, 1009);
    this->m_gameLayerB1 = createBatch(texGame, false, -20, 1005);
    this->m_gameBlendingLayerB1 = createBatch(texGame, true, -40, 1006);
    this->m_fireLayerB1 = createBatch(texFire, false, -120, 0);
    this->m_fireBlendingLayerB1 = createBatch(texFire, true, -140, 0);
    this->m_gameLayerB2 = createBatch(texGame, false, -320, 1007);
    this->m_gameBlendingLayerB2 = createBatch(texGame, true, -340, 1009);
    this->m_glowLayerT1 = createBatch(texGlow, true, 120, 1010);
    this->m_glowLayerB1 = createBatch(texGlow, true, -280, 1012);
    this->m_glowLayerB2 = createBatch(texGlow, true, -580, 1013);
    this->m_glowLayerB5 = createBatch(texGlow, true, -1480, 0);
    this->m_glowLayerB4 = createBatch(texGlow, true, -1180, 0);
    this->m_glowLayerB3 = createBatch(texGlow, true, -880, 0);
    this->m_glowLayerT2 = createBatch(texGlow, true, 410, 0);
    this->m_glowLayerT3 = createBatch(texGlow, true, 710, 0);
    this->m_glowLayerT4 = createBatch(texGlow, true, 1010, 0);
    this->m_gameLayerB5 = createBatch(texGame, false, -1220, 0);
    this->m_gameBlendingLayerB5 = createBatch(texGame, true, -1240, 0);
    this->m_gameLayerB4 = createBatch(texGame, false, -920, 0);
    this->m_gameBlendingLayerB4 = createBatch(texGame, true, -940, 0);
    this->m_gameLayerB3 = createBatch(texGame, false, -620, 0);
    this->m_gameBlendingLayerB3 = createBatch(texGame, true, -640, 0);
    this->m_gameLayerT2 = createBatch(texGame, false, 660, 0);
    this->m_gameLayerT3 = createBatch(texGame, false, 960, 0);
    this->m_gameBlendingLayerT3 = createBatch(texGame, true, 940, 0);
    this->m_gameLayerT4 = createBatch(texGame, false, 1340, 0);
    this->m_gameBlendingLayerT4 = createBatch(texGame, true, 1320, 0);
    this->m_gameBlendingLayerT5 = createBatch(texGame, true, 1360, 0);
    this->m_fireBlendingLayerT5 = createBatch(texFire, true, 1240, 0);
    this->m_fireBlendingLayerT4 = createBatch(texFire, true, 1200, 0);
    this->m_fireLayerT4 = createBatch(texFire, false, 1220, 0);
    this->m_fireLayerT3 = createBatch(texFire, false, 860, 0);
    this->m_fireBlendingLayerT3 = createBatch(texFire, true, 840, 0);
    this->m_fireLayerT2 = createBatch(texFire, false, 560, 0);
    this->m_fireBlendingLayerT2 = createBatch(texFire, true, 540, 0);
    this->m_fireLayerT1 = createBatch(texFire, false, 260, 0);
    this->m_fireBlendingLayerT1 = createBatch(texFire, true, 240, 0);
    this->m_fireLayerB2 = createBatch(texFire, false, -420, 0);
    this->m_fireBlendingLayerB2 = createBatch(texFire, true, -440, 0);
    this->m_fireLayerB3 = createBatch(texFire, false, -720, 0);
    this->m_fireBlendingLayerB3 = createBatch(texFire, true, -740, 0);
    this->m_fireLayerB4 = createBatch(texFire, false, -1020, 0);
    this->m_fireBlendingLayerB4 = createBatch(texFire, true, -1040, 0);
    this->m_fireLayerB5 = createBatch(texFire, false, -1320, 0);
    this->m_fireBlendingLayerB5 = createBatch(texFire, true, -1340, 0);

    this->m_specialLayerB5 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerB5, -1260);
    this->m_specialLayerB4 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerB4, -960);
    this->m_specialLayerB3 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerB3, -660);
    this->m_specialLayerB2 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerB2, -360);
    this->m_specialLayerB1 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerB1, -60);
    this->m_specialLayerT1 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerT1, 320);
    this->m_specialLayerT2 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerT2, 620);
    this->m_specialLayerT3 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerT3, 920);
    this->m_specialLayerT4 = CCNodeContainer::create();
    this->m_objectLayer->addChild(this->m_specialLayerT4, 1300);

    cocos2d::CCTexture2D::setAliasTexParameters(texPixel);
    this->m_pixelBlendingLayerT5 = createBatch(texPixel, true, 1120, 0);
    this->m_pixelLayerT4 = createBatch(texPixel, false, 1100, 0);
    this->m_pixelBlendingLayerT4 = createBatch(texPixel, true, 1080, 0);
    this->m_pixelLayerT3 = createBatch(texPixel, false, 780, 0);
    this->m_pixelBlendingLayerT3 = createBatch(texPixel, true, 760, 0);
    this->m_pixelLayerT2 = createBatch(texPixel, false, 480, 0);
    this->m_pixelBlendingLayerT2 = createBatch(texPixel, true, 460, 0);
    this->m_pixelLayerT1 = createBatch(texPixel, false, 180, 0);
    this->m_pixelBlendingLayerT1 = createBatch(texPixel, true, 160, 0);
    this->m_pixelLayerB1 = createBatch(texPixel, false, -200, 0);
    this->m_pixelBlendingLayerB1 = createBatch(texPixel, true, -220, 0);
    this->m_pixelLayerB2 = createBatch(texPixel, false, -500, 0);
    this->m_pixelBlendingLayerB2 = createBatch(texPixel, true, -520, 0);
    this->m_pixelLayerB3 = createBatch(texPixel, false, -800, 0);
    this->m_pixelBlendingLayerB3 = createBatch(texPixel, true, -820, 0);
    this->m_pixelLayerB4 = createBatch(texPixel, false, -1100, 0);
    this->m_pixelBlendingLayerB4 = createBatch(texPixel, true, -1120, 0);
    this->m_pixelLayerB5 = createBatch(texPixel, false, -1400, 0);
    this->m_pixelBlendingLayerB5 = createBatch(texPixel, true, -1420, 0);

    this->m_game2BlendingLayerT5 = createBatch(texGame2, true, 1180, 0);
    this->m_game2BlendingLayerT4 = createBatch(texGame2, true, 1140, 0);
    this->m_game2LayerT4 = createBatch(texGame2, false, 1160, 0);
    this->m_game2LayerT3 = createBatch(texGame2, false, 820, 0);
    this->m_game2BlendingLayerT3 = createBatch(texGame2, true, 800, 0);
    this->m_game2LayerT2 = createBatch(texGame2, false, 520, 0);
    this->m_game2BlendingLayerT2 = createBatch(texGame2, true, 500, 0);
    this->m_game2LayerT1 = createBatch(texGame2, false, 220, 0);
    this->m_game2BlendingLayerT1 = createBatch(texGame2, true, 200, 0);
    this->m_game2LayerB0 = createBatch(texGame2, false, -5, 0);
    this->m_game2LayerB1 = createBatch(texGame2, false, -160, 0);
    this->m_game2BlendingLayerB1 = createBatch(texGame2, true, -180, 0);
    this->m_game2LayerB2 = createBatch(texGame2, false, -460, 0);
    this->m_game2BlendingLayerB2 = createBatch(texGame2, true, -480, 0);
    this->m_game2LayerB3 = createBatch(texGame2, false, -760, 0);
    this->m_game2BlendingLayerB3 = createBatch(texGame2, true, -780, 0);
    this->m_game2LayerB4 = createBatch(texGame2, false, -1060, 0);
    this->m_game2BlendingLayerB4 = createBatch(texGame2, true, -1080, 0);
    this->m_game2LayerB5 = createBatch(texGame2, false, -1360, 0);
    this->m_game2BlendingLayerB5 = createBatch(texGame2, true, -1380, 0);

    this->m_particleBlendingLayerT5 = createBatch(texParticle, true, 1074, 0);
    this->m_particleLayerT4 = createBatch(texParticle, false, 1070, 0);
    this->m_particleBlendingLayerT4 = createBatch(texParticle, true, 1066, 0);
    this->m_particleLayerT3 = createBatch(texParticle, false, 754, 0);
    this->m_particleBlendingLayerT3 = createBatch(texParticle, true, 750, 0);
    this->m_particleLayerT2 = createBatch(texParticle, false, 454, 0);
    this->m_particleBlendingLayerT2 = createBatch(texParticle, true, 450, 0);
    this->m_particleLayerT1 = createBatch(texParticle, false, 154, 0);
    this->m_particleBlendingLayerT1 = createBatch(texParticle, true, 150, 0);
    this->m_particleLayerB1 = createBatch(texParticle, false, -224, 0);
    this->m_particleBlendingLayerB1 = createBatch(texParticle, true, -226, 0);
    this->m_particleLayerB2 = createBatch(texParticle, false, -530, 0);
    this->m_particleBlendingLayerB2 = createBatch(texParticle, true, -532, 0);
    this->m_particleLayerB3 = createBatch(texParticle, false, -830, 0);
    this->m_particleBlendingLayerB3 = createBatch(texParticle, true, -832, 0);
    this->m_particleLayerB4 = createBatch(texParticle, false, -1130, 0);
    this->m_particleBlendingLayerB4 = createBatch(texParticle, true, -1132, 0);
    this->m_particleLayerB5 = createBatch(texParticle, false, -1430, 0);
    this->m_particleBlendingLayerB5 = createBatch(texParticle, true, -1432, 0);

    this->m_game2BlendingLayerB0 = createBatch(texGame2, true, -150, 0);
    this->m_pixelBlendingLayerB0 = createBatch(texPixel, true, -190, 0);
    this->m_fireBlendingLayerB0 = createBatch(texFire, true, -110, 0);

    if (this->m_batchNodes) {
        unsigned int count = cocos2d::CCArray::count(this->m_batchNodes);
        for (unsigned int i = 0; i < count; ++i) {
            cocos2d::CCNode* node = static_cast<cocos2d::CCNode*>(cocos2d::CCArray::objectAtIndex(this->m_batchNodes, i));
            if (!node) break;
            node->setUseChildIndex(true);
        }
    }

    this->m_shaderLayer = ShaderLayer::create();
    this->m_shaderLayer->retain();
    this->m_shaderLayer->update(0.0001f);
    this->m_shaderLayer->m_gameLayer = this;
}

void GJBaseGameLayer::setupLevelStart(LevelSettingsObject* settings) {
    PlayerObject::flipGravity(this->m_player1, settings->m_isFlipped, true);
    PlayerObject::doReversePlayer(this->m_player1, settings->m_reverseGameplay);
    PlayerObject::rotateGameplayOnly(this->m_player1, settings->m_rotateGameplay);

    if (settings->m_startDual) {
        toggleDualMode(nullptr, true, nullptr, true);
    }

    PlayerObject::togglePlayerScale(this->m_player1, settings->m_startMini, false);

    if (this->m_gameState.m_isDualMode) {
        PlayerObject::togglePlayerScale(this->m_player2, settings->m_startMini, false);
        PlayerObject::flipGravity(this->m_player2, !this->m_player1->m_isUpsideDown, true);
        PlayerObject::doReversePlayer(this->m_player2, this->m_player1->m_isGoingLeft);
    }

    int startMode = settings->m_startMode;
    if (settings->m_platformerMode || this->m_isPlatformer) {
        if (startMode == 4 || startMode == 7) {
            settings->m_startMode = 0;
            startMode = 0;
        }
    }

    int modeValue = 6;
    switch (startMode) {
        case 1:
            modeValue = 5;
            switchToFlyMode(this->m_player1, nullptr, true, 5);
            if (this->m_gameState.m_isDualMode) {
                switchToFlyMode(this->m_player2, nullptr, true, 5);
            }
            break;
        case 2:
            modeValue = 16;
            switchToRollMode(this->m_player1, nullptr, true);
            if (this->m_gameState.m_isDualMode) {
                switchToRollMode(this->m_player2, nullptr, true);
            }
            break;
        case 3:
            modeValue = 19;
            switchToFlyMode(this->m_player1, nullptr, true, 19);
            if (this->m_gameState.m_isDualMode) {
                switchToFlyMode(this->m_player2, nullptr, true, 19);
            }
            break;
        case 4:
            modeValue = 26;
            switchToFlyMode(this->m_player1, nullptr, true, 26);
            if (this->m_gameState.m_isDualMode) {
                switchToFlyMode(this->m_player2, nullptr, true, 26);
            }
            break;
        case 5:
            modeValue = 27;
            switchToRobotMode(this->m_player1, nullptr, true);
            if (this->m_gameState.m_isDualMode) {
                switchToRobotMode(this->m_player2, nullptr, true);
            }
            break;
        case 6:
            modeValue = 33;
            switchToSpiderMode(this->m_player1, nullptr, true);
            if (this->m_gameState.m_isDualMode) {
                switchToSpiderMode(this->m_player2, nullptr, true);
            }
            break;
        case 7:
            modeValue = 41;
            switchToFlyMode(this->m_player1, nullptr, true, 41);
            if (this->m_gameState.m_isDualMode) {
                switchToFlyMode(this->m_player2, nullptr, true, 41);
            }
            break;
        default:
            break;
    }

    updateDualGround(this->m_player1, modeValue, true, 0.0f);

    switch (settings->m_startSpeed) {
        case Speed::Slow:
            updateTimeMod(0.7f, true, false);
            break;
        case Speed::Fast:
            updateTimeMod(1.1f, true, false);
            break;
        case Speed::Faster:
            updateTimeMod(1.3f, true, false);
            break;
        case Speed::Fastest:
            updateTimeMod(1.6f, true, false);
            break;
        default:
            break;
    }
}

void GJBaseGameLayer::setupReplay(std::string inputs) {
}

void GJBaseGameLayer::shakeCamera(float duration, float strength, float interval) {
    if (duration <= 0.0f || strength <= 0.0f) return;
    if (this->m_skipCameraShake) return;

    if (strength > 100.0f) strength = 100.0f;

    this->m_gameState.m_unkUint64_1 = 0.0f;
    this->m_gameState.m_unkPoint34 = cocos2d::CCPoint(0.0f, 0.0f);
    this->m_gameState.m_cameraShakeFactor = duration;
    this->m_gameState.m_unkUint16 = interval;
    this->m_gameState.m_cameraShakeEnabled = false;
    this->m_gameState.m_unkUint15 = strength;
}

bool GJBaseGameLayer::shouldExitHackedLevel() {
    unsigned int totalLevels = (unsigned int)(this->m_sectionSizes.size());
    int emptyCount = 0;
    int maxGroupID = 0;
    int nonEmptyCount = 0;

    for (unsigned int i = 0; i < totalLevels; ++i) {
        std::vector<int>* sizes = this->m_sectionSizes[i];
        if (!sizes) continue;
        for (size_t j = 0; j < sizes->size(); ++j) {
            int count = (*sizes)[j];
            if (maxGroupID < count) maxGroupID = count;
            if (count > 0) nonEmptyCount++;
            else emptyCount++;
        }
    }

    float ratio = (nonEmptyCount + emptyCount > 0) ? (float)emptyCount / (float)nonEmptyCount : 0.0f;
    int threshold = (ratio <= 0.9f) ? 8000 : 4000;
    return maxGroupID > threshold;
}

bool GJBaseGameLayer::shouldUseSubstepForButton(float dt) {
    if (!this->m_clickBetweenSteps) return false;

    double dtDouble = dt;
    double timestamp = this->m_timestamp;
    double midpoint = dtDouble * 0.5 + timestamp;
    double endpoint = dtDouble + timestamp;

    if (this->m_queuedButtons.empty()) return false;

    for (auto& cmd : this->m_queuedButtons) {
        double cmdTime = cmd.m_timestamp;
        if (cmdTime < midpoint) continue;
        if (endpoint <= cmdTime) return false;
        if (buttonIsRelevant(&cmd)) return true;
    }
    return false;
}

void GJBaseGameLayer::sortAllGroupsX() {
    for (int i = 1; i < 10000; ++i) {
        cocos2d::CCArray* group = this->m_groups[i];
        if (!group) continue;
        qsort(group->data->arr, group->data->num, 8, (int(*)(const void*, const void*))xCompPosition);
    }
}

void GJBaseGameLayer::sortGroups() {
    bool sortAll = false;
    if (this->m_levelSettings && this->m_levelSettings->m_sortGroups) {
        sortAll = true;
    }

    if (sortAll) {
        sortAllGroupsX();
        return;
    }

    std::unordered_set<int> targetGroups;
    if (this->m_objects) {
        cocos2d::ccArray* data = this->m_objects->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;

            if (obj->m_objectID == 1268) {
                SpawnTriggerGameObject* spawn = static_cast<SpawnTriggerGameObject*>(obj);
                for (auto& chanceObj : spawn->m_chanceObjects) {
                    targetGroups.insert(chanceObj.m_groupID);
                }
            } else if (obj->m_objectID == 3607 && !this->m_isEditor) {
                this->m_sequenceTriggers.push_back(static_cast<SequenceTriggerGameObject*>(obj));
            }
        }
    }

    for (int groupID : targetGroups) {
        cocos2d::CCArray* group = getGroup(groupID);
        if (cocos2d::CCArray::count(group) > 1) {
            qsort(group->data->arr, group->data->num, 8, (int(*)(const void*, const void*))xCompPosition);
        }
    }
}

void GJBaseGameLayer::sortSectionVector() {
    size_t total = this->m_nonEffectObjects.size();
    for (size_t i = 0; i < total; ++i) {
        std::vector<std::vector<GameObject*>*>* sections = this->m_nonEffectObjects[i];
        if (!sections) continue;

        for (size_t j = 0; j < sections->size(); ++j) {
            std::vector<GameObject*>* vec = (*sections)[j];
            if (!vec || vec->empty()) continue;

            int count = (*this->m_nonEffectObjectsSizes[i])[j];
            std::sort(vec->begin(), vec->begin() + count, [](GameObject* a, GameObject* b) {
                return a->m_uniqueID < b->m_uniqueID;
            });
            for (int k = 0; k < count; ++k) {
                (*vec)[k]->m_innerSectionIndex = k;
            }
        }
    }

    size_t cbTotal = this->m_collisionBlockSections.size();
    for (size_t i = 0; i < cbTotal; ++i) {
        std::vector<std::vector<GameObject*>*>* sections = this->m_collisionBlockSections[i];
        if (!sections) continue;

        for (size_t j = 0; j < sections->size(); ++j) {
            std::vector<GameObject*>* vec = (*sections)[j];
            if (!vec || vec->empty()) continue;

            int count = (*this->m_collisionBlockSectionSizes[i])[j];
            std::sort(vec->begin(), vec->begin() + count, [](GameObject* a, GameObject* b) {
                return a->m_uniqueID < b->m_uniqueID;
            });
            for (int k = 0; k < count; ++k) {
                (*vec)[k]->m_innerSectionIndex = k;
            }
        }
    }
}

void GJBaseGameLayer::sortStickyGroups() {
    cocos2d::CCDictionary* groups = cocos2d::CCDictionary::create();

    unsigned int objectCount = cocos2d::CCArray::count(this->m_objects);
    for (unsigned int i = 0; i < objectCount; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(this->m_objects, i));
        if (!obj) break;
        int linkedGroup = obj->m_linkedGroup;
        if (linkedGroup <= 0) continue;

        cocos2d::CCArray* group = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(groups, linkedGroup));
        if (!group) {
            group = cocos2d::CCArray::create();
            cocos2d::CCDictionary::setObject(groups, group, linkedGroup);
        }
        cocos2d::CCArray::addObject(group, obj);
    }

    this->m_lastUsedLinkedID = 0;

    if (!groups) return;
    cocos2d::CCDictElement* element = groups->m_pElements;
    while (element) {
        cocos2d::CCArray* group = static_cast<cocos2d::CCArray*>(element->m_pObject);
        this->m_lastUsedLinkedID++;
        int newLinkedID = this->m_lastUsedLinkedID;

        unsigned int count = cocos2d::CCArray::count(group);
        for (unsigned int i = 0; i < count; ++i) {
            GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(group, i));
            obj->m_linkedGroup = newLinkedID;
        }
        cocos2d::CCDictionary::setObject(this->m_linkedGroupDict, group, newLinkedID);

        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJBaseGameLayer::spawnGroup(int group, bool ordered, double delay, std::vector<int> const& remapKeys, int triggerID, int controlID) {
    if (!GJEffectManager::isGroupEnabled(this->m_effectManager, group)) return;
    if (this->m_spawnAbuse) return;

    int remapValue = 0;
    if (!remapKeys.empty()) {
        remapValue = -remapKeys.back();
    }

    int enable22 = this->m_enable22Changes ? triggerID : 0;
    std::tuple<int, int, int> key = std::make_tuple(group, enable22, remapValue);

    auto it = this->m_spawnTuples.find(key);
    if (it != this->m_spawnTuples.end()) {
        this->m_spawnCount++;
        if (this->m_spawnCount > 1000) {
            this->m_spawnAbuse = true;
        }
        return;
    }

    this->m_spawnTuples.insert(key);
    cocos2d::CCArray* objects = getGroup(group);

    if (ordered) {
        spawnObjectsInOrder(objects, delay, remapKeys, triggerID, controlID);
    } else if (objects) {
        unsigned int count = cocos2d::CCArray::count(objects);
        for (unsigned int i = 0; i < count; ++i) {
            GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(objects, i));
            if (!obj) break;
            spawnObject(obj, delay, remapKeys);
            if (this->m_spawnAbuse) break;
        }
    }
}

void GJBaseGameLayer::spawnGroupTriggered(int targetID, float delay, bool spawnOrdered, std::vector<int> const& remapKeys, int uniqueID, int controlID) {
    if (this->m_spawnAbuse) return;
    if (delay <= 0.0f) {
        spawnGroup(targetID, spawnOrdered, (double)(-delay), remapKeys, uniqueID, controlID);
    } else {
        GJEffectManager::spawnGroup(this->m_effectManager, targetID, delay, spawnOrdered, remapKeys, uniqueID, controlID);
    }
}

void GJBaseGameLayer::spawnObject(GameObject* object, double delay, std::vector<int> const& remapKeys) {
    if (this->m_spawnAbuse) return;
    if (object->m_classType != GameObjectClassType::Effect) {
        object->animationTriggered();
        return;
    }

    if (object->m_isGroupDisabled) return;
    if (!object->m_bUseChildIndex && object->hasBeenActivated()) return;
    if (!object->m_spawnTriggered) return;
    if (!GameObject::isSpawnableTrigger(object)) return;

    if (remapKeys.empty()) {
        object->triggerActivated(0.0f);
        if (object->m_objectID == 1268) {
            std::vector<int> newRemaps;
            SpawnTriggerGameObject::updateRemapKeys(static_cast<SpawnTriggerGameObject*>(object), &newRemaps);
            object->m_spawnDelay = delay;
        }
        object->triggerObject(this, 0, nullptr);
        return;
    }

    std::unordered_map<int, int> remap;
    applyRemap(static_cast<EffectGameObject*>(object), &remapKeys, &remap);
    object->triggerActivated(0.0f);
    if (object->m_objectID == 1268) {
        std::vector<int> newRemaps;
        SpawnTriggerGameObject::updateRemapKeys(static_cast<SpawnTriggerGameObject*>(object), &newRemaps);
        object->m_spawnDelay = delay;
    }
    object->triggerObject(this, 0, nullptr);
    restoreRemap(static_cast<EffectGameObject*>(object), remap);
}

void GJBaseGameLayer::spawnObjectsInOrder(cocos2d::CCArray* objects, double delay, std::vector<int> const& remapKeys, int uniqueID, int controlID) {
    if (this->m_spawnAbuse || !objects) return;

    unsigned int count = cocos2d::CCArray::count(objects);
    if (count == 0) return;

    float firstTime = 0.0f;
    bool hasFirst = false;

    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(objects, i));
        if (!obj) break;

        bool isTrigger = false;
        if (obj->m_classType == GameObjectClassType::Effect) {
            isTrigger = GameObject::isSpawnableTrigger(obj);
        } else if (obj->m_objectID == 2065) {
            isTrigger = true;
        }

        if (!isTrigger) continue;

        float objTime = obj->getObjectPosition().x;
        if (!hasFirst) {
            firstTime = objTime;
            hasFirst = true;
            spawnObject(obj, 0.0, remapKeys);
        } else {
            float relativeTime = (objTime - firstTime) / 311.580109f - (float)delay;
            if (relativeTime <= 0.0f) {
                spawnObject(obj, 0.0, remapKeys);
            } else {
                GJEffectManager::spawnObject(this->m_effectManager, obj, relativeTime, remapKeys, uniqueID, controlID);
            }
        }

        if (this->m_spawnAbuse) return;
    }
}

cocos2d::CCParticleSystemQuad* GJBaseGameLayer::spawnParticle(char const* plist, int zOrder, cocos2d::tCCPositionType positionType, cocos2d::CCPoint position) {
    if (GameManager::sharedState()->m_performanceMode) return nullptr;

    cocos2d::CCParticleSystemQuad* particle = cocos2d::CCParticleSystemQuad::create(plist);
    particle->setTag(zOrder);
    particle->setPositionType(positionType);
    particle->setAutoRemoveOnFinish(true);
    this->m_objectLayer->addChild(particle, zOrder);
    particle->setPosition(position);
    return particle;
}

void GJBaseGameLayer::spawnParticleTrigger(SpawnParticleGameObject* object) {
    float rotation = object->m_rotation + (float)rand() * 4.6566e-10f * 2.0f - 1.0f;
    rotation += object->m_rotationVariance;
    float scale = object->m_scale + ((float)rand() * 4.6566e-10f * 2.0f - 1.0f) * object->m_scaleVariance;

    cocos2d::CCPoint pos = object->getPosition();

    int centerID = object->m_centerGroupID;
    if (centerID > 0) {
        GameObject* center = tryGetObject(centerID);
        if (center) {
            pos = center->getPosition();
            if (object->m_matchRotation) {
                rotation += center->getRotation();
            }
        }
    }

    pos.x += object->m_offset.x + ((float)rand() * 4.6566e-10f * 2.0f - 1.0f) * object->m_offsetVariance.x;
    pos.y += object->m_offset.y + ((float)rand() * 4.6566e-10f * 2.0f - 1.0f) * object->m_offsetVariance.y;

    spawnParticleTrigger(object->m_targetGroupID, pos, rotation, scale);
}

void GJBaseGameLayer::spawnParticleTrigger(int particleID, cocos2d::CCPoint position, float rotation, float scale) {
    SpawnParticleGameObject* trigger = static_cast<SpawnParticleGameObject*>(tryGetObject(particleID));
    if (trigger) {
        spawnParticleTrigger(trigger);
    }
}

void GJBaseGameLayer::spawnPlayer2() {
    if (!this->m_player2->getParent()) {
        cocos2d::CCNode* parent = this->m_player1->getParent();
        parent->addChild(this->m_player2, 59);
        if (!this->m_isEditor) {
            PlayerObject::addAllParticles(this->m_player2);
        }
    }
    PlayerObject::spawnFromPlayer(this->m_player2, true);
}

cocos2d::CCPoint GJBaseGameLayer::speedForShaderTarget(int groupID) {
    cocos2d::CCPoint result(0.0f, 0.0f);

    if (groupID == -1) {
        cocos2d::CCPoint pos = this->m_player1->getPosition();
        cocos2d::CCPoint lastPos = this->m_player1->m_lastPosition;
        result.x = pos.x - lastPos.x;
        result.y = pos.y - lastPos.y;
        return result;
    }

    if (groupID == -2) {
        if (this->m_gameState.m_isDualMode) {
            cocos2d::CCPoint pos = this->m_player2->getPosition();
            cocos2d::CCPoint lastPos = this->m_player2->m_lastPosition;
            result.x = pos.x - lastPos.x;
            result.y = pos.y - lastPos.y;
            return result;
        }
        cocos2d::CCPoint pos = this->m_player1->getPosition();
        cocos2d::CCPoint lastPos = this->m_player1->m_lastPosition;
        result.x = pos.x - lastPos.x;
        result.y = pos.y - lastPos.y;
        return result;
    }

    if (groupID <= 0) {
        result.x = -this->m_gameState.m_cameraStepDiff.x;
        result.y = -this->m_gameState.m_cameraStepDiff.y;
        return result;
    }

    GameObject* mainObject = tryGetMainObject(groupID);
    if (mainObject) {
        cocos2d::CCPoint pos = mainObject->getRealPosition();
        cocos2d::CCPoint lastPos = *mainObject->getLastPosition();
        result.x = pos.x - lastPos.x;
        result.y = pos.y - lastPos.y;
    }
    return result;
}

cocos2d::CCArray* GJBaseGameLayer::staticObjectsInRect(cocos2d::CCRect rect, bool enabledGroups) {
    cocos2d::CCArray* result = cocos2d::CCArray::create();

    int outerMin;
    if (rect.origin.x > 0.0f) {
        if (rect.origin.x < 10000000.0f) {
            float scaled = rect.origin.x * this->m_sectionXFactor - 1.0f;
            outerMin = (scaled > 0.0f) ? (int)scaled : -1;
        } else {
            float scaled = this->m_sectionXFactor * 10000000.0f - 1.0f;
            outerMin = (scaled > 0.0f) ? (int)scaled : -1;
        }
    } else {
        outerMin = 0;
    }

    int outerMax;
    if (rect.origin.x + rect.size.width > 0.0f) {
        if (rect.origin.x + rect.size.width < 10000000.0f) {
            outerMax = (int)((rect.origin.x + rect.size.width) * this->m_sectionXFactor + 1.0f);
        } else {
            outerMax = (int)(this->m_sectionXFactor * 10000000.0f + 1.0f);
        }
        int maxIndex = (int)this->m_nonEffectObjects.size() - 1;
        if (outerMax > maxIndex) outerMax = maxIndex;
    } else {
        outerMax = 1;
    }

    int middleMin;
    if (rect.origin.y > 0.0f) {
        if (rect.origin.y < 10000000.0f) {
            float scaled = rect.origin.y * this->m_sectionYFactor - 1.0f;
            middleMin = (scaled > 0.0f) ? (int)scaled : -1;
        } else {
            float scaled = this->m_sectionYFactor * 10000000.0f - 1.0f;
            middleMin = (scaled > 0.0f) ? (int)scaled : -1;
        }
    } else {
        middleMin = 0;
    }

    int middleMax;
    if (rect.origin.y + rect.size.height > 0.0f) {
        if (rect.origin.y + rect.size.height < 10000000.0f) {
            middleMax = (int)((rect.origin.y + rect.size.height) * this->m_sectionYFactor + 1.0f);
        } else {
            middleMax = (int)(this->m_sectionYFactor * 10000000.0f + 1.0f);
        }
    } else {
        middleMax = 1;
    }

    for (int sx = outerMin; sx <= outerMax; ++sx) {
        if (sx >= (int)this->m_nonEffectObjects.size()) break;
        std::vector<std::vector<GameObject*>*>* xVec = this->m_nonEffectObjects[sx];
        if (!xVec) continue;

        int maxY = (int)xVec->size() - 1;
        if (middleMax < maxY) maxY = middleMax;

        for (int sy = middleMin; sy <= maxY; ++sy) {
            std::vector<GameObject*>* vec = (*xVec)[sy];
            if (!vec) continue;

            int count = (*this->m_nonEffectObjectsSizes[sx])[sy];
            for (int i = 0; i < count; ++i) {
                GameObject* obj = (*vec)[i];

                bool pass = false;
                if (obj->m_isActivated) {
                    pass = true;
                } else if (!enabledGroups || obj->getGroupDisabled()) {
                    continue;
                } else {
                    pass = true;
                }

                if (!pass) continue;

                int type = obj->getType();
                if (type != GameObjectType::Solid && type != GameObjectType::Breakable &&
                    type != GameObjectType::Hazard && type != GameObjectType::AnimatedHazard) {
                    continue;
                }

                cocos2d::CCRect objRect = *obj->getObjectRect();
                if (rect.intersectsRect(objRect)) {
                    cocos2d::CCArray::addObject(result, obj);
                }
            }
        }
    }
    return result;
}

void GJBaseGameLayer::updateAllObjectSection() {
    if (!this->m_objects) return;
    unsigned int count = cocos2d::CCArray::count(this->m_objects);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(this->m_objects, i));
        if (!obj) break;
        updateObjectSection(obj);
    }
}

void GJBaseGameLayer::updateAttemptTime(float attemptTime) {
}

void GJBaseGameLayer::updateBGArtSpeed(float modX, float modY) {
    if (fabsf(modX) > 999.0f) modX = 0.1f;
    if (fabsf(modY) > 999.0f) modY = 0.1f;
    this->m_gameState.m_unkFloat5 = modX;
    this->m_gameState.m_unkFloat6 = modY;
}

void GJBaseGameLayer::updateCameraEdge(int direction, int value) {
    switch (direction) {
        case 1: this->m_gameState.m_cameraEdgeValue0 = value; break;
        case 2: this->m_gameState.m_cameraEdgeValue1 = value; break;
        case 3: this->m_gameState.m_cameraEdgeValue2 = value; break;
        case 4: this->m_gameState.m_cameraEdgeValue3 = value; break;
        default: break;
    }
}

void GJBaseGameLayer::updateCameraMode(EffectGameObject* obj, bool updateDual) {
    int isFreeMode = obj->m_cameraIsFreeMode;
    int previousFreeMode = this->m_gameState.m_unkBool8;
    this->m_gameState.m_unkBool8 = isFreeMode;
    this->m_gameState.m_unkBool9 = obj->m_cameraDisableGridSnap;

    if (obj->m_cameraEditCameraSettings) {
        float easingValue = obj->m_cameraEasingValue;
        float clampedEasing = 1.0f;
        if (easingValue >= 1.0f && easingValue <= 40.0f) {
            clampedEasing = easingValue;
        } else if (easingValue > 40.0f) {
            clampedEasing = 40.0f;
        }
        this->m_gameState.m_unkFloat3 = clampedEasing;

        float paddingValue = obj->m_cameraPaddingValue;
        float clampedPadding = 0.0f;
        if (paddingValue >= 0.0f) {
            clampedPadding = (paddingValue <= 1.0f) ? paddingValue : 1.0f;
        }
        this->m_gameState.m_unkFloat2 = clampedPadding;
    }

    if (previousFreeMode != isFreeMode && updateDual) {
        updateDualGround(this->m_player1, this->m_gameState.m_dualRelated, false, 0.0f);
    }
}

void GJBaseGameLayer::updateCameraOffsetX(float offsetX, float duration, int easingType, float easingRate, int uniqueID, int controlID) {
    this->m_gameState.m_unkPoint1.x = offsetX;
    if (duration <= 0.0f) {
        this->m_gameState.stopTweenAction(15);
        this->m_gameState.m_cameraOffset.x = this->m_gameState.m_unkPoint1.x;
    } else {
        this->m_gameState.tweenValue(this->m_gameState.m_cameraOffset.x, offsetX, 15, duration, easingType, easingRate, uniqueID, controlID);
    }
}

void GJBaseGameLayer::updateCameraOffsetY(float offsetY, float duration, int easingType, float easingRate, int uniqueID, int controlID) {
    this->m_gameState.m_unkPoint1.y = offsetY;
    if (duration <= 0.0f) {
        this->m_gameState.stopTweenAction(16);
        this->m_gameState.m_cameraOffset.y = this->m_gameState.m_unkPoint1.y;
    } else {
        this->m_gameState.tweenValue(this->m_gameState.m_cameraOffset.y, offsetY, 16, duration, easingType, easingRate, uniqueID, controlID);
    }
}

void GJBaseGameLayer::updateDisabledObjectsLastPos(cocos2d::CCArray* objects) {
}

void GJBaseGameLayer::updateGameplayOffsetX(int offsetX, bool staticOffset) {
    this->m_gameState.m_unkInt1 = offsetX;
    this->m_gameState.m_unkBool2 = staticOffset;
}

void GJBaseGameLayer::updateGameplayOffsetY(int offsetY, bool staticOffset) {
    this->m_gameState.m_unkInt2 = offsetY;
    this->m_gameState.m_unkBool3 = staticOffset;
}

void GJBaseGameLayer::updateInternalCamOffsetX(float offsetX, float duration, float easingRate) {
    this->m_gameState.m_unkPoint27.x = offsetX;
    if (duration <= 0.0f) {
        this->m_gameState.stopTweenAction(18);
        this->m_gameState.m_unkPoint26.x = this->m_gameState.m_unkPoint27.x;
    } else {
        this->m_gameState.tweenValue(this->m_gameState.m_unkPoint26.x, offsetX, 18, duration, 1, easingRate, -1, -1);
    }
}

void GJBaseGameLayer::updateInternalCamOffsetY(float offsetY, float duration, float easingRate) {
    this->m_gameState.m_unkPoint27.y = offsetY;
    if (duration <= 0.0f) {
        this->m_gameState.stopTweenAction(19);
        this->m_gameState.m_unkPoint26.y = this->m_gameState.m_unkPoint27.y;
    } else {
        this->m_gameState.tweenValue(this->m_gameState.m_unkPoint26.y, offsetY, 19, duration, 1, easingRate, -1, -1);
    }
}

void GJBaseGameLayer::updateKeyframeOrder(int keyframeGroup) {
    cocos2d::CCArray* array = static_cast<cocos2d::CCArray*>(cocos2d::CCDictionary::objectForKey(this->m_keyframeGroups, keyframeGroup));
    if (!array || cocos2d::CCArray::count(array) == 0) return;

    GameObject* first = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(array, 0));
    int baseUniqueID = (first->m_uniqueID >> 16);
    int baseFlag = (first->m_unknownSmallBool ? 1 : 0);
    int nextUniqueID = baseUniqueID + 1;

    unsigned int count = cocos2d::CCArray::count(array);
    for (unsigned int i = 0; i < count; ++i) {
        GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(array, i));
        obj->m_keyframeIndex = i;
        obj->m_unknownSmallBool = (baseFlag != 0);
        if (baseFlag != 0) {
            obj->m_keyframeUniqueID = nextUniqueID++;
            obj->m_uniqueID = (baseUniqueID << 16) | (obj->m_uniqueID & 0xFFFF);
        }
    }
}

void GJBaseGameLayer::updateMGArtSpeed(float modX, float modY) {
    if (fabsf(modX) > 999.0f) modX = 0.3f;
    if (fabsf(modY) > 999.0f) modY = 0.5f;
    this->m_gameState.m_unkFloat7 = modX;
    this->m_gameState.m_unkFloat8 = modY;
}

void GJBaseGameLayer::updateMGOffsetY(float offsetY, float duration, int easingType, float easingRate, int uniqueID, int controlID) {
    this->m_gameState.m_unkInt3 = *reinterpret_cast<int*>(&offsetY);
    this->m_gameState.m_unkInt4 = *reinterpret_cast<int*>(&duration);
    if (duration <= 0.0f) {
        this->m_gameState.stopTweenAction(20);
        this->m_gameState.m_middleGroundOffsetY = *reinterpret_cast<float*>(&this->m_gameState.m_unkInt3);
    } else {
        this->m_gameState.tweenValue(this->m_gameState.m_middleGroundOffsetY, offsetY, 20, duration, easingType, easingRate, uniqueID, controlID);
    }
}

void GJBaseGameLayer::updateMaxGameplayY() {
    if (this->m_levelSettings && LevelSettingsObject::shouldUseYSection(this->m_levelSettings)) {
        if (!this->m_objects) {
            this->m_maxGameplayY = 1590.0f;
            return;
        }
        unsigned int count = cocos2d::CCArray::count(this->m_objects);
        float maxY = 1200.0f;
        for (unsigned int i = 0; i < count; ++i) {
            GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(this->m_objects, i));
            if (!obj) break;
            float objY = obj->getPosition().y;
            if (objY > maxY) maxY = objY;
        }
        this->m_maxGameplayY = maxY + 90.0f + 300.0f;
    } else {
        this->m_maxGameplayY = 2790.0f;
    }
}

void GJBaseGameLayer::updateOBB2(cocos2d::CCRect rect) {
    cocos2d::CCPoint center(rect.origin.x + rect.size.width * 0.5f,
                            rect.origin.y + rect.size.height * 0.5f);
    this->m_obb2->calculateWithCenter(center, 0.0f, rect.size.width, rect.size.height);
}

void GJBaseGameLayer::updateObjectSection(GameObject* object) {
    int currentOuter = object->m_outerSectionIndex;
    if (currentOuter < 0) return;

    int newOuter;
    float posX = object->m_positionX;
    if (posX <= 0.0f) {
        newOuter = 0;
    } else if (posX < 10000000.0f) {
        newOuter = (int)(posX * this->m_sectionXFactor);
    } else {
        newOuter = (int)(this->m_sectionXFactor * 10000000.0f);
    }

    int newMiddle;
    float posY = object->m_positionY;
    if (posY <= 0.0f) {
        newMiddle = 0;
    } else if (posY < 10000000.0f) {
        newMiddle = (int)(posY * this->m_sectionYFactor);
    } else {
        newMiddle = (int)(this->m_sectionYFactor * 10000000.0f);
    }

    if (newOuter == currentOuter && newMiddle == object->m_middleSectionIndex) return;

    reorderObjectSection(object);

    if (object->m_isActivated) {
        int outer = object->m_outerSectionIndex;
        if (outer > this->m_rightSectionIndex || outer < this->m_leftSectionIndex) {
            cocos2d::CCDictionary::setObject(this->m_objectsToDeactivate, object, object->m_uniqueID);
            object->m_unk3ee = true;
            return;
        }
        int middle = object->m_middleSectionIndex;
        if (middle > this->m_topSectionIndex || middle < this->m_bottomSectionIndex) {
            cocos2d::CCDictionary::setObject(this->m_objectsToDeactivate, object, object->m_uniqueID);
            object->m_unk3ee = true;
        }
    }
}

void GJBaseGameLayer::updatePlatformerTime() {
    int playTimeMS = getPlayTimerMilli();
    int seconds = (int)((float)playTimeMS / 1000.0f);
    int centiseconds = (int)((float)(playTimeMS - 1000 * seconds) / 10.0f);

    updateTimeLabel(seconds, centiseconds, true);

    auto it = this->m_labelObjects.find(-1);
    if (it == this->m_labelObjects.end() || it->second.empty()) return;

    cocos2d::CCString* formatted = cocos2d::CCString::createWithFormat("%i.%02d", seconds, centiseconds);
    std::string formattedStr = formatted->getCString();

    std::string secondsStr = GameToolbox::intToString(seconds);

    for (LabelGameObject* label : it->second) {
        if (!label->m_isActivated) continue;
        if (label->m_showSecondsOnly) {
            LabelGameObject::updateLabel(label, secondsStr);
        } else {
            LabelGameObject::updateLabel(label, formattedStr);
        }
    }
}

void GJBaseGameLayer::updatePlayerCollisionBlocks() {
    GameObject* blocks[2] = { this->m_player1CollisionBlock, this->m_player2CollisionBlock };
    PlayerObject* players[2] = { this->m_player1, this->m_player2 };

    for (int i = 0; i < 2; ++i) {
        GameObject* block = blocks[i];
        PlayerObject* player = players[i];
        if (!block || !player) continue;

        cocos2d::CCPoint pos = player->getRealPosition();
        block->setStartPos(pos);

        block->m_startRotationX = player->getRotation();
        block->m_startRotationY = player->getRotation();

        block->m_scaleX = player->m_scaleX;
        block->m_scaleY = player->m_scaleY;
        block->m_spriteWidthScale = player->m_spriteWidthScale;
        block->m_spriteHeightScale = player->m_spriteHeightScale;
        block->m_isOrientedBoxDirty = true;

        block->calculateOrientedBox();
        reorderObjectSection(block);
    }
}

void GJBaseGameLayer::updateQueuedLabels() {
    for (auto& pair : this->m_labelObjects) {
        for (LabelGameObject* label : pair.second) {
            if (label->m_isActivated) {
                LabelGameObject::updateLabelIfDirty(label);
            }
        }
    }
    for (auto& pair : this->m_timeLabelObjects) {
        for (LabelGameObject* label : pair.second) {
            if (label->m_isActivated) {
                LabelGameObject::updateLabelIfDirty(label);
            }
        }
    }
}

void GJBaseGameLayer::updateReplay() {
}

void GJBaseGameLayer::updateSavePositionObjects() {
    if (this->m_gameState.m_unkUint5 != 0 && this->m_gameState.m_unkUint5 + 1000 > this->m_gameState.m_unkUint6) return;

    this->m_gameState.m_unkUint5 = this->m_gameState.m_unkUint6;
    this->m_gameState.m_unkUint11++;

    for (auto& pair : this->m_savePositionValues) {
        int groupID = pair.first;
        int maxCount = pair.second;
        cocos2d::CCPoint position;
        int startIndex = 0;

        if (groupID > 0) {
            GameObject* main = this->m_savePositionObjects[groupID];
            if (!main) continue;
            position = *main->getLastPosition();
            startIndex = main->m_unk52C;
        } else if (groupID == -1) {
            position = *this->m_player1->getLastPosition();
            startIndex = this->m_player1->m_unk52C;
        } else if (groupID == -2) {
            position = *this->m_player2->getLastPosition();
            startIndex = this->m_player2->m_unk52C;
        } else if (groupID == -3) {
            position = this->m_gameState.m_cameraPosition2;
            startIndex = this->m_gameState.m_unkUint12;
        } else {
            continue;
        }

        int index = 2 * (this->m_gameState.m_unkUint11 % maxCount + startIndex);
        if ((size_t)(index + 1) < this->m_gameState.m_unkVecFloat1.size()) {
            this->m_gameState.m_unkVecFloat1[index] = position.x;
            this->m_gameState.m_unkVecFloat1[index + 1] = position.y;
        }
    }
}

void GJBaseGameLayer::updateSpecialGroupData() {
    generateSpawnRemap();
    regenerateEnterEasingBuffers();
    sortGroups();
    optimizeMoveGroups();
    prepareSavePositionObjects();
    sortStickyGroups();
    generateTargetGroups();
    positionUIObjects();
    generateVisibilityGroups();
    refreshKeyframeAnims();
    processSFXObjects();
}

void GJBaseGameLayer::updateSpecialLabels() {
    updatePlatformerTime();

    auto pointsIt = this->m_labelObjects.find(-2);
    if (pointsIt != this->m_labelObjects.end() && !pointsIt->second.empty()) {
        if (this->m_points != this->m_gameState.m_points) {
            this->m_points = this->m_gameState.m_points;
            this->m_pointsString = GameToolbox::pointsToString(this->m_points);
        }

        for (LabelGameObject* label : pointsIt->second) {
            if (label->m_isActivated) {
                LabelGameObject::updateLabel(label, this->m_pointsString);
            }
        }
    }

    auto attemptsIt = this->m_labelObjects.find(-3);
    if (attemptsIt != this->m_labelObjects.end() && !attemptsIt->second.empty()) {
        std::string attemptsStr = GameToolbox::intToString(this->m_attempts);
        for (LabelGameObject* label : attemptsIt->second) {
            if (label->m_isActivated) {
                LabelGameObject::updateLabel(label, attemptsStr);
            }
        }
    }
}

void GJBaseGameLayer::updateTimeLabel(int seconds, int centiseconds, bool decimals) {
}

void GJBaseGameLayer::updateTimeMod(float speed, bool players, bool noEffects) {
    if (players) {
        this->m_gameState.m_timeModRelated2 = 0;
        this->m_gameState.m_timeModRelated = 0.0f;
        PlayerObject::updateTimeMod(this->m_player1, speed, noEffects);
        if (this->m_gameState.m_isDualMode) {
            PlayerObject::updateTimeMod(this->m_player2, speed, noEffects);
        }
    } else {
        this->m_gameState.m_timeModRelated2 = noEffects ? 1 : 0;
        this->m_gameState.m_timeModRelated = speed;
    }
}

void GJBaseGameLayer::updateTimeWarp(float timeWarp) {
    float clamped = timeWarp;
    if (clamped > 2.0f) clamped = 2.0f;
    if (clamped < 0.1f) clamped = 0.1f;

    this->m_gameState.m_timeWarp = clamped;
    this->m_gameState.m_queuedTimeWarp = 0.0f;

    if (clamped == 1.0f) {
        applyTimeWarp(1.0f);
    }
}

void GJBaseGameLayer::updateVerifyDamage() {
}

void GJBaseGameLayer::updateCollisionBlocks() {
    if (cocos2d::CCArray::count(this->m_collisionBlocks) == 0) return;

    updatePlayerCollisionBlocks();

    unsigned int blockCount = cocos2d::CCArray::count(this->m_collisionBlocks);
    for (unsigned int i = 0; i < blockCount; ++i) {
        EffectGameObject* block = static_cast<EffectGameObject*>(cocos2d::CCArray::objectAtIndex(this->m_collisionBlocks, i));
        if (!block) break;
        if (block->m_isGroupDisabled) continue;

        if (!block->m_isUIObject) {
            int outer = block->m_outerSectionIndex;
            int middle = block->m_middleSectionIndex;

            int outerMin = (outer <= 1) ? 0 : outer - 1;
            int outerMax = outer + 1;
            int maxOuter = (int)this->m_collisionBlockSections.size() - 1;
            if (outerMax > maxOuter) outerMax = maxOuter;

            int middleMin = (middle <= 1) ? 0 : middle - 1;
            int middleMax = middle + 1;

            if (block->m_hasExtendedCollision) {
                cocos2d::CCRect* objRect = block->getObjectRectPointer();
                float minX = objRect->getMinX();
                if (minX > 0.0f) {
                    if (minX < 10000000.0f) {
                        float scaled = minX * this->m_sectionXFactor - 1.0f;
                        outerMin = (scaled > 0.0f) ? (int)scaled : -1;
                    } else {
                        float scaled = this->m_sectionXFactor * 10000000.0f - 1.0f;
                        outerMin = (scaled > 0.0f) ? (int)scaled : -1;
                    }
                } else {
                    outerMin = 0;
                }

                float maxX = objRect->getMaxX();
                if (maxX > 0.0f) {
                    if (maxX < 10000000.0f) {
                        float scaled = maxX * this->m_sectionXFactor + 1.0f;
                        outerMax = (int)scaled;
                    } else {
                        outerMax = (int)(this->m_sectionXFactor * 10000000.0f + 1.0f);
                    }
                    int maxIndex = (int)this->m_collisionBlockSections.size() - 1;
                    if (outerMax > maxIndex) outerMax = maxIndex;
                } else {
                    outerMax = 1;
                }

                float minY = objRect->getMinY();
                if (minY > 0.0f) {
                    if (minY < 10000000.0f) {
                        float scaled = minY * this->m_sectionYFactor - 1.0f;
                        middleMin = (scaled > 0.0f) ? (int)scaled : -1;
                    } else {
                        float scaled = this->m_sectionYFactor * 10000000.0f - 1.0f;
                        middleMin = (scaled > 0.0f) ? (int)scaled : -1;
                    }
                } else {
                    middleMin = 0;
                }

                float maxY = objRect->getMaxY();
                if (maxY > 0.0f) {
                    if (maxY < 10000000.0f) {
                        middleMax = (int)(maxY * this->m_sectionYFactor + 1.0f);
                    } else {
                        middleMax = (int)(this->m_sectionYFactor * 10000000.0f + 1.0f);
                    }
                } else {
                    middleMax = 1;
                }
            }

            for (int sx = outerMin; sx <= outerMax; ++sx) {
                std::vector<std::vector<GameObject*>*>* xVec = this->m_collisionBlockSections[sx];
                if (!xVec) continue;
                int yEnd = (int)xVec->size() - 1;
                if (middleMax < yEnd) yEnd = middleMax;
                for (int sy = middleMin; sy <= yEnd; ++sy) {
                    std::vector<GameObject*>* vec = (*xVec)[sy];
                    if (!vec) continue;
                    int count = (*this->m_collisionBlockSectionSizes[sx])[sy];
                    checkCollisionBlocks(block, vec, count);
                }
            }

            if (this->m_calcCollisionBlockObjectsSize > 0) {
                checkCollisionBlocks(block, &this->m_calcCollisionBlockObjects, this->m_calcCollisionBlockObjectsSize);
            }
        } else if (this->m_calcCollisionBlockObjects2Size > 0) {
            checkCollisionBlocks(block, &this->m_calcCollisionBlockObjects2, this->m_calcCollisionBlockObjects2Size);
        }

        if (block->m_isUIObject) {
            int lastIndex = this->m_calcCollisionBlockObjects2Size - 1;
            this->m_calcCollisionBlockObjects2Size = lastIndex;
            this->m_calcCollisionBlockObjects2[block->m_innerSectionIndex] = this->m_calcCollisionBlockObjects2[lastIndex];
            this->m_calcCollisionBlockObjects2[block->m_innerSectionIndex]->m_innerSectionIndex = block->m_innerSectionIndex;
        } else if (block->m_hasExtendedCollision) {
            int lastIndex = this->m_calcCollisionBlockObjectsSize - 1;
            this->m_calcCollisionBlockObjectsSize = lastIndex;
            this->m_calcCollisionBlockObjects[block->m_innerSectionIndex] = this->m_calcCollisionBlockObjects[lastIndex];
            this->m_calcCollisionBlockObjects[block->m_innerSectionIndex]->m_innerSectionIndex = block->m_innerSectionIndex;
        } else {
            int outer = block->m_outerSectionIndex;
            int middle = block->m_middleSectionIndex;
            std::vector<int>* sizes = this->m_collisionBlockSectionSizes[outer];
            std::vector<GameObject*>* vec = (*this->m_collisionBlockSections[outer])[middle];
            int lastIndex = (*sizes)[middle] - 1;
            (*sizes)[middle] = lastIndex;
            (*vec)[block->m_innerSectionIndex] = (*vec)[lastIndex];
            (*vec)[block->m_innerSectionIndex]->m_innerSectionIndex = block->m_innerSectionIndex;
        }
    }

    blockCount = cocos2d::CCArray::count(this->m_collisionBlocks);
    for (unsigned int i = 0; i < blockCount; ++i) {
        GameObject* block = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(this->m_collisionBlocks, i));
        if (!block) break;

        if (block->m_isUIObject) {
            int idx = this->m_calcCollisionBlockObjects2Size;
            if ((size_t)idx < this->m_calcCollisionBlockObjects2.size()) {
                this->m_calcCollisionBlockObjects2[idx] = static_cast<EffectGameObject*>(block);
            } else {
                this->m_calcCollisionBlockObjects2.push_back(static_cast<EffectGameObject*>(block));
            }
            this->m_calcCollisionBlockObjects2Size = idx + 1;
            block->m_innerSectionIndex = idx;
        } else if (block->m_hasExtendedCollision) {
            int idx = this->m_calcCollisionBlockObjectsSize;
            if ((size_t)idx < this->m_calcCollisionBlockObjects.size()) {
                this->m_calcCollisionBlockObjects[idx] = block;
            } else {
                this->m_calcCollisionBlockObjects.push_back(block);
            }
            this->m_calcCollisionBlockObjectsSize = idx + 1;
            block->m_innerSectionIndex = idx;
        } else {
            int outer = block->m_outerSectionIndex;
            int middle = block->m_middleSectionIndex;
            std::vector<int>* sizes = this->m_collisionBlockSectionSizes[outer];
            std::vector<GameObject*>* vec = (*this->m_collisionBlockSections[outer])[middle];
            int idx = (*sizes)[middle];
            if ((size_t)idx < vec->size()) {
                (*vec)[idx] = block;
            } else {
                vec->push_back(block);
            }
            (*sizes)[middle] = idx + 1;
            block->m_innerSectionIndex = idx;
        }
    }
}

void GJBaseGameLayer::updateDebugDraw() {
    this->m_debugDrawNode->clear();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float objectScale = this->m_objectLayer->getScale();
    float lineWidth = 0.25f / objectScale;

    cocos2d::CCPoint layerPos = this->m_objectLayer->getPosition();
    cocos2d::CCPoint worldPos = layerPos * (1.0f / objectScale);
    cocos2d::CCPoint origin = worldPos - cocos2d::CCPoint(winSize.width * 0.5f / objectScale,
                                                            winSize.height * 0.5f / objectScale);
    cocos2d::CCRect drawRect(origin.x, origin.y, winSize.width / objectScale, winSize.height / objectScale);
    this->m_debugDrawNode->enableDrawArea(&drawRect);

    int sectionCount = (int)this->m_sections.size();
    bool drawObjects = true;

    if (this->m_isEditor) {
        if (!this->m_isDebugDrawEnabled) drawObjects = false;
        else if (this->m_leftSectionIndex >= sectionCount) drawObjects = false;
        else if (this->m_leftSectionIndex > this->m_rightSectionIndex) drawObjects = false;
    } else {
        if (this->m_leftSectionIndex >= sectionCount) drawObjects = false;
        else if (this->m_leftSectionIndex > this->m_rightSectionIndex) drawObjects = false;
    }

    if (drawObjects) {
        for (int sx = this->m_leftSectionIndex; sx <= this->m_rightSectionIndex && sx < sectionCount; ++sx) {
            std::vector<std::vector<GameObject*>*>* xVec = this->m_sections[sx];
            if (!xVec) continue;

            int yEnd = (int)xVec->size() - 1;
            if (this->m_topSectionIndex < yEnd) yEnd = this->m_topSectionIndex;
            for (int sy = this->m_bottomSectionIndex; sy <= yEnd; ++sy) {
                std::vector<GameObject*>* vec = (*xVec)[sy];
                if (!vec) continue;

                int count = (*this->m_sectionSizes[sx])[sy];
                for (int i = 0; i < count; ++i) {
                    GameObject* obj = (*vec)[i];
                    if (!obj->m_isActivated && !this->m_isEditor) continue;

                    GameObjectType type = obj->m_objectType;
                    if (type == GameObjectType::Decoration) continue;
                    if (this->m_playbackMode != PlaybackMode::Not && obj->m_isGroupDisabled) continue;

                    if (obj->m_objectRadius > 0.0f) {
                        if (type == GameObjectType::Hazard) {
                            cocos2d::CCPoint pos = obj->getPosition();
                            cocos2d::ccColor4F fill = { 1.0f, 0.0f, 0.0f, 1.0f };
                            cocos2d::ccColor4F border = { 0.0f, 0.0f, 0.0f, 0.0f };
                            unsigned int segments = 8;
                            float radius = obj->getObjectRadius();
                            if (radius > 8.0f) segments = (unsigned int)radius;
                            this->m_debugDrawNode->drawCircle(pos, radius, &fill, lineWidth, &border, segments);
                        }
                        continue;
                    }

                    if (obj->getType() == GameObjectType::Slope) {
                        cocos2d::CCRect objRect = *obj->getObjectRect();
                        GameObject::determineSlopeDirection(obj);
                        cocos2d::CCPoint points[3];
                        points[0] = cocos2d::CCPoint(objRect.origin.x, objRect.origin.y);
                        points[1] = cocos2d::CCPoint(objRect.origin.x, objRect.origin.y + objRect.size.height);
                        points[2] = cocos2d::CCPoint(objRect.origin.x + objRect.size.width, objRect.origin.y);
                        cocos2d::CCPoint topRight(objRect.origin.x + objRect.size.width, objRect.origin.y + objRect.size.height);
                        switch (obj->m_slopeDirection) {
                            case 0: case 7: points[1] = topRight; break;
                            case 1: case 5: points[0] = topRight; break;
                            case 3: case 6: points[2] = topRight; break;
                            default: break;
                        }
                        cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
                        cocos2d::ccColor4F border = obj->m_isGripSlope ? cocos2d::ccColor4F{ 1.0f, 0.0f, 0.25f, 1.0f }
                                                                       : cocos2d::ccColor4F{ 0.0f, 0.25f, 1.0f, 1.0f };
                        this->m_debugDrawNode->drawPolygon(points, 3, &fill, &border, 0, lineWidth);
                        continue;
                    }

                    cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
                    cocos2d::ccColor4F border;
                    if (obj->getType() == GameObjectType::Hazard || obj->getType() == GameObjectType::AnimatedHazard) {
                        border = { 1.0f, 0.0f, 0.0f, 1.0f };
                    } else if (obj->getType() != GameObjectType::Solid) {
                        if (!obj->m_greenDebugDraw) continue;
                        border = { 0.0f, 1.0f, 0.0f, 1.0f };
                    } else if (obj->m_isPassable) {
                        border = { 1.0f, 1.0f, 0.0f, 1.0f };
                    } else {
                        border = { 0.0f, 0.25f, 1.0f, 1.0f };
                    }

                    if (this->m_anticheatSpike == obj) continue;

                    bool wasRectDirty = obj->m_isObjectRectDirty;
                    bool wasBoxCalculated = obj->m_boxOffsetCalculated;

                    cocos2d::CCRect objRect = *obj->getObjectRect();

                    int rotation = (int)obj->getRotation();
                    if ((rotation % 90) != 0 && (this->m_isEditor || obj->m_shouldUseOuterOb)) {
                        obj->calculateOrientedBox();
                        OBB2D* obb = obj->getOrientedBox();
                        this->m_debugDrawNode->drawPolygon(obb->m_corners.__elems_, 4, &fill, &border, 0, lineWidth);
                    } else {
                        cocos2d::CCPoint p1(objRect.origin.x, objRect.origin.y);
                        cocos2d::CCPoint p2(objRect.origin.x + objRect.size.width, objRect.origin.y + objRect.size.height);
                        this->m_debugDrawNode->drawRect(p1, p2, &fill, &border, 0, lineWidth);
                    }

                    obj->m_isObjectRectDirty = wasRectDirty;
                    obj->m_boxOffsetCalculated = wasBoxCalculated;
                }
            }
        }
    }

    if (this->m_isEditor) return;
    if (this->m_disablePlayerHitbox && !(this->m_hitboxesOnDeath && this->m_playerDied)) return;

    this->m_player1->updateOrientedBox();
    if (this->m_player1->m_isBall) {
        cocos2d::CCRect playerRect = *this->m_player1->getObjectRect();
        cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
        cocos2d::ccColor4F border = { 1.0f, 1.0f, 0.0f, 1.0f };
        this->m_debugDrawNode->drawRect(cocos2d::CCPoint(playerRect.origin.x, playerRect.origin.y),
                                        cocos2d::CCPoint(playerRect.origin.x + playerRect.size.width, playerRect.origin.y + playerRect.size.height),
                                        &fill, &border, 0, lineWidth);
    } else {
        OBB2D* obb = this->m_player1->getOrientedBox();
        cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
        cocos2d::ccColor4F border = { 1.0f, 1.0f, 0.0f, 1.0f };
        this->m_debugDrawNode->drawPolygon(obb->m_corners.__elems_, 4, &fill, &border, 0, lineWidth);
    }

    if (this->m_gameState.m_isDualMode) {
        this->m_player2->updateOrientedBox();
        if (this->m_player2->m_isBall) {
            cocos2d::CCRect playerRect = *this->m_player2->getObjectRect();
            cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
            cocos2d::ccColor4F border = { 1.0f, 1.0f, 0.0f, 1.0f };
            this->m_debugDrawNode->drawRect(cocos2d::CCPoint(playerRect.origin.x, playerRect.origin.y),
                                            cocos2d::CCPoint(playerRect.origin.x + playerRect.size.width, playerRect.origin.y + playerRect.size.height),
                                            &fill, &border, 0, lineWidth);
        } else {
            OBB2D* obb = this->m_player2->getOrientedBox();
            cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
            cocos2d::ccColor4F border = { 1.0f, 1.0f, 0.0f, 1.0f };
            this->m_debugDrawNode->drawPolygon(obb->m_corners.__elems_, 4, &fill, &border, 0, lineWidth);
        }
    }

    if (this->m_hitboxesOnDeath && this->m_playerDied) {
        cocos2d::CCRect p1Rect = *this->m_player1->getObjectRect();
        cocos2d::ccColor4F fill = { 0.0f, 0.0f, 0.0f, 0.0f };
        cocos2d::ccColor4F border = { 1.0f, 1.0f, 0.0f, 0.5f };
        this->m_debugDrawNode->drawRect(cocos2d::CCPoint(p1Rect.origin.x, p1Rect.origin.y),
                                        cocos2d::CCPoint(p1Rect.origin.x + p1Rect.size.width, p1Rect.origin.y + p1Rect.size.height),
                                        &fill, &border, 0, lineWidth);

        if (this->m_gameState.m_isDualMode) {
            cocos2d::CCRect p2Rect = *this->m_player2->getObjectRect();
            this->m_debugDrawNode->drawRect(cocos2d::CCPoint(p2Rect.origin.x, p2Rect.origin.y),
                                            cocos2d::CCPoint(p2Rect.origin.x + p2Rect.size.width, p2Rect.origin.y + p2Rect.size.height),
                                            &fill, &border, 0, lineWidth);
        }
    }
}

void GJBaseGameLayer::visitWithColorFlash() {
    GJEffectManager* em = this->m_effectManager;

    cocos2d::ccColor3B colorBG = GJEffectManager::activeColorForIndex(em, 1000);
    cocos2d::ccColor3B colorG1 = GJEffectManager::activeColorForIndex(em, 1001);
    cocos2d::ccColor3B colorG2 = GJEffectManager::activeColorForIndex(em, 1009);

    cocos2d::ccHSVValue hsv = { 1.0f, 1.0f, 1.0f, false, false };
    cocos2d::ccColor3B bgTinted = GameToolbox::transformColor(colorBG, hsv);
    cocos2d::ccColor3B g1Tinted = GameToolbox::transformColor(colorG1, hsv);
    cocos2d::ccColor3B g2Tinted = GameToolbox::transformColor(colorG2, hsv);

    this->m_background->setColor(bgTinted);
    GJGroundLayer::updateGround01Color(this->m_groundLayer, g1Tinted);
    GJGroundLayer::updateGround01Color(this->m_groundLayer2, g1Tinted);
    GJGroundLayer::updateGround02Color(this->m_groundLayer, g2Tinted);
    GJGroundLayer::updateGround02Color(this->m_groundLayer2, g2Tinted);

    cocos2d::CCNode::visit(this);

    this->m_background->setColor(colorBG);
    GJGroundLayer::updateGround01Color(this->m_groundLayer, colorG1);
    GJGroundLayer::updateGround01Color(this->m_groundLayer2, colorG1);
    GJGroundLayer::updateGround02Color(this->m_groundLayer, colorG2);
    GJGroundLayer::updateGround02Color(this->m_groundLayer2, colorG2);
}

void GJBaseGameLayer::updateZoom(float zoom, float duration, int easing, float rate, int uniqueID, int controlID) {
    float targetZoom = zoom;
    if (targetZoom <= 0.0f) {
        targetZoom = 1.0f;
    } else if (targetZoom >= 3.0f) {
        targetZoom = 3.0f;
    } else if (targetZoom <= 0.4f) {
        targetZoom = 0.4f;
    }

    this->m_gameState.m_targetCameraZoom = targetZoom;

    if (duration <= 0.0f) {
        this->m_gameState.stopTweenAction(14);
        this->m_gameState.m_cameraZoom = targetZoom;
    } else {
        this->m_gameState.tweenValue(this->m_gameState.m_cameraZoom, targetZoom, 14, duration, easing, rate, uniqueID, controlID);
    }

    this->m_gameState.m_unkUint1 = duration;

    if (!this->m_skipArtReload && this->m_gameState.m_unkBool7) {
        GameObjectType activeMode = PlayerObject::getActiveMode(this->m_player1);
        updateDualGround(this->m_player1, (int)activeMode, duration == 0.0f, duration);
    }
}

void GJBaseGameLayer::visit() {
    if (this->m_loadingProgress < 1.0f) {
        cocos2d::CCNode::visit(this);
        return;
    }

    bool flipped = false;
    if (this->m_player1 && isFlipping()) {
        float levelFlipping = this->m_gameState.m_levelFlipping;
        if (this->m_cameraFlip != 1.0f) {
            if (levelFlipping > 0.5f) flipped = true;
        } else {
            if (levelFlipping < 0.5f) flipped = true;
        }
    }

    if (flipped) {
        flipObjects();
    }

    cocos2d::CCRect bgOrigRect = this->m_background->m_obRect;
    cocos2d::CCRect bgNewRect = bgOrigRect;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::CCPoint savedBgPos = this->m_background->getPosition();
    cocos2d::CCPoint savedGroundPos = this->m_groundLayer2->getPosition();
    bool ground2Visible = this->m_groundLayer->isVisible() && this->m_groundLayer->m_showGround2;
    bool groundClipped = !ground2Visible;

    this->m_groundLayer->m_ground1Sprite->setOpacity(100);

    int savedGradientUpdate = -1;
    bool restoredGradient = false;
    cocos2d::CCRect savedBgRect;
    cocos2d::CCPoint savedBgPositionRestore;
    bool shaderAdjusted = false;

    if (this->m_gameState.m_cameraAngle != 0.0f) {
        cocos2d::CCPoint offset = center;

        float bgY = this->m_background->getPosition().y;
        float groundY = this->m_groundLayer->getPosition().y;
        float halfCamHeight = this->m_cameraHeightOffset * 0.5f;
        if (groundY >= -halfCamHeight) groundY = this->m_groundLayer->getPosition().y;
        float targetBgY;
        if (groundClipped) {
            targetBgY = this->m_background->getPosition().y + halfCamHeight;
        } else {
            targetBgY = bgY - groundY;
        }

        cocos2d::CCPoint bgPos = this->m_background->getPosition();
        bgPos.x += -this->m_cameraWidthOffset * 0.5f;
        bgPos.y += -targetBgY;
        this->m_background->setPosition(bgPos);

        bgOrigRect = this->m_background->m_obRect;
        savedBgRect = bgOrigRect;

        float halfWidth = this->m_cameraWidthOffset * 0.5f;
        float bgScale = this->m_background->getScale();
        bgNewRect.origin.x -= halfWidth / bgScale;
        bgNewRect.size.width += this->m_cameraWidthOffset / bgScale;
        bgNewRect.origin.y += targetBgY / bgScale;

        if (this->m_groundLayer2->isVisible()) {
            float ground2Y = this->m_groundLayer2->getPosition().y;
            float winHeight = winSize.height + halfCamHeight;
            if (ground2Y <= winHeight) ground2Y = this->m_groundLayer2->getPosition().y;
            float ground2Target = ground2Y;

            float heightDelta = (ground2Target - this->m_background->getPosition().y) + 1.0f;
            float scale2 = this->m_background->getScale();
            bgNewRect.size.height -= heightDelta / scale2;
            float scale3 = this->m_background->getScale();
            bgNewRect.origin.y += heightDelta / scale3;
            bgNewRect.size.height = heightDelta / scale3;
        } else {
            float winHeight = winSize.height + halfCamHeight;
            float ground2Target = winHeight;

            float heightDelta = (ground2Target - this->m_background->getPosition().y) + 1.0f;
            float scale2 = this->m_background->getScale();
            bgNewRect.size.height -= heightDelta / scale2;
            float scale3 = this->m_background->getScale();
            bgNewRect.origin.y += heightDelta / scale3;
            bgNewRect.size.height = heightDelta / scale3;
        }

        float bgX = this->m_background->getPosition().x;
        float newBgX = -1.0f - (this->m_cameraWidthOffset * 0.5f) - bgX;
        bgPos = this->m_background->getPosition();
        bgPos.x += newBgX;
        this->m_background->setPosition(bgPos);

        float bgScaleX = this->m_background->getScale();
        bgNewRect.origin.x += newBgX / bgScaleX;

        float winWidth = winSize.width + this->m_cameraWidthOffset + 1.0f;
        float bgScaleY = this->m_background->getScale();
        bgNewRect.size.width = winWidth / bgScaleY;
        this->m_background->setTextureRect(bgNewRect);

        int currentCommandIndex = this->m_gameState.m_commandIndex;
        if (currentCommandIndex != s_lastGradientUpdate) {
            s_lastGradientUpdate = currentCommandIndex;

            float gradientOffsetY;
            if (groundClipped) {
                gradientOffsetY = halfCamHeight;
            } else {
                float groundY2 = this->m_groundLayer->getPosition().y;
                gradientOffsetY = halfCamHeight;
                if (groundY2 >= -halfCamHeight) gradientOffsetY = -this->m_groundLayer->getPosition().y;
            }

            if (this->m_gradientLayers) {
                cocos2d::CCDictElement* element = this->m_gradientLayers->m_pElements;
                while (element) {
                    GJGradientLayer* layer = static_cast<GJGradientLayer*>(element->m_pObject);
                    if (!layer->getParent()) {
                        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
                        continue;
                    }

                    GameObject* trigger = layer->m_triggerObject;
                    if (trigger) {
                        GameObject* mainTop = tryGetMainObject(trigger->m_unknown5);
                        GameObject* minY = tryGetMainObject(trigger->m_unk704);
                        GameObject* mainBottom = tryGetMainObject(trigger->m_unknown2);
                        GameObject* maxY = tryGetMainObject(trigger->m_unk708);

                        float halfCamWidth = this->m_cameraWidthOffset * 0.5f;
                        float halfCamHeightGrad = this->m_cameraHeightOffset * 0.5f;
                        float scale = 1.0f;

                        if (layer->m_noScale) {
                            scale = this->m_objectLayer->getScale();
                            halfCamWidth /= scale;
                            gradientOffsetY /= scale;
                            halfCamHeightGrad /= scale;
                        }

                        if (layer->m_bShouldPremultiply) {
                            cocos2d::CCPoint pos = layer->getPosition();
                            pos.x -= halfCamWidth;
                            pos.y -= gradientOffsetY;
                            layer->setPosition(pos);

                            if (halfCamWidth + (halfCamWidth + halfCamWidth) != 0.0f) {
                                float currentScaleX = layer->getScaleX();
                                float totalWidth = winSize.width;
                                float ratio = (halfCamWidth + (totalWidth * currentScaleX)) / (winSize.width * currentScaleX);
                                layer->setScaleX(currentScaleX * ratio);
                            }

                            if (halfCamHeightGrad + (halfCamHeightGrad + halfCamHeightGrad) != 0.0f) {
                                float currentScaleY = layer->getScaleY();
                                float totalHeight = winSize.height;
                                float ratio = (halfCamHeightGrad + (totalHeight * currentScaleY)) / (winSize.height * currentScaleY);
                                layer->setScaleY(currentScaleY * ratio);
                            }
                        }
                    }

                    element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
                }
            }
        }

        shaderAdjusted = false;
        if (this->m_shaderLayer && this->m_shaderLayer->m_state.m_usesShaders && this->m_shaderLayer->m_state.m_minBlendingLayer > 0) {
            savedBgRect = this->m_background->m_obRect;
            savedBgPositionRestore = this->m_background->getPosition();

            float pad = 100.0f / this->m_background->getScale();
            cocos2d::CCRect padRect = savedBgRect;
            if (groundClipped || this->m_gameState.m_cameraAngle == 0.0f) {
                padRect.origin.x -= pad;
                padRect.size.width += pad * 2.0f;
                padRect.size.height += pad * 2.0f;
                padRect.origin.y -= pad;
                this->m_background->setTextureRect(padRect);

                cocos2d::CCPoint pos = savedBgPositionRestore;
                pos.x += -100.0f;
                pos.y += 0.0f;
                this->m_background->setPosition(pos);
            } else {
                padRect.origin.y -= pad;
                padRect.size.width += pad * 2.0f;
                padRect.origin.x -= pad;
                padRect.size.height += pad;
                this->m_background->setTextureRect(padRect);

                cocos2d::CCPoint pos = savedBgPositionRestore;
                pos.x += -100.0f;
                pos.y += 0.0f;
                this->m_background->setPosition(pos);
            }
            shaderAdjusted = true;
        }

        if (this->m_flashNode->isVisible()) {
            visitWithColorFlash();
            if (shaderAdjusted) {
                this->m_background->setTextureRect(savedBgRect);
                this->m_background->setPosition(savedBgPositionRestore);
            }
        } else {
            cocos2d::CCNode::visit(this);
            if (shaderAdjusted) {
                this->m_background->setTextureRect(savedBgRect);
                this->m_background->setPosition(savedBgPositionRestore);
            }
        }

        for (int i = 0; i < 3; ++i) {
            cocos2d::CCNode* parent;
            if (i == 0) parent = this->m_objectParent;
            else if (i == 1) parent = this->m_inShaderParent;
            else parent = this->m_aboveShaderParent;

            cocos2d::CCArray* children = parent->getChildren();
            if (children) {
                unsigned int count = cocos2d::CCArray::count(children);
                for (unsigned int j = 0; j < count; ++j) {
                    cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(cocos2d::CCArray::objectAtIndex(children, j));
                    if (!child) break;
                    cocos2d::CCPoint pos = child->getPosition();
                    pos += center;
                    child->setPosition(pos);
                }
            }

            parent->setRotation(this->m_gameState.m_cameraAngle);
            parent->setPosition(center);
        }

        this->m_uiTriggerUI->setRotation(-this->m_gameState.m_cameraAngle);
        cocos2d::CCNode* uiParent = this->m_uiTriggerUI->getParent();
        cocos2d::CCPoint uiPos = cocos2d::CCPoint(0.0f, 0.0f);
        if (uiParent) uiPos = cocos2d::CCNode::convertToNodeSpace(uiParent, cocos2d::CCPoint(0.0f, 0.0f));
        this->m_uiTriggerUI->setPosition(uiPos);
    } else {
        float groundY;
        if (this->m_groundLayer->getPosition().y >= 0.0f) {
            groundY = this->m_groundLayer->getPosition().y;
        } else {
            groundY = 0.0f;
        }

        float bgY = this->m_background->getPosition().y;
        float offsetY;
        if (groundClipped) {
            offsetY = -1.0f - bgY;
        } else {
            offsetY = (groundY - bgY) - 1.0f;
        }

        cocos2d::CCPoint bgPos = this->m_background->getPosition();
        bgPos.y += offsetY;
        this->m_background->setPosition(bgPos);

        float bgScale = this->m_background->getScale();
        bgNewRect.origin.y -= offsetY / bgScale;

        float ground2Y;
        if (this->m_groundLayer2->isVisible()) {
            ground2Y = winSize.height;
            if (this->m_groundLayer2->getPosition().y <= winSize.height) {
                ground2Y = this->m_groundLayer2->getPosition().y;
            }
        } else {
            ground2Y = winSize.height;
        }

        float heightDelta = (ground2Y - this->m_background->getPosition().y) + 1.0f;
        float scale2 = this->m_background->getScale();
        bgNewRect.size.height -= heightDelta / scale2;
        float scale3 = this->m_background->getScale();
        bgNewRect.origin.y += heightDelta / scale3;
        bgNewRect.size.height = heightDelta / scale3;

        float bgX = this->m_background->getPosition().x;
        float newBgX = -1.0f - bgX;
        bgPos = this->m_background->getPosition();
        bgPos.x += newBgX;
        this->m_background->setPosition(bgPos);

        float bgScaleX = this->m_background->getScale();
        bgNewRect.origin.x += newBgX / bgScaleX;

        float winWidth = winSize.width + 2.0f;
        float bgScaleY = this->m_background->getScale();
        bgNewRect.size.width = winWidth / bgScaleY;
        this->m_background->setTextureRect(bgNewRect);

        if (this->m_flashNode->isVisible()) {
            visitWithColorFlash();
        } else {
            cocos2d::CCNode::visit(this);
        }

        if (shaderAdjusted) {
            this->m_background->setTextureRect(savedBgRect);
            this->m_background->setPosition(savedBgPositionRestore);
        }

        if (this->m_gameState.m_cameraAngle != 0.0f) {
            for (int i = 0; i < 3; ++i) {
                cocos2d::CCNode* parent;
                if (i == 0) parent = this->m_objectParent;
                else if (i == 1) parent = this->m_inShaderParent;
                else parent = this->m_aboveShaderParent;

                cocos2d::CCArray* children = parent->getChildren();
                if (children) {
                    unsigned int count = cocos2d::CCArray::count(children);
                    for (unsigned int j = 0; j < count; ++j) {
                        cocos2d::CCNode* child = static_cast<cocos2d::CCNode*>(cocos2d::CCArray::objectAtIndex(children, j));
                        if (!child) break;
                        cocos2d::CCPoint pos = child->getPosition();
                        pos += center;
                        child->setPosition(pos);
                    }
                }

                parent->setRotation(0.0f);
                parent->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
            }

            this->m_uiTriggerUI->setRotation(0.0f);
            this->m_uiTriggerUI->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
        }
    }

    this->m_background->setTextureRect(bgOrigRect);
    this->m_background->setPosition(savedBgPos);

    if (flipped) {
        flipObjects();
    }
}

void GJBaseGameLayer::queueTimeWarp(float timeWarp) {
    m_gameState.m_queuedTimeWarp = timeWarp;
}

void GJBaseGameLayer::reAddToStickyGroup(GameObject* object) {
    int uniqueID = object->m_uniqueID;
    auto it = m_stickyGroups.find(uniqueID);
    if (it == m_stickyGroups.end()) return;
    if (object->m_linkedGroup) return;

    int linkedGroup = it->second;
    cocos2d::CCArray* stickyGroup = getStickyGroup(linkedGroup);
    if (!stickyGroup) return;

    cocos2d::CCArray::addObject(stickyGroup, object);
    object->m_linkedGroup = linkedGroup;
    m_stickyGroups.erase(uniqueID);
}

void GJBaseGameLayer::refreshCounterLabels() {
    for (auto& pair : m_labelObjects) {
        int itemID = pair.first;
        int count = GJEffectManager::countForItem(m_effectManager, itemID);
        std::string countStr = std::to_string(count);

        for (LabelGameObject* label : pair.second) {
            LabelGameObject::updateLabel(label, countStr);
        }
    }
}

void GJBaseGameLayer::regenerateEnterEasingBuffers() {
    m_enterEasingValues.clear();
    m_enterEasingIndices.clear();
    m_enterEasingValuesIndex = 0;

    if (!m_objects) return;

    cocos2d::ccArray* data = m_objects->data;
    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = static_cast<GameObject*>(data->arr[i]);
        if (!obj) break;

        switch (obj->m_objectID) {
            case 3006:
            case 3007:
            case 3008:
            case 3009:
            case 3010:
            case 3011:
            case 3012:
            case 3013:
            case 3014:
            case 3015:
            case 3017:
            case 3018:
            case 3019:
            case 3020:
            case 3021:
                generateEnterEasingBuffers(static_cast<EnterEffectObject*>(obj));
                break;
            default:
                break;
        }
    }
}

void GJBaseGameLayer::registerStateObject(EffectGameObject* object) {
    m_gameState.m_stateObjects[object->m_uniqueID] = static_cast<EnhancedGameObject*>(object);
}

void GJBaseGameLayer::resetActiveEnterEffects() {
    for (int& channel : m_gameState.m_enterChannelMap) channel = -2;
    for (int& channel : m_gameState.m_exitChannelMap) channel = -2;

    m_gameState.m_enterEffectInstanceVectors.clear();
    m_gameState.m_exitEffectInstanceVectors.clear();
}

void GJBaseGameLayer::resetAudio() {
    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    FMODAudioEngine::clearAllAudio(engine);
    engine->m_system->update();
}

void GJBaseGameLayer::resetCamera() {
    m_gameState.m_unkPoint18 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint17 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint19 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint21 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint20 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint22 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint23 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint10 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint5  = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint12 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkPoint13 = cocos2d::CCPoint(0.0f, 0.0f);

    m_gameState.m_unkBool28 = false;
    m_gameState.m_unkBool29 = false;
    m_gameState.m_unkFloat2 = 0.5f;
    m_gameState.m_unkBool8 = 0;
    m_gameState.m_unkUint13 = 0;
    m_gameState.m_unkFloat3 = 10.0f;

    updateZoom(0.0f, 0.0f, 0, 0.0f, -1, -1);
    updateCameraOffsetX(0.0f, 0.0f, 0, 0.0f, -1, -1);
    updateCameraOffsetY(0.0f, 0.0f, 0, 0.0f, -1, -1);
    restoreDefaultGameplayOffsetX();
    restoreDefaultGameplayOffsetY();
    updateInternalCamOffsetY(0.0f, 0.0f, 0.0f);
    updateInternalCamOffsetX(0.0f, 0.0f, 0.0f);

    this->m_gameState.m_cameraEdgeValue0 = 0;
    this->m_gameState.m_cameraEdgeValue1 = 0;
    this->m_gameState.m_cameraEdgeValue2 = 0;
    this->m_gameState.m_cameraEdgeValue3 = 0;

    updateScreenRotation(0.0f, false, false, 0.0f, -1, 0.0f, -1, -1);

    this->m_cameraWidthOffset = 0.0f;
    this->m_cameraHeightOffset = 0.0f;
    this->m_cameraUnzoomedHeightOffset = 0.0f;
    this->m_targetCameraHeightOffset = 0.0f;
    this->m_calculateTargetHeightOffset = true;

    m_gameState.m_cameraStepDiff = cocos2d::CCPoint(0.0f, 0.0f);

    resetStaticCamera(true, true);
    stopCameraShake();

    m_gameState.m_unkBool4 = 0;
    m_gameState.m_unkBool5 = 0;
    m_gameState.m_unkInt13 = 0;
    m_gameState.m_unkPoint14 = cocos2d::CCPoint(1.0f, 1.0f);
}

void GJBaseGameLayer::resetGradientLayers() {
    if (!m_gradientLayers) return;

    cocos2d::CCDictElement* element = m_gradientLayers->m_pElements;
    if (!element) return;

    while (element) {
        GJGradientLayer* layer = static_cast<GJGradientLayer*>(element->m_pObject);
        layer->removeFromParentAndCleanup(false);
        layer->m_triggerObject = 0;
        layer->m_gradientID = 0;
        layer->setVisible(false);
        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJBaseGameLayer::resetGroupCounters(bool reset) {
    for (auto& pair : m_labelObjects) {
        for (LabelGameObject* label : pair.second) {
            if (reset) {
                std::string value = "0";
                LabelGameObject::updateLabel(label, value);
            } else {
                LabelGameObject::updatePreviewLabel(label);
            }
        }
    }

    for (auto& pair : m_timeLabelObjects) {
        for (LabelGameObject* label : pair.second) {
            if (reset) {
                std::string value = label->m_showSecondsOnly ? "0" : "0.00";
                LabelGameObject::updateLabel(label, value);
            } else {
                LabelGameObject::updatePreviewLabel(label);
            }
        }
    }
}

void GJBaseGameLayer::resetLevel() {
}

void GJBaseGameLayer::resetMoveOptimizedValue() {
    if (!m_objects) return;

    cocos2d::ccArray* data = m_objects->data;
    for (unsigned int i = 0; i < data->num; ++i) {
        GameObject* obj = static_cast<GameObject*>(data->arr[i]);
        if (!obj) break;
        obj->m_unk3ee = obj->m_isGroupDisabled;
    }
}

void GJBaseGameLayer::resetPlayer() {
    cocos2d::CCPoint startPos(0.0f, 105.0f);

    if (m_startPosObject) {
        startPos = m_startPosObject->getPosition();
    } else {
        int spawnGroup = m_levelSettings->m_spawnGroup;
        if (spawnGroup > 0) {
            GameObject* spawnObj = tryGetObject(spawnGroup);
            if (spawnObj) {
                startPos = spawnObj->getRealPosition();
                if (!m_levelSettings->m_platformerMode) {
                    startPos.x = 0.0f;
                }
            }
        }
    }

    this->m_player1->setStartPos(startPos);
    this->m_player1->resetObject();

    this->m_player2->setStartPos(startPos);
    this->m_player2->resetObject();

    this->m_player1->m_decreaseBoostSlide = !this->m_levelSettings->m_decreaseBoostSlide;
    this->m_player2->m_decreaseBoostSlide = !this->m_levelSettings->m_decreaseBoostSlide;
}

void GJBaseGameLayer::resetSpawnChannelIndex() {
    m_gameState.m_currentChannel = 0;

    cocos2d::CCArray* keys = cocos2d::CCDictionary::allKeys(m_spawnObjects);
    unsigned int count = cocos2d::CCArray::count(keys);
    for (unsigned int i = 0; i < count; ++i) {
        cocos2d::CCString* key = static_cast<cocos2d::CCString*>(
            cocos2d::CCArray::objectAtIndex(keys, i));
        int channelID = key->intValue();
        m_gameState.m_spawnChannelRelated0[channelID] = 0;
        m_gameState.m_spawnChannelRelated1[channelID] = false;
    }
}

void GJBaseGameLayer::resetStaticCamera(bool resetX, bool resetY) {
    if (resetX) {
        GJGameState::stopTweenAction(&m_gameState, 10);
        GJGameState::stopTweenAction(&m_gameState, 12);
        m_gameState.m_unkPoint6.x  = 0.0f;
        m_gameState.m_unkPoint8.x  = 0.0f;
        m_gameState.m_unkPoint17.x = 0.0f;
        m_gameState.m_unkPoint18.x = 0.0f;
        m_gameState.m_unkPoint22.x = 0.0f;
    }

    if (resetY) {
        GJGameState::stopTweenAction(&m_gameState, 11);
        GJGameState::stopTweenAction(&m_gameState, 13);
        m_gameState.m_unkPoint6.y  = 0.0f;
        m_gameState.m_unkPoint8.y  = 0.0f;
        m_gameState.m_unkPoint17.y = 0.0f;
        m_gameState.m_unkPoint18.y = 0.0f;
        m_gameState.m_unkPoint22.y = 0.0f;
    }
}

void GJBaseGameLayer::updateExtraGameLayers() {
    this->m_inShaderObjectLayer->setPosition(this->m_objectLayer->getPosition());
    this->m_inShaderObjectLayer->setScaleX(this->m_objectLayer->getScaleX());
    this->m_inShaderObjectLayer->setScaleY(this->m_objectLayer->getScaleY());

    this->m_aboveShaderObjectLayer->setPosition(this->m_objectLayer->getPosition());
    this->m_aboveShaderObjectLayer->setScaleX(this->m_objectLayer->getScaleX());
    this->m_aboveShaderObjectLayer->setScaleY(this->m_objectLayer->getScaleY());
}

void GJBaseGameLayer::resetStoppedAreaObjects() {
    int index = 0;
    int i = 0;

    while (this->m_areaObjectsCount > i) {
        int commandIndex = this->m_gameState.m_commandIndex;
        GameObject* object = this->m_areaObjects.m_start[index];
        int lastCommand = object->m_unk4C8;

        object->m_isDirty = true;
        object->m_isObjectRectDirty = true;
        object->m_isOrientedBoxDirty = true;

        if (lastCommand < commandIndex) {
            int dirty = 0;

            if (object->m_positionXOffset != 0.0f) {
                object->m_isUnmodifiedPosDirty = true;
                float offset = object->m_positionXOffset;
                dirty = 1;
                object->m_positionXOffset = 0.0f;
                object->m_positionX = object->m_positionX - offset;
            }

            if (object->m_positionYOffset != 0.0f) {
                object->m_isUnmodifiedPosDirty = true;
                float offset = object->m_positionYOffset;
                dirty = 1;
                object->m_positionYOffset = 0.0f;
                object->m_positionY = object->m_positionY - offset;
            }

            float scaleXOffset = object->m_unk2BC;
            float scaleYOffset = object->m_unk2C0;

            if (scaleXOffset != 0.0f || scaleYOffset != 0.0f) {
                float newScaleX = object->m_scaleX - scaleXOffset;
                float newScaleYOffset = object->m_scaleYOffset - scaleYOffset;
                float newScaleY = object->m_scaleY - scaleYOffset;

                object->m_scaleXOffset = object->m_scaleXOffset - scaleXOffset;
                object->m_scaleX = newScaleX;
                object->m_unk2BC = 0.0f;
                object->m_scaleYOffset = newScaleYOffset;
                object->m_scaleY = newScaleY;
                object->m_unk2C0 = 0.0f;
                object->setRScale(1.0f);
            }

            float rotXOffset = object->m_unk2A8;
            float rotYOffset = object->m_unk2B0;

            if (rotXOffset != 0.0f || rotYOffset != 0.0f) {
                float newRotYOffset = object->m_rotationYOffset - rotYOffset;
                float newRotXOffset = object->m_rotationXOffset - rotXOffset;

                object->m_unk2A8 = 0.0f;
                object->m_rotationYOffset = newRotYOffset;
                object->m_rotationXOffset = newRotXOffset;
                object->m_unk2B0 = 0.0f;
                object->setRRotation(0.0f);
            }

            bool isDecoration = object->m_isDecoration2;
            object->m_unk4C8 = this->m_gameState.m_commandIndex;

            if (!isDecoration) {
                unsigned int cmdIdx = this->m_gameState.m_commandIndex;

                if (object->m_unk4C4 != cmdIdx) {
                    object->m_unk4C4 = cmdIdx;
                    object->m_lastPosition.x = object->m_positionX;
                    object->m_lastPosition.y = object->m_positionY;
                    GameObject::dirtifyObjectRect(object);
                    cmdIdx = this->m_gameState.m_commandIndex;
                }

                if (object->m_unk4CC != cmdIdx) {
                    object->m_unk4CC = cmdIdx;
                    object->m_customScaleX = object->m_scaleX;
                    object->m_customScaleY = object->m_scaleY;
                    GameObject::dirtifyObjectRect(object);
                }
            }

            if (dirty) {
                this->updateObjectSection(object);
            }
        }

        ++i;
        ++index;
        object->setRRotation(0.0f);
        object->setRScale(1.0f);
    }
}

void GJBaseGameLayer::triggerGradientCommand(GradientTriggerObject* object) {
    if (object->m_disableAll) {
        resetGradientLayers();
        return;
    }

    int gradientID = object->m_gradientID;
    if (gradientID > 999) gradientID = 999;

    GJGradientLayer* layer = static_cast<GJGradientLayer*>(
        cocos2d::CCDictionary::objectForKey(m_gradientLayers, gradientID));

    if (!layer) {
        layer = GJGradientLayer::create();
        layer->m_gradientID = gradientID;
        cocos2d::CCDictionary::setObject(m_gradientLayers, layer, gradientID);
    }

    if (object->m_disable) {
        layer->removeFromParentAndCleanup(false);
        layer->m_triggerObject = 0;
        return;
    }

    layer->m_triggerObject = reinterpret_cast<int>(object);
    layer->m_baseColor = object->getMainColorMode();
    layer->m_detailColor = object->getSecondaryColorMode();
    layer->m_blendingMode = object->m_blendingMode;

    cocos2d::ccBlendFunc blend;
    bool premultiply;

    int mode = object->m_blendingMode;
    if (mode == 2) {
        premultiply = true;
        blend.src = GL_ONE;
        blend.dst = GL_ONE;
    } else if (mode == 3) {
        premultiply = true;
        blend.src = GL_ONE;
        blend.dst = GL_ONE;
    } else {
        premultiply = false;
        if (mode == 1) {
            blend.src = GL_ONE;
            blend.dst = GL_ONE_MINUS_SRC_ALPHA;
        } else {
            blend.src = GL_SRC_ALPHA;
            blend.dst = GL_ONE;
        }
    }

    layer->setBlendFunc(blend);
    layer->m_blendingLayer = object->m_blendingLayer;
    layer->m_bShouldPremultiply = premultiply;

    int zOrderBase = maxZOrderForShaderZ(object->m_blendingLayer);

    cocos2d::CCNode* parent = nullptr;
    int blendingLayer = object->m_blendingLayer;

    if (blendingLayer == 2) {
        parent = m_middleground ? m_middleground->getParent() : m_background;
    } else if (blendingLayer == 3) {
        parent = m_specialLayerB5;
    } else if (blendingLayer == 4) {
        parent = m_specialLayerB4;
    } else if (blendingLayer == 5) {
        parent = m_specialLayerB3;
    } else if (blendingLayer == 6) {
        parent = m_specialLayerB2;
    } else if (blendingLayer == 7) {
        parent = m_specialLayerB1;
    } else if (blendingLayer == 8) {
        parent = m_player1;
    } else if (blendingLayer == 9) {
        parent = m_specialLayerT1;
    } else if (blendingLayer == 10) {
        parent = m_specialLayerT2;
    } else if (blendingLayer == 11) {
        parent = m_specialLayerT3;
    } else if (blendingLayer == 12) {
        parent = m_specialLayerT4;
    } else if (blendingLayer == 13) {
        parent = m_groundLayer;
    } else if (blendingLayer == 14) {
        parent = m_uiTriggerUI;
    } else if (blendingLayer == 15) {
        parent = m_aboveShaderParent;
    } else {
        parent = m_background;
    }

    if (blendingLayer == 1) {
        zOrderBase = -54;
    } else if (blendingLayer == 2) {
        zOrderBase = -14;
    } else if (blendingLayer == 13) {
        zOrderBase = 46;
    } else if (blendingLayer == 14) {
        zOrderBase = 56;
    }

    int objectZOrder = object->getObjectZOrder();
    if (objectZOrder < -5) objectZOrder = -5;
    if (objectZOrder > 5) objectZOrder = 5;

    layer->m_noScale = true;
    if (blendingLayer == 1 || blendingLayer == 2 || blendingLayer == 13 ||
        blendingLayer == 14 || blendingLayer == 15) {
        layer->m_noScale = false;
    }

    int finalZOrder = zOrderBase + objectZOrder;

    if (layer->getParent() == parent) {
        parent->reorderChild(layer, finalZOrder);
    } else {
        layer->removeFromParentAndCleanup(false);
        parent->addChild(layer, finalZOrder);
    }
}

void GJBaseGameLayer::updateGradientLayers() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float invScale = 1.0f / m_objectLayer->getScale();

    cocos2d::CCPoint layerPos = m_objectLayer->getPosition() * invScale;
    cocos2d::CCPoint negLayerPos = -layerPos;

    cocos2d::CCPoint halfWin(winSize.width * 0.5f, winSize.height * 0.5f);
    cocos2d::CCPoint winScaled = cocos2d::CCPoint(winSize.width, winSize.height) * invScale;
    cocos2d::CCPoint winHalf = winScaled * 0.5f;
    cocos2d::CCPoint center = negLayerPos + winHalf;

    float diagonal = sqrtf(winScaled.y * winScaled.y + winScaled.x * winScaled.x) * 0.5f;
    cocos2d::CCPoint minPoint = center - cocos2d::CCPoint(diagonal, diagonal);
    cocos2d::CCPoint maxPoint = center + cocos2d::CCPoint(diagonal, diagonal);

    cocos2d::CCPoint boundMin(0.0f, 0.0f);
    cocos2d::CCPoint boundMax(winSize.width, winSize.height);

    if (m_gameState.m_cameraAngle != 0.0f) {
        float expanded = sqrtf(winSize.height * winSize.height + winSize.width * winSize.width) * 0.5f;
        boundMin = cocos2d::CCPoint(
            -(expanded - winSize.width * 0.5f),
            -(expanded - winSize.height * 0.5f));
        boundMax = cocos2d::CCPoint(
            expanded + winSize.width * 0.5f,
            expanded + winSize.height * 0.5f);
    }

    cocos2d::CCPoint bgMin = negLayerPos;
    bgMin.x -= 20.0f;
    bgMin.y -= 20.0f;
    cocos2d::CCPoint bgMax = layerPos + winSize;
    bgMax.x += 20.0f;
    bgMax.y += 20.0f;

    cocos2d::CCPoint neg20(-20.0f, -20.0f);
    cocos2d::CCPoint winPlus20(winSize.width + 20.0f, winSize.height + 20.0f);

    m_activeGradients = 0;

    if (!m_gradientLayers) return;
    cocos2d::CCDictElement* element = m_gradientLayers->m_pElements;
    if (!element) return;

    while (element) {
        GJGradientLayer* layer = static_cast<GJGradientLayer*>(element->m_pObject);

        if (!layer->isVisible()) {
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            continue;
        }

        GameObject* trigger = reinterpret_cast<GameObject*>(layer->m_triggerObject);
        if (!trigger) {
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            continue;
        }

        if (trigger->m_uniqueID != trigger->m_uniqueID) {
            layer->removeFromParentAndCleanup(false);
            layer->m_triggerObject = 0;
            layer->m_gradientID = 0;
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            continue;
        }

        float opacity = opacityForObject(trigger);
        if (m_isEditor && m_playbackMode != PlaybackMode::Playing) {
            opacity *= trigger->m_opacityMod;
        }

        cocos2d::ccColor3B color1 = GJEffectManager::activeColorForIndex(
            m_effectManager, trigger->m_unk52C);
        cocos2d::ccColor3B color2 = GJEffectManager::activeColorForIndex(
            m_effectManager, trigger->m_unk52C);
        (void)color2;

        GJSpriteColor* baseColor = trigger->getMainColor();
        if (baseColor && (baseColor->m_hsv.h != 0.0f || baseColor->m_hsv.s != 0.0f ||
                          baseColor->m_hsv.v != 0.0f || baseColor->m_hsv.absoluteSaturation != 0)) {
            color1 = GameToolbox::transformColor(color1, baseColor->m_hsv);
        }

        GJSpriteColor* detailColor = trigger->getSecondaryColor();
        if (detailColor && (detailColor->m_hsv.h != 0.0f || detailColor->m_hsv.s != 0.0f ||
                            detailColor->m_hsv.v != 0.0f || detailColor->m_hsv.absoluteSaturation != 0)) {
            color2 = GameToolbox::transformColor(color2, detailColor->m_hsv);
        }

        float opacity1 = GJEffectManager::activeOpacityForIndex(
            m_effectManager, trigger->m_unk52C) * 255.0f * opacity;
        float opacity2 = GJEffectManager::activeOpacityForIndex(
            m_effectManager, trigger->m_unk52C) * 255.0f * opacity;

        if (opacity1 == 0.0f && opacity2 == 0.0f) {
            layer->setVisible(false);
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            continue;
        }

        if (!color1.r && !color1.g && !color1.b &&
            !color2.r && !color2.g && !color2.b) {
            layer->setVisible(false);
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            continue;
        }

        float angle = -trigger->getRotation() * 0.017453f;
        cocos2d::CCPoint direction(cosf(angle), sinf(angle));
        layer->m_AlongVector = direction;

        GameObject* topObj = tryGetMainObject(trigger->m_unk704);
        GameObject* botObj = tryGetMainObject(trigger->m_unk708);
        GameObject* leftObj = tryGetMainObject(trigger->m_unk70C);
        GameObject* rightObj = tryGetMainObject(trigger->m_unk710);

        bool wasHidden = layer->isVisible();

        cocos2d::CCPoint v1(0.0f, 0.0f);
        cocos2d::CCPoint v2(0.0f, 0.0f);
        cocos2d::CCPoint v3(0.0f, 0.0f);
        cocos2d::CCPoint v4(0.0f, 0.0f);

        cocos2d::CCPoint p1(0.0f, 0.0f);
        cocos2d::CCPoint p2(0.0f, 0.0f);

        if (!wasHidden) {
            v1 = boundMin;
            v2 = boundMax;
            p1 = cocos2d::CCPoint(v1.x, v2.y);
            p2 = cocos2d::CCPoint(v2.x, v1.y);

            if (topObj) v1 = topObj->getRealPosition();
            if (rightObj) v2 = rightObj->getRealPosition();
            if (leftObj) p1 = leftObj->getRealPosition();
            if (botObj) p2 = botObj->getRealPosition();
        } else {
            v1 = neg20;
            v2 = winPlus20;
            p1 = cocos2d::CCPoint(neg20.x, winPlus20.y);
            p2 = cocos2d::CCPoint(winPlus20.x, neg20.y);

            if (topObj) v1 = topObj->getRealPosition();
            if (rightObj) v2 = rightObj->getRealPosition();
            if (leftObj) p1 = leftObj->getRealPosition();
            if (botObj) p2 = botObj->getRealPosition();
        }

        (void)v3;
        (void)v4;

        layer->setPosition(v1);
        layer->setScaleX((v2.x - v1.x) / winSize.width);
        layer->setScaleY((v2.y - v1.y) / winSize.height);

        cocos2d::CCPoint verts[4];
        verts[0] = v1;
        verts[1] = p1;
        verts[2] = v2;
        verts[3] = p2;

        layer->setVertices(verts[0], verts[1], verts[2], verts[3]);

        float minX = std::min(std::min(v1.x, v2.x), std::min(p1.x, p2.x));
        float maxX = std::max(std::max(v1.x, v2.x), std::max(p1.x, p2.x));
        float minY = std::min(std::min(v1.y, v2.y), std::min(p1.y, p2.y));
        float maxY = std::max(std::max(v1.y, v2.y), std::max(p1.y, p2.y));

        if (maxX > bgMax.x || minX < bgMin.x || maxY > bgMax.y || minY < bgMin.y) {
            layer->setVisible(false);
            layer->setColor(color1);
            layer->setOpacity(opacity1);
            element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
            continue;
        }

        layer->setColor(color1);
        layer->setOpacity(opacity1);
        (void)opacity2;
        layer->setVisible(true);
        m_activeGradients++;

        element = static_cast<cocos2d::CCDictElement*>(element->hh.next);
    }
}

void GJBaseGameLayer::updateParticles(float dt) {
    unsigned int i = 0;
    while (i < (unsigned int)cocos2d::CCArray::count(m_temporaryParticles)) {
        cocos2d::CCParticleSystemQuad* particle =
            static_cast<cocos2d::CCParticleSystemQuad*>(
                cocos2d::CCArray::objectAtIndex(m_temporaryParticles, i));

        if (!particle->isAutoRemoveOnFinish()) {
            i++;
            int count = particle->getTotalParticles();
            m_particleCount += count;
            particle->update(dt);
            continue;
        }

        unsigned int uid = particle->getID();
        m_customParticlesUIDs.erase(uid);
        cocos2d::CCArray::fastRemoveObjectAtIndex(m_temporaryParticles, i);
    }

    unsigned int j = 0;
    while (j < (unsigned int)cocos2d::CCArray::count(m_unclaimedParticles)) {
        cocos2d::CCParticleSystemQuad* particle =
            static_cast<cocos2d::CCParticleSystemQuad*>(
                cocos2d::CCArray::objectAtIndex(m_unclaimedParticles, j));

        if (!particle->isFull()) {
            j++;
            continue;
        }

        int tag = particle->getTag();
        std::string key = m_particleCountToParticleString[tag];
        cocos2d::CCArray* customArray = static_cast<cocos2d::CCArray*>(
            cocos2d::CCDictionary::objectForKey(m_customParticles, &key));
        if (customArray) {
            cocos2d::CCArray::addObject(customArray, particle);
        }
        cocos2d::CCArray::fastRemoveObjectAtIndex(m_unclaimedParticles, j);
    }
}

void GJBaseGameLayer::updateTimerLabels() {
    for (auto it = m_timeLabelObjects.begin(); it != m_timeLabelObjects.end(); ++it) {
        int itemID = it.first;
        std::vector<LabelGameObject*>& labels = it.second;

        double timeValue = GJEffectManager::timeForItem(m_effectManager, itemID);
        double absValue = fabs(timeValue);
        float absFloat = (float)absValue;

        int seconds = (int)absFloat;
        int centiseconds = (int)llroundf((absFloat - seconds) * 100.0f);
        if (centiseconds == 100) {
            seconds++;
            centiseconds = 0;
        }

        std::string secondsStr = GameToolbox::intToString(seconds);

        std::string fullStr;
        if (timeValue < 0.0) {
            fullStr += "-";
        }
        fullStr += std::to_string(seconds);
        fullStr += ".";
        if (centiseconds == 0) {
            fullStr += "00";
        } else if (centiseconds <= 9) {
            fullStr += "0";
            fullStr += std::to_string(centiseconds);
        } else {
            fullStr += std::to_string(centiseconds);
        }

        for (LabelGameObject* label : labels) {
            const std::string& text = label->m_showSecondsOnly ? secondsStr : fullStr;
            LabelGameObject::queueUpdateLabel(label, text);
        }
    }
}

void GJBaseGameLayer::queueButton(int button, bool push, bool isPlayer2, double timestamp) {
    if (!button) return;

    if (m_queuedButtons.size() < m_queuedButtons.capacity()) {
        PlayerButtonCommand& cmd = m_queuedButtons.emplace_back();
        cmd.m_button = button;
        cmd.m_isPush = push;
        cmd.m_isPlayer2 = isPlayer2;
        cmd.m_step = 0;
        cmd.m_timestamp = timestamp;
        return;
    }

    PlayerButtonCommand cmd;
    cmd.m_button = button;
    cmd.m_isPush = push;
    cmd.m_isPlayer2 = isPlayer2;
    cmd.m_step = 0;
    cmd.m_timestamp = timestamp;
    m_queuedButtons.push_back(cmd);
}

void GJBaseGameLayer::recordAction(int button, bool down, bool player2) {
    if (!m_recordInputs || m_useReplay) return;
    if (m_queuedRecordedButtons.size() > 0x1869F) return;

    RecordButtonCommand cmd;
    cmd.m_button = button;
    cmd.m_isPush = down;
    cmd.m_isPlayer2 = player2;
    cmd.m_step = 0;
    m_queuedRecordedButtons.push_back(cmd);
}

int GJBaseGameLayer::registerSpawnRemap(std::vector<ChanceObject>& spawnRemap) {
    std::unordered_map<int, int> remap;
    m_spawnRemapTriggers.push_back(remap);

    auto& target = m_spawnRemapTriggers.back();
    for (auto& chance : spawnRemap) {
        target[chance.m_groupID] = chance.m_chance;
    }

    return (int)m_spawnRemapTriggers.size() - 1;
}

static int compKeyframeID(GameObject* a, GameObject* b) {
    if (a->m_keyframeGroup < b->m_keyframeGroup) return -1;
    if (a->m_keyframeGroup > b->m_keyframeGroup) return 1;
    return 0;
}

static int compShadowCopy(GameObject* a, GameObject* b) {
    if (a->m_keyframeIndex < b->m_keyframeIndex) return -1;
    if (a->m_keyframeIndex > b->m_keyframeIndex) return 1;
    return 0;
}

void GJBaseGameLayer::refreshKeyframeAnims() {
    cocos2d::CCDictionary::removeAllObjects(m_keyframeGroups);
    m_keyframeGroup = 0;

    cocos2d::CCDictionary* groups = cocos2d::CCDictionary::create();

    if (m_objects) {
        cocos2d::ccArray* data = m_objects->data;
        for (unsigned int i = 0; i < data->num; ++i) {
            GameObject* obj = static_cast<GameObject*>(data->arr[i]);
            if (!obj) break;
            if (obj->m_objectID != 3032) continue;

            cocos2d::CCArray* array = static_cast<cocos2d::CCArray*>(
                cocos2d::CCDictionary::objectForKey(groups, obj->m_keyframeGroup));
            if (!array) {
                array = cocos2d::CCArray::create();
                array->setTag(obj->m_keyframeGroup);
                cocos2d::CCDictionary::setObject(groups, array, obj->m_keyframeGroup);
            }
            cocos2d::CCArray::addObject(array, obj);
        }
    }

    cocos2d::CCArray* keys = cocos2d::CCDictionary::allKeys(groups);
    qsort(keys->data->arr, keys->data->num, 8u,
          (int(*)(const void*, const void*))compKeyframeID);

    for (unsigned int i = 0; i < cocos2d::CCArray::count(keys); ++i) {
        cocos2d::CCString* keyStr = static_cast<cocos2d::CCString*>(
            cocos2d::CCArray::objectAtIndex(keys, i));
        int groupID = keyStr->intValue();

        cocos2d::CCArray* array = static_cast<cocos2d::CCArray*>(
            cocos2d::CCDictionary::objectForKey(groups, groupID));

        qsort(array->data->arr, array->data->num, 8u,
              (int(*)(const void*, const void*))compShadowCopy);

        array->setTag(m_keyframeGroup);
        cocos2d::CCDictionary::setObject(m_keyframeGroups, array, m_keyframeGroup);

        for (unsigned int j = 0; j < cocos2d::CCArray::count(array); ++j) {
            GameObject* obj = static_cast<GameObject*>(cocos2d::CCArray::objectAtIndex(array, j));
            obj->m_keyframeGroup = m_keyframeGroup;
            obj->m_keyframeIndex = j;
        }

        ++m_keyframeGroup;
    }
}

void GJBaseGameLayer::resetSongTriggerValues() {
    m_gameState.m_proximityVolumeRelated.clear();
    m_gameState.m_levelTime = 0.0f;
}

void GJBaseGameLayer::resetLevelVariables() {
    m_gameState.m_pauseCounter = 0;
    m_gameState.m_background = m_levelSettings->m_backgroundIndex;
    m_gameState.m_ground = m_levelSettings->m_groundIndex;
    m_gameState.m_middleground = m_levelSettings->m_middleGroundIndex;
    m_gameState.m_unkBool31 = false;
    m_gameState.m_unkBool30 = false;

    m_gameState.m_unkMapPairGJGameEventIntInt.clear();
    m_allowedButtons.clear();

    resetSPTriggered();
    unlinkAllEvents();

    GameManager* gm = GameManager::sharedState();
    gm->printGJLog();
    gm->clearGJLog();

    FMODAudioEngine::sharedEngine()->updateReverb(Generic, 0);

    updateBGArtSpeed(0.1f, 0.1f);
    updateMGArtSpeed(0.3f, 0.5f);

    m_startingFromBeginning = false;
    m_activeSfxTriggers.clear();
    clearActivatedAudioTriggers();
    removeTemporaryParticles();

    m_gameState.m_points = 0;
    m_gameState.m_unkBool32 = false;
    updateSpecialLabels();

    m_levelEndAnimationStarted = false;

    if (m_levelSettings) {
        m_effectManager->m_unk798 = !m_levelSettings->m_allowMultiRotation;
        m_enable22Changes = m_levelSettings->m_enable22Changes;

        if (m_level && m_level->m_levelType == GJLevelType::Main &&
            (m_isPlatformer || m_level->m_levelID.random - m_level->m_levelID.seed == 22)) {
            m_enable22Changes = true;
        }

        m_allowStaticRotate = m_levelSettings->m_allowStaticRotate;
        m_fixNegativeScale = m_levelSettings->m_fixNegativeScale;

        bool disableSqueeze = !(m_levelSettings->m_enablePlayerSqueeze || m_isPlatformer);
        m_player1->m_disablePlayerSqueeze = disableSqueeze;
        m_player2->m_disablePlayerSqueeze = disableSqueeze;

        m_player1->m_fixGravityBug = m_levelSettings->m_fixGravityBug;
        m_player2->m_fixGravityBug = m_levelSettings->m_fixGravityBug;

        m_player1->m_reverseSync = m_levelSettings->m_reverseSync;
        m_player2->m_reverseSync = m_levelSettings->m_reverseSync;

        m_player1->m_enable22Changes = !m_enable22Changes;
        m_player2->m_enable22Changes = !m_enable22Changes;

        m_player1->m_enableImpulseFix = m_levelSettings->m_enableImpulseFix;
        m_player2->m_enableImpulseFix = m_levelSettings->m_enableImpulseFix;
    }

    resetActiveEnterEffects();
    resetSongTriggerValues();

    m_gameState.m_gameObjectPhysics.clear();
    m_gameState.m_stateObjects.clear();

    m_gameState.m_commandIndex = 0;
    m_gameState.m_currentProgress = 0;
    m_gameState.m_unkUint11 = 0;
    m_gameState.m_unkUint5 = 0;
    m_gameState.m_unkUint6 = 0;

    if (!m_isPlatformer) {
        m_queuedButtons.clear();
    }

    m_player1->releaseAllButtons();
    m_player2->releaseAllButtons();

    m_extraDelta = 0.0f;
    m_unk3251 = true;
    m_dualTouchTrigger = false;

    GJEffectManager::resetEffects(m_effectManager);
    m_gameState.m_activatedObjectIDs.clear();

    if (m_shaderLayer) {
        m_shaderLayer->resetAllShaders();
        updateShaderLayer(0.0f);
    }

    resetGradientLayers();

    m_gameState.m_moveEffectInstances.clear();
    m_gameState.m_rotateEffectInstances.clear();
    m_gameState.m_scaleEffectInstances.clear();
    m_gameState.m_fadeEffectInstances.clear();
    m_gameState.m_tintEffectInstances.clear();
    m_gameState.m_unsortedAreaEffects.clear();

    m_gameState.m_dynamicMoveActions.clear();
    m_gameState.m_dynamicRotateActions.clear();
    m_gameState.m_advanceFollowInstances.clear();

    updateTimeWarp(1.0f);
    m_skipArtReload = true;

    updateMGOffsetY(0.0f, 0.0f, 0, 0.0f, -1, -1);
    m_background->setPosition(cocos2d::CCPoint(m_background->getPosition().x, 0.0f));

    resetCamera();

    m_groundLayer->m_unk1cc = 0.0f;
    m_groundLayer2->m_unk1cc = 0.0f;

    float scaleX = m_groundLayer->getScaleX();
    m_groundLayer->scaleGround(scaleX);
    m_groundLayer2->scaleGround(m_groundLayer2->getScaleX());

    m_groundLayer->updateShadowXPos(0.0f, 0.0f);
    m_groundLayer->updateShadows();
    m_groundLayer2->updateShadowXPos(0.0f, 0.0f);
    m_groundLayer2->updateShadows();

    togglePlayerStreakBlend(true, false);

    m_gameState.m_timeModRelated = 0.0f;
    m_gameState.m_unkUint64_1 = 0.0f;
    m_gameState.m_unkPoint34 = cocos2d::CCPoint(0.0f, 0.0f);
    m_gameState.m_unkUint16 = 0.0f;

    m_groundLayer->toggleVisible02(true);
    m_groundLayer2->toggleVisible02(true);

    m_gameState.m_unkFloat9 = 0.0f;
    m_gameState.stopTweenAction(25);
    m_gameState.m_unkBool1 = false;
    m_gameState.m_portalY = 0.0f;
    m_gameState.m_unkBool6 = false;
    m_gameState.m_unkInt12 = 0;
    m_unk322a = m_freezeStartCamera;
    m_playerDied = false;

    clearPickedUpItems();

    m_gameState.m_unkBool10 = false;
    m_gameState.m_unkBool11 = false;
    m_resetActiveObjects = false;
    m_gameState.m_levelFlipping = 0.0f;
    m_cameraFlip = 1.0f;

    stopActionByTag(14);

    m_gameState.m_lastActivatedPortal1 = nullptr;
    m_gameState.m_lastActivatedPortal2 = nullptr;

    resetPlayer();
    resetSpawnChannelIndex();

    m_flashNode->stopAllActions();
    m_flashNode->setVisible(false);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_gameState.m_cameraPosition.y = m_player1->getPosition()->y + 90.0f;

    if (m_objectsDeactivated) {
        for (int x = m_leftSectionIndex; x <= m_rightSectionIndex && x < (int)m_sections.size(); x++) {
            if (!m_sections[x]) continue;
            for (int y = m_bottomSectionIndex; y <= m_topSectionIndex && y < (int)m_sections[x]->size(); y++) {
                std::vector<GameObject*>* section = (*m_sections[x])[y];
                if (!section) continue;
                for (GameObject* obj : *section) {
                    obj->deactivateObject(true);
                }
            }
        }
    }

    toggleDualMode(nullptr, false, nullptr, false);
}

void GJBaseGameLayer::setGroupParent(GameObject* object, int groupID) {
    if (groupID <= 0 || !object) return;

    GameObject* currentParent = getGroupParent(groupID);
    if (object == currentParent || (currentParent && m_keepGroupParents)) return;

    removeGroupParent(groupID);
    m_parentGroupsDict->setObject(object, groupID);

    cocos2d::CCArray* parents = static_cast<cocos2d::CCArray*>(
        m_parentGroupIDs->objectForKey(object->m_uniqueID));
    if (!parents) {
        parents = cocos2d::CCArray::create();
        m_parentGroupIDs->setObject(parents, object->m_uniqueID);
    }

    for (unsigned int i = 0; i < cocos2d::CCArray::count(parents); ++i) {
        if (static_cast<int>(reinterpret_cast<uintptr_t>(
                cocos2d::CCArray::objectAtIndex(parents, i)->__vftable)) == groupID) {
            return;
        }
    }

    GroupParentEntry* entry = new GroupParentEntry();
    entry->m_groupID = groupID;
    cocos2d::CCObject::autorelease(entry);
    cocos2d::CCArray::addObject(parents, entry);

    object->m_hasGroupParentsString = true;
}

void GJBaseGameLayer::update(float dt) {
    tryResumeAudio();

    if (!m_loadingProgress || !m_started) {
        applyTimeWarp(m_gameState.m_timeWarp);
        m_gameState.m_totalTime += dt;
        m_gameState.m_unkDouble3 += dt / m_gameState.m_timeWarp;

        GJEffectManager::updateEffects(m_effectManager, dt);
        m_player1->updateEffects(dt);
        m_player2->updateEffects(dt);

        updateGradientLayers();
        updateShaderLayer(dt);
        postUpdate(dt);
        updateGuideArt();
        updateAudioVisualizer();

        m_timestamp = cocos2d::JniHelper::getPlatformTimestamp();
        return;
    }

    double modifiedDt = getModifiedDelta(dt);
    if (modifiedDt <= 0.0) return;
    dt = (float)modifiedDt;

    m_player1->m_maybeIsColliding = false;
    m_player2->m_maybeIsColliding = false;

    if (!m_player1->m_isLocked) m_player1->setPosition(m_player1->m_position);
    if (m_gameState.m_isDualMode && !m_player2->m_isLocked)
        m_player2->setPosition(m_player2->m_position);

    float timeWarp = m_gameState.m_timeWarp;
    float timeWarpClamped = std::min(timeWarp, 1.0f);
    float speed = timeWarpClamped * 60.0f * 4.0f;
    int steps = std::max(1, (int)roundf(speed));
    float stepDt = dt / steps;

    for (int step = 0; step < steps; step++) {
        bool isHalfTick = false;
        bool isLastTick = (step == steps - 1);

        m_allowedButtons.clear();
        m_isBetweenSteps = false;

        if (shouldUseSubstepForButton(stepDt)) {
            m_isBetweenSteps = true;
        }

        processCommands(stepDt, isHalfTick, isLastTick);

        m_player1->resetTouchedRings(false);
        if (m_gameState.m_isDualMode) m_player2->resetTouchedRings(false);

        m_player1->resetCollisionLog(false);
        if (m_gameState.m_isDualMode) m_player2->resetCollisionLog(false);

        GJEffectManager::preCollisionCheck(m_effectManager);
        m_spawnTuples.clear();

        float deltaX = m_player1->getPosition()->x - m_player1->getLastPosition()->x;
        float deltaY = m_player1->getPosition()->y - m_player1->getLastPosition()->y;
        m_effectManager->m_unk780 = deltaX;
        m_effectManager->m_unk784 = deltaY;
        m_effectManager->m_unk788 = -m_gameState.m_cameraStepDiff.x;
        m_effectManager->m_unk78C = -m_gameState.m_cameraStepDiff.y;
        m_effectManager->m_unk790 = m_player1->getRotation() - m_player1->m_unkUnused3;
        m_effectManager->m_unk794 = m_player2->getRotation() - m_player2->m_unkUnused3;

        GJEffectManager::updateTimers(m_effectManager, stepDt, m_gameState.m_timeWarp);
        GJEffectManager::prepareMoveActions(m_effectManager, stepDt, false);
        processMoveActionsStep(stepDt, isLastTick);
        GJEffectManager::postMoveActions(m_effectManager);

        updateVerifyDamage();

        m_ignoreDamage = true;
        int collisionResult = checkCollisions(m_player1, stepDt, m_ignoreDamage);
        if (collisionResult == 1 && m_varianceValues[1975 + 268] && !m_ignoreDamage)
            playerTookDamage(m_player1);

        if (m_gameState.m_isDualMode) {
            m_player2->updateInternalActions(stepDt);
            collisionResult = checkCollisions(m_player2, stepDt, m_ignoreDamage);
            if (collisionResult == 1 && m_varianceValues[1975 + 268] && !m_ignoreDamage)
                playerTookDamage(m_player2);
        }

        m_gameState.m_levelTime += stepDt / m_gameState.m_timeWarp;

        if (isHalfTick) checkSpawnObjects();

        m_player1->m_unkUnused3 = m_player1->getRotation();
        m_player2->m_unkUnused3 = m_player2->getRotation();

        m_player1->updateRotation(stepDt);
        if (m_gameState.m_isDualMode) m_player2->updateRotation(stepDt);

        m_player1->m_shipRotation = m_player1->getPosition();
        if (m_gameState.m_isDualMode) m_player2->m_shipRotation = m_player2->getPosition();

        updateSavePositionObjects();
        m_gameState.m_unkUint17 = getMaxPortalY();
        checkForEnd();

        m_timestamp += stepDt;

        if (isHalfTick) {
            GJEffectManager::postCollisionCheck(m_effectManager);
            checkRepellPlayer();
            processStateObjects();
            m_gameState.processStateTriggers();
        }

        m_gameState.updateTweenActions(stepDt);
        updateCamera(stepDt);
        checkSnapshot();
        processCheckpoints();
    }

    if (m_gameState.m_queuedTimeWarp > 0.0f) {
        updateTimeWarp(m_gameState.m_queuedTimeWarp);
        applyTimeWarp(m_gameState.m_timeWarp);
    }

    if (m_isPlatformer) updateSpecialLabels();
    updateTimerLabels();
    updateQueuedLabels();

    if (!m_player1->m_isLocked) m_player1->m_position = m_player1->getPosition();
    if (m_gameState.m_isDualMode && !m_player2->m_isLocked)
        m_player2->m_position = m_player2->getPosition();

    float levelFlipping = m_gameState.m_levelFlipping;
    if (levelFlipping != 0.0f && levelFlipping != 1.0f) {
        cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        float flipX = (m_player1->getPosition()->x / m_gameState.m_cameraZoom -
                      (m_player1->getPosition()->x - m_gameState.m_cameraPosition.x) * 2.0f) * levelFlipping;
        cocos2d::CCPoint pos = m_player1->getPosition();
        m_player1->setPosition(cocos2d::CCPoint(pos.x + flipX, pos.y));
        if (m_gameState.m_isDualMode) {
            pos = m_player2->getPosition();
            m_player2->setPosition(cocos2d::CCPoint(pos.x + flipX, pos.y));
        }
    }

    float shakeFactor = m_gameState.m_cameraShakeFactor;
    m_gameState.m_cameraShakeEnabled = shakeFactor > 0.0f;
    if (shakeFactor > 0.0f) m_gameState.m_cameraShakeFactor = shakeFactor - dt;

    GJEffectManager::updateEffects(m_effectManager, dt);
    m_player1->updateEffects(dt);
    m_player2->updateEffects(dt);
    updateGradientLayers();
    updateShaderLayer(dt);
    postUpdate(dt);
    updateGuideArt();
    updateProximityVolumeEffects();
    updateAudioVisualizer();

    if (!isFlipping() && m_gameState.m_unkBool12) flipFinished();

    m_songTriggerInterval += dt;
    if (m_songTriggerInterval >= 1.0f) {
        m_songTriggerInterval = 0.0f;
        for (auto it = m_gameState.m_songTriggerStateVectors.begin();
             it != m_gameState.m_songTriggerStateVectors.end();) {
            if (!FMODAudioEngine::sharedEngine()->isMusicPlaying(it->first))
                it = m_gameState.m_songTriggerStateVectors.erase(it);
            else ++it;
        }
    }

    m_timestamp = cocos2d::JniHelper::getPlatformTimestamp();
}

void GJBaseGameLayer::updateAudioVisualizer() {
    if (!m_showAudioVisualizer) return;

    FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
    float elapsed = engine->m_audioState.m_elapsed;

    m_audioVisualizerBG->updateVisualizer(
        engine->m_musicVisualizerVolume,
        engine->m_musicVisualizerPeak,
        elapsed - engine->m_musicVisualizerTime);

    m_audioVisualizerSFX->updateVisualizer(
        engine->m_sfxVisualizerVolume,
        engine->m_sfxVisualizerPeak,
        elapsed - engine->m_sfxVisualizerTime);
}

void GJBaseGameLayer::updateCameraBGArt(cocos2d::CCPoint position, float zoom) {
    bool isEditorMode = m_isEditor;
    bool isNotPlaying = m_isEditor && !m_started;
    int editorFlag = isEditorMode ? (isNotPlaying ? 1 : 0) : 0;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    cocos2d::CCPoint halfSize = cocos2d::CCPoint(
        (winSize.width / m_gameState.m_cameraZoom) * 0.5f,
        (winSize.height / m_gameState.m_cameraZoom) * 0.5f);

    cocos2d::CCPoint center = position + halfSize;

    cocos2d::CCPoint targetHalfSize = cocos2d::CCPoint(
        (winSize.width / m_gameState.m_unkFloat4) * 0.5f,
        (winSize.height / m_gameState.m_unkFloat4) * 0.5f);

    cocos2d::CCPoint targetCenter = m_gameState.m_cameraPosition2 + targetHalfSize;
    cocos2d::CCPoint delta = targetCenter - center;

    if (m_objectsDeactivated) delta = cocos2d::CCPoint(0.0f, 0.0f);

    float levelFlip = m_gameState.m_levelFlipping;
    float flipFactor = (levelFlip > 0.5f) ? (levelFlip - 0.5f) * -2.0f : 1.0f - levelFlip * 2.0f;

    cocos2d::CCPoint bgPos = m_background->getPosition();
    float newBgX = bgPos.x + delta.x * m_gameState.m_unkFloat5 * flipFactor;
    float newBgY = bgPos.y + delta.y * m_gameState.m_unkFloat6;
    cocos2d::CCPoint newPos(newBgX, newBgY);

    float bgWidth = m_gameState.m_unkFloat10;
    if (bgWidth != 0.0f) {
        newPos.x = fmodf(newPos.x, bgWidth);
        if (newPos.x > 0.0f) newPos.x -= bgWidth;
    }
    m_background->setPosition(newPos);

    if (m_middleground) {
        float mgScale = m_middleground->m_groundScale;
        float mgScaleTarget;
        if (editorFlag) {
            mgScaleTarget = m_objectLayer->getScaleY() * 1.2f;
        } else {
            float factor = 1.0f - (1.0f - fabsf(m_objectLayer->getScaleY())) * 0.3f;
            mgScaleTarget = factor * 1.2f;
        }

        if (mgScale != mgScaleTarget) {
            mgScale = m_middleground->scaleGround(mgScaleTarget) * 0.5f;
        }

        float mgX = delta.x * m_gameState.m_unkFloat7 * flipFactor + mgScale;
        float mgY = m_objectLayer->getPosition().y +
                    (m_middleground->m_ground2Offset + m_gameState.m_middleGroundOffsetY) * mgScale;

        if (m_gameState.m_unkFloat4 != zoom) {
            mgX = (halfSize.x / mgScale - halfSize.x / mgScaleTarget) * 0.5f;
        }

        m_middleground->setPosition(cocos2d::CCPoint(0.0f, mgY));

        float groundX = m_middleground->m_ground1Sprite->getPosition().x + mgX;
        float texWidth = m_middleground->m_textureWidth;
        while (groundX < -texWidth) groundX += texWidth;
        while (groundX > 0.0f) groundX -= texWidth;

        if (m_gameState.m_cameraAngle != 0.0f) groundX -= texWidth;

        m_middleground->updateGroundPos(cocos2d::CCPoint(
            groundX, m_middleground->m_ground1Sprite->getPosition().y));
    }

    float groundScale = 0.0f;
    if (m_gameState.m_unkFloat4 != zoom) {
        groundScale = m_groundLayer->scaleGround(zoom);
        m_groundLayer2->scaleGround(zoom);
    }
    groundScale *= 0.5f;

    float groundX = m_groundLayer->m_ground1Sprite->getPosition().x + delta.x * flipFactor + groundScale;
    float texWidth = m_groundLayer->m_textureWidth;
    while (groundX < -texWidth) groundX += texWidth;
    while (groundX > 0.0f) groundX -= texWidth;

    if (m_gameState.m_cameraAngle != 0.0f) groundX -= texWidth;

    m_groundLayer->updateGroundPos(cocos2d::CCPoint(
        groundX, m_groundLayer->m_ground1Sprite->getPosition().y));
    m_groundLayer2->updateGroundPos(cocos2d::CCPoint(
        groundX, m_groundLayer2->m_ground1Sprite->getPosition().y));

    float halfHeightOffset = m_cameraUnzoomedHeightOffset * zoom * 0.5f;
    float screenHeight = winSize.height + halfHeightOffset;

    float yPos = m_objectLayer->getPosition().y + zoom * 91.0f;
    float minY = -(halfHeightOffset - (halfHeightOffset + screenHeight * 0.5f + 1.0f) * m_gameState.m_unkFloat9);
    if (yPos < minY) yPos = minY;

    m_groundLayer->setPosition(cocos2d::CCPoint(0.0f, yPos));
    m_groundLayer2->setPosition(cocos2d::CCPoint(0.0f,
        screenHeight - m_gameState.m_unkFloat9 * (screenHeight - (screenHeight * 0.5f + 1.0f))));

    bool showGround1 = !m_gameState.m_hideGround && -halfHeightOffset < m_groundLayer->getPosition().y;
    m_groundLayer->toggleVisible01(showGround1);

    bool showGround2 = !m_gameState.m_hideGround && screenHeight > m_groundLayer2->getPosition().y;
    m_groundLayer2->toggleVisible01(showGround2);
}

void GJBaseGameLayer::updateCamera(float dt) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float targetWidth = winSize.width / m_gameState.m_unkFloat4;
    float width = winSize.width / m_gameState.m_cameraZoom;
    float height = winSize.height / m_gameState.m_cameraZoom;

    float minX = -99999.0f, maxX = -99999.0f, minY = -99999.0f, maxY = -99999.0f;

    if (m_gameState.m_cameraEdgeValue1 > 0) maxX = getCameraEdgeValue(2) - width;
    if (m_gameState.m_cameraEdgeValue0 > 0) minX = getCameraEdgeValue(1);
    if (m_gameState.m_cameraEdgeValue2 > 0) minY = getCameraEdgeValue(3) - height;
    if (m_gameState.m_cameraEdgeValue3 > 0) maxY = getCameraEdgeValue(4);

    if (m_endPortal) {
        float limit = m_levelLength - width - m_gameState.m_cameraOffset.x;
        if (minX == -99999.0f || minX > limit) minX = limit;
    }

    if ((m_levelSettings->m_propertykA23 || m_unk322a || m_isTestMode) &&
        !m_levelSettings->m_propertykA24 && maxX <= 15.0f) {
        maxX = 15.0f;
    }

    bool isSideways = m_player1->m_isSideways;
    bool isGoingLeft = m_player1->m_isGoingLeft;
    bool isUpsideDown = m_player1->m_isUpsideDown;
    bool isFlying = PlayerObject::isFlying(m_player1);

    float smoothX = 1.0f, smoothY = 1.0f;
    if (dt > 0.0f && !m_objectsDeactivated) {
        smoothX = m_gameState.m_unkBool28 ? 1.0f : (m_gameState.m_unkPoint18.x != 0.0f ? 1.0f : 0.0f);
        smoothY = m_gameState.m_unkBool29 ? 1.0f : (m_gameState.m_unkPoint18.y != 0.0f ? 1.0f : 0.0f);
    }

    if (dt > 0.0f && m_gameState.m_unkBool20 != isSideways) {
        if (m_gameState.m_unkPoint17.x == 0.0f && smoothX == 0.0f) {
            m_gameState.m_unkPoint18.x = 1.0f;
            m_gameState.m_unkPoint19.x = 1.0f;
            m_gameState.m_unkPoint21.x = 1.0f;
            m_gameState.m_unkPoint20.x = 2.0f;
            m_gameState.m_unkPoint22.x = 0.0f;
        }
        if (m_gameState.m_unkPoint17.y == 0.0f && smoothY == 0.0f) {
            m_gameState.m_unkPoint18.y = 1.0f;
            m_gameState.m_unkPoint19.y = 1.0f;
            m_gameState.m_unkPoint21.y = 1.0f;
            m_gameState.m_unkPoint20.y = 2.0f;
            m_gameState.m_unkPoint22.y = 0.0f;
        }
    }

    if (m_gameState.m_unkBool28) m_gameState.m_unkBool28 = false;
    if (m_gameState.m_unkBool29) m_gameState.m_unkBool29 = false;

    float dirX = isGoingLeft ? -1.0f : 1.0f;
    float dirY = isSideways ? (isGoingLeft ? -1.0f : 1.0f) : (isUpsideDown ? -1.0f : 1.0f);
    if (isSideways) dirX = 0.0f;

    float offsetX = dirX * m_gameState.m_unkInt1 / (m_gameState.m_unkBool2 ? 1.0f : m_gameState.m_cameraZoom);
    float offsetY = dirY * m_gameState.m_unkInt2 / (m_gameState.m_unkBool3 ? 1.0f : m_gameState.m_cameraZoom);

    cocos2d::CCPoint targetPos = m_gameState.m_cameraPosition + cocos2d::CCPoint(offsetX, offsetY);

    if (m_isPlatformer) {
        float groundY = isUpsideDown ? 55.0f : 27.5f;
        float topY = isUpsideDown ? 27.5f : 55.0f;
        if (isSideways) std::swap(groundY, topY);

        float playerY = m_player1->getPosition()->y;
        if (!isFlying) {
            float minY2 = m_player1->m_lastGroundedPos.y - groundY;
            if (targetPos.y < minY2) targetPos.y = minY2;
            float maxY2 = m_player1->m_lastGroundedPos.y + topY;
            if (targetPos.y > maxY2) targetPos.y = maxY2;
        }
    } else {
        float followY = isSideways ? m_player1->getPosition()->x : m_player1->getPosition()->y;
        if (m_gameState.m_unkBool7) {
            float dual = m_gameState.m_unkInt14;
            float dy = followY + m_gameState.m_unkFloat9 * (dual - followY);
            if (!isSideways) targetPos.y = dy;
            else targetPos.x = dy;
        } else {
            float dual = m_gameState.m_unkInt10;
            if (!isSideways) targetPos.y = followY + offsetY;
            else targetPos.x = followY + offsetX;
        }
    }

    if (maxX != -99999.0f && targetPos.x < maxX) targetPos.x = maxX;
    if (minX != -99999.0f && targetPos.x > minX) targetPos.x = minX;
    if (maxY != -99999.0f && targetPos.y < maxY) targetPos.y = maxY;
    if (minY != -99999.0f && targetPos.y > minY) targetPos.y = minY;

    if (m_gameState.m_cameraShakeEnabled) applyShake(&targetPos);

    if (m_cameraFlip == -1.0f) targetPos.x = -targetPos.x - width;

    m_objectLayer->setScaleX(m_cameraFlip * m_gameState.m_cameraZoom);
    m_objectLayer->setScaleY(m_gameState.m_cameraZoom);
    m_objectLayer->setPosition((targetPos - m_gameState.m_cameraPosition) * m_gameState.m_cameraZoom);

    updateCameraBGArt(targetPos, m_gameState.m_cameraZoom);

    m_gameState.m_unkFloat4 = m_gameState.m_cameraZoom;
    m_gameState.m_unkBool20 = isSideways;
    m_gameState.m_unkBool21 = isGoingLeft;
    m_gameState.m_unkPoint4 = (targetPos - m_gameState.m_cameraPosition2) * 240.0f;
    m_gameState.m_cameraPosition2 = targetPos;
    m_gameState.m_unkInt12 = targetPos.y + height * 0.5f - m_gameState.m_cameraOffset.y;
    m_gameState.m_unkPoint3 = m_gameState.m_cameraOffset;
    m_gameState.m_unkPoint30 = cocos2d::CCPoint(0.0f, 0.0f);

    updateExtraGameLayers();
}

void GJBaseGameLayer::updateEnterEffects(float dt) {
    for (auto& pair : m_gameState.m_enterEffectInstanceVectors) {
        for (auto& instance : pair.second)
            instance.updateTransitions(dt, this);
    }
    for (auto& pair : m_gameState.m_exitEffectInstanceVectors) {
        for (auto& instance : pair.second)
            instance.updateTransitions(dt, this);
    }
}

void GJBaseGameLayer::updateGroundShadows() {
    if (!m_groundLayer || !m_groundLayer2) return;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint groundPos = m_groundLayer->getPosition();

    float angle = m_gameState.m_cameraAngle * 0.017453f;
    m_cameraObb2->calculateWithCenter(winSize / m_gameState.m_cameraZoom, angle,
                                      m_gameState.m_cameraAngle, 0.0f);

    cocos2d::CCPoint corners[4] = {
        m_cameraObb2->m_corners.__elems_[0],
        m_cameraObb2->m_corners.__elems_[1],
        m_cameraObb2->m_corners.__elems_[2],
        m_cameraObb2->m_corners.__elems_[3]
    };

    cocos2d::CCPoint intersections[4];
    int count = 0;

    for (int i = 0; i < 4; i++) {
        cocos2d::CCPoint p = corners[i];
        cocos2d::CCPoint q = corners[(i + 1) % 4];
        cocos2d::CCPoint inter;
        if (ccpIntersectPoint(p, q, groundPos, groundPos + cocos2d::CCPoint(winSize.width, 0.0f), &inter))
            intersections[count++] = inter;
    }

    if (count == 2) {
        float x1 = intersections[0].x;
        float x2 = intersections[1].x;
        if (x1 > x2) std::swap(x1, x2);

        float minDist = x2 - x1;
        if (minDist < m_gameState.m_cameraZoom * 180.0f) {
            float diff = m_gameState.m_cameraZoom * 180.0f - minDist;
            x1 -= diff * 0.5f;
            x2 += diff * 0.5f;
        }

        x1 /= m_gameState.m_cameraZoom;
        x2 /= m_gameState.m_cameraZoom;

        m_groundLayer->updateShadowXPos(x1, x2);
        m_groundLayer2->updateShadowXPos(x1, x2);
    }
}

void GJBaseGameLayer::updateLevelColors() {
    if (!m_background || !m_groundLayer) return;

    cocos2d::ccColor3B bgColor = GJEffectManager::activeColorForIndex(m_effectManager, 1000);
    m_background->setColor(bgColor);

    cocos2d::ccColor3B g1Color = GJEffectManager::activeColorForIndex(m_effectManager, 1001);
    m_groundLayer->updateGround01Color(g1Color);
    m_groundLayer2->updateGround01Color(g1Color);

    if (m_groundLayer->m_ground2Sprite) {
        cocos2d::ccColor3B g2Color = GJEffectManager::activeColorForIndex(m_effectManager, 1009);
        m_groundLayer->updateGround02Color(g2Color);
        m_groundLayer2->updateGround02Color(g2Color);
    }

    cocos2d::ccColor3B lineColor = GJEffectManager::activeColorForIndex(m_effectManager, 1002);
    float lineOpacity = GJEffectManager::activeOpacityForIndex(m_effectManager, 1002);

    m_groundLayer->m_lineSprite->setColor(lineColor);
    m_groundLayer2->m_lineSprite->setColor(lineColor);
    m_groundLayer->m_lineSprite->setOpacity((unsigned int)(lineOpacity * 255.0f));
    m_groundLayer2->m_lineSprite->setOpacity((unsigned int)(lineOpacity * 255.0f));

    bool lineBlend = GJEffectManager::shouldBlend(m_effectManager, 1002);
    m_groundLayer->updateLineBlend(lineBlend);
    m_groundLayer2->updateLineBlend(lineBlend);

    if (m_middleground) {
        cocos2d::ccColor3B mg1Color = GJEffectManager::activeColorForIndex(m_effectManager, 1013);
        cocos2d::ccColor3B mg2Color = GJEffectManager::activeColorForIndex(m_effectManager, 1014);
        float mg1Opacity = GJEffectManager::activeOpacityForIndex(m_effectManager, 1013);
        float mg2Opacity = GJEffectManager::activeOpacityForIndex(m_effectManager, 1014);

        m_middleground->updateGroundColor(mg1Color, true);
        m_middleground->updateGroundColor(mg2Color, false);
        m_middleground->updateGroundOpacity((unsigned int)(mg1Opacity * 255.0f), true);
        m_middleground->updateGroundOpacity((unsigned int)(mg2Opacity * 255.0f), false);

        bool mg1Blend = GJEffectManager::shouldBlend(m_effectManager, 1013);
        bool mg2Blend = GJEffectManager::shouldBlend(m_effectManager, 1014);
        m_middleground->updateMG01Blend(mg1Blend);
        m_middleground->updateMG02Blend(mg2Blend);
    }
}

void GJBaseGameLayer::updateProximityVolumeEffects() {
    for (auto& pair : m_gameState.m_proximityVolumeRelated) {
        SFXTriggerInstance instance = pair.second;
        float volume = volumeForProximityEffect(&instance);
        FMODAudioEngine::sharedEngine()->setChannelVolumeMod(
            pair.first.first, (AudioTargetType)pair.first.second, volume);
    }
}

void GJBaseGameLayer::updateShaderLayer(float dt) {
    if (!m_shaderLayer) return;

    ShaderState& state = m_shaderLayer->m_state;

    if (state.m_grayscaleTintChannel > 0)
        state.m_grayscaleTint = GJEffectManager::activeColorForIndex(m_effectManager, state.m_grayscaleTintChannel);
    else
        state.m_grayscaleTint = cocos2d::ccc3(255, 255, 255);

    if (state.m_blurRefChannel > 0)
        state.m_blurRefColor = GJEffectManager::activeColorForIndex(m_effectManager, state.m_blurRefChannel);
    else
        state.m_blurRefColor = m_background->getColor();

    if (state.m_radialBlurTarget)
        state.m_radialBlurCenter = positionForShaderTarget(state.m_radialBlurTargetID);

    if (state.m_pinchTargetEnabledX)
        state.m_pinchCenter.x = positionForShaderTarget(state.m_pinchTargetIDX).x;
    if (state.m_pinchTargetEnabledY)
        state.m_pinchCenter.y = positionForShaderTarget(state.m_pinchTargetIDY).y;

    if (state.m_shockWaveTargetID)
        m_shaderLayer->updateShockWaveCenter(positionForShaderTarget(state.m_shockWaveTargetID));

    if (state.m_shockLineTargetID)
        m_shaderLayer->updateShockLineCenter(positionForShaderTarget(state.m_shockLineTargetID));

    if (state.m_lensCircleTargetID)
        state.m_lensCircleCenter = positionForShaderTarget(state.m_lensCircleTargetID);

    if (state.m_bulgeTargetID)
        state.m_bulgeCenter = positionForShaderTarget(state.m_bulgeTargetID);

    if (state.m_lensCircleTintChannel > 0) {
        state.m_lensCircleTint = GJEffectManager::activeColorForIndex(m_effectManager, state.m_lensCircleTintChannel);
        state.m_lensCircleAdditive = GJEffectManager::shouldBlend(m_effectManager, state.m_lensCircleTintChannel);
    } else {
        state.m_lensCircleTint = cocos2d::ccc3(0, 0, 0);
        state.m_lensCircleAdditive = false;
    }

    float speedX = 0.0f, speedY = 0.0f;
    if (state.m_motionBlurTargetIDX) speedX = speedForShaderTarget(state.m_motionBlurTargetIDX).x;
    if (state.m_motionBlurTargetIDY) speedY = speedForShaderTarget(state.m_motionBlurTargetIDY).y;

    m_shaderLayer->updateMotionBlurSpeedX(speedX, dt * 60.0f);
    m_shaderLayer->updateMotionBlurSpeedY(speedY, dt * 60.0f);

    m_shaderLayer->update(dt);
    m_shaderLayer->performCalculations();

    if (state.m_usesShaders) {
        if (!m_shaderLayer->getParent() || state.m_zLayerDirty) {
            if (m_middleground) {
                cocos2d::CCNode* mgParent = (state.m_maxBlendingLayer <= 1 || state.m_minBlendingLayer != 2)
                                            ? m_objectParent : m_inShaderParent;
                reparentObject(m_middleground, mgParent);
            }

            m_aboveShaderParent = (state.m_minBlendingLayer <= 1 || state.m_maxBlendingLayer > 12)
                                   ? m_objectParent : m_inShaderParent;

            if (m_groundLayer) reparentObject(m_groundLayer, m_aboveShaderParent);
            if (m_groundLayer2) reparentObject(m_groundLayer2, m_aboveShaderParent);

            cocos2d::CCNode* uiParent = (state.m_minBlendingLayer <= 1 || state.m_maxBlendingLayer > 13)
                                       ? m_objectParent : m_inShaderParent;
            reparentObject(m_uiTriggerUI, uiParent);

            if (!m_shaderLayer->getParent())
                addChild(m_shaderLayer, state.m_minBlendingLayer > 1 ? 1 : 0);
            else
                m_shaderLayer->setZOrder(state.m_minBlendingLayer > 1 ? 1 : 0);

            state.m_zLayerDirty = false;
        }
    } else {
        if (m_shaderLayer->getParent())
            m_shaderLayer->removeFromParentAndCleanup(false);
    }

    bool hasChildren1 = m_inShaderParent->getChildrenCount() > 1 || m_inShaderObjectLayer->getChildrenCount() > 0;
    m_inShaderParent->setVisible(hasChildren1);

    bool hasChildren2 = m_aboveShaderParent->getChildrenCount() > 1 || m_aboveShaderObjectLayer->getChildrenCount() > 0;
    m_aboveShaderParent->setVisible(hasChildren2);
}

void GJBaseGameLayer::processActivatedAudioTriggers(float levelTime) {
    m_processingAudioTriggers = true;

    std::unordered_set<int> activeMusicChannels;
    bool hasDefaultSong = false;

    for (auto& pair : m_gameState.m_songChannelStates) {
        int channelID = pair.first;
        SongChannelState& state = pair.second;

        if (state.m_activatedTrigger) activateSongTrigger(state.m_activatedTrigger);

        if (state.m_songTrigger) {
            SongTriggerGameObject* trigger = state.m_songTrigger;
            trigger->m_spawnTriggerDelay = state.m_spawnTriggerDelay;

            float pitch = FMODAudioEngine::pitchForIdx(trigger->m_speed);
            float volume = trigger->m_volume;
            std::string songPath = MusicDownloadManager::pathForSong(trigger->m_soundID);
            bool dontReset = trigger->m_dontReset;
            bool loop = trigger->m_loop;

            FMODAudioEngine::sharedEngine()->loadMusic(songPath, pitch, 0.0f, volume, loop, channelID, 0, dontReset);

            if (loop && (trigger->m_start > 0 || trigger->m_end > 0))
                FMODAudioEngine::sharedEngine()->startMusic(trigger->m_start, trigger->m_end, 0, 0, 1, channelID, 1, dontReset);

            std::vector<SongTriggerState>* stateVector = nullptr;
            auto it = m_gameState.m_songTriggerStateVectors.find(channelID);
            if (it != m_gameState.m_songTriggerStateVectors.end()) stateVector = &it->second;

            if (processSongState(channelID, trigger->m_spawnTriggerDelay, levelTime,
                                 trigger->m_start, volume, pitch, stateVector, trigger))
                activeMusicChannels.insert(channelID);

            if (channelID == 0) hasDefaultSong = true;
        } else if (channelID == 0 && state.m_spawnTriggerDelay > 0.0f) {
            hasDefaultSong = true;
        }
    }

    if (!hasDefaultSong && !m_gameState.m_unkBool30) {
        std::string audioFileName = m_level->getAudioFileName();
        bool loop = m_isPlatformer;
        bool dontReset = m_levelSettings->m_dontReset;

        FMODAudioEngine::sharedEngine()->loadMusic(audioFileName, 1.0f, 0.0f, 1.0f, loop, 0, 0, dontReset);

        int songOffsetMs = (int)(m_levelSettings->m_songOffset * 1000.0f);

        std::vector<SongTriggerState>* stateVector = nullptr;
        auto it = m_gameState.m_songTriggerStateVectors.find(0);
        if (it != m_gameState.m_songTriggerStateVectors.end()) stateVector = &it->second;

        if (processSongState(0, 0.0f, levelTime, songOffsetMs, 1.0f, 1.0f, stateVector, nullptr))
            activeMusicChannels.insert(0);
    }

    FMODAudioEngine::sharedEngine()->stopMusicNotInSet(activeMusicChannels);

    std::unordered_map<int, std::vector<int>> triggeredSfx;
    std::unordered_map<int, std::vector<int>> triggeredGroups;
    std::unordered_map<int, int> stateIndices;
    std::unordered_map<int, float> times;
    std::unordered_map<int, int> stateIndexMap;

    for (SFXTriggerState& state : m_gameState.m_sfxTriggerStates) {
        SFXTriggerGameObject* trigger = state.m_sfxTriggerGameObject;
        int objectID = trigger->m_objectID;

        if (objectID == 3602) {
            if (canProcessSFX(&state, stateIndices, times, m_gameState.m_sfxTriggerStates)) {
                int uniqueID = trigger->m_uniqueID;
                triggeredSfx[uniqueID].push_back(state.m_unkInt1);
                stateIndexMap[state.m_unkInt1] = state.m_unkInt2;
                if (trigger->m_sfxGroup > 0)
                    triggeredGroups[trigger->m_sfxGroup].push_back(state.m_unkInt1);
            } else {
                state.m_processed = true;
            }
        } else if (objectID == 3603) {
            std::unordered_set<int> triggerSet;

            if (trigger->m_groupID > 0) {
                cocos2d::CCArray* group = getGroup(trigger->m_groupID);
                if (group && cocos2d::CCArray::count(group)) {
                    for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
                        GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
                        if (!obj) break;
                        if (obj->m_objectID == 3602) {
                            int uid = obj->m_uniqueID;
                            auto it = triggeredSfx.find(uid);
                            if (it != triggeredSfx.end())
                                for (int idx : it->second) triggerSet.insert(idx);
                        }
                    }
                }
            }

            if (trigger->m_sfxUniqueID > 0 && stateIndices.count(trigger->m_sfxUniqueID)) {
                int resolvedIdx = stateIndices[trigger->m_sfxUniqueID];
                int resolvedUID = m_gameState.m_sfxTriggerStates[resolvedIdx].m_sfxTriggerGameObject->m_uniqueID;
                auto it = triggeredSfx.find(resolvedUID);
                if (it != triggeredSfx.end())
                    for (int idx : it->second) triggerSet.insert(idx);
            }

            for (auto& pair : stateIndexMap)
                if (triggerSet.count(pair.first))
                    processSFXState(&m_gameState.m_sfxTriggerStates[pair.second], &state, 0, levelTime);

            if (trigger->m_sfxGroup > 0) {
                auto it = triggeredGroups.find(trigger->m_sfxGroup);
                if (it != triggeredGroups.end())
                    for (int idx : it->second)
                        if (stateIndexMap.count(idx))
                            processSFXState(&m_gameState.m_sfxTriggerStates[stateIndexMap[idx]], &state, 1, levelTime);
            }

            if (trigger->m_changeVolume) processSFXGroupEdit(&state, &stateIndices, state.m_unkFloat1);
            if (trigger->m_changeSpeed)  processSFXGroupEdit(&state, &times, state.m_unkFloat3);
        }
    }

    for (int i = 0; i < 2; ++i) {
        auto& map = (i == 0) ? times : stateIndices;
        for (auto& pair : map)
            FMODAudioEngine::sharedEngine()->updateChannel(
                pair.first, SFXGroup, i == 0 ? Volume : Pitch, 0.0f, pair.second);
    }

    for (size_t i = 0; i < m_gameState.m_sfxTriggerStates.size(); ++i) {
        SFXTriggerState* state = &m_gameState.m_sfxTriggerStates[i];
        processSFXState(state, nullptr, 0, levelTime);

        SFXTriggerGameObject* trigger = state->m_sfxTriggerGameObject;
        int objectID = trigger->m_objectID;

        if (objectID == 3602) {
            if (state->m_processed) continue;

            FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
            int savedEnd = trigger->m_end;
            bool savedPreload = trigger->m_preload;
            bool loop = trigger->m_loop;

            engine->m_audioState.m_elapsed = state->m_unkDouble1;
            trigger->m_preload = true;

            if (!loop && trigger->m_fadeOut) {
                int end = savedEnd;
                if (end <= 0) {
                    float pitch = trigger->m_speed ? FMODAudioEngine::pitchForIdx(trigger->m_speed) : 1.0f;
                    end = (int)(pitch * (trigger->m_soundDuration * 1000.0f));
                }
                trigger->m_end = (int)((double)end - state->m_unkDouble2);
            }

            activateSFXTrigger(trigger);
            trigger->m_preload = savedPreload;
            trigger->m_end = savedEnd;
            continue;
        } else if (objectID == 3603) {
            float savedDuration = trigger->m_duration;
            int targetGroupID = trigger->m_targetGroupID;
            float remaining = (savedDuration + trigger->m_spawnTriggerDelay) - levelTime;
            trigger->m_duration = remaining;

            bool shouldActivate = false;
            if (remaining < 0.0f) {
                trigger->m_applyDisabled = true;
                if (targetGroupID > 0) shouldActivate = true;
            } else if (targetGroupID > 0 || !trigger->m_applyDisabled) {
                shouldActivate = true;
            }

            if (shouldActivate) activateSFXEditTrigger(trigger);

            trigger->m_duration = savedDuration;
            trigger->m_applyDisabled = false;
        }
    }

    m_gameState.m_sfxTriggerStates.clear();
    FMODAudioEngine::sharedEngine()->m_audioState.m_elapsed = levelTime;
    m_varianceValues[1975 + 1861] = 0;
    m_processingAudioTriggers = false;
}

float GJBaseGameLayer::volumeForProximityEffect(SFXTriggerInstance& instance) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint cameraCenter = m_gameState.m_cameraPosition +
        cocos2d::CCPoint(winSize.width / m_gameState.m_cameraZoom * 0.5f,
                          winSize.height / m_gameState.m_cameraZoom * 0.5f);

    SFXTriggerGameObject* trigger = instance.m_sfxTriggerGameObject;
    cocos2d::CCArray* group = getGroup(instance.m_groupID1);
    cocos2d::CCPoint sourcePos;
    bool hasSource = false;

    if (trigger->m_cameraDistance) { sourcePos = cameraCenter; hasSource = true; }
    else if (trigger->m_targetPlayer1) { sourcePos = m_player1->getPosition(); hasSource = true; }
    else if (trigger->m_targetPlayer2 && m_gameState.m_isDualMode) {
        sourcePos = m_player2->getPosition(); hasSource = true;
    } else {
        GameObject* center = tryGetMainObject(instance.m_groupID2);
        if (center) { sourcePos = center->getRealPosition(); hasSource = true; }
    }

    if (!group || cocos2d::CCArray::count(group) == 0 || !hasSource) return 1.0f;

    float nearVol = trigger->m_volumeNear;
    float farVol = trigger->m_volumeFar;
    float minDistNear = trigger->m_minDistNear * 3.0f;
    float minDistMedium = trigger->m_minDistMedium * 3.0f + minDistNear;
    float minDistFar = trigger->m_minDistFar * 3.0f + minDistMedium;

    float minDist = minDistNear;
    for (unsigned int i = 0; i < cocos2d::CCArray::count(group); ++i) {
        GameObject* obj = (GameObject*)cocos2d::CCArray::objectAtIndex(group, i);
        float dist = ccpDistance(sourcePos, obj->getPosition());
        if (dist < minDist) minDist = dist;
    }

    if (trigger->m_targetPlayer1 && trigger->m_targetPlayer2 && m_gameState.m_isDualMode) {
        float d = ccpDistance(sourcePos, m_player2->getPosition());
        if (d < minDist) minDist = d;
    }

    if (minDist <= minDistNear) return nearVol;
    if (minDist < minDistMedium) {
        float t = (minDist - minDistNear) / (minDistMedium - minDistNear);
        return nearVol + (trigger->m_volumeMedium - nearVol) * t;
    }
    if (minDist < minDistFar) {
        float t = (minDist - minDistMedium) / (minDistFar - minDistMedium);
        return trigger->m_volumeMedium + (farVol - trigger->m_volumeMedium) * t;
    }
    return std::max(0.0f, std::min(1.0f, farVol));
}