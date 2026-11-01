#include "ShaderLayer.h"
#include "GameManager.h"

static cocos2d::CCPoint pixelOffset;

ShaderLayer* ShaderLayer::create() {
    ShaderLayer* layer = new ShaderLayer();
    if (layer->init()) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool ShaderLayer::init() {
    if (!cocos2d::CCLayer::init())
        return false;

    GameManager* gameManager = GameManager::sharedState();
    setupShader(gameManager->m_shouldResetShader);
    double value = (float)((float)((float)((float)rand() * 4.6566e-10) * 100.0) + 10000.0);
    m_state.m_startTime = value;
    m_state.m_time = value;
    updateZLayer(0, 0, false);
    m_state.m_zLayerDirty = true;
    return true;
}

cocos2d::CCPoint ShaderLayer::objectPosToShaderPos(cocos2d::CCPoint position) {
    cocos2d::CCPoint point3;
    cocos2d::CCPoint point4;
    cocos2d::CCPoint returnVal;

    cocos2d::CCNode::convertToWorldSpace(m_gameLayer->m_objectLayer, &position);
    cocos2d::CCNode::convertToNodeSpace(this, &point3);
    point4 = point3;
    cocos2d::CCPoint result(
        point3.x / m_targetTextureSize.width,
        point3.y / m_targetTextureSize.height);
    result = result * m_scaleFactor;
    float value = result.x / m_state.m_textureScaleX;
    float value2 = result.y / m_state.m_textureScaleY;
    returnVal.y = value;
    returnVal.x = value2;
    return returnVal;
}

void ShaderLayer::performCalculations() {
    double m_time = m_state.m_time;
    if (m_state.m_prevTime != m_time) {
        m_state.m_usesShaders = false;
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        m_state.m_prevTime = m_time;
        cocos2d::CCGLProgram::use(cCGLProgram);
        prePixelateShader();
        preCommonShader();
        preShockWaveShader();
        preShockLineShader();
        preGlitchShader();
        preChromaticShader();
        preChromaticGlitchShader();
        preLensCircleShader();
        preRadialBlurShader();
        preBulgeShader();
        prePinchShader();
        preMotionBlurShader();
        preGrayscaleShader();
        preSepiaShader();
        preInvertColorShader();
        preHueShiftShader();
        preColorChangeShader();
        preSplitScreenShader();
    }
}

void ShaderLayer::preBulgeShader() {
    float m_bulgeValue = m_state.m_bulgeValue;
    if (m_bulgeValue > 0.0f || cocos2d::CCNode::getActionByTag(this, 21)) {
        m_state.m_usesShaders = true;
    }
    if (m_bulgeValue <= 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_bulgeValueUniform, 0.0f);
    }
}

void ShaderLayer::preChromaticGlitchShader() {
    if (!m_state.m_cGEnable ||
        (m_state.m_cGRGBOffset == 0.0f &&
         m_state.m_cGStrength == 0.0f &&
         m_state.m_cGLineStrength == 0.0f)) {
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_cGRGBOffsetUniform, 0.0f);
        if (m_state.m_cGEnable &&
            (cocos2d::CCNode::getActionByTag(this, 4) ||
             cocos2d::CCNode::getActionByTag(this, 5) ||
             cocos2d::CCNode::getActionByTag(this, 6) ||
             cocos2d::CCNode::getActionByTag(this, 3))) {
            m_state.m_cGActive = false;
            m_state.m_usesShaders = true;
        } else {
            m_state.m_cGActive = false;
        }
    } else {
        bool m_cGRelative = m_state.m_cGRelative;
        m_state.m_usesShaders = true;
        m_state.m_cGActive = true;
        float value;
        if (m_cGRelative && m_gameLayer != nullptr) {
            float scale = m_gameLayer->m_objectLayer->getScale();
            value = fabsf(scale);
        } else {
            value = 1.0f;
        }
        double m_time = m_state.m_time;
        float m_cGTime = m_state.m_cGTime;
        float m_cGSpeed = m_state.m_cGSpeed;
        double value2 = m_time - m_state.m_cGStartTime;
        float value3 = m_state.m_cGRGBOffset * 0.1f;
        float value4 = value * m_shockWaveTimeMult;
        m_state.m_cGStartTime = (float)m_time;
        float value5 = (double)((int)(float)(m_cGTime + (float)(m_cGSpeed * (float)value2)) % 1000)
            + (float)((float)(m_cGTime + (float)(m_cGSpeed * (float)value2))
                    - (float)(int)(float)(m_cGTime + (float)(m_cGSpeed * (float)value2)));
        m_state.m_cGTime = value5;
        if ((float)(m_state.m_cGRGBOffset * 0.1f) == 0.0f)
            value3 = 0.001f;
        float value6 = 1.0f - (float)((float)(value4 * m_state.m_cGLineThickness) * 0.2f);
        if (value6 <= 0.001f)
            value6 = 0.001f;
        if (m_state.m_cGLineStrength == 0.0f)
            value6 = 0.0f;
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram2,
            m_cGRGBOffsetUniform,
            (float)((float)(value3 * 0.1f) * m_state.m_textureScaleX) * value4);
        cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_cGTimeUniform, value5);
        cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram4,
            m_cGStrengthUniform,
            (float)((float)(m_state.m_cGStrength * 0.02f) * m_state.m_textureScaleX) * value4);
        cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram5,
            m_cGHeightUniform,
            m_state.m_cGSegmentHeight / (float)((float)(value4 * m_state.m_textureScaleY) * m_aspectRatio));
        cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram6, m_cGLineThickUniform, value6);
        cocos2d::CCGLProgram* cCGLProgram7 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram7,
            m_cGLineStrengthUniform,
            m_state.m_cGLineStrength * 0.05f);
    }
}

void ShaderLayer::preChromaticShader() {
    if (m_state.m_chromaticTargetX != 0.0f ||
        m_state.m_chromaticTargetY != 0.0f ||
        cocos2d::CCNode::getActionByTag(this, 1) ||
        cocos2d::CCNode::getActionByTag(this, 2)) {
        m_state.m_usesShaders = true;
    }
    float value;
    if (m_state.m_chromaticRelative && m_gameLayer != nullptr)
        value = fabsf(m_gameLayer->m_objectLayer->getScale());
    else
        value = 1.0f;
    float value2 = (float)(m_scaleFactor / 1.2019f) / m_targetTextureSize.height;
    float value3 = (float)(m_scaleFactor / 1.2019f) / m_targetTextureSize.width;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram,
        m_chromaticXOffUniform,
        value * (float)((float)(value3 * m_state.m_chromaticTargetX) * m_state.m_textureScaleX));
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram2,
        m_chromaticYOffUniform,
        value * (float)((float)(value2 * m_state.m_chromaticTargetY) * m_state.m_textureScaleY));
}

void ShaderLayer::preColorChangeShader() {
    if (m_state.m_colorChangeCR == 1.0f &&
        m_state.m_colorChangeCG == 1.0f &&
        m_state.m_colorChangeCB == 1.0f &&
        m_state.m_colorChangeBR == 0.0f &&
        m_state.m_colorChangeBG == 0.0f &&
        m_state.m_colorChangeBB == 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith3f(cCGLProgram, m_colorChangeCUniform, 0.0f, 0.0f, 0.0f);
        if (cocos2d::CCNode::getActionByTag(this, 42) ||
            cocos2d::CCNode::getActionByTag(this, 43) ||
            cocos2d::CCNode::getActionByTag(this, 44) ||
            cocos2d::CCNode::getActionByTag(this, 39) ||
            cocos2d::CCNode::getActionByTag(this, 40) ||
            cocos2d::CCNode::getActionByTag(this, 41)) {
            m_state.m_usesShaders = true;
        }
    } else {
        m_state.m_usesShaders = true;
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        float m_colorChangeCR = m_state.m_colorChangeCR;
        if (m_colorChangeCR <= 0.001f)
            m_colorChangeCR = 0.001f;
        cocos2d::CCGLProgram::setUniformLocationWith3f(
            cCGLProgram2,
            m_colorChangeCUniform,
            m_colorChangeCR,
            m_state.m_colorChangeCG,
            m_state.m_colorChangeCB);
        cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith3f(
            cCGLProgram3,
            m_colorChangeBUniform,
            m_state.m_colorChangeBR,
            m_state.m_colorChangeBG,
            m_state.m_colorChangeBB);
    }
}

