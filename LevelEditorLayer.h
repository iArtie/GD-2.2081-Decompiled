#pragma once

#include "cocos2d.h"
#include "GJBaseGameLayer.h"
#include "GJGameLevel.h"
#include "GameObject.h"
#include "EffectGameObject.h"
#include "PlayerObject.h"
#include "StartPosObject.h"
#include "EditorUI.h"
#include "DrawGridLayer.h"
#include "GJEffectManager.h"
#include "GJGroundLayer.h"
#include "GJMGLayer.h"
#include "ShaderLayer.h"
#include "LevelSettingsObject.h"
#include "OBB2D.h"
#include "UndoObject.h"
#include "DelayedSpawnNode.h"
#include "ColorAction.h"
#include "KeyframeGameObject.h"
#include "ParticleGameObject.h"
#include "TextGameObject.h"
#include "LabelGameObject.h"
#include "SFXTriggerGameObject.h"
#include "CheckpointGameObject.h"
#include <string>
#include <vector>
#include <unordered_set>

class LevelEditorLayer : public GJBaseGameLayer {
public:
    static LevelEditorLayer* create(GJGameLevel* level, bool noUI);
    static cocos2d::CCScene* scene(GJGameLevel* level, bool noUI);

    virtual bool init(GJGameLevel* level, bool noUI) override;
    virtual void onEnter() override;
    virtual ~LevelEditorLayer() override;

