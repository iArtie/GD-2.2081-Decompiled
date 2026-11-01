#include "Objects/ShaderGameObject.h"
#include "Layers/GJBaseGameLayer.h"

ShaderGameObject* ShaderGameObject::create(const char* frame)
{
    ShaderGameObject* instance = new ShaderGameObject();
    if (instance->init(frame))
    {
        instance->autorelease();
        return instance;
    }
    delete instance;
    return nullptr;
}

void ShaderGameObject::customSetup()
{
    EffectGameObject::customSetup();

    switch (m_objectID)
    {
        case 2923:
            m_waveWidth = 0.0f;
            m_targetX = 0.0f;
            m_targetY = 0.0f;
            break;
        case 2924:
            m_targetX = 0.0f;
            m_targetY = 0.0f;
            break;
        case 2922:
            m_strength = 0.0f;
            break;
        default:
            break;
    }
}

std::string ShaderGameObject::getSaveString(GJBaseGameLayer* layer)
{
    std::string baseString = EffectGameObject::getSaveString(layer);
    std::string result = baseString;

    auto appendInt = [&result](const char* key, int value) {
        result += ",";
        result += key;
        result += ",";
        result += std::to_string(value);
    };

    auto appendFloat = [&result](const char* key, float value) {
        result += ",";
        result += key;
        result += ",";
        result += cocos2d::CCString::createWithFormat("%.3f", value)->getCString();
    };

    if (m_speed != 0.0f)
        appendFloat("175", m_speed);

    if (m_strength != 0.0f)
        appendFloat("176", m_strength);

    if (m_outer != 0.0f)
        appendFloat("191", m_outer);

    if (m_timeOff != 0.0f)
        appendFloat("177", m_timeOff);

    if (m_screenOffsetX != 0.0f)
        appendFloat("290", m_screenOffsetX);

    if (m_screenOffsetY != 0.0f)
        appendFloat("291", m_screenOffsetY);

    if (m_waveWidth != 0.0f)
        appendFloat("179", m_waveWidth);

    if (m_targetX != 0.0f)
        appendFloat("180", m_targetX);

    if (m_targetY != 0.0f)
        appendFloat("189", m_targetY);

    if (m_flip)
        appendInt("185", m_flip);

    if (m_fadeIn != 0.0f)
        appendFloat("181", m_fadeIn);

    if (m_fadeOut != 0.0f)
        appendFloat("182", m_fadeOut);

    if (m_invert)
        appendInt("184", m_invert);

    if (m_rotate)
        appendInt("186", m_rotate);

    if (m_dual)
        appendInt("187", m_dual);

    if (m_useX)
        appendInt("188", m_useX);

    if (m_useY)
        appendInt("190", m_useY);

    if (m_snapGrid)
        appendInt("194", m_snapGrid);

    if (m_hardEdges)
        appendInt("515", m_hardEdges);

    if (m_disableAll)
        appendInt("192", m_disableAll);

    if (m_inner != 0.0f)
        appendFloat("183", m_inner);

    if (m_maxSize != 0.0f)
        appendFloat("512", m_maxSize);

    if (m_duration != 0.0f)
        appendFloat("10", m_duration);

    if (m_easingType)
        appendInt("30", m_easingType);

    if (m_easingRate != 0.0f)
        appendFloat("85", m_easingRate);

    if (m_targetGroupID)
        appendInt("51", m_targetGroupID);

    if (m_centerGroupID)
        appendInt("71", m_centerGroupID);

    if (m_targetPlayer1)
        appendInt("138", m_targetPlayer1);

    if (m_targetPlayer2)
        appendInt("200", m_targetPlayer2);

    if (m_followCPP)
        appendInt("201", m_followCPP);

    if (m_zLayerMin)
        appendInt("196", m_zLayerMin);

    if (m_zLayerMax)
        appendInt("197", m_zLayerMax);

    if (m_animate)
        appendInt("513", m_animate);

    if (m_relative)
        appendInt("514", m_relative);

    if (m_editorDisabled)
        appendInt("531", m_editorDisabled);

    return result;
}

ShaderGameObject::~ShaderGameObject()
{
}