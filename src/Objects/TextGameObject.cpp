#include "Objects/TextGameObject.h"
#include "Layers/GJBaseGameLayer.h"
#include "Managers/GameManager.h"
#include "Utils/LevelTools.h"

TextGameObject* TextGameObject::create(cocos2d::CCTexture2D* texture) {
    TextGameObject* returnVal = new TextGameObject();
    if (returnVal->init(texture)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

bool TextGameObject::init(cocos2d::CCTexture2D* texture) {
    return GameObject::init("", texture);
}

std::string TextGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = GameObject::getSaveString(layer);
    saveString += ",31,";
    saveString += LevelTools::base64EncodeString(m_text);
    if (m_kerning) {
        saveString += ",488,";
        saveString += std::to_string(m_kerning);
    }
    return saveString;
}

int TextGameObject::getTextKerning() {
    return m_kerning;
}

void TextGameObject::updateTextKerning(int kerning) {
    m_kerning = kerning;
}

void TextGameObject::updateTextObject(std::string text, bool defaultFont) {
    if (text.empty())
        text = "A";

    m_text = text;

    cocos2d::ccColor3B color = {255, 255, 255};
    if (m_pChildren && m_pChildren->count() > 0) {
        cocos2d::CCNode* child = (cocos2d::CCNode*)m_pChildren->objectAtIndex(0);
        color = child->getColor();
    }

    bool hasColor = false;
    if (m_isSelected) {
        hasColor = true;
        if (color.r == 255 && color.g == 255 && color.b == 255)
            hasColor = false;
    }

    removeAllChildrenWithCleanup(true);
    cocos2d::CCArray* letters = cocos2d::CCArray::create();

    const char* fontFile;
    if (defaultFont)
        fontFile = "bigFont.fnt";
    else
        fontFile = GameManager::getFontFile(GameManager::sharedState()->m_loadedFont);

    cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::createBatched(text.c_str(), fontFile, letters, m_kerning);
    setContentSize(label->getContentSize());
    label->setPosition(cocos2d::CCNode::convertToNodeSpace(this, &cocos2d::CCPoint(0.0f, 0.0f)));

    for (unsigned int i = 0; i < letters->count(); i++) {
        cocos2d::CCNode* letter = (cocos2d::CCNode*)letters->objectAtIndex(i);
        if (hasColor)
            letter->setColor(color);
        addChild(letter);
    }

    m_hasCustomChild = true;
    m_isObjectRectDirty = true;
    m_isOrientedBoxDirty = true;
    m_isDirty = true;
    setDirty(true);
    m_bDontDraw = true;
}

TextGameObject::~TextGameObject() {
}