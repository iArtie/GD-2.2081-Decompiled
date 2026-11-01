#include "Utils/OBB2D.h"

OBB2D* OBB2D::create(cocos2d::CCPoint center, float width, float height, float rotationAngle)
{
    OBB2D* returnVal = new OBB2D();
    if (returnVal->init(center, width, height, rotationAngle))
    {
        returnVal->autorelease();
    }
    else
    {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool OBB2D::init(cocos2d::CCPoint center, float width, float height, float rotationAngle)
{
    bool returnVal = cocos2d::CCNode::init();
    if (returnVal)
    {
        calculateWithCenter(center, width, height, rotationAngle);
    }
    return returnVal;
}

void OBB2D::calculateWithCenter(cocos2d::CCPoint center, float width, float height, float rotationAngle)
{
    m_center = center;

    float cosA = cosf(rotationAngle);
    float sinA = sinf(rotationAngle);

    cocos2d::CCPoint axisX(cosA, sinA);
    cocos2d::CCPoint axisY(-sinA, cosA);

    axisX = axisX * (width * 0.5f);
    axisY = axisY * (height * 0.5f);

    m_corners[0] = center - axisX - axisY;
    m_corners[1] = center + axisX - axisY;
    m_corners[2] = center + axisX + axisY;
    m_corners[3] = center - axisX + axisY;

    computeAxes();
    orderCorners();
}

void OBB2D::computeAxes()
{
    m_edges[0] = m_corners[1] - m_corners[0];
    m_edges[1] = m_corners[3] - m_corners[0];

    for (int i = 0; i < 2; i++)
    {
        float lenSq = m_edges[i].x * m_edges[i].x + m_edges[i].y * m_edges[i].y;
        m_edges[i] = m_edges[i] * (1.0f / lenSq);
        m_projections[i] = m_corners[0].x * m_edges[i].x + m_corners[0].y * m_edges[i].y;
    }
}

void OBB2D::orderCorners()
{
    cocos2d::CCPoint topLeft;
    cocos2d::CCPoint topRight;
    cocos2d::CCPoint bottomLeft;
    cocos2d::CCPoint bottomRight;

    topLeft = m_corners[0];
    topRight = m_corners[0];

    for (int i = 1; i < 4; i++)
    {
        if (m_corners[i].y > topLeft.y ||
            (m_corners[i].y == topLeft.y && m_corners[i].x < topLeft.x))
        {
            topRight = topLeft;
            topLeft = m_corners[i];
        }
        else if (m_corners[i].y > topRight.y ||
                 (m_corners[i].y == topRight.y && m_corners[i].x < topRight.x))
        {
            topRight = m_corners[i];
        }
    }

    if (topLeft.x <= topRight.x)
    {
        m_positions[0] = topLeft;
        m_positions[1] = topRight;
    }
    else
    {
        m_positions[0] = topRight;
        m_positions[1] = topLeft;
    }

    bottomLeft = m_corners[0];
    bottomRight = m_corners[0];

    for (int i = 1; i < 4; i++)
    {
        if (m_corners[i].y < bottomLeft.y ||
            (m_corners[i].y == bottomLeft.y && m_corners[i].x > bottomLeft.x))
        {
            bottomRight = bottomLeft;
            bottomLeft = m_corners[i];
        }
        else if (m_corners[i].y < bottomRight.y ||
                 (m_corners[i].y == bottomRight.y && m_corners[i].x > bottomRight.x))
        {
            bottomRight = m_corners[i];
        }
    }

    if (bottomLeft.x > bottomRight.x)
    {
        m_positions[2] = bottomLeft;
        m_positions[3] = bottomRight;
    }
    else
    {
        m_positions[2] = bottomRight;
        m_positions[3] = bottomLeft;
    }
}

cocos2d::CCRect OBB2D::getBoundingRect()
{
    float maxX = 0.0f;
    float minX = 0.0f;
    float maxY = 0.0f;
    float minY = 0.0f;

    for (int i = 0; i < 4; i++)
    {
        cocos2d::CCPoint p = m_corners[i];

        if (p.x > maxX)
            maxX = p.x;
        if (p.x < minX || minX == 0.0f)
            minX = p.x;
        if (p.y > maxY)
            maxY = p.y;
        if (p.y < minY || minY == 0.0f)
            minY = p.y;
    }

    return cocos2d::CCRect(minX, minY, maxX - minX, maxY - minY);
}

bool OBB2D::overlaps()
{
    return overlaps1Way() && OBB2D::overlaps1Way();
}

bool OBB2D::overlaps1Way()
{
    return false;
}

OBB2D::~OBB2D()
{
}