    void activatedAudioTrigger(SFXTriggerGameObject* object);
    void addDelayedSpawn(EffectGameObject* object, float delay);
    void addKeyframe(KeyframeGameObject* object);
    void addPlayerCollisionBlock();
    void addPlayerPoint(cocos2d::CCPoint point);
    void addSpecial(GameObject* object);
    void addToGroup(GameObject* object, int groupID, bool triggerGroup);
    void addToRedoList(UndoObject* object);
    void addToUndoList(UndoObject* object, bool keepRedo);
    void addTouchPoint(cocos2d::CCPoint point);
    void applyAttributeState(GameObject* dest, GameObject* src);
    void applyGroupState(GameObject* dest, GameObject* src);
    void breakApartTextObject(TextGameObject* object);
    bool canPasteState();
    void checkpointActivated(CheckpointGameObject* object);
    void clearPlayerPoints();
    void clearTouchPoints();
    void copyObjectState(GameObject* object);
    void copyParticleState(ParticleGameObject* object);
    void createObjectsFromSetup(std::string* setup);
    void didRotateGameplay();
    void dirtifyTriggers();
    cocos2d::CCArray* duplicateKeyframeAnimation(int keyframeGroup);
    void fastUpdateDisabledGroups();
    GameObject* findGameObject(int uniqueID);
    StartPosObject* findStartPosObject();
    void forceShowSelectedObjects(bool show);
    void fullUpdateDisabledGroups();
    cocos2d::CCArray* getAllObjects();
    DelayedSpawnNode* getDelayedSpawnNode();
    cocos2d::CCPoint getGridPos(cocos2d::CCPoint position);
    float getLastObjectX();
    std::string getLevelString();
    std::string getLockedLayers();
    int getNextColorChannel();
    int getNextFreeAreaEffectID(const std::unordered_set<int>* exclude);
    int getNextFreeBlockID(const std::unordered_set<int>* exclude);
    int getNextFreeEditorLayer(const std::unordered_set<int>* exclude);
    int getNextFreeEnterChannel(const std::unordered_set<int>* exclude);
    int getNextFreeGradientID(const std::unordered_set<int>* exclude);
    int getNextFreeGroupID(const std::unordered_set<int>* exclude);
    int getNextFreeItemID(const std::unordered_set<int>* exclude);
    int getNextFreeOrderChannel(const std::unordered_set<int>* exclude);
    int getNextFreeSFXGroupID(const std::unordered_set<int>* exclude);
    int getNextFreeSFXID(const std::unordered_set<int>* exclude);
    cocos2d::CCPoint getRelativeOffset(GameObject* object);
    std::string getSFXIDs();
    cocos2d::CCPoint getSavedEditorPosition(int index);
    std::string getSavedEditorPositions();
    int getSectionCount();
    int getSelectedEditorOrder();
    cocos2d::CCPoint getSelectedEffectPos();
    int getSelectedOrderChannel();
    std::string getSongIDs(bool* multipleSongs);
    cocos2d::CCArray* getTriggerGroup(int id);
    void handleAction(bool undo, cocos2d::CCArray* undoObjects);
    bool hasAction(bool redo);
    bool isLayerLocked(int layer);
    void levelSettingsUpdated();
    void manualUpdateObjectColors(GameObject* object);
    GameObject* objectAtPosition(cocos2d::CCPoint position);
    void objectMoved(GameObject* object);
    cocos2d::CCArray* objectsAtPosition(cocos2d::CCPoint position);
    void onPausePlaytest();
    void onPlaytest();
    void onResumePlaytest();
    void onStopPlaytest();
    void pasteAttributeState(GameObject* object, cocos2d::CCArray* objects);
    void pasteColorState(GameObject* object, cocos2d::CCArray* objects);
    void pasteGroupState(GameObject* object, cocos2d::CCArray* objects);
    void playerTookDamage(PlayerObject* player);
    cocos2d::CCPoint posForTime(float time);
    void postUpdate(float dt);
    void processLoadedMoveActions();
    void quickUpdateAllPositions();
    void recreateGroups();
    void redoLastAction();
    void refreshSpecial(GameObject* object);
    void removeAllObjects();
    void removeAllObjectsOfType(int objectID);
    void removeFromGroup(GameObject* object, int groupID);
    void removeObject(GameObject* object, bool noUndo);
    void removePlayerCollisionBlock();
    void removeSpecial(GameObject* object);
    void resetDelayedSpawnNodes();
    void resetEffectTriggerOptim(GameObject* object, cocos2d::CCArray* objects);
    void resetMovingObjects();
    void resetObjectVector();
    void resetPlayback();
    void resetSPTriggered();
    void resetToggledGroups();
    void resetToggledGroupsAndObjects();
    void resetUnusedColorChannels();
    void reverseKeyframeAnimationOrder(int keyframeGroup);
    void reverseObjectChanged(EffectGameObject* object);
    float rotationForSlopeNearObject(GameObject* object);
    void saveEditorPosition(cocos2d::CCPoint* position, int index);
    bool shouldBlend(int colorID);
    void sortBatchnodeChildren(float unused);
    void stopPlayback();
    void stopTriggersInGroup(int id, float delay);
    void timeObjectChanged();
    void toggleBackground(bool visible);
    void toggleGrid(bool visible);
    void toggleGround(bool enable);
    void toggleGroupPreview(int id, bool enable);
    void toggleLockActiveLayer();
    void transferDefaultColors(GJEffectManager* src, GJEffectManager* dest);
    void triggerFollowCommand(EffectGameObject* object);
    void triggerPlayerFollowCommand(EffectGameObject* object);
    void triggerRotateCommand(EffectGameObject* object);
    bool tryUpdateSpeedObject(EffectGameObject* object, bool noPreview);
    void undoLastAction();
    void unlockAllLayers();
    void updateAnimateOnTriggerObjects(bool animate);
    void updateArt(float dt);
    void updateBlendValues();
    void updateDebugDraw();
    void updateDisabledObjectsLastPos(cocos2d::CCArray* objects);
    void updateEditorMode();
    void updateGameObjects();
    void updateGridLayer();
    void updateGridLayerParent();
    void updateKeyframeObjects();
    void updateKeyframeVisibility(bool visible);
    void updateLevelFont(int index);
    void updateObjectColors(cocos2d::CCArray* gameObjects);
    static void updateObjectLabel(GameObject* object);
    void updateObjectSection(GameObject* object);
    void updateOptions();
    void updatePreviewAnim();
    void updatePreviewParticle(ParticleGameObject* object);
    void updatePreviewParticles();
    void updateToggledGroups();
    void updateVisibility(float dt);
    bool validGroup(GameObject* object, bool checkLockedLayers);

private:
    // Base game layer members
    GJGameLevel* m_level = nullptr;
    GJEffectManager* m_effectManager = nullptr;
    GJGroundLayer* m_groundLayer = nullptr;
    GJGroundLayer* m_groundLayer2 = nullptr;
    GJMGLayer* m_middleground = nullptr;
    ShaderLayer* m_shaderLayer = nullptr;
    cocos2d::CCSprite* m_background = nullptr;
    cocos2d::CCLayer* m_objectLayer = nullptr;
    cocos2d::CCNode* m_inShaderParent = nullptr;
    cocos2d::CCNode* m_aboveShaderParent = nullptr;
    cocos2d::CCLayer* m_inShaderObjectLayer = nullptr;
    cocos2d::CCLayer* m_aboveShaderObjectLayer = nullptr;
    cocos2d::CCSpriteBatchNode* m_gameLayerT1 = nullptr;
    cocos2d::CCArray* m_batchNodes = nullptr;
    cocos2d::CCArray* m_objects = nullptr;
    cocos2d::CCArray* m_undoObjects = nullptr;
    cocos2d::CCArray* m_redoObjects = nullptr;
    cocos2d::CCDictionary* m_triggerGroupsDict = nullptr;
    std::vector<cocos2d::CCArray*> m_triggerGroups;
    std::vector<GameObject*> m_objectVector;
    std::vector<bool> m_blendingColors;
    std::vector<bool> m_blendingColors2;
    std::vector<bool> m_lockedLayers;
    std::vector<bool> m_toggledGroups;
    std::vector<unsigned char> m_toggledGroupStates;
    std::vector<bool> m_sortedGroups;
    std::vector<float> m_spawnGroupDelays;
    std::vector<cocos2d::CCPoint> m_savedPositions;

    // Editor specific
    EditorUI* m_editorUI = nullptr;
    DrawGridLayer* m_drawGridLayer = nullptr;
    cocos2d::CCDrawNode* m_debugDrawNode = nullptr;
    LevelSettingsObject* m_levelSettings = nullptr;
    OBB2D* m_currentOBB2D = nullptr;

