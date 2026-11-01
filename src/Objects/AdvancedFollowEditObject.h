#pragma once

#include <string>

class GJBaseGameLayer;

class AdvancedFollowEditObject {
public:
    static AdvancedFollowEditObject* create(const char* frame);
    std::string getSaveString(GJBaseGameLayer* layer);
    ~AdvancedFollowEditObject();

private:
    double m_modX;
    double m_modXVariance;
    double m_modY;
    double m_modYVariance;
    int m_redirectDirection;
    int m_targetControlID;
};