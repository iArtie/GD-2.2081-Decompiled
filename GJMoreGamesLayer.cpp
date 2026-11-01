#include "GJMoreGamesLayer.h"
#include "CustomListView.h"
#include "PlatformToolbox.h"

GJMoreGamesLayer* GJMoreGamesLayer::create() {
    auto layer = new GJMoreGamesLayer();
    if (layer && layer->init("RobTop Games")) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

void GJMoreGamesLayer::customSetup() {
    auto gamesList = getMoreGamesList();
    m_moreGamesList = gamesList;
    m_moreGamesList->retain();
    
    auto listView = CustomListView::create(m_moreGamesList, BoomListType::Unknown, 220.0f, 356.0f);
    m_listLayer->addChild(listView, 6);
    
    if (m_moreGamesList->count() == 0) {
        auto director = cocos2d::CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        
        auto label = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        label->setScale(0.6f);
        m_mainLayer->addChild(label, 10);
        label->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    }
}

cocos2d::CCArray* GJMoreGamesLayer::getMoreGamesList() {
    auto format = cocos2d::CCTexture2D::defaultAlphaPixelFormat();
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
    
    auto result = cocos2d::CCArray::create();
    
    std::vector<std::string> gameIds = {"gjm", "gjw", "gjz"};
    
    for (const auto& gameId : gameIds) {
        std::string filename;
        if (PlatformToolbox::isHD()) {
            filename = "promo_" + gameId + "-hd.png";
        } else {
            filename = "promo_" + gameId + ".png";
        }
        
        if (PlatformToolbox::doesFileExist(filename)) {
            result->addObject(cocos2d::CCString::create(gameId));
            cocos2d::CCTextureCache::sharedTextureCache()->addImage(filename.c_str());
        }
    }
    
    cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(format);
    
    return result;
}

GJMoreGamesLayer::~GJMoreGamesLayer() {
    if (m_moreGamesList) {
        m_moreGamesList->release();
    }
}