    // Players
    PlayerObject* m_player1 = nullptr;
    PlayerObject* m_player2 = nullptr;

    // Game state
    PlaybackMode m_playbackMode = PlaybackMode::Not;
    bool m_previewMode = false;
    bool m_alwaysPreviewMode = false;
    bool m_playbackActive = false;
    bool m_playTestSmoothFix = false;
    float m_playtestZoom = 1.0f;
    float m_playtestTimeWarp = 1.0f;
    float m_playtestCameraZoom = 1.0f;
    float m_trailTimer = 0.0f;

    // Triggers
    cocos2d::CCArray* m_colorTriggers = nullptr;
    cocos2d::CCArray* m_pulseTriggers = nullptr;
    cocos2d::CCArray* m_alphaTriggers = nullptr;
    cocos2d::CCArray* m_spawnTriggers = nullptr;
    cocos2d::CCArray* m_moveTriggers = nullptr;
    cocos2d::CCArray* m_cameraGuideTriggers = nullptr;
    cocos2d::CCArray* m_playtestTriggers = nullptr;
    cocos2d::CCArray* m_spawnOrderObjects = nullptr;
    cocos2d::CCArray* m_durationObjects = nullptr;
    cocos2d::CCArray* m_particleObjects = nullptr;
    cocos2d::CCArray* m_keyframeObjects = nullptr;
    cocos2d::CCDictionary* m_keyframeGroups = nullptr;
    cocos2d::CCArray* m_collisionBlocks = nullptr;

    // Spawn
    std::vector<DelayedSpawnNode*> m_delayedSpawnNodes;
    std::vector<DelayedSpawnNode*> m_delayedSpawnNodes2;
    bool m_sortSpawnNodes = false;

    // Player points
    std::vector<cocos2d::CCPoint> m_playerPoints;
    std::vector<cocos2d::CCPoint> m_player2Points;
    std::vector<cocos2d::CCPoint> m_touchPoints;

    // Counters
    struct IntRandom {
        int value = 0;
        int seed = 0;
        int random = 0;
    };
    IntRandom m_objectCount;
    IntRandom m_coinCount;

    // Section data
    struct SectionData {
        std::vector<std::vector<GameObject*>*> m_start;
        std::vector<std::vector<GameObject*>*> m_finish;
    };
    SectionData m_sections;
    std::vector<std::vector<int>*> m_sectionSizes;
    float m_sectionXFactor = 1.0f;
    float m_sectionYFactor = 1.0f;
    int m_leftSectionIndex = 0;
    int m_rightSectionIndex = 0;
    int m_bottomSectionIndex = 0;
    int m_topSectionIndex = 0;

    // Copy state
    GameObject* m_copyStateObject = nullptr;
    ParticleGameObject* m_particleObject = nullptr;

    // Playback
    EffectGameObject* m_playbackObject = nullptr;
    int m_playbackOrder = 0;
    cocos2d::CCPoint m_previewPosition;

    // Misc
    cocos2d::CCSprite* m_playtestDeathSprite = nullptr;
    StartPosObject* m_startPosObject = nullptr;
    cocos2d::CCArray* m_spawnObjectsArray = nullptr;
    std::vector<GameObject*> m_loadedMoveObjects;
    std::vector<GameObject*> m_activeObjects;
    int m_activeObjectsCount = 0;
    bool m_startOptimization = false;
    bool m_initializing = false;
    bool m_removingAll = false;
    bool m_layerLockingEnabled = false;
    bool m_increaseMaxUndoRedo = false;
    bool m_alwaysHideParticleIcons = false;
    bool m_hideParticleIcons = false;
    bool m_previewParticles = false;
    bool m_previewAnimations = false;
    bool m_previewShaders = false;
    bool m_ignoreDamage = false;
    bool m_drawTriggerBoxes = false;
    bool m_isDebugDrawEnabled = false;
    bool m_showGrid = false;
    bool m_drawEffectLines = false;
    bool m_showGround = false;
    bool m_showDurationLines = false;
    bool m_hideBackground = false;
    bool m_hideGridOnPlay = false;
    bool m_playtestOrderChanged = false;
    bool m_gv0120 = false;
    bool m_isEditor = false;
    bool m_activateAudioTriggers = false;
    bool m_updateColorSprites = false;
    bool m_unk3b22 = false;
    bool m_colorTriggersChanged = false;
    bool m_pulseTriggersChanged = false;
    bool m_alphaTriggersChanged = false;
    bool m_spawnTriggersChanged = false;
    bool m_spawnOrderObjectsChanged = false;
    bool m_nonSquareRotation = false;
    bool m_playerDied = false;
    bool m_soloMode = false;
    int m_currentLayer = 0;
    int m_duplicateKeyframeGroup = 0;
    float m_loadingProgress = 0.0f;
    float m_timePlayed = 0.0f;
    float m_unkFloat = 0.0f;

    // Large arrays for debug
    cocos2d::CCPoint* m_debugDrawPoints = nullptr;
};