void ShaderLayer::preCommonShader() {
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith3f(
        cCGLProgram,
        m_blurRefColorUniform,
        (float)m_state.m_blurRefColor.r / 255.0f,
        (float)m_state.m_blurRefColor.g / 255.0f,
        (float)m_state.m_blurRefColor.b / 255.0f);
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    float value = 0.0f;
    if (m_state.m_minBlendingLayer > 1)
        value = 1.0f;
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_blurUseRefUniform, value);
    cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_blurIntensityUniform, m_state.m_blurIntensity + 1.0f);
    cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith2f(
        cCGLProgram4,
        m_textureScaleUniform,
        m_state.m_textureScaleX,
        m_state.m_textureScaleY);
    cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith2f(
        cCGLProgram5,
        m_textureScaleInvUniform,
        1.0f / m_state.m_textureScaleX,
        1.0f / m_state.m_textureScaleY);
    cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
    float value2;
    if (m_state.m_minBlendingLayer <= 1) {
        value2 = 0.0f;
    } else {
        value2 = 0.0f;
        if (m_state.m_blurOnlyEmpty)
            value2 = 1.0f;
    }
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram6, m_blurOnlyEmptyUniform, value2);
    float value3 = m_screenSize.height / m_screenSize.width;
    cocos2d::CCGLProgram* cCGLProgram7 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram7, m_screenAspectINVUniform, value3);
    cocos2d::CCGLProgram* cCGLProgram8 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram8, m_shaderPositionUniform, 1.0f / value3);
}

void ShaderLayer::preGlitchShader() {
    if (m_state.m_glitchStrength <= 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram6, m_glitchTopUniform, 0.0f);
    } else {
        bool m_glitchRelative = m_state.m_glitchRelative;
        m_state.m_usesShaders = true;
        float value;
        if (m_glitchRelative && m_gameLayer != nullptr)
            value = fabsf(m_gameLayer->m_objectLayer->getScale());
        else
            value = 1.0f;
        float value2 = value * m_shockWaveTimeMult;
        float value3 = floor(m_state.m_glitchSpeed * m_state.m_time * 20.0f);
        cocos2d::CCPoint point2;
        cocos2d::CCPoint::CCPoint(&point2, value3, 2345.0f);
        float value4 = (float)(random2d((float*)&point2) * m_state.m_textureScaleY) * value2;
        cocos2d::CCPoint::CCPoint(&point2, value3, 9035.0f);
        float value5 = random2d((float*)&point2);
        float value6 = (float)(value4
                   + (float)((float)((float)((float)(value5 * m_state.m_glitchSliceHeight) * m_state.m_glitchStrength)
                                   * m_state.m_textureScaleY)
                           * value2))
           + floorf(
               value4
             + (float)((float)((float)((float)(value5 * m_state.m_glitchSliceHeight) * m_state.m_glitchStrength)
                             * m_state.m_textureScaleY)
                     * value2));
        cocos2d::CCPoint::CCPoint(&point2, value3, 9625.0f);
        randomRange(&point2);
        float value72 = value7;
        cocos2d::CCPoint point;
        cocos2d::CCPoint::CCPoint(&point, value3, 9545.0f);
        randomRange(&point);
        float value82 = value8;
        cocos2d::CCPoint::CCPoint(&point2, value3, 7205.0f);
        randomRange(&point2);
        cocos2d::CCPoint p1;
        cocos2d::CCPoint::CCPoint(&p1, value82, value9);
        cocos2d::CCPoint::CCPoint(&point2, value3, 9545.0f);
        float value10 = random2d((float*)&point2);
        float value102 = value10;
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_glitchBotUniform, value4 * m_aspectRatio);
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_glitchTopUniform, value6 * m_aspectRatio);
        cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_glitchXOffsetUniform, value72);
        cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram4, m_glitchColOffsetUniform, p1.x, m_aspectRatio * p1.y);
        cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram5, m_glitchRndUniform, value102);
    }
}

void ShaderLayer::preGrayscaleShader() {
    if (m_state.m_grayscaleValue > 0.0f || cocos2d::CCNode::getActionByTag(this, 32))
        m_state.m_usesShaders = true;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_grayscaleValueUniform, m_state.m_grayscaleValue);
    if (m_state.m_grayscaleValue > 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram2,
            m_grayscaleUseLumUniform,
            (float)m_state.m_grayscaleUseLum);
        cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith3f(
            cCGLProgram3,
            m_grayscaleTintUniform,
            (float)m_state.m_grayscaleTint.r / 255.0f,
            (float)m_state.m_grayscaleTint.g / 255.0f,
            (float)m_state.m_grayscaleTint.b / 255.0f);
    }
}

void ShaderLayer::preHueShiftShader() {
    float m_hueShiftDegrees = m_state.m_hueShiftDegrees;
    if (m_hueShiftDegrees != 0.0f || cocos2d::CCNode::getActionByTag(this, 38)) {
        m_state.m_usesShaders = true;
    }
    float value = m_hueShiftDegrees * 0.017453f;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    float value2 = cosf(value);
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_hueShiftCosAUniform, value2);
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    float value3 = sinf(value);
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_hueShiftSinAUniform, value3);
}

void ShaderLayer::preInvertColorShader() {
    float m_invertColorEditRGB = m_state.m_invertColorEditRGB;
    if (m_invertColorEditRGB > 0.0f || cocos2d::CCNode::getActionByTag(this, 34)) {
        m_state.m_usesShaders = true;
    }
    float value = m_invertColorEditRGB * m_state.m_invertColorR;
    float value2 = m_invertColorEditRGB * m_state.m_invertColorG;
    float value3 = m_invertColorEditRGB * m_state.m_invertColorB;
    if (m_state.m_invertColorClampRGB) {
        if (value >= 1.0f)
            value = 1.0f;
        if (value2 >= 1.0f)
            value2 = 1.0f;
        if (value3 >= 1.0f)
            value3 = 1.0f;
    }
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith4f(
        cCGLProgram,
        m_invertColorValueUniform,
        value,
        value2,
        value3,
        m_state.m_invertColorEditRGB);
}

void ShaderLayer::preLensCircleShader() {
    float m_lensCircleStrength = m_state.m_lensCircleStrength;
    if (m_lensCircleStrength > 0.0f || cocos2d::CCNode::getActionByTag(this, 13)) {
        m_state.m_usesShaders = true;
    }
    if (m_lensCircleStrength <= 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
        float m_lensCircleAdditive = 0.0f;
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram5, m_lensCircleAdditiveUniform, m_lensCircleAdditive);
        return;
    }
    float m_lensCircleFade;
    float value;
    if (!m_state.m_lensCircleRelative || m_gameLayer == nullptr) {
        m_lensCircleFade = m_state.m_lensCircleFade;
        value = (float)(1.0f * m_state.m_lensCircleSize) * m_shockWaveTimeMult;
    } else {
        float value3 = fabsf(m_gameLayer->m_objectLayer->getScale());
        m_lensCircleFade = m_state.m_lensCircleFade;
        value = (float)(value3 * m_state.m_lensCircleSize) * m_shockWaveTimeMult;
    }
    float value2;
    if (m_lensCircleFade >= 0.0f) {
        value2 = value - m_lensCircleFade;
        if (value2 < 0.0f)
            value2 = 0.0f;
    } else {
        value2 = value - 0.0f;
        if (value2 < 0.0f)
            value2 = 0.0f;
    }
    if (value < 0.0f)
        value = 0.0f;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_lensCircleStartUniform, value2);
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_lensCircleEndUniform, value);
    cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram3,
        m_lensCircleStrengthUniform,
        m_state.m_lensCircleStrength);
    cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith3f(
        cCGLProgram4,
        m_lensCircleTintUniform,
        (float)m_state.m_lensCircleTint.r / 255.0f,
        (float)m_state.m_lensCircleTint.g / 255.0f,
        (float)m_state.m_lensCircleTint.b / 255.0f);
    cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram5, m_lensCircleAdditiveUniform, (float)m_state.m_lensCircleAdditive);
}

