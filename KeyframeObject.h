#pragma once

#include <vector>

struct SplineData {
    std::vector<double> m_x;
    std::vector<double> m_y;
    std::vector<double> m_b;
    std::vector<double> m_c;
    std::vector<double> m_d;
    int m_type;
    int m_left;
    int m_right;
    bool m_madeMonotonic;
    double m_c0;
    double m_leftValue;
    double m_rightValue;
};

class KeyframeObject {
public:
    void setupSpline(std::vector<KeyframeObject*>* objects);

    SplineData m_spline1;
    SplineData m_spline2;
    double m_unk170;
    double m_unk000;
    cocos2d::CCPoint m_unk188;
};