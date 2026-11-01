#include "LabelGameObject.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "GJBaseGameLayer.h"

LabelGameObject* LabelGameObject::create() {
    auto obj = new LabelGameObject();
    if (obj && obj->init()) {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool LabelGameObject::init() {
    if (!GameObject::init("edit_eCounterBtn_001.png")) {
        return false;
    }
    m_bUnkBool2 = false;
    m_label = nullptr;
    m_labelColorLocked = false;
    m_updateLabel = false;
    m_labelDirty = false;
    m_kerning = 0;
    m_alignment = 0;
    m_shownSpecial = 0;
    m_isTimeCounter = false;
    m_itemID = 0;
    m_showSecondsOnly = 0;
    m_shouldBlendBase = false;
    _realOpacity = 255;
    _realColor = cocos2d::ccc3(255, 255, 255);
    return true;
}

void LabelGameObject::addMainSpriteToParent(bool reorder) {
    GameObject::addMainSpriteToParent(reorder);
    
    if (!m_label) {
        return;
    }
    
    if (m_shouldBlendBase) {
        m_label->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    } else {
        m_label->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
    }
}

void LabelGameObject::setupCustomSprites(const std::string& frameName) {
    GameObject::setupCustomSprites(frameName);
    
    if (m_objectID == 1615) {
        m_bDontDraw = true;
        createLabel("0");
    }
}

void LabelGameObject::createLabel(const std::string& text) {
    if (m_label) {
        removeLabel();
    }
    
    auto manager = GameManager::sharedState();
    auto fontFile = manager->getFontFile(manager->m_loadedFont);
    
    m_label = cocos2d::CCLabelBMFont::create(text.c_str(), fontFile.c_str());
    m_label->setPosition(cocos2d::CCPoint(m_obRect.size.width * 0.5f, m_obRect.size.height * 0.5f));
    addChild(m_label);
    
    updateLabelAlign(m_alignment);
    updateTextKerning(m_kerning);
    
    m_labelDirty = false;
}

void LabelGameObject::removeLabel() {
    if (m_label) {
        queueUpdateLabel(m_label->getString());
        m_label->removeMeAndCleanup();
        m_label = nullptr;
    }
}

void LabelGameObject::queueUpdateLabel(const std::string& text) {
    if (!m_updateLabel) {
        m_labelString = text;
        m_labelDirty = true;
    }
}

void LabelGameObject::updateLabelIfDirty() {
    if (m_labelDirty) {
        updateLabel(m_labelString);
    }
}

void LabelGameObject::updateLabel(const std::string& text) {
    if (!m_label) {
        return;
    }
    m_label->setString(text.c_str());
}

void LabelGameObject::updateLabel(float value) {
    updateLabel(std::to_string(static_cast<int>(value)));
}

void LabelGameObject::updateLabelAlign(int alignment) {
    m_alignment = alignment;
    
    if (!m_label) {
        return;
    }
    
    float anchorX = 0.5f;
    float anchorY = 0.5f;
    
    if (alignment == 1) {
        anchorX = 0.0f;
    } else if (alignment == 2) {
        anchorX = 1.0f;
    }
    
    m_label->setAnchorPoint(cocos2d::CCPoint(anchorX, anchorY));
}

void LabelGameObject::updateTextKerning(int kerning) {
    m_kerning = kerning;
    if (m_label) {
        m_label->m_nExtraKerning = kerning;
    }
}

void LabelGameObject::setObjectColor(const cocos2d::ccColor3B* color) {
    GameObject::setObjectColor(color);
    
    if (m_label && !m_labelColorLocked) {
        m_label->setColor(_realColor);
    }
}

void LabelGameObject::setOpacity(unsigned char opacity) {
    GameObject::setOpacity(opacity);
    
    if (m_label) {
        m_label->setOpacity(_realOpacity);
    }
}

void LabelGameObject::unlockLabelColor() {
    m_labelColorLocked = false;
}

void LabelGameObject::resetObject() {
    m_updateLabel = false;
    EnhancedGameObject::resetObject();
}

void LabelGameObject::updatePreviewLabel() {
    std::string text;
    
    switch (m_shownSpecial) {
        case -1:
            text = "C:GT";
            break;
        case -2:
            text = "C:GP";
            break;
        case -3:
            text = "C:AT";
            break;
        default:
            text = (m_isTimeCounter ? "T:" : "C:") + std::to_string(m_itemID);
            break;
    }
    
    updateLabel(text);
}

std::string LabelGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string result = GameObject::getSaveString(layer);
    
    char buffer[512];
    int pos = 0;
    
    auto appendInt = [&](int value, int key) {
        if (value) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, ",%i,%i", key, value);
        }
    };
    
    appendInt(m_itemID, 80);
    appendInt(m_showSecondsOnly, 389);
    appendInt(m_shownSpecial, 390);
    appendInt(m_alignment, 391);
    appendInt(m_isTimeCounter, 466);
    appendInt(m_kerning, 488);
    
    result += buffer;
    
    return result;
}

int LabelGameObject::getTextKerning() {
    return m_kerning;
}

LabelGameObject::~LabelGameObject() {
}