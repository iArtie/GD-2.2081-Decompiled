#include "Objects/KeyframeObject.h"
#include "tk_spline.h"

void KeyframeObject::setupSpline(std::vector<KeyframeObject*>* objects) {
    size_t count = objects->size();
    if (count <= 2) {
        return;
    }
    
    std::vector<double> xData(count);
    std::vector<double> yData(count);
    std::vector<double> timeData(count);
    
    for (size_t i = 0; i < count; ++i) {
        auto obj = (*objects)[i];
        if (i == 0) {
            xData[i] = 0.0;
            yData[i] = 0.0;
            timeData[i] = 0.0;
        } else {
            auto prev = (*objects)[i - 1];
            xData[i] = xData[i - 1] + obj->m_unk188.x;
            yData[i] = yData[i - 1] + obj->m_unk188.y;
            double dt = prev->m_unk000;
            if (dt < 0.00001) dt = 0.00001;
            timeData[i] = timeData[i - 1] + dt;
        }
    }
    
    tk::spline spline1;
    spline1.set_points(timeData, xData, 30);
    
    m_spline1.m_x = spline1.m_x;
    m_spline1.m_y = spline1.m_y;
    m_spline1.m_b = spline1.m_b;
    m_spline1.m_c = spline1.m_c;
    m_spline1.m_d = spline1.m_d;
    m_spline1.m_type = spline1.m_type;
    m_spline1.m_left = spline1.m_left;
    m_spline1.m_right = spline1.m_right;
    m_spline1.m_madeMonotonic = spline1.m_madeMonotonic;
    m_spline1.m_c0 = spline1.m_c0;
    m_spline1.m_leftValue = spline1.m_leftValue;
    m_spline1.m_rightValue = spline1.m_rightValue;
    
    tk::spline spline2;
    spline2.set_points(timeData, yData, 30);
    
    m_spline2.m_x = spline2.m_x;
    m_spline2.m_y = spline2.m_y;
    m_spline2.m_b = spline2.m_b;
    m_spline2.m_c = spline2.m_c;
    m_spline2.m_d = spline2.m_d;
    m_spline2.m_type = spline2.m_type;
    m_spline2.m_left = spline2.m_left;
    m_spline2.m_right = spline2.m_right;
    m_spline2.m_madeMonotonic = spline2.m_madeMonotonic;
    m_spline2.m_c0 = spline2.m_c0;
    m_spline2.m_leftValue = spline2.m_leftValue;
    m_spline2.m_rightValue = spline2.m_rightValue;
    
    m_unk170 = timeData.back();
}