void ShaderLayer::preMotionBlurShader() {
    float m_motionBlurTargetX = m_state.m_motionBlurTargetX;
    if (m_motionBlurTargetX == 0.0f && m_state.m_motionBlurTargetY == 0.0f) {
        if (cocos2d::CCNode::getActionByTag(this, 19)) {
            m_motionBlurTargetX = m_state.m_motionBlurTargetX;
        } else {
            if (!cocos2d::CCNode::getActionByTag(this, 20)) {
                cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
                cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram5, m_motionBlurValueUniform, 0.0f, 0.0f);
                return;
            }
            m_motionBlurTargetX = m_state.m_motionBlurTargetX;
        }
        m_state.m_usesShaders = true;
    }
    if (m_motionBlurTargetX != 0.0f || m_state.m_motionBlurTargetY != 0.0f) {
        float m_motionBlurSpeedX;
        float m_motionBlurSpeedY;
        if (m_state.m_motionBlurTargetIDX) {
            m_motionBlurSpeedX = m_state.m_motionBlurSpeedX;
            if (!m_state.m_motionBlurTargetIDY) {
                if (m_motionBlurSpeedX == 0.0f) {
                    m_motionBlurSpeedX = 0.001f;
                    m_motionBlurSpeedY = 1.0f;
                } else {
                    m_motionBlurSpeedY = 1.0f;
                }
            } else {
                m_motionBlurSpeedY = m_state.m_motionBlurSpeedY;
                if (m_motionBlurSpeedX == 0.0f)
                    m_motionBlurSpeedX = 0.001f;
                if (m_motionBlurSpeedY == 0.0f)
                    m_motionBlurSpeedY = 0.001f;
            }
        } else {
            if (!m_state.m_motionBlurTargetIDY) {
                m_motionBlurSpeedX = 1.0f;
                m_motionBlurSpeedY = 1.0f;
            } else {
                m_motionBlurSpeedY = m_state.m_motionBlurSpeedY;
                m_motionBlurSpeedX = 1.0f;
                if (m_motionBlurSpeedY == 0.0f)
                    m_motionBlurSpeedY = 0.001f;
            }
        }
        float value;
        if (!m_state.m_motionBlurRelative)
            value = 1.0f;
        else if (m_gameLayer != nullptr) {
            float value6 = m_gameLayer->m_objectLayer->getScale();
            m_motionBlurTargetX = m_state.m_motionBlurTargetX;
            value = fabsf(value6);
        } else {
            value = 1.0f;
        }
        float value2 = 0.1f;
        float value3 = (float)(m_motionBlurSpeedY * m_state.m_motionBlurTargetY) * 0.1f;
        float p1 = (float)(value
                 * (float)((float)((float)(m_motionBlurSpeedX * m_motionBlurTargetX) * 0.1f)
                         * m_state.m_textureScaleX));
        float p2 = (float)(m_shockWaveTimeMult * (float)(value * (float)(value3 * m_state.m_textureScaleY)));
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram, m_motionBlurValueUniform, p1, p2);
        if (m_state.m_radialBlurSize != 0.0f) {
            cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
            cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram4, m_motionBlurMultUniform, 1.0f);
            return;
        }
        float value22;
        float value4;
        if (m_state.m_motionBlurDual) {
            value2 = 0.2f;
            value4 = m_state.m_radialBlurFade * 0.2f;
            value22 = -0.2f;
        } else {
            value22 = 0.1f;
            value4 = m_state.m_radialBlurFade * 0.1f;
            value22 = -0.1f;
        }
        if (value4 > value2)
            value4 = value2;
        if (value22 < value4)
            value22 = value4;
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_blurFadeUniform, value22);
        cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
        float value5;
        if (m_state.m_motionBlurDual)
            value5 = 20.0f;
        else
            value5 = 36.0f;
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram3,
            m_motionBlurMultUniform,
            1.0f / (float)(9.0f - (float)(value22 * value5)));
        cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram4, m_motionBlurDualUniform, (float)m_state.m_motionBlurDual);
    }
}

void ShaderLayer::prePinchShader() {
    if (m_state.m_pinchTargetX != 0.0f ||
        m_state.m_pinchTargetY != 0.0f ||
        cocos2d::CCNode::getActionByTag(this, 25) ||
        cocos2d::CCNode::getActionByTag(this, 26)) {
        m_state.m_usesShaders = true;
    }
}

void ShaderLayer::prePixelateShader() {
    if (m_state.m_pixelateTargetX > 1.0f ||
        m_state.m_pixelateTargetY > 1.0f ||
        cocos2d::CCNode::getActionByTag(this, 9) ||
        cocos2d::CCNode::getActionByTag(this, 10)) {
        bool m_pixelateHardEdges = m_state.m_pixelateHardEdges;
        m_state.m_usesShaders = true;
        m_state.m_pixelatePixelating = true;
        toggleAntiAlias(!m_pixelateHardEdges);
    } else {
        m_state.m_pixelatePixelating = false;
        toggleAntiAlias(m_configuredAntiAlias);
    }
    if (m_state.m_pixelateTargetX < 1.0f)
        m_state.m_pixelateTargetX = 1.0f;
    if (m_state.m_pixelateTargetY < 1.0f)
        m_state.m_pixelateTargetY = 1.0f;
    float value;
    if (!m_state.m_pixelateRelative || m_gameLayer == nullptr) {
        value = 1.0f;
    } else {
        value = fabsf(m_gameLayer->m_objectLayer->getScale());
    }
    if (m_state.m_pixelatePixelating) {
        float value2 = value * m_scaleFactor;
        float m_pixelateTargetX = value2 * m_state.m_pixelateTargetX;
        float m_pixelateTargetY = value2 * m_state.m_pixelateTargetY;
        m_state.m_textureScaleX = 1.0f / m_pixelateTargetX;
        m_state.m_textureScaleY = 1.0f / m_pixelateTargetY;
        cocos2d::CCRenderTexture::updateInternalScale(m_renderTexture, 1.0f / m_pixelateTargetX, 1.0f / m_pixelateTargetY);
        m_sprite->setScaleX(m_sprite, m_pixelateTargetX);
        m_sprite->setScaleY(m_sprite, m_pixelateTargetY);
    } else {
        float m_pixelateTargetX = m_state.m_pixelateTargetX;
        float m_pixelateTargetY = m_state.m_pixelateTargetY;
        m_state.m_textureScaleX = 1.0f / m_pixelateTargetX;
        m_state.m_textureScaleY = 1.0f / m_pixelateTargetY;
        cocos2d::CCRenderTexture::updateInternalScale(m_renderTexture, 0.0f, 0.0f);
        m_sprite->setScaleX(m_sprite, m_pixelateTargetX);
        m_sprite->setScaleY(m_sprite, m_pixelateTargetY);
    }
}

void ShaderLayer::preRadialBlurShader() {
    float m_radialBlurSize = m_state.m_radialBlurSize;
    if (m_radialBlurSize != 0.0f || cocos2d::CCNode::getActionByTag(this, 16)) {
        m_state.m_usesShaders = true;
    }
    if (m_radialBlurSize == 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_blurFadeUniform, 0.0f);
    } else {
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        float value = m_radialBlurSize * 0.111111111f * 0.200000003f;
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_radialBlurValueUniform, value);
        float value2 = m_state.m_radialBlurFade * 0.2f;
        if (value2 > 0.2f)
            value2 = 0.2f;
        else if (value2 < 0.0f)
            value2 = 0.0f;
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_blurFadeUniform, value2);
    }
}

void ShaderLayer::preSepiaShader() {
    if (m_state.m_sepiaValue > 0.0f || cocos2d::CCNode::getActionByTag(this, 33))
        m_state.m_usesShaders = true;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_sepiaValueUniform, m_state.m_sepiaValue);
}

