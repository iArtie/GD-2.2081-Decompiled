#include "Editor/GJSmartBlockPreview.h"

GJSmartBlockPreview* GJSmartBlockPreview::create(const std::string& key) {
    auto preview = new GJSmartBlockPreview();
    if (preview && preview->init(key)) {
        preview->autorelease();
        return preview;
    }
    delete preview;
    return nullptr;
}

bool GJSmartBlockPreview::init(const std::string& key) {
    if (!CCNode::init()) {
        return false;
    }
    
    setContentSize(cocos2d::CCSize(90.0f, 90.0f));
    m_prefabKey = key;
    m_infoLabel = nullptr;
    m_selectSprite = nullptr;
    m_selected = false;
    
    return true;
}

void GJSmartBlockPreview::addChance(int count, int total) {
    createInfoLabel();
    
    if (total == 0) {
        m_infoLabel->setVisible(false);
        return;
    }
    
    m_infoLabel->setPosition(cocos2d::CCPoint(45.0f, -15.0f));
    
    std::string text;
    if (count > 0) {
        float percent = (static_cast<float>(count) / total) * 100.0f;
        text = cocos2d::CCString::createWithFormat("%i (%.1f%%)", count, percent)->getCString();
    } else {
        text = "0";
    }
    
    m_infoLabel->setString(text.c_str());
    m_infoLabel->limitLabelWidth(100.0f, 0.8f, 0.0f);
    m_infoLabel->setVisible(true);
}

void GJSmartBlockPreview::addCount(int count) {
    createInfoLabel();
    
    m_infoLabel->setPosition(cocos2d::CCPoint(90.0f, 90.0f));
    m_infoLabel->setString(cocos2d::CCString::createWithFormat("%i", count)->getCString());
    m_infoLabel->limitLabelWidth(20.0f, 0.6f, 0.0f);
}

void GJSmartBlockPreview::createInfoLabel() {
    if (m_infoLabel) {
        return;
    }
    
    m_infoLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(m_infoLabel);
}

void GJSmartBlockPreview::toggleSelectItem(bool selected) {
    if (m_selected == selected) {
        return;
    }
    
    m_selected = selected;
    
    if (!m_selectSprite) {
        m_selectSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
        addChild(m_selectSprite);
        m_selectSprite->setScale(2.8f);
        m_selectSprite->setPosition(cocos2d::CCPoint(45.0f, 45.0f));
    }
    
    m_selectSprite->setVisible(selected);
}

GJSmartBlockPreview::~GJSmartBlockPreview() {
}