#include "Layers/ParticlePreviewLayer.h"
#include "Utils/GameToolbox.h"

ParticlePreviewLayer* ParticlePreviewLayer::create(cocos2d::CCParticleSystemQuad* particleSystem)
{
    ParticlePreviewLayer* returnVal = new ParticlePreviewLayer();
    if (returnVal->init(particleSystem))
    {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

bool ParticlePreviewLayer::init(cocos2d::CCParticleSystemQuad* particleSystem)
{
    cocos2d::ccColor4B color = { 0, 0, 0, 255 };
    bool returnVal = cocos2d::CCLayerColor::initWithColor(color, 120.0f, 180.0f);
    if (returnVal)
    {
        m_particleSystem = particleSystem;
        addChild(particleSystem, 100);
    }
    return returnVal;
}

void ParticlePreviewLayer::draw()
{
    cocos2d::CCLayerColor::draw();

    bool drawGravity = false;
    bool drawSpeed = false;

    if (m_particleMode == 1)
    {
        drawGravity = (m_drawMode == 2);
        drawSpeed = (m_drawMode == 3);
    }
    else if (m_particleMode == 2)
    {
        drawSpeed = (m_drawMode == 3);
        drawGravity = true;
    }
    else if (m_particleMode == 3)
    {
        drawGravity = (m_drawMode == 2);
        drawSpeed = (m_drawMode == 3);
    }

    if (m_gravityMode)
    {
        drawGravity = true;
        if (m_drawMode == 1)
            drawGravity = false;
    }

    if (!m_gravityMode)
    {
        float baseAngle = -(float)m_particleSystem->getAngle() * 0.017453f;
        float angleVar;

        if (fabsf(m_particleSystem->getAngleVar()) < 180.0f)
        {
            float absAngleVar = fabsf(m_particleSystem->getAngleVar());
            angleVar = (absAngleVar * 0.017453f) * 2.0f;
        }
        else
        {
            angleVar = 6.2832f;
        }

        cocos2d::ccDrawColor4B(0, 255, 0, 255);

        float startRadius = m_particleSystem->getStartRadius();
        if (startRadius > 0.0f)
        {
            const cocos2d::CCPoint& pos = m_particleSystem->getPosition();
            float angle = -(baseAngle + angleVar * 0.5f) + 3.14159265f;
            cocos2d::ccDrawCircleSegment(pos, startRadius, angle, angleVar, 24, false, 1.0f, 1.0f);
        }

        cocos2d::ccDrawColor4B(255, 150, 0, 255);

        float endRadius = m_particleSystem->getEndRadius();
        if (endRadius > 0.0f)
        {
            const cocos2d::CCPoint& pos = m_particleSystem->getPosition();
            float angle = -(baseAngle + angleVar * 0.5f) + 3.14159265f;
            cocos2d::ccDrawCircleSegment(pos, endRadius, angle, angleVar, 24, false, 1.0f, 1.0f);
        }
        return;
    }

    if (m_drawMode == 1 || drawGravity)
    {
        cocos2d::ccDrawColor4B(0, 255, 0, 255);

        const cocos2d::CCPoint& pos = m_particleSystem->getPosition();
        const cocos2d::CCPoint& posVar = m_particleSystem->getPosVar();

        cocos2d::CCPoint topLeft = pos - posVar;
        cocos2d::CCPoint bottomRight = pos + posVar;

        cocos2d::ccDrawRect(topLeft, bottomRight);
    }

    if (drawGravity)
    {
        cocos2d::ccDrawColor4B(0, 255, 255, 255);

        const cocos2d::CCPoint& pos = m_particleSystem->getPosition();
        const cocos2d::CCPoint& gravity = m_particleSystem->getGravity();

        cocos2d::CCPoint gravityOffset = gravity * 0.1f;
        cocos2d::CCPoint gravityEnd = pos + gravityOffset;

        cocos2d::ccDrawLine(pos, gravityEnd);
    }

    if (drawSpeed)
    {
        cocos2d::ccDrawColor4B(255, 255, 0, 255);

        const cocos2d::CCPoint& pos = m_particleSystem->getPosition();

        float speed = m_particleSystem->getSpeed() * 0.25f;
        float angle = m_particleSystem->getAngle() * 0.017453f;

        float sinAngle = sinf(angle);
        float cosAngle = cosf(angle);

        cocos2d::CCPoint speedOffset(speed * cosAngle, speed * sinAngle);
        cocos2d::CCPoint speedEnd = pos + speedOffset;

        cocos2d::ccDrawLine(pos, speedEnd);

        float angleVar;
        if (fabsf(m_particleSystem->getAngleVar()) < 180.0f)
        {
            float absAngleVar = fabsf(m_particleSystem->getAngleVar());
            angleVar = (absAngleVar * 0.017453f) * 2.0f;
        }
        else
        {
            angleVar = 6.2832f;
        }

        const cocos2d::CCPoint& pos2 = m_particleSystem->getPosition();
        cocos2d::ccDrawCircleSegment(pos2, 20.0f, angle - angleVar * 0.5f, angleVar, 4, false, 1.0f, 1.0f);
    }
}

void ParticlePreviewLayer::visit()
{
    float anchorX = getAnchorPoint().x * -120.0f;
    float anchorY = getAnchorPoint().y * -180.0f;
    float scaleX = getScaleX();
    float scaleY = getScaleY();

    cocos2d::CCRect rect(anchorX, anchorY, scaleX * 120.0f, scaleY * 180.0f);

    GameToolbox::preVisitWithClipRect(rect);
    cocos2d::CCNode::visit();
    GameToolbox::postClipVisit();
}

void ParticlePreviewLayer::postVisit()
{
    glDisable(GL_SCISSOR_TEST);
}

ParticlePreviewLayer::~ParticlePreviewLayer()
{
}