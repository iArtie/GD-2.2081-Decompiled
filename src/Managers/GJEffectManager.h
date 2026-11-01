// GJEffectManager.h
#pragma once

#include "Platform/cocos2d.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

class ColorAction;
class ColorActionSprite;
class ColorActionSprite;
class GroupCommandObject2;
class CCMoveCNode;
class OpacityEffectAction;
class PulseEffectAction;
class SpawnTriggerAction;
class TouchToggleAction;
class ToggleTriggerAction;
class CollisionTriggerAction;
class CountTriggerAction;
class TimerTriggerAction;
class TriggerEffectDelegate;
class InheritanceNode;
class EffectManagerState;
class TimerItem;
class CAState;

enum class GJActionCommand {
    Stop = 0,
    Pause = 1,
    Resume = 2
};

class GJEffectManager : public cocos2d::CCNode {
public:
    static GJEffectManager* create();
    bool init();
    virtual ~GJEffectManager();

    cocos2d::ccColor3B activeColorForIndex(int index);
    float activeOpacityForIndex(int index);
    void addAllInheritedColorActions(cocos2d::CCArray* actions);
    void addCountToItem(int itemId, int value);
    void calculateBaseActiveColors();
    void calculateInheritedColor(int id, ColorAction* action);
    bool checkCollision(const int* blockAID, const int* blockBID);
    void colorActionChanged(ColorAction* action);
    bool colorExists(int col);
    cocos2d::ccColor3B colorForIndex(int index);
    void controlActionsForControlID(int id, GJActionCommand command);
    int countForItem(int id);
    cocos2d::CCArray* getAllColorActions();
    cocos2d::CCArray* getAllColorSprites();
    ColorAction* getColorAction(int index);
    ColorActionSprite* getColorSprite(int index);
    static cocos2d::ccColor3B getMixedColor(cocos2d::ccColor3B color1, cocos2d::ccColor3B color2, float ratio);
    CCMoveCNode* getMoveCommandNode(GroupCommandObject2* command);
    CCMoveCNode* getMoveCommandObject();
    OpacityEffectAction* getOpacityActionForGroup(int id);
    std::string getPersistentStateString();
    std::string getSaveString();
    GroupCommandObject2* getTempGroupCommand();
    void handleObjectCollision(bool triggerOnExit, int blockAID, int blockBID);
    bool hasActiveDualTouch();
    bool hasBeenTriggered(int objectUniqueID, int playerUniqueID);
    bool hasPulseEffectForGroupID(int id);
    bool isGroupEnabled(int id);
    void itemWasUsed(int id);
    void loadFromState(EffectManagerState* state);
    void loadPersistentStateString(std::string state);
    void objectsCollided(int blockAID, int blockBID);
    float opacityForIndex(int index);
    float opacityModForGroup(int id);
    void pauseTimer(int id);
    void playerButton(bool down, bool player1);
    void playerDied();
    void postCollisionCheck();
    void postMoveActions();
    void preCollisionCheck();
    void prepareMoveActions(float dt, bool intermediate);
    void processColors();
    void processCopyColorPulseActions();
    void processInheritedColors();
    void processMoveCalculations();
    void processPulseActions();
    void registerRotationCommand(GroupCommandObject2* object, bool clear);
    void removeAllPulseActions();
    void removeColorAction(int id);
    void removePersistentFromAllItems();
    void removePersistentFromAllTimers();
    void removeTriggeredID(int objectUniqueID, int playerUniqueID);
    void reset();
    void resetEffects();
    void resetMoveActions();
    void resetTempGroupCommands(bool noRemove);
    void resetToggledGroups();
    void resetTriggeredIDs();
    void resetUsedItemState();
    void resumeTimer(int id);
    void saveCompletedMove(int groupId, double dx, double dy);
    void saveToState(EffectManagerState* state);
    void setColorAction(ColorAction* action, int id);
    void setFollowing(int targetID, int targetPosID, bool following);
    void setupFromString(std::string str);
    bool shouldBlend(int id);
    void storeTriggeredID(int objectUniqueID, int playerUniqueID);
    double timeForItem(int id);
    bool timerExists(int id);
    void timerWasUsed(int id);
    void toggleGroup(int id, bool activate);
    void toggleItemPersistent(int id, bool persistent);
    void toggleTimerPersistent(int id, bool persistent);
    void transferPersistentItems();
    void traverseInheritanceChain(InheritanceNode* node);
    CCMoveCNode* tryGetMoveCommandNode(int id);
    void updateActiveOpacityEffects();
    void updateColorAction(ColorAction* action);
    void updateColorEffects(float dt);
    void updateCountForItem(int id, int count);
    void updateEffects(float dt);
    void updateOpacityAction(OpacityEffectAction* action);
    void updateOpacityEffects(float dt);
    void updatePulseEffects(float dt);
    void updateSpawnTriggers(float dt);
    void updateTimer(int id, double time);
    void updateTimers(float dt, float timeWarp);
    bool wasFollowing(int targetID, int targetPosID);
    bool wouldCreateLoop(InheritanceNode* node, int id);

private:
    cocos2d::CCDictionary* m_unkObject148;
    cocos2d::CCDictionary* m_unkDict150;
    std::vector<ColorAction*> m_colorActionVector;
    cocos2d::CCDictionary* m_colorActionDict;
    std::unordered_map<int, bool> m_unkMap288;
    std::unordered_map<int, CCMoveCNode*> m_unkMap618;
    std::vector<DynamicMoveCalculation> m_unkVector708;
    std::unordered_map<int, OpacityEffectAction> m_opacityEffectMap;
    std::vector<PulseEffectAction> m_pulseEffectVector;
    std::unordered_map<int, std::vector<PulseEffectAction>> m_pulseEffectMap;
    std::vector<bool> m_unkVector2f0;
    std::vector<SpawnTriggerAction> m_spawnTriggerActions;
    TriggerEffectDelegate* m_triggerEffectDelegate;
    std::set<std::pair<int, int>> m_unkMap498;
    std::vector<bool> m_unkVector438;
    std::unordered_set<int> m_unkMap460;
    std::vector<TouchToggleAction> m_unkVector1e0;
    std::vector<ToggleTriggerAction> m_unkVector248;
    std::unordered_map<int, int> m_itemCountMap;
    std::unordered_set<int> m_unk3f0;
    std::unordered_set<int> m_unk430;
    std::unordered_map<int, int> m_persistentItemCountMap;
    std::unordered_set<int> m_persistentTimerItemSet;
    std::unordered_map<int, TimerItem> m_timerItemMap;
    std::unordered_set<int> m_unkMap4c8;
    std::vector<CCMoveCNode*> m_unkVector548;
    std::vector<GroupCommandObject2*> m_unkVector530;
    std::vector<GroupCommandObject2*> m_unkVector518;
    std::unordered_map<int, std::vector<TimerTriggerAction>> m_unkMap3f8;
    std::vector<CCMoveCNode*> m_unkVector6c0;
    std::unordered_map<int, std::pair<double, double>> m_unkMap578;
    std::vector<GroupCommandObject2> m_unkVector560;
    std::unordered_map<int, std::vector<CountTriggerAction>> m_countTriggerActions;
    std::vector<CollisionTriggerAction> m_unkVector230;
    std::map<std::pair<int, int>, std::vector<GroupCommandObject2*>> m_unkMap770;
    std::vector<GroupCommandObject2*> m_unkVector5b0;
    std::unordered_map<int, std::vector<GroupCommandObject2*>> m_unkMap5c8;
    std::vector<GroupCommandObject2*> m_unkVector600;
    std::unordered_map<int, CCMoveCNode*> m_unkMap650;
    std::unordered_map<int, CCMoveCNode*> m_unkMap688;
    std::vector<CCMoveCNode*> m_unkVector6d8;
    std::vector<CCMoveCNode*> m_unkVector6f0;
    float m_unk780;
    float m_unk788;
    float m_unk784;
    float m_unk78C;
    bool m_unk798;
    float m_unk790;
    float m_unk794;
    cocos2d::CCDictionary* m_unkDict268;
    std::vector<InheritanceNode*> m_unkVector270;
    std::vector<ColorActionSprite*> m_colorActionSpriteVector;
    int m_size;
    cocos2d::CCArray* m_unkArray430;
};