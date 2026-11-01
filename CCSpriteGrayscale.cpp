#include "CCSpriteGrayscale.h"
#include "cocos2d.h"

CCSpriteGrayscale* CCSpriteGrayscale::create(const std::string* file) {
    CCSpriteGrayscale* sprite = new CCSpriteGrayscale();
    if (sprite && sprite->initWithFile(file->c_str())) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

CCSpriteGrayscale* CCSpriteGrayscale::createWithSpriteFrame(cocos2d::CCSpriteFrame* frame) {
    if (!frame) {
        return nullptr;
    }

    CCSpriteGrayscale* sprite = new CCSpriteGrayscale();
    if (sprite && sprite->initWithSpriteFrame(frame)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

CCSpriteGrayscale* CCSpriteGrayscale::createWithSpriteFrameName(const std::string* frameName) {
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->c_str());
    return createWithSpriteFrame(frame);
}

CCSpriteGrayscale* CCSpriteGrayscale::createWithTexture(cocos2d::CCTexture2D* texture) {
    CCSpriteGrayscale* sprite = new CCSpriteGrayscale();
    if (sprite && sprite->initWithTexture(texture)) {
        sprite->autorelease();
        return sprite;
    }
    delete sprite;
    return nullptr;
}

std::string CCSpriteGrayscale::getShaderName() {
    return "grayscale_program";
}

const char* CCSpriteGrayscale::shaderBody() {
    return "\n"
           "    #ifdef GL_ES\n"
           "    precision mediump float;\n"
           "    #endif\n"
           "\n"
           "    varying vec4 v_fragmentColor;\n"
           "    varying vec2 v_texCoord;\n"
           "    uniform sampler2D u_texture;\n"
           "\n"
           "    uniform vec3 u_lum;\n"
           "\n"
           "    void main(void) {\n"
           "        vec4 color = texture2D(u_texture, v_texCoord);\n"
           "        float gray = dot(color.rgb * v_fragmentColor.rgb, u_lum);\n"
           "        float normalization = (v_fragmentColor.r + v_fragmentColor.g + v_fragmentColor.b) / 3.0;\n"
           "        gray /= normalization;\n"
           "        gl_FragColor = vec4(v_fragmentColor.rgb*vec3(gray), color.a * v_fragmentColor.a);\n"
           "    }\n"
           "    ";
}

CCSpriteGrayscale::~CCSpriteGrayscale() {
    // Destructor
}