void ShaderLayer::preShockLineShader() {
    double m_shockLineStartTime = m_state.m_shockLineStartTime;
    if (m_shockLineStartTime == 0.0 ||
        (m_state.m_shockLineStrength <= 0.0f)) {
        cocos2d::CCGLProgram* cCGLProgram11 = m_sprite->getShaderProgram();
        m_shockLineMaxDistVal = 0.0f;
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram11, m_shockLineMaxDistValUniform, m_shockLineMaxDistVal);
        return;
    }
    double m_time = m_state.m_time;
    float m_shockLineTime = m_state.m_shockLineTime;
    float m_shockLineSpeed = m_state.m_shockLineSpeed;
    float m_shockLineStrength = m_state.m_shockLineStrength;
    m_state.m_shockLineStartTime = m_time;
    float value = m_shockLineTime + (m_time - m_shockLineStartTime) * m_shockLineSpeed;
    m_state.m_shockLineTime = value;
    float m_shockLineThickness = m_state.m_shockLineThickness;
    m_state.m_usesShaders = true;
    float value2;
    if (m_shockLineThickness > 0.001f)
        value2 = m_shockLineThickness * 0.2f;
    else
        value2 = 0.0002f;
    float value3 = value2 * m_state.m_shockLineFadeIn;
    float value4 = value2 * m_state.m_shockLineFadeOut;
    float value5 = value + (float)(m_state.m_shockLineTimeOffset * 0.1f);
    if (m_state.m_shockLineInvert) {
        float value6;
        if (m_state.m_shockLineDual)
            value6 = 0.5f;
        else
            value6 = 1.0f;
        value5 = -(value5 - value6 * (float)((float)(value2 + value3) + value4));
    }
    float value7 = value5 - value3;
    float value8 = (float)(value5 - value3) - value2;
    float value9 = (float)((float)(value5 - value3) + value3) + 0.001f;
    if ((float)(value5 - value3) <= value8)
        value8 = value5 - value3;
    float value82 = value8 - value4;
    if (value8 <= value82)
        value82 = value8;
    float m_shockLineWidth = m_state.m_shockLineWidth;
    float value10;
    if (m_shockLineWidth <= 0.001f)
        value10 = 22000.0f;
    else
        value10 = 22.0f / m_shockLineWidth;
    float value11;
    if (m_state.m_shockLineRelative && m_gameLayer != nullptr)
        value11 = fabsf(m_gameLayer->m_objectLayer->getScale());
    else
        value11 = 1.0f;
    float value12 = value11 * m_shockWaveTimeMult;
    float m_shockLineMaxSize = m_state.m_shockLineMaxSize;
    m_shockLineTime1 = value9 * value12;
    m_shockLineTime2 = value7 * value12;
    float value13 = (value8 + 0.001f) * value12;
    m_shockLineTime4 = value82 * value12;
    float value14 = 0.0f;
    m_shockLineTime3 = value13;
    if (m_shockLineMaxSize != 0.0f)
        value14 = 1.0f / (value12 * (m_shockLineMaxSize * 0.5f));
    m_shockLineMaxDistVal = value14;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_shockLineTimeUniform, value5 * value10);
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_shockLineTime1Uniform, m_shockLineTime1);
    cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_shockLineTime2Uniform, m_shockLineTime2);
    cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram4, m_shockLineTime3Uniform, m_shockLineTime3);
    cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram5, m_shockLineTime4Uniform, m_shockLineTime4);
    cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith2f(
        cCGLProgram6,
        m_shockLineStrengthUniform,
        (float)((float)(m_state.m_shockLineStrength * 0.01f) * m_state.m_textureScaleX) * value12,
        (float)((float)(m_state.m_shockLineStrength * 0.01f) * m_state.m_textureScaleY) * value12);
    cocos2d::CCGLProgram* cCGLProgram7 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram7, m_shockLineWavesUniform, value10 / value12);
    cocos2d::CCGLProgram* cCGLProgram8 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram8,
        m_shockLineAxisUniform,
        (float)m_state.m_shockLineAxis);
    cocos2d::CCGLProgram* cCGLProgram9 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram9,
        m_shockLineDirectionUniform,
        (float)m_state.m_shockLineDirection);
    cocos2d::CCGLProgram* cCGLProgram10 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram10,
        m_shockLineDualUniform,
        (float)m_state.m_shockLineDual);
    cocos2d::CCGLProgram* cCGLProgram11 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram11, m_shockLineMaxDistValUniform, m_shockLineMaxDistVal);
}

void ShaderLayer::preShockWaveShader() {
    float m_shockWaveStartTime = m_state.m_shockWaveStartTime;
    if (m_shockWaveStartTime <= 0.0f) {
        cocos2d::CCGLProgram* cCGLProgram11 = m_sprite->getShaderProgram();
        m_shockWaveMaxDistVal = 0.0f;
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram11, m_shockWaveMaxDistValUniform, m_shockWaveMaxDistVal);
        return;
    }
    double m_time = m_state.m_time;
    double value = m_time - m_shockWaveStartTime;
    bool flag = m_state.m_shockWaveStrength > 0.0f;
    double value2 = m_state.m_shockWaveTime + value * m_state.m_shockWaveSpeed;
    m_state.m_shockWaveStartTime = (float)m_time;
    float value22 = value2;
    m_state.m_shockWaveTime = value22;
    if (!flag) {
        cocos2d::CCGLProgram* cCGLProgram11 = m_sprite->getShaderProgram();
        m_shockWaveMaxDistVal = 0.0f;
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram11, m_shockWaveMaxDistValUniform, m_shockWaveMaxDistVal);
        return;
    }
    float m_shockWaveThickness = m_state.m_shockWaveThickness;
    m_state.m_usesShaders = true;
    float value3;
    if (m_shockWaveThickness > 0.001f)
        value3 = m_shockWaveThickness * 0.2f;
    else
        value3 = 0.0002f;
    float value4 = value22 + (float)(m_state.m_shockWaveTimeOffset * 0.1f);
    float value5 = value3 * m_state.m_shockWaveFadeIn;
    float value6 = value3 * m_state.m_shockWaveFadeOut;
    float value42 = value4;
    if (m_state.m_shockWaveInvert)
        value42 = 1.0f;
    float value7 = -(float)(value5 - (float)(value42 * value4));
    float value72 = value7 - value3;
    float value8 = (float)(value7 + value5) + 0.001f;
    if (value7 <= value72)
        value72 = value7;
    float value722 = value72 - value6;
    if (value72 <= value722)
        value722 = value72;
    float m_shockWaveWidth = m_state.m_shockWaveWidth;
    float value9;
    if (m_shockWaveWidth <= 0.001f)
        value9 = 22000.0f;
    else
        value9 = 22.0f / m_shockWaveWidth;
    float value10;
    if (m_state.m_shockWaveRelative && m_gameLayer != nullptr)
        value10 = fabsf(m_gameLayer->m_objectLayer->getScale());
    else
        value10 = 1.0f;
    float value11 = value10 * m_shockWaveTimeMult;
    float m_shockWaveMaxSize = m_state.m_shockWaveMaxSize;
    m_shockWaveTime4 = value722 * value11;
    m_shockWaveTime1 = value8 * value11;
    float value12 = (value72 + 0.001f) * value11;
    m_shockWaveTime2 = value7 * value11;
    float value13 = 0.0f;
    m_shockWaveTime3 = value12;
    if (m_shockWaveMaxSize != 0.0f)
        value13 = 1.0f / (value11 * (m_shockWaveMaxSize * 0.5f));
    float value14 = m_state.m_shockWaveOuter * value11;
    float value15 = (float)(value11 * m_state.m_shockWaveInner) * 0.5f;
    m_shockWaveMaxDistVal = value13;
    m_shockWaveMaxSize = value14;
    m_shockWaveMinSize = value15;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_shockWaveTimeUniform, value9 * value4);
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_shockWaveTime1Uniform, m_shockWaveTime1);
    cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_shockWaveTime2Uniform, m_shockWaveTime2);
    cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram4, m_shockWaveTime3Uniform, m_shockWaveTime3);
    cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram5, m_shockWaveTime4Uniform, m_shockWaveTime4);
    cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
    float m_textureScaleY = m_state.m_textureScaleY;
    if (m_state.m_textureScaleX <= m_textureScaleY)
        m_textureScaleY = m_state.m_textureScaleX;
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram6,
        m_shockWaveStrengthUniform,
        (float)((float)(m_state.m_shockWaveStrength * 0.1f) * m_textureScaleY) * value11);
    cocos2d::CCGLProgram* cCGLProgram7 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram7, m_shockWaveWavesUniform, value9 / value11);
    cocos2d::CCGLProgram* cCGLProgram8 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(
        cCGLProgram8,
        m_shockWaveInvertUniform,
        (float)m_state.m_shockWaveInvert);
    cocos2d::CCGLProgram* cCGLProgram9 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram9, m_shockWaveMinSizeUniform, m_shockWaveMinSize);
    cocos2d::CCGLProgram* cCGLProgram10 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram10, m_shockWaveMaxSizeUniform, m_shockWaveMaxSize);
    cocos2d::CCGLProgram* cCGLProgram11 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram11, m_shockWaveMaxDistValUniform, m_shockWaveMaxDistVal);
}

