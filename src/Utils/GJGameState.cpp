// GJGameState.cpp
#include "Utils/GJGameState.h"
#include "Objects/GameObject.h"
#include "GameObjectPhysics.h"
#include "Utils/GJValueTween.h"
#include "Objects/EnhancedGameObject.h"
#include "Objects/EnterEffectInstance.h"
#include "SongChannelState.h"
#include "SongTriggerState.h"
#include "SFXTriggerState.h"
#include "EventTriggerInstance.h"
#include "DynamicObjectAction.h"
#include "AdvancedFollowInstance.h"
#include "SFXTriggerInstance.h"

GJGameState::GJGameState() {
    m_cameraZoom = 1.0f;
    m_targetCameraZoom = 1.0f;
    m_cameraOffset = cocos2d::CCPointZero;
    m_unkPoint1 = cocos2d::CCPointZero;
    m_unkPoint2 = cocos2d::CCPointZero;
    m_unkPoint3 = cocos2d::CCPointZero;
    m_unkPoint4 = cocos2d::CCPointZero;
    m_unkPoint5 = cocos2d::CCPointZero;
    m_unkPoint6 = cocos2d::CCPointZero;
    m_unkPoint7 = cocos2d::CCPointZero;
    m_unkPoint8 = cocos2d::CCPointZero;
    m_unkPoint9 = cocos2d::CCPointZero;
    m_unkPoint10 = cocos2d::CCPointZero;
    m_unkPoint11 = cocos2d::CCPointZero;
    m_unkPoint12 = cocos2d::CCPointZero;
    m_unkPoint13 = cocos2d::CCPointZero;
    m_unkPoint14 = cocos2d::CCPointZero;
    m_unkPoint15 = cocos2d::CCPointZero;
    m_unkPoint16 = cocos2d::CCPointZero;
    m_unkPoint17 = cocos2d::CCPointZero;
    m_unkPoint18 = cocos2d::CCPointZero;
    m_unkPoint19 = cocos2d::CCPointZero;
    m_unkPoint20 = cocos2d::CCPointZero;
    m_unkPoint21 = cocos2d::CCPointZero;
    m_unkPoint22 = cocos2d::CCPointZero;
    m_unkPoint23 = cocos2d::CCPointZero;
    m_unkPoint24 = cocos2d::CCPointZero;
    m_unkPoint25 = cocos2d::CCPointZero;
    m_unkPoint26 = cocos2d::CCPointZero;
    m_unkPoint27 = cocos2d::CCPointZero;
    m_unkPoint28 = cocos2d::CCPointZero;
    m_unkPoint29 = cocos2d::CCPointZero;
    m_unkPoint30 = cocos2d::CCPointZero;
    m_unkPoint32 = cocos2d::CCPointZero;
    m_unkPoint34 = cocos2d::CCPointZero;
    m_cameraPosition = cocos2d::CCPointZero;
    m_cameraPosition2 = cocos2d::CCPointZero;
    m_cameraStepDiff = cocos2d::CCPointZero;

    m_unkBool1 = false;
    m_unkBool2 = false;
    m_unkBool3 = false;
    m_unkBool4 = false;
    m_unkBool5 = false;
    m_unkBool6 = false;
    m_unkBool7 = false;
    m_unkBool8 = false;
    m_unkBool9 = false;
    m_unkBool10 = false;
    m_unkBool11 = false;
    m_unkBool12 = false;
    m_unkBool20 = false;
    m_unkBool21 = false;
    m_unkBool22 = false;
    m_unkBool26 = false;
    m_unkBool27 = false;
    m_unkBool28 = false;
    m_unkBool29 = false;
    m_unkBool30 = false;
    m_unkBool31 = false;
    m_unkBool32 = false;
    m_playerStreakBlend = true;
    m_isDualMode = false;

    m_unkInt1 = 0;
    m_unkInt2 = 0;
    m_unkInt3 = 0;
    m_unkInt4 = 0;
    m_unkInt5 = 0;
    m_unkInt6 = 0;
    m_unkInt7 = 0;
    m_unkInt8 = 0;
    m_unkInt9 = 0;
    m_unkInt10 = 0;
    m_unkInt11 = 0;
    m_unkInt12 = 0;
    m_unkInt13 = 0;
    m_unkInt14 = 0;
    m_unkInt15 = 0;
    m_commandIndex = 0;
    m_currentProgress = 0;
    m_unkUint4 = 0;
    m_unkUint5 = 0;
    m_unkUint6 = 0;
    m_unkUint7 = 0;
    m_unkUint8 = 0;
    m_lastActivatedPortal1 = 0;
    m_lastActivatedPortal2 = 0;
    m_unkUint11 = 0;
    m_unkUint12 = 0;
    m_timeModRelated2 = 0;
    m_dualRelated = 0;
    m_background = 0;
    m_ground = 0;
    m_middleground = 0;
    m_points = 0;
    m_pauseCounter = 0;
    m_pauseBufferTimer = 0;
    m_currentChannel = 0;
    m_rotateChannel = 0;

    m_unkFloat5 = 0.1f;
    m_unkFloat6 = 0.1f;
    m_middleGroundOffsetY = 0.0f;
    m_unkFloat2 = 0.5f;
    m_unkFloat3 = 10.0f;
    m_portalY = 0.0f;
    m_unkFloat7 = 0.3f;
    m_unkFloat8 = 0.5f;
    m_unkFloat4 = 1.0f;
    m_timeWarp = 1.0f;
    m_queuedTimeWarp = 0.0f;
    m_timeWarpRelated = 1.0f;
    m_unkUint3 = 0.0f;
    m_totalTime = 0.0f;
    m_levelTime = 0.0f;
    m_unkDouble3 = 0.0f;
    m_levelFlipping = 0.0f;
    m_unkFloat9 = 0.0f;
    m_unkUint10 = 0.0f;
    m_unkFloat10 = 0.0f;
    m_timeModRelated = 0.0f;
    m_unkUint13 = 0.0f;
    m_unkUint14 = 0.0f;
    m_unkUint15 = 0.0f;
    m_unkUint16 = 0.0f;
    m_unkUint64_1 = 0.0f;
    m_unkUint17 = 0.0f;

    m_cameraEdgeValue0 = 0;
    m_cameraEdgeValue1 = 0;
    m_cameraEdgeValue2 = 0;
    m_cameraEdgeValue3 = 0;
    m_cameraShakeEnabled = false;
    m_cameraShakeFactor = 0.0f;
}

