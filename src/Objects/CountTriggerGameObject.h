#pragma once

#include "Platform/cocos2d.h"
#include "Objects/EffectGameObject.h"
#include "Layers/GJBaseGameLayer.h"
#include <string>

class CountTriggerGameObject : public EffectGameObject {
public:
    static CountTriggerGameObject* create(const char* frame);
    
    virtual std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~CountTriggerGameObject();

    // Getters/Setters for trigger properties
    int getTargetGroupID() const { return m_targetGroupID; }
    void setTargetGroupID(int id) { m_targetGroupID = id; }
    
    int getItemID() const { return m_itemID; }
    void setItemID(int id) { m_itemID = id; }
    
    int getPickupCount() const { return m_pickupCount; }
    void setPickupCount(int count) { m_pickupCount = count; }
    
    int getActivateGroup() const { return m_activateGroup; }
    void setActivateGroup(int group) { m_activateGroup = group; }
    
    int getPickupTriggerMode() const { return m_pickupTriggerMode; }
    void setPickupTriggerMode(int mode) { m_pickupTriggerMode = mode; }
    
    bool getIsOverride() const { return m_isOverride; }
    void setIsOverride(bool override) { m_isOverride = override; }
    
    float getPickupTriggerMultiplier() const { return m_pickupTriggerMultiplier; }
    void setPickupTriggerMultiplier(float multiplier) { m_pickupTriggerMultiplier = multiplier; }
    
    bool getMultiActivate() const { return m_multiActivate; }
    void setMultiActivate(bool multi) { m_multiActivate = multi; }

protected:
    // Trigger properties
    int m_targetGroupID;           // Target group to affect
    int m_itemID;                  // Item ID to count
    int m_pickupCount;             // Number of pickups needed
    int m_activateGroup;           // Group to activate
    int m_pickupTriggerMode;       // Trigger mode (0=equal, 1=greater, etc.)
    bool m_isOverride;             // Whether to override existing value
    float m_pickupTriggerMultiplier; // Multiplier for pickup value
    bool m_multiActivate;          // Whether to activate multiple times
};