void ShaderLayer::preSplitScreenShader() {
    float m_splitTargetRows = m_state.m_splitTargetRows;
    float m_splitTargetCols = m_state.m_splitTargetCols;
    bool flag;
    if (m_splitTargetRows != 0.0f || m_splitTargetCols != 0.0f) {
        flag = true;
        m_state.m_usesShaders = true;
    } else if (cocos2d::CCNode::getActionByTag(this, 45)) {
        m_splitTargetRows = m_state.m_splitTargetRows;
        m_splitTargetCols = m_state.m_splitTargetCols;
        flag = true;
        m_state.m_usesShaders = true;
    } else if (cocos2d::CCNode::getActionByTag(this, 46)) {
        m_splitTargetRows = m_state.m_splitTargetRows;
        m_splitTargetCols = m_state.m_splitTargetCols;
        flag = true;
        m_state.m_usesShaders = true;
    } else {
        flag = false;
        m_splitTargetCols = m_state.m_splitTargetCols;
    }
    m_state.m_splitActive = flag;
    float value = m_splitTargetRows + 1.0f;
    float value2 = m_splitTargetCols + 1.0f;
    cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram, m_rowmodUniform, value);
    cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_colmodUniform, value2);
    cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
    float value3 = ((float)-value + 1.0f) * m_state.m_textureScaleY;
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_rowmodCalcUniform, value3);
    cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
    float value4 = ((float)-value2 + 1.0f) * m_state.m_textureScaleX;
    cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram4, m_colmodCalcUniform, value4);
    if (m_state.m_splitActive) {
        if (m_gameLayer)
            m_gameLayer->m_objectLayer->getScale();
        float value5 = (float)(pixelOffset.x / m_textureContentSize.width) * m_state.m_textureScaleX;
        float value6 = (float)(pixelOffset.y / m_textureContentSize.height) * m_state.m_textureScaleY;
        cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram5, m_splitXStartUniform, -value5);
        cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram6, m_splitXRangeUniform, value5 + (float)(1.0f - value5));
        cocos2d::CCGLProgram* cCGLProgram7 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram7,
            m_splitXRangeMultUniform,
            1.0f / (float)(value5 + (float)(1.0f - value5)));
        cocos2d::CCGLProgram* cCGLProgram8 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram8, m_splitYStartUniform, -value6);
        cocos2d::CCGLProgram* cCGLProgram9 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram9, m_splitYRangeUniform, value6 + (float)(1.0f - value6));
        cocos2d::CCGLProgram* cCGLProgram10 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram10,
            m_splitYRangeMultUniform,
            1.0f / (float)(value6 + (float)(1.0f - value6)));
    }
}

cocos2d::CCPoint ShaderLayer::prepareTargetContainer() {
    if (!m_gameLayer) {
        return cocos2d::CCPoint(0.0f, 0.0f);
    }
    const cocos2d::CCPoint* point3 = m_gameLayer->m_objectLayer->getPosition();
    cocos2d::CCPoint point4(*point3);
    cocos2d::CCNode* m_objectParent;
    if (m_state.m_minBlendingLayer <= 1)
        m_objectParent = m_gameLayer->m_objectParent;
    else
        m_objectParent = m_gameLayer->m_inShaderParent;
    cocos2d::CCPoint point5(0.0f, 0.0f);
    if (m_state.m_pixelateSnapGrid && m_state.m_pixelatePixelating) {
        float value7;
        float scaleFactor2;
        if (m_state.m_pixelateRelative && m_gameLayer != nullptr) {
            float value6 = m_gameLayer->m_objectLayer->getScale();
            float value8 = fabsf(value6);
            scaleFactor2 = m_scaleFactor;
            value7 = value8 * m_scaleFactor;
        } else {
            scaleFactor2 = m_scaleFactor;
            value7 = 1.0f * m_scaleFactor;
        }
        cocos2d::CCPoint point6(
            floorf(point4.x / (float)((float)(value7 * m_state.m_pixelateTargetX) / scaleFactor2))
                * (float)((float)(value7 * m_state.m_pixelateTargetX) / scaleFactor2),
            floorf(point4.y / (float)((float)(value7 * m_state.m_pixelateTargetY) / scaleFactor2))
                * (float)((float)(value7 * m_state.m_pixelateTargetY) / scaleFactor2));
        point4 = point4 - point6;
        point5 = point4;
    }
    pixelOffset = point5;
    cocos2d::CCPoint m_objectParentPos = m_objectParent->getPosition();
    m_savedCameraPosition = m_objectParentPos;
    m_objectParent->setScaleX(m_state.m_textureScaleX);
    m_objectParent->setScaleY(m_state.m_textureScaleY);
    m_savedCameraRotation = m_objectParent->getRotation();
    cocos2d::CCSize point6Size(m_screenSize);
    cocos2d::CCRect rect(-1.0f, -1.0f, point6Size.width + 2.0f, point6Size.height + 2.0f);
    m_sprite->setTextureRect(rect);
    cocos2d::CCSize targetSize(
        m_targetTextureSize.width - point6Size.width,
        m_targetTextureSize.height - point6Size.height);
    m_targetTextureSizeExtra = targetSize;
    float m_textureScaleX = m_state.m_textureScaleX;
    float value2 = point5.y - (float)(m_targetTextureSizeExtra.height * 0.5f);
    float y = m_savedCameraPosition.y;
    float m_textureScaleY = m_state.m_textureScaleY;
    float value3 = m_savedCameraPosition.x - (float)(point5.x - (float)(m_targetTextureSizeExtra.width * 0.5f));
    point5.x = point5.x - (float)(m_targetTextureSizeExtra.width * 0.5f);
    point5.y = value2;
    cocos2d::CCPoint newPos(value3 * m_textureScaleX, (float)(y - value2) * m_textureScaleY);
    m_objectParent->setPosition(newPos);
    return point5;
}

bool ShaderLayer::resetAllShaders() {
    cocos2d::CCNode::stopAllActions();
    GJShaderState::reset(&m_state);
    m_state.m_time = m_state.m_startTime;
    return updateZLayer(0, 0, false);
}

void ShaderLayer::resetTargetContainer() {
    if (m_gameLayer) {
        cocos2d::CCNode* m_inShaderParent = m_gameLayer->m_inShaderParent;
        if (m_state.m_minBlendingLayer <= 1)
            m_inShaderParent = m_gameLayer->m_objectParent;
        m_inShaderParent->setPosition(&m_savedCameraPosition);
        m_inShaderParent->setScale(1.0f);
        m_inShaderParent->setRotation(m_savedCameraRotation);
        setRotation(0.0f);
    }
}

void ShaderLayer::setupBulgeShader() {
    m_sprite->getShaderProgram();
    m_bulgeValueUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_bulgeValue2Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_bulgeOriginUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_bulgeRadiusUniform = glGetUniformLocation();
}

void ShaderLayer::setupChromaticGlitchUniforms() {
    m_sprite->getShaderProgram();
    m_cGRGBOffsetUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_cGYOffsetUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_cGTimeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_cGStrengthUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_cGHeightUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_cGLineThickUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_cGLineStrengthUniform = glGetUniformLocation();
}

void ShaderLayer::setupChromaticUniforms() {
    m_sprite->getShaderProgram();
    m_chromaticXOffUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_chromaticYOffUniform = glGetUniformLocation();
}

void ShaderLayer::setupColorChangeShader() {
    m_sprite->getShaderProgram();
    m_colorChangeCUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_colorChangeBUniform = glGetUniformLocation();
}

void ShaderLayer::setupCommonUniforms() {
    m_sprite->getShaderProgram();
    m_textureScaleUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_textureScaleInvUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_screenAspectUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_screenAspectINVUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shaderPositionUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_blurRefColorUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_blurUseRefUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_blurIntensityUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_blurFadeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_blurOnlyEmptyUniform = glGetUniformLocation();
}

void ShaderLayer::setupGlitchUniforms() {
    m_sprite->getShaderProgram();
    m_glitchBotUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_glitchTopUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_glitchXOffsetUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_glitchColOffsetUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_glitchRndUniform = glGetUniformLocation();
}

void ShaderLayer::setupGrayscaleShader() {
    m_sprite->getShaderProgram();
    m_grayscaleValueUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_grayscaleTintUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_grayscaleUseLumUniform = glGetUniformLocation();
}

void ShaderLayer::setupHueShiftShader() {
    m_sprite->getShaderProgram();
    m_hueShiftCosAUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_hueShiftSinAUniform = glGetUniformLocation();
}

void ShaderLayer::setupInvertColorShader() {
    m_sprite->getShaderProgram();
    m_invertColorValueUniform = glGetUniformLocation();
}

void ShaderLayer::setupLensCircleShader() {
    m_sprite->getShaderProgram();
    m_lensCircleOriginUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_lensCircleStartUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_lensCircleEndUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_lensCircleStrengthUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_lensCircleTintUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_lensCircleAdditiveUniform = glGetUniformLocation();
}

void ShaderLayer::setupMotionBlurShader() {
    m_sprite->getShaderProgram();
    m_motionBlurValueUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_motionBlurMultUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_motionBlurDualUniform = glGetUniformLocation();
}

void ShaderLayer::setupPinchShader() {
    m_sprite->getShaderProgram();
    m_pinchValueUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_pinchCenterPosUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_pinchCalcUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_pinchRadiusUniform = glGetUniformLocation();
}

void ShaderLayer::setupRadialBlurShader() {
    m_sprite->getShaderProgram();
    m_radialBlurCenterUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_radialBlurValueUniform = glGetUniformLocation();
}

void ShaderLayer::setupSepiaShader() {
    m_sprite->getShaderProgram();
    m_sepiaValueUniform = glGetUniformLocation();
}

