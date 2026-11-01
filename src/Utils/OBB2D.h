#pragma once

#include "Platform/cocos2d.h"
#include <array>

class OBB2D : public cocos2d::CCNode {
public:
    static OBB2D* create(cocos2d::CCPoint center, float width, float height, float rotationAngle);

    bool init(cocos2d::CCPoint center, float width, float height, float rotationAngle);

    void calculateWithCenter(cocos2d::CCPoint center, float width, float height, float rotationAngle);
    void computeAxes();
    void orderCorners();
    cocos2d::CCRect getBoundingRect();
    bool overlaps();
    bool overlaps1Way();

    virtual ~OBB2D();

protected:
    std::array<cocos2d::CCPoint, 4> m_corners;
    std::array<cocos2d::CCPoint, 4> m_edges;
    std::array<cocos2d::CCPoint, 4> m_positions;
    cocos2d::CCPoint m_center;
    std::array<float, 2> m_projections;
};