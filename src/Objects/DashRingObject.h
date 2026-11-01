#pragma once

#include "Objects/RingObject.h"
#include <string>

class DashRingObject : public RingObject {
public:
    static DashRingObject* create(const char* frame);
    
    virtual std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~DashRingObject();

    // Getters and Setters
    float getDashSpeed() const { return m_dashSpeed; }
    void setDashSpeed(float speed) { m_dashSpeed = speed; }
    
    float getEndBoost() const { return m_endBoost; }
    void setEndBoost(float boost) { m_endBoost = boost; }
    
    bool getAllowCollide() const { return m_allowCollide; }
    void setAllowCollide(bool allow) { m_allowCollide = allow; }
    
    bool getStopSlide() const { return m_stopSlide; }
    void setStopSlide(bool stop) { m_stopSlide = stop; }
    
    float getMaxDuration() const { return m_maxDuration; }
    void setMaxDuration(float duration) { m_maxDuration = duration; }

protected:
    float m_dashSpeed;
    float m_endBoost;
    bool m_allowCollide;
    bool m_stopSlide;
    float m_maxDuration;
};