void ShaderLayer::setupShader(bool reset) {
    if (m_sprite) {
        m_sprite->removeMeAndCleanup();
        m_sprite = nullptr;
    }
    if (m_renderTexture) {
        m_renderTexture->release();
        m_renderTexture = nullptr;
    }
    cocos2d::CCShaderCache* cCShaderCache = cocos2d::CCShaderCache::sharedShaderCache();
    cocos2d::CCGLProgram* cCGLProgram = cocos2d::CCShaderCache::programForKey(cCShaderCache, "custom_program");
    m_shader = cCGLProgram;
    if (cCGLProgram) {
        if (reset)
            cocos2d::CCGLProgram::reset(cCGLProgram);
    } else {
        cocos2d::CCGLProgram* cCGLProgram2 = new cocos2d::CCGLProgram();
        m_shader = cCGLProgram2;
        cocos2d::CCShaderCache* cCShaderCache2 = cocos2d::CCShaderCache::sharedShaderCache();
        cocos2d::CCShaderCache::addProgram(cCShaderCache2, m_shader, "custom_program");
        m_shader->release();
        char* p0 = (char*)vertTest();
        cocos2d::CCRect p1;
        uberShader(&p1);
        cocos2d::CCGLProgram::initWithVertexShaderByteArray(m_shader, p0, *(const char**)&p1.origin);
        cocos2d::CCGLProgram::addAttribute(m_shader, "a_position", 0);
        cocos2d::CCGLProgram::addAttribute(m_shader, "a_color", 1u);
        cocos2d::CCGLProgram::addAttribute(m_shader, "a_texCoord", 2u);
        cocos2d::CCGLProgram::link(m_shader);
        cocos2d::CCGLProgram::updateUniforms(m_shader);
        GameManager::sharedState()->m_shouldResetShader = false;
    }
    cocos2d::CCDirector* cCDirector = cocos2d::CCDirector::sharedDirector();
    const cocos2d::CCSize* size = cCDirector->m_pobOpenGLView->getFrameSize();
    cocos2d::CCSize size2(*size);
    m_screenSize = size2;
    float height = size2.height;
    cocos2d::CCDirector* cCDirector2 = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCSize winSize = cocos2d::CCDirector::getWinSize(cCDirector2);
    float value = 1.0f / (float)(m_screenSize.height / m_screenSize.width);
    m_scaleFactor = height / winSize.height;
    m_aspectRatio = value;
    cocos2d::CCDirector* cCDirector3 = cocos2d::CCDirector::sharedDirector();
    float ContentScaleFactor = cocos2d::CCDirector::getContentScaleFactor(cCDirector3);
    cocos2d::CCDirector* cCDirector4 = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCDirector::setContentScaleFactor(cCDirector4, 1.0f);
    m_renderTexture = cocos2d::CCRenderTexture::create((int)size2.width, (int)size2.height);
    cocos2d::CCDirector* cCDirector5 = cocos2d::CCDirector::sharedDirector();
    cocos2d::CCDirector::setContentScaleFactor(cCDirector5, ContentScaleFactor);
    cocos2d::CCDirector* cCDirector6 = cocos2d::CCDirector::sharedDirector();
    m_shockWaveTimeMult = (float)(1.0f / cocos2d::CCDirector::getScreenScaleFactorW(cCDirector6)) * 1.2019f;
    cocos2d::CCSize targetExtra(0.0f, 0.0f);
    m_targetTextureSizeExtra = targetExtra;
    m_renderTexture->retain();
    cocos2d::CCSprite* sprite2 = cocos2d::CCSprite::create();
    m_sprite = sprite2;
    cocos2d::CCTexture2D* cCTexture2D = m_renderTexture->getSprite()->getTexture();
    sprite2->setTexture(cCTexture2D);
    cocos2d::CCSize contentSize = m_sprite->getTexture()->getContentSize();
    m_textureContentSize = contentSize;
    cocos2d::CCSize targetSize(size2.width, size2.height);
    m_targetTextureSize = targetSize;
    cocos2d::CCRect rect(
        -1.0f,
        -1.0f,
        m_targetTextureSize.width + 2.0f,
        m_targetTextureSize.height + 2.0f);
    m_sprite->setTextureRect(rect);
    m_sprite->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    m_sprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    m_sprite->setFlipY(true);
    addChild(m_sprite);
    GameManager* gameManager = GameManager::sharedState();
    bool GameVariable = GameManager::getGameVariable(gameManager, "0155");
    m_configuredAntiAlias = !GameVariable;
    toggleAntiAlias(!GameVariable);
    m_sprite->setShaderProgram(m_shader);
    setupCommonUniforms();
    setupShockWaveUniforms();
    setupShockLineUniforms();
    setupGlitchUniforms();
    setupChromaticUniforms();
    setupChromaticGlitchUniforms();
    setupLensCircleShader();
    setupRadialBlurShader();
    setupBulgeShader();
    setupPinchShader();
    setupMotionBlurShader();
    setupGrayscaleShader();
    setupSepiaShader();
    setupInvertColorShader();
    setupHueShiftShader();
    setupColorChangeShader();
    setupSplitScreenShader();
}

void ShaderLayer::setupShockLineUniforms() {
    m_sprite->getShaderProgram();
    m_shockLineTimeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineTime1Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineTime2Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineTime3Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineTime4Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineAxisUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineDirectionUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineDualUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineWavesUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineStrengthUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineCenterUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockLineMaxDistValUniform = glGetUniformLocation();
}

void ShaderLayer::setupShockWaveUniforms() {
    m_sprite->getShaderProgram();
    m_shockWaveTimeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveTime1Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveTime2Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveTime3Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveTime4Uniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveStrengthUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveWavesUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveCenterUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveInvertUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveMinSizeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveMaxSizeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_shockWaveMaxDistValUniform = glGetUniformLocation();
}

void ShaderLayer::setupSplitScreenShader() {
    m_sprite->getShaderProgram();
    m_rowmodUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_colmodUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_rowmodCalcUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_colmodCalcUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_splitXStartUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_splitXRangeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_splitXRangeMultUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_splitYStartUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_splitYRangeUniform = glGetUniformLocation();
    m_sprite->getShaderProgram();
    m_splitYRangeMultUniform = glGetUniformLocation();
}

void ShaderLayer::toggleAntiAlias(bool antiAlias) {
    if (m_antiAlias != antiAlias) {
        m_antiAlias = antiAlias;
        if (antiAlias) {
            cocos2d::CCSprite* sprite3 = m_renderTexture->getSprite();
            cocos2d::CCTexture2D* cCTexture2D2 = sprite3->getTexture();
            cCTexture2D2->setAntiAliasTexParameters();
        } else {
            cocos2d::CCSprite* sprite2 = m_renderTexture->getSprite();
            cocos2d::CCTexture2D* cCTexture2D = sprite2->getTexture();
            cCTexture2D->setAliasTexParameters();
        }
    }
}

void ShaderLayer::triggerHueShift(float fadeTime, float degrees, int easingType, float easingRate) {
    tweenValueAuto(degrees, 38, fadeTime, easingType, easingRate);
}

void ShaderLayer::triggerSepia(float fadeTime, float target, int easingType, float easingRate) {
    tweenValueAuto(target, 33, fadeTime, easingType, easingRate);
}

void ShaderLayer::update(float dt) {
    cocos2d::CCSize size(m_screenSize);
    float value = dt;
    if (m_targetTextureSize.width != size.width || m_targetTextureSize.height != size.height) {
        setupShader(false);
        value = dt;
    }
    m_state.m_time = m_state.m_time + value;
    GJShaderState::updateTweenActions(&m_state, value);
}

