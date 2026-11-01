// EffectManagerState.h
#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>

struct TimerItem;
struct TimerTriggerAction;
struct KeyframeObject;
struct GroupCommandObject2;
struct OpacityEffectAction;
struct SpawnTriggerAction;
struct ToggleTriggerAction;
struct CollisionTriggerAction;
struct TouchToggleAction;
struct CountTriggerAction;
struct PulseEffectAction;
struct CAState;

class EffectManagerState {
public:
    ~EffectManagerState();

private:
    std::unordered_map<int, std::vector<TimerTriggerAction>> m_timerTriggerActionMap;
    std::unordered_map<int, TimerItem> m_timerItemMap;
    std::unordered_set<int> m_activeTimerIds;
    std::unordered_map<int, std::pair<double, double>> m_pairMap;
    std::vector<GroupCommandObject2> m_groupCommandObjects;
    std::unordered_map<int, bool> m_boolMap;
    std::unordered_map<int, int> m_itemCountMap;
    std::vector<SpawnTriggerAction> m_spawnTriggerActions;
    std::vector<ToggleTriggerAction> m_toggleTriggerActions;
    std::vector<CollisionTriggerAction> m_collisionTriggerActions;
    std::vector<TouchToggleAction> m_touchToggleActions;
    std::unordered_map<int, OpacityEffectAction> m_opacityEffectActionMap;
    std::map<int, int> m_intMap;
    std::unordered_set<int> m_activeEffectIds;
    std::unordered_map<int, std::vector<CountTriggerAction>> m_countTriggerActionMap;
    std::unordered_map<int, std::vector<PulseEffectAction>> m_pulseEffectActionMap;
    std::vector<PulseEffectAction> m_unknownPulseEffects;
    std::vector<CAState> m_unknownCAStates;
};