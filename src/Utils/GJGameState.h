// GJGameState.h
#pragma once

#include "Platform/cocos2d.h"
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>

class GameObject;
class GameObjectPhysics;
class GJValueTween;
class GJActionCommand;
class EnhancedGameObject;
class EnterEffectInstance;
class SongChannelState;
class SongTriggerState;
class SFXTriggerState;
class EventTriggerInstance;
class DynamicObjectAction;
class AdvancedFollowInstance;
class SFXTriggerInstance;

enum class GJGameEvent;
enum class GJActionCommand;

class GJGameState {
public:
    GJGameState();
    ~GJGameState();

    void controlTweenAction(int uniqueID, int controlID, GJActionCommand command);
    GameObjectPhysics* getGameObjectPhysics(GameObject* object);
    void processStateTriggers();
    void stopTweenAction(int action);
    void updateTweenAction(float value, int action);
    void updateTweenActions(float tweenValue);

    // Camera
    float m_cameraZoom;
    float m_targetCameraZoom;
    cocos2d::CCPoint m_cameraOffset;
    cocos2d::CCPoint m_cameraPosition;
    cocos2d::CCPoint m_cameraPosition2;
    float m_cameraAngle;
    float m_targetCameraAngle;
    int m_cameraEdgeValue0;
    int m_cameraEdgeValue1;
    int m_cameraEdgeValue2;
    int m_cameraEdgeValue3;
    bool m_cameraShakeEnabled;
    float m_cameraShakeFactor;
    cocos2d::CCPoint m_cameraStepDiff;

    // Points
    cocos2d::CCPoint m_unkPoint1;
    cocos2d::CCPoint m_unkPoint2;
    cocos2d::CCPoint m_unkPoint3;
    cocos2d::CCPoint m_unkPoint4;
    cocos2d::CCPoint m_unkPoint5;
    cocos2d::CCPoint m_unkPoint6;
    cocos2d::CCPoint m_unkPoint7;
    cocos2d::CCPoint m_unkPoint8;
    cocos2d::CCPoint m_unkPoint9;
    cocos2d::CCPoint m_unkPoint10;
    cocos2d::CCPoint m_unkPoint11;
    cocos2d::CCPoint m_unkPoint12;
    cocos2d::CCPoint m_unkPoint13;
    cocos2d::CCPoint m_unkPoint14;
    cocos2d::CCPoint m_unkPoint15;
    cocos2d::CCPoint m_unkPoint16;
    cocos2d::CCPoint m_unkPoint17;
    cocos2d::CCPoint m_unkPoint18;
    cocos2d::CCPoint m_unkPoint19;
    cocos2d::CCPoint m_unkPoint20;
    cocos2d::CCPoint m_unkPoint21;
    cocos2d::CCPoint m_unkPoint22;
    cocos2d::CCPoint m_unkPoint23;
    cocos2d::CCPoint m_unkPoint24;
    cocos2d::CCPoint m_unkPoint25;
    cocos2d::CCPoint m_unkPoint26;
    cocos2d::CCPoint m_unkPoint27;
    cocos2d::CCPoint m_unkPoint28;
    cocos2d::CCPoint m_unkPoint29;
    cocos2d::CCPoint m_unkPoint30;
    cocos2d::CCPoint m_unkPoint32;
    cocos2d::CCPoint m_unkPoint34;

    // Bools
    bool m_unkBool1;
    bool m_unkBool2;
    bool m_unkBool3;
    bool m_unkBool4;
    bool m_unkBool5;
    bool m_unkBool6;
    bool m_unkBool7;
    bool m_unkBool8;
    bool m_unkBool9;
    bool m_unkBool10;
    bool m_unkBool11;
    bool m_unkBool12;
    bool m_unkBool20;
    bool m_unkBool21;
    bool m_unkBool22;
    bool m_unkBool26;
    bool m_unkBool27;
    bool m_unkBool28;
    bool m_unkBool29;
    bool m_unkBool30;
    bool m_unkBool31;
    bool m_unkBool32;
    bool m_playerStreakBlend;
    bool m_isDualMode;