void ShaderLayer::updateEffectOffsets(cocos2d::CCPoint center) {
    cocos2d::CCSize size(m_screenSize);
    cocos2d::CCPoint point3(0.5f, 0.5f);
    cocos2d::CCPoint point4(center);
    cocos2d::CCPoint point8(center.x / size.width, center.y / size.height);
    point4 = point8;
    point4 = point4 * m_scaleFactor;
    point4 = point4;
    if (m_state.m_shockWaveStartTime > 0.0f) {
        cocos2d::CCPoint p1;
        if (m_state.m_shockWaveTarget && m_gameLayer) {
            cocos2d::CCPoint point7(m_state.m_shockWaveCenter);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(point7);
            p1 = shaderPos;
        } else {
            float value24 = point3.x + m_state.m_shockWaveScreenOffset.x * 0.5f;
            float value25 = point3.y + m_state.m_shockWaveScreenOffset.y * 0.5f;
            cocos2d::CCPoint point8b(value24, value25);
            p1 = point8b;
            cocos2d::CCPoint point8c(point3);
            rotatePoint((float*)&p1, (float*)&point8c, m_savedCameraRotation);
            p1 = p1 - point4;
        }
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram, m_shockWaveCenterUniform, p1.x, p1.y);
        cocos2d::CCPoint point8d(0.5f, 0.5f);
        point8d = point8d - p1;
        float value2 = sqrtf((float)(p1.y * p1.y) + (float)(p1.x * p1.x));
        float value22 = value2;
        if (m_shockWaveUnk46c) {
            if (m_shockWaveMinSize <= value2)
                m_shockWaveMinSize = value2;
            value22 = m_shockWaveMaxSize - m_shockWaveMinSize;
        }
        float shockWaveTime43;
        if (m_shockWaveTime3 <= m_shockWaveTime4) {
            if (m_shockWaveTime2 <= m_shockWaveTime3) {
                shockWaveTime43 = m_shockWaveTime2;
            } else {
                shockWaveTime43 = m_shockWaveTime3;
            }
        } else {
            shockWaveTime43 = m_shockWaveTime4;
        }
        float shockWaveTime44 = m_shockWaveTime4;
        if (m_shockWaveTime3 >= m_shockWaveTime4) {
            shockWaveTime44 = m_shockWaveTime3;
        }
        if (m_shockWaveTime1 < (float)(value22 - 1.4142f) == shockWaveTime43 >= (float)(value22 + 1.4142f)) {
            cocos2d::CCGLProgram* cCGLProgram15 = m_sprite->getShaderProgram();
            cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram15, m_shockWaveCenterUniform, p1.x, p1.y);
        } else {
            cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
            cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram2, m_shockWaveTimeUniform, 0.0f);
        }
    }
    if (m_state.m_shockLineStartTime > 0.0f) {
        cocos2d::CCPoint p1;
        if (m_state.m_shockLineTarget && m_gameLayer) {
            cocos2d::CCPoint point7(m_state.m_shockLineCenter);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(point7);
            p1 = shaderPos;
        } else {
            float m_shockLineScreenOffset = m_state.m_shockLineScreenOffset.x;
            float value28 = point3.x + m_shockLineScreenOffset * 0.5f;
            float value29 = point3.y + m_state.m_shockLineScreenOffset.y * 0.5f;
            cocos2d::CCPoint point8e(value28, value29);
            p1 = point8e;
            float savedCameraRotation5 = m_savedCameraRotation;
            cocos2d::CCPoint point8f(point3);
            rotatePoint((float*)&p1, (float*)&point8f, savedCameraRotation5);
            p1 = p1 - point4;
        }
        cocos2d::CCPoint point8g(0.5f, 0.5f);
        point8g = point8g - p1;
        float y;
        if (m_state.m_shockLineAxis)
            y = p1.y;
        else
            y = p1.x;
        if (m_state.m_shockLineDual)
            y = fabsf(y);
        if (m_state.m_shockLineDirection)
            y = 1.0f - y;
        float shockLineTime23;
        if (m_shockLineTime3 > m_shockLineTime4) {
            if (m_shockLineTime2 <= m_shockLineTime4)
                shockLineTime23 = m_shockLineTime2;
            else
                shockLineTime23 = m_shockLineTime4;
        } else {
            if (m_shockLineTime2 <= m_shockLineTime3)
                shockLineTime23 = m_shockLineTime2;
            else
                shockLineTime23 = m_shockLineTime3;
        }
        float shockLineTime24 = m_shockLineTime4;
        if (m_shockLineTime3 >= m_shockLineTime4)
            shockLineTime24 = m_shockLineTime3;
        if (m_shockLineTime1 < (float)(y - 1.0f) == shockLineTime23 >= (float)(y + 1.0f)) {
            cocos2d::CCGLProgram* cCGLProgram14 = m_sprite->getShaderProgram();
            if (m_state.m_shockLineAxis)
                cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram14, m_shockLineCenterUniform, p1.y);
            else
                cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram14, m_shockLineCenterUniform, p1.x);
        } else {
            cocos2d::CCGLProgram* cCGLProgram3 = m_sprite->getShaderProgram();
            cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram3, m_shockLineTimeUniform, 0.0f);
        }
    }
    if (m_state.m_lensCircleStrength > 0.0f) {
        cocos2d::CCPoint p1;
        if (m_state.m_lensCircleTargetID && m_gameLayer) {
            cocos2d::CCPoint point7(m_state.m_lensCircleCenter);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(point7);
            p1 = shaderPos;
        } else {
            float value26 = point3.x + m_state.m_lensCircleScreenOffset.x * 0.5f;
            float value27 = point3.y + m_state.m_lensCircleScreenOffset.y * 0.5f;
            cocos2d::CCPoint point8h(value26, value27);
            p1 = point8h;
            float savedCameraRotation4 = m_savedCameraRotation;
            cocos2d::CCPoint point8i(point3);
            rotatePoint((float*)&p1, (float*)&point8i, savedCameraRotation4);
            p1 = p1 - point4;
        }
        cocos2d::CCGLProgram* cCGLProgram4 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(
            cCGLProgram4,
            m_lensCircleOriginUniform,
            p1.x,
            p1.y / m_aspectRatio);
    }
    if (m_state.m_radialBlurSize != 0.0f) {
        cocos2d::CCPoint p1;
        if (m_state.m_radialBlurTarget && m_gameLayer) {
            cocos2d::CCPoint point7(m_state.m_radialBlurCenter);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(point7);
            p1 = shaderPos;
        } else {
            float value31 = point3.x + m_state.m_radialBlurScreenOffset.x * 0.5f;
            float value32 = point3.y + m_state.m_radialBlurScreenOffset.y * 0.5f;
            cocos2d::CCPoint point8j(value31, value32);
            p1 = point8j;
            float savedCameraRotation7 = m_savedCameraRotation;
            cocos2d::CCPoint point8k(point3);
            rotatePoint((float*)&p1, (float*)&point8k, savedCameraRotation7);
            p1 = p1 - point4;
        }
        cocos2d::CCGLProgram* cCGLProgram13 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(
            cCGLProgram13,
            m_radialBlurCenterUniform,
            p1.x * m_state.m_textureScaleX,
            p1.y * m_state.m_textureScaleY);
    }
    float m_pinchTargetX = m_state.m_pinchTargetX;
    float m_pinchTargetY = m_state.m_pinchTargetY;
    bool flag = m_pinchTargetX == 0.0f && m_pinchTargetY != 0.0f;
    float value4;
    if (m_pinchTargetX == 0.0f && m_pinchTargetY != 0.0f) {
        value4 = -0.001f;
    } else {
        value4 = -m_pinchTargetX;
        flag = m_pinchTargetX != 0.0f;
    }
    float value5;
    if (flag && m_pinchTargetY == 0.0f)
        value5 = -0.001f;
    else
        value5 = -m_pinchTargetY;
    float value6;
    if (m_state.m_pinchRelative && m_gameLayer != nullptr)
        value6 = fabsf(m_gameLayer->m_objectLayer->getScale());
    else
        value6 = 1.0f;
    cocos2d::CCPoint point5(value4 * 0.1f, value5 * 0.1f);
    cocos2d::CCGLProgram* cCGLProgram5 = m_sprite->getShaderProgram();
    cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram5, m_pinchValueUniform, point5.x, point5.y);
    if (m_state.m_pinchTargetX != 0.0f || m_state.m_pinchTargetY != 0.0f) {
        cocos2d::CCPoint p1;
        if ((m_state.m_pinchTargetEnabledX || m_state.m_pinchTargetEnabledY) && m_gameLayer) {
            cocos2d::CCPoint point7(m_state.m_pinchCenter);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(point7);
            p1 = shaderPos;
        } else {
            float value19 = point3.x + m_state.m_pinchScreenOffset.x * 0.5f;
            float value20 = point3.y + m_state.m_pinchScreenOffset.y * 0.5f;
            cocos2d::CCPoint point8l(value19, value20);
            p1 = point8l;
            float savedCameraRotation2 = m_savedCameraRotation;
            cocos2d::CCPoint point8m(point3);
            rotatePoint((float*)&p1, (float*)&point8m, savedCameraRotation2);
            p1 = p1 - point4;
        }
        cocos2d::CCGLProgram* cCGLProgram10 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram10, m_pinchCenterPosUniform, p1.x, p1.y);
        cocos2d::CCGLProgram* cCGLProgram11 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(
            cCGLProgram11,
            m_pinchRadiusUniform,
            (float)(value6 * m_shockWaveTimeMult) * m_state.m_pinchRadius);
        float x = m_state.m_pinchModifier.x;
        if (x <= 0.001f)
            x = 0.001f;
        float value15 = m_state.m_pinchModifier.y;
        if (value15 <= 0.001f)
            value15 = 0.001f;
        cocos2d::CCGLProgram* cCGLProgram16 = m_sprite->getShaderProgram();
        float value16 = atanf(x * (float)((float)-point5.x * 20.0f));
        float value17 = atanf(value15 * (float)((float)-point5.y * 20.0f));
        cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram16, m_pinchCalcUniform, x / value16, value15 / value17);
    }
    if (m_state.m_cGActive) {
        cocos2d::CCPoint point6;
        if (m_state.m_cGRelativePos && m_gameLayer) {
            cocos2d::CCPoint p1(0.0f, 0.0f);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(p1);
            cocos2d::CCPoint diff = p1 - shaderPos;
            point6 = diff;
        } else {
            cocos2d::CCPoint point8n(0.0f, 0.0f);
            point6 = point8n;
            float savedCameraRotation6 = m_savedCameraRotation;
            cocos2d::CCPoint point8o(point3);
            rotatePoint((float*)&point6, (float*)&point8o, savedCameraRotation6);
            point6 = point6 - point4;
        }
        cocos2d::CCGLProgram* cCGLProgram12 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram12, m_cGYOffsetUniform, point6.y * m_state.m_textureScaleY);
    }
    if (m_state.m_bulgeValue > 0.0f) {
        cocos2d::CCPoint p1;
        if (m_state.m_bulgeTargetID && m_gameLayer) {
            cocos2d::CCPoint point7(m_state.m_bulgeCenter);
            cocos2d::CCPoint shaderPos = objectPosToShaderPos(point7);
            p1 = shaderPos;
        } else {
            float value21 = point3.x + m_state.m_bulgeScreenOffset.x * 0.5f;
            float value23 = point3.y + m_state.m_bulgeScreenOffset.y * 0.5f;
            cocos2d::CCPoint point8p(value21, value23);
            p1 = point8p;
            float savedCameraRotation3 = m_savedCameraRotation;
            cocos2d::CCPoint point8q(point3);
            rotatePoint((float*)&p1, (float*)&point8q, savedCameraRotation3);
            p1 = p1 - point4;
        }
        cocos2d::CCGLProgram* cCGLProgram6 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith2f(cCGLProgram6, m_bulgeOriginUniform, p1.x, p1.y);
        float value8;
        if (m_state.m_bulgeRelative && m_gameLayer != nullptr)
            value8 = fabsf(m_gameLayer->m_objectLayer->getScale());
        else
            value8 = 1.0f;
        float value9 = value8 * m_shockWaveTimeMult;
        float value10 = value9 * 0.5f * 1.44444442f;
        cocos2d::CCPoint point8r(value10, (float)(value9 * 0.5f) * 1.4444f);
        p1 = point8r;
        float value11 = sqrtf((float)(p1.y * p1.y) + (float)(p1.x * p1.x));
        float value12 = 6.28318531f / (value11 + value11) * 0.449999988f * m_state.m_bulgeValue;
        if (value12 <= 0.001f)
            value12 = 0.001f;
        float value13 = value11 / tanf(value12 * value11);
        cocos2d::CCGLProgram* cCGLProgram7 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram7, m_bulgeValueUniform, value12);
        cocos2d::CCGLProgram* cCGLProgram8 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram8, m_bulgeValue2Uniform, value13);
        cocos2d::CCGLProgram* cCGLProgram9 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformLocationWith1f(cCGLProgram9, m_bulgeRadiusUniform, value9 * m_state.m_bulgeRadius);
    }
}

