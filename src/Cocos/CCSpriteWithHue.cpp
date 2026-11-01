#include "Cocos/CCSpriteWithHue.h"
#include "Platform/cocos2d.h"

CCSpriteWithHue* CCSpriteWithHue::create(const std::string* file) {
    CCSpriteWithHue* sprite = new CCSpriteWithHue();
    if (sprite && sprite->initWithFile(file->c_str())) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

CCSpriteWithHue* CCSpriteWithHue::createWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
    if (!frame) {
        return nullptr;
    }

    CCSpriteWithHue* sprite = new CCSpriteWithHue();
    if (sprite && sprite->initWithSpriteFrame(frame)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

CCSpriteWithHue* CCSpriteWithHue::createWithSpriteFrameName(const std::string* frameName) {
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->c_str());
    return createWithSpriteFrame(frame);
}

CCSpriteWithHue* CCSpriteWithHue::createWithTexture(cocos2d::CCTexture2D* texture) {
    CCSpriteWithHue* sprite = new CCSpriteWithHue();
    if (sprite && sprite->initWithTexture(texture)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

void CCSpriteWithHue::draw() {
    if (m_bDontDraw || !_realOpacity) {
        return;
    }

    cocos2d::ccGLEnable(m_eGLServerState);

    cocos2d::CCGLProgram* program = this->getShaderProgram();
    program->use();
    program->setUniformsForBuiltins();

    cocos2d::ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);

    if (m_pobTexture) {
        cocos2d::ccGLBindTexture2D(m_pobTexture->getName());
    }

    cocos2d::ccGLEnableVertexAttribs(cocos2d::kCCVertexAttribFlag_PosColorTex);

    program->setUniformLocationWithMatrix3fv(m_uHueLoc, m_colorMatrix.data()->data(), 1);
    program->setUniformLocationWith1f(m_uAlphaLoc, getAlpha());
    program->setUniformLocationWith3f(m_uLumLoc, m_luminance[0], m_luminance[1], m_luminance[2]);

    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ++g_uNumberOfDraws;
}

float CCSpriteWithHue::getAlpha() {
    return static_cast<float>(_displayedOpacity) / 255.0f;
}

float CCSpriteWithHue::getHue() {
    return m_hue;
}

std::string CCSpriteWithHue::getShaderName() {
    return "hue_program";
}

void CCSpriteWithHue::getUniformLocations() {
    cocos2d::CCGLProgram* program = this->getShaderProgram();
    m_uHueLoc = program->getUniformLocation("u_hue");
    m_uAlphaLoc = program->getUniformLocation("u_alpha");
    m_uLumLoc = program->getUniformLocation("u_lum");
}

void CCSpriteWithHue::initShader() {
    std::string shaderName = getShaderName();
    cocos2d::CCGLProgram* program = cocos2d::CCShaderCache::sharedShaderCache()->programForKey(shaderName.c_str());

    if (!program) {
        program = new cocos2d::CCGLProgram();
        program->initWithVertexShaderByteArray(cocos2d::ccPositionTextureColor_vert, shaderBody());
        program->addAttribute("a_position", 0);
        program->addAttribute("a_color", 1);
        program->addAttribute("a_texCoord", 2);
        program->link();
        program->updateUniforms();

        cocos2d::CCShaderCache::sharedShaderCache()->addProgram(program, shaderName.c_str());
        program->release();
    }

    this->setShaderProgram(program);
    getUniformLocations();
    updateColor();
}

bool CCSpriteWithHue::initWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
    if (!frame) {
        return false;
    }

    bool result = this->initWithTexture(frame->getTexture(), &frame->m_obRect);
    this->setDisplayFrame(frame);
    return result;
}

bool CCSpriteWithHue::initWithTexture(cocos2d::CCTexture2D* texture) {
    if (!cocos2d::CCSprite::initWithTexture(texture)) {
        return false;
    }

    setupDefaultSettings();
    initShader();
    return true;
}

void CCSpriteWithHue::setCustomLuminance(float r, float g, float b) {
    m_luminance[0] = r;
    m_luminance[1] = g;
    m_luminance[2] = b;
}

void CCSpriteWithHue::setEvenLuminance(float luminance) {
    float value = luminance * 0.33f;
    m_luminance[0] = value;
    m_luminance[1] = value;
    m_luminance[2] = value;
}

void CCSpriteWithHue::setHue(float hue) {
    m_hue = hue;
    updateColorMatrix();
}

void CCSpriteWithHue::setHueDegrees(float degrees) {
    while (degrees < 0.0f) {
        degrees += 360.0f;
    }
    float radians = (degrees / 360.0f) * 6.28318531f;
    setHue(radians);
}

void CCSpriteWithHue::setLuminance(float luminance) {
    m_luminance[0] = luminance * 0.21f;
    m_luminance[1] = luminance * 0.72f;
    m_luminance[2] = luminance * 0.07f;
}

void CCSpriteWithHue::setupDefaultSettings() {
    m_hue = 0.0f;
    setLuminance(1.0f);
}

void CCSpriteWithHue::updateColor() {
    cocos2d::CCSprite::updateColor();
    updateColorMatrix();
}

void CCSpriteWithHue::updateColorMatrix() {
    hueMatrix(m_colorMatrix.data(), m_hue);
    float alpha = getAlpha();
    premultiplyAlpha(m_colorMatrix.data(), alpha);
}

void CCSpriteWithHue::updateHue(float unused) {
    float newHue = m_hue + 0.0523598776f;
    setHue(newHue);
}

const char* CCSpriteWithHue::shaderBody() {
    return "\n"
           "    #ifdef GL_ES\n"
           "    precision mediump float;\n"
           "    #endif\n"
           "\n"
           "    varying vec4 v_fragmentColor;\n"
           "    varying vec2 v_texCoord;\n"
           "    uniform sampler2D u_texture;\n"
           "\n"
           "    uniform mat3 u_hue;\n"
           "    uniform float u_alpha;\n"
           "    uniform vec3 u_lum;\n"
           "\n"
           "    void main(void) {\n"
           "        vec4 color = texture2D(u_texture, v_texCoord);\n"
           "        vec3 transformed = u_hue * color.rgb;\n"
           "        float gray = dot(transformed * v_fragmentColor.rgb, u_lum);\n"
           "        vec3 result = v_fragmentColor.rgb * vec3(gray);\n"
           "        gl_FragColor = vec4(result, color.a * v_fragmentColor.a * u_alpha);\n"
           "    }\n"
           "    ";
}

CCSpriteWithHue::~CCSpriteWithHue() {
    // Destructor
}