    // Ints
    int m_unkInt1;
    int m_unkInt2;
    int m_unkInt3;
    int m_unkInt4;
    int m_unkInt5;
    int m_unkInt6;
    int m_unkInt7;
    int m_unkInt8;
    int m_unkInt9;
    int m_unkInt10;
    int m_unkInt11;
    int m_unkInt12;
    int m_unkInt13;
    int m_unkInt14;
    int m_unkInt15;
    int m_commandIndex;
    int m_currentProgress;
    int m_unkUint4;
    int m_unkUint5;
    int m_unkUint6;
    int m_unkUint7;
    int m_unkUint8;
    int m_lastActivatedPortal1;
    int m_lastActivatedPortal2;
    int m_unkUint11;
    int m_unkUint12;
    int m_timeModRelated2;
    int m_dualRelated;
    int m_background;
    int m_ground;
    int m_middleground;
    int m_points;
    int m_pauseCounter;
    int m_pauseBufferTimer;

    // Floats
    float m_unkFloat5;
    float m_unkFloat6;
    float m_middleGroundOffsetY;
    float m_unkFloat2;
    float m_unkFloat3;
    float m_portalY;
    float m_unkFloat7;
    float m_unkFloat8;
    float m_unkFloat4;
    float m_timeWarp;
    float m_queuedTimeWarp;
    float m_timeWarpRelated;
    float m_unkUint3;
    float m_totalTime;
    float m_levelTime;
    float m_unkDouble3;
    float m_levelFlipping;
    float m_unkFloat9;
    float m_unkUint10;
    float m_unkFloat10;
    float m_timeModRelated;
    float m_unkUint13;
    float m_unkUint14;
    float m_unkUint15;
    float m_unkUint16;
    float m_unkUint64_1;
    float m_unkUint17;

    int m_currentChannel;
    int m_rotateChannel;

    // Containers
    std::unordered_map<int, int> m_spawnChannelRelated0;
    std::unordered_map<int, bool> m_spawnChannelRelated1;
    std::unordered_map<int, GJValueTween> m_tweenActions;
    std::unordered_map<int, GameObjectPhysics> m_gameObjectPhysics;
    std::unordered_map<int, EnhancedGameObject*> m_stateObjects;
    std::unordered_map<int, std::vector<EnterEffectInstance>> m_enterEffectInstanceVectors;
    std::unordered_map<int, std::vector<EnterEffectInstance>> m_exitEffectInstanceVectors;
    std::unordered_map<int, SongChannelState> m_songChannelStates;
    std::unordered_map<int, std::vector<SongTriggerState>> m_songTriggerStateVectors;
    std::unordered_set<int> m_unsortedAreaEffects;
    std::unordered_map<int, std::vector<int>> m_unkUMap8;
    std::vector<SFXTriggerState> m_sfxTriggerStates;
    std::vector<float> m_unkVecFloat1;
    std::vector<int> m_enterChannelMap;
    std::vector<int> m_exitChannelMap;
    std::vector<EnterEffectInstance> m_moveEffectInstances;
    std::vector<EnterEffectInstance> m_rotateEffectInstances;
    std::vector<EnterEffectInstance> m_scaleEffectInstances;
    std::vector<EnterEffectInstance> m_fadeEffectInstances;
    std::vector<EnterEffectInstance> m_tintEffectInstances;
    std::vector<AdvancedFollowInstance> m_advanceFollowInstances;
    std::vector<DynamicObjectAction> m_dynamicMoveActions;
    std::vector<DynamicObjectAction> m_dynamicRotateActions;
    std::map<std::pair<GJGameEvent, int>, int> m_unkMapPairGJGameEventIntInt;
    std::map<std::pair<GJGameEvent, int>, std::vector<EventTriggerInstance>> m_unkMapPairGJGameEventIntVectorEventTriggerInstance;
    std::map<std::pair<int, int>, SFXTriggerInstance> m_proximityVolumeRelated;
    std::set<std::pair<int, int>> m_activatedObjectIDs;
};