void ShaderLayer::updateMotionBlurSpeedX(float x, float dt) {
    float m_motionBlurFollowEaseX = m_state.m_motionBlurFollowEaseX;
    if (m_motionBlurFollowEaseX > 1.0f) {
        if (dt > 0.0f)
            m_motionBlurFollowEaseX = m_motionBlurFollowEaseX / dt;
        x = m_state.m_motionBlurSpeedX
          + (float)((float)(x - m_state.m_motionBlurSpeedX) / m_motionBlurFollowEaseX);
    }
    m_state.m_motionBlurSpeedX = x;
}

void ShaderLayer::updateMotionBlurSpeedY(float y, float dt) {
    float m_motionBlurFollowEaseY = m_state.m_motionBlurFollowEaseY;
    if (m_motionBlurFollowEaseY > 1.0f) {
        if (dt > 0.0f)
            m_motionBlurFollowEaseY = m_motionBlurFollowEaseY / dt;
        y = m_state.m_motionBlurSpeedY
          + (float)((float)(y - m_state.m_motionBlurSpeedY) / m_motionBlurFollowEaseY);
    }
    m_state.m_motionBlurSpeedY = y;
}

void ShaderLayer::updateShockLineCenter(cocos2d::CCPoint center) {
    if (m_state.m_shockLineCenterDirty) {
        if (!m_state.m_shockLineCenterMoving)
            m_state.m_shockLineCenterDirty = false;
        m_state.m_shockLineCenter = center;
    }
}

void ShaderLayer::updateShockWaveCenter(cocos2d::CCPoint center) {
    if (m_state.m_shockWaveCenterDirty) {
        if (!m_state.m_shockWaveCenterMoving)
            m_state.m_shockWaveCenterDirty = false;
        m_state.m_shockWaveCenter = center;
    }
}

bool ShaderLayer::updateZLayer(int minLayer, int maxLayer, bool noParticles) {
    if (minLayer <= 0)
        minLayer = 1;
    if (maxLayer <= 0)
        maxLayer = 15;
    if (m_state.m_minBlendingLayer != minLayer) {
        m_state.m_minBlendingLayer = minLayer;
        m_state.m_zLayerDirty = true;
    }
    if (m_state.m_maxBlendingLayer != maxLayer) {
        m_state.m_maxBlendingLayer = maxLayer;
        m_state.m_zLayerDirty = true;
    }
    if (m_state.m_noPlayerParticles != noParticles) {
        int m_minBlendingLayer = m_state.m_minBlendingLayer;
        m_state.m_noPlayerParticles = noParticles;
        if (m_minBlendingLayer == 8 || m_state.m_maxBlendingLayer == 8)
            m_state.m_zLayerDirty = true;
    }
    return (bool)this;
}

void ShaderLayer::visit() {
    if (m_bVisible) {
        cocos2d::CCDirector* cCDirector = cocos2d::CCDirector::sharedDirector();
        float ContentScaleFactor = cocos2d::CCDirector::getContentScaleFactor(cCDirector);
        cocos2d::CCDirector* cCDirector2 = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCDirector::setContentScaleFactor(cCDirector2, m_scaleFactor);
        prepareTargetContainer();
        cocos2d::CCPoint rectOrigin(
            m_targetTextureSizeExtra.width * 0.5f,
            m_targetTextureSizeExtra.height * 0.5f);
        cocos2d::CCPoint point;
        point = rectOrigin + point;
        kmGLPushMatrix();
        if (m_pGrid && m_pGrid->m_bActive)
            cocos2d::CCGridBase::beforeDraw(m_pGrid);
        unsigned int intVal = 0;
        cocos2d::CCNode::transform();
        cocos2d::CCGLProgram* cCGLProgram = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::use(cCGLProgram);
        cocos2d::CCGLProgram* cCGLProgram2 = m_sprite->getShaderProgram();
        cocos2d::CCGLProgram::setUniformsForBuiltins(cCGLProgram2);
        cocos2d::CCPoint point4(point);
        updateEffectOffsets(point4);
        preSplitScreenShader();
        cocos2d::CCRenderTexture::beginWithClear(m_renderTexture, 0.0f, 0.0f, 0.0f, 0.0f);
        cocos2d::ccArray* data = m_pChildren->data;
        unsigned int num = data->num;
        if (data->num) {
            do {
                cocos2d::CCSprite* sprite2 = (cocos2d::CCSprite*)data->arr[intVal];
                if (sprite2 != m_sprite) {
                    sprite2->visit();
                    num = data->num;
                }
                ++intVal;
            } while (intVal < num);
        }
        cocos2d::CCRenderTexture::end(m_renderTexture);
        cocos2d::CCRect rect(m_sprite->m_obRect);
        cocos2d::CCPoint point3(
            rect.origin.x / m_state.m_textureScaleX,
            rect.origin.y / m_state.m_textureScaleY);
        cocos2d::CCPoint point2;
        point2 = point3 + point;
        m_sprite->setPosition(&point2);
        m_sprite->visit();
        m_uOrderOfArrival = 0;
        cocos2d::CCGridBase* pGrid2 = m_pGrid;
        if (pGrid2) {
            if (pGrid2->m_bActive)
                cocos2d::CCGridBase::afterDraw(pGrid2, this);
        }
        kmGLPopMatrix();
        resetTargetContainer();
        cocos2d::CCDirector* cCDirector3 = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCDirector::setContentScaleFactor(cCDirector3, ContentScaleFactor);
    }
}

ShaderLayer::~ShaderLayer() {
}