void GJGameState::controlTweenAction(int uniqueID, int controlID, GJActionCommand command) {
    auto it = m_tweenActions.begin();
    while (it != m_tweenActions.end()) {
        bool matches = false;
        if (controlID == -1) {
            if (it->second.m_uniqueID == uniqueID) {
                matches = true;
            }
        } else {
            if (it->second.m_controlID == controlID) {
                matches = true;
            }
        }

        if (matches) {
            switch (command) {
                case GJActionCommand::Stop:
                    it = m_tweenActions.erase(it);
                    continue;
                case GJActionCommand::Pause:
                    it->second.m_paused = true;
                    break;
                case GJActionCommand::Resume:
                    it->second.m_paused = false;
                    break;
                default:
                    break;
            }
        }
        ++it;
    }
}

GameObjectPhysics* GJGameState::getGameObjectPhysics(GameObject* object) {
    int uniqueID = object->m_uniqueID;
    auto it = m_gameObjectPhysics.find(uniqueID);
    if (it != m_gameObjectPhysics.end()) {
        it->second.m_commandIndex = m_commandIndex;
        return &it->second;
    }

    GameObjectPhysics physics;
    physics.m_position = cocos2d::CCPointZero;
    physics.m_velocity = cocos2d::CCPointZero;
    physics.m_rotation = 0.0f;
    physics.m_scale = 1.0f;
    physics.m_commandIndex = m_commandIndex;
    physics.m_object = object;

    m_gameObjectPhysics[uniqueID] = physics;
    return &m_gameObjectPhysics[uniqueID];
}

void GJGameState::processStateTriggers() {
    auto it = m_activatedObjectIDs.begin();
    while (it != m_activatedObjectIDs.end()) {
        if (it->first >= m_commandIndex) {
            it = m_activatedObjectIDs.erase(it);
        } else {
            ++it;
        }
    }
}

void GJGameState::stopTweenAction(int action) {
    auto it = m_tweenActions.find(action);
    if (it != m_tweenActions.end()) {
        m_tweenActions.erase(it);
    }
}

void GJGameState::updateTweenAction(float value, int action) {
    switch (action) {
        case 1:
            m_cameraPosition.x = value;
            break;
        case 2:
            m_cameraPosition.y = value;
            break;
        case 7:
            m_levelFlipping = value;
            break;
        case 9:
            m_portalY = value;
            break;
        case 10:
            m_unkPoint6.x = value;
            break;
        case 11:
            m_unkPoint6.y = value;
            break;
        case 12:
            m_unkPoint8.x = value;
            break;
        case 13:
            m_unkPoint8.y = value;
            break;
        case 14:
            m_cameraZoom = value;
            break;
        case 15:
            m_cameraOffset.x = value;
            break;
        case 16:
            m_cameraOffset.y = value;
            break;
        case 17:
            m_cameraAngle = value;
            break;
        case 18:
            m_unkPoint26.x = value;
            break;
        case 19:
            m_unkPoint26.y = value;
            break;
        case 20:
            m_middleGroundOffsetY = value;
            break;
        case 21:
            m_unkPoint12.x = value;
            break;
        case 22:
            m_unkPoint12.y = value;
            break;
        case 25:
            m_unkFloat9 = value;
            break;
        default:
            break;
    }
}

void GJGameState::updateTweenActions(float tweenValue) {
    auto it = m_tweenActions.begin();
    while (it != m_tweenActions.end()) {
        it->second.step(tweenValue);
        updateTweenAction(it->second.m_currentValue, it->second.m_actionID);
        if (it->second.m_finished) {
            it = m_tweenActions.erase(it);
        } else {
            ++it;
        }
    }
}

GJGameState::~GJGameState() {
    // Clear all containers
    m_sfxTriggerStates.clear();
    m_songTriggerStateVectors.clear();
    m_songChannelStates.clear();
    m_proximityVolumeRelated.clear();
    m_unkUMap8.clear();
    m_dynamicRotateActions.clear();
    m_dynamicMoveActions.clear();
    m_advanceFollowInstances.clear();
    m_unsortedAreaEffects.clear();
    m_tintEffectInstances.clear();
    m_fadeEffectInstances.clear();
    m_scaleEffectInstances.clear();
    m_rotateEffectInstances.clear();
    m_moveEffectInstances.clear();
    m_exitChannelMap.clear();
    m_enterChannelMap.clear();
    m_exitEffectInstanceVectors.clear();
    m_enterEffectInstanceVectors.clear();
    m_unkMapPairGJGameEventIntInt.clear();
    m_unkMapPairGJGameEventIntVectorEventTriggerInstance.clear();
    m_stateObjects.clear();
    m_activatedObjectIDs.clear();
    m_unkVecFloat1.clear();
    m_gameObjectPhysics.clear();
    m_tweenActions.clear();
    m_spawnChannelRelated1.clear();
    m_spawnChannelRelated0.clear();
}