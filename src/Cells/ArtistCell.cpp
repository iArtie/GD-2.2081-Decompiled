#include "Cells/ArtistCell.h"
#include "Models/SongInfoObject.h"
#include "Cells/TableViewCell.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Platform/cocos2d.h"

ArtistCell::ArtistCell(char* charPtr, float value, float value2)
    : TableViewCell(charPtr, value, value2)
{
    init();
}

void ArtistCell::draw()
{
    cocos2d::CCSize size(m_width, m_height);
    
    cocos2d::ccDrawColor4B(0, 0, 0, 75);
    glLineWidth(1.0f);
    
    cocos2d::CCPoint p1(1.0f, size.height);
    cocos2d::CCPoint p2(size.width - 1.0f, size.height);
    cocos2d::ccDrawLine(p1, p2);
    
    cocos2d::CCPoint p3(1.0f, 0.0f);
    cocos2d::CCPoint p4(size.width - 1.0f, 0.0f);
    cocos2d::ccDrawLine(p3, p4);
}

bool ArtistCell::init()
{
    m_songInfo = nullptr;
    return true;
}

void ArtistCell::loadFromObject(SongInfoObject* object)
{
    m_mainLayer->removeAllChildrenWithCleanup(true);
    m_songInfo = object;
    
    // Artist name label
    cocos2d::CCLabelBMFont* artistLabel = cocos2d::CCLabelBMFont::create(
        object->m_artistName.c_str(),
        "bigFont.fnt"
    );
    artistLabel->limitLabelWidth(200.0f, 0.8f, 0.0f);
    m_mainLayer->addChild(artistLabel);
    artistLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    artistLabel->setPosition(15.0f, m_height * 0.5f);
    
    // Create menu for buttons
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    m_mainLayer->addChild(menu, 3);
    
    // Newgrounds button
    ButtonSprite* ngButton = ButtonSprite::create(
        "View",
        nullptr,
        0,
        0,
        0.6f,
        true,
        "bigFont.fnt",
        "GJ_button_01.png",
        nullptr,
        30.0f
    );
    ngButton->setScale(0.9f);
    
    CCMenuItemSpriteExtra* ngMenuItem = CCMenuItemSpriteExtra::create(
        ngButton,
        nullptr,
        this,
        menu_selector(ArtistCell::onNewgrounds)
    );
    ngMenuItem->setSizeMult(1.2f);
    menu->addChild(ngMenuItem);
    
    cocos2d::CCPoint ngPos = menu->convertToNodeSpace(
        cocos2d::CCPoint(m_width - 38.0f, m_height * 0.5f)
    );
    ngMenuItem->setPosition(ngPos);
    
    // YouTube button (only if channel exists)
    if (!object->m_youtubeChannel.empty())
    {
        cocos2d::CCSprite* ytIcon = cocos2d::CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png");
        ytIcon->setScale(0.9f);
        
        CCMenuItemSpriteExtra* ytMenuItem = CCMenuItemSpriteExtra::create(
            ytIcon,
            nullptr,
            this,
            menu_selector(ArtistCell::onYouTube)
        );
        ytMenuItem->setSizeMult(1.2f);
        menu->addChild(ytMenuItem);
        
        cocos2d::CCPoint ytPos = ngMenuItem->getPosition();
        ytPos.x -= 52.0f;
        ytMenuItem->setPosition(ytPos);
    }
}

void ArtistCell::onNewgrounds(cocos2d::CCObject* sender)
{
    if (!m_songInfo)
        return;
    
    cocos2d::CCApplication* app = cocos2d::CCApplication::sharedApplication();
    
    std::string url = "https://" + m_songInfo->m_artistName + ".newgrounds.com";
    app->openURL(url.c_str());
}

void ArtistCell::onYouTube(cocos2d::CCObject* sender)
{
    if (!m_songInfo)
        return;
    
    cocos2d::CCApplication* app = cocos2d::CCApplication::sharedApplication();
    
    std::string url = "https://www.youtube.com/" + m_songInfo->m_youtubeChannel;
    app->openURL(url.c_str());
}

void ArtistCell::updateBGColor(int index)
{
    cocos2d::ccColor3B color;
    if (index & 1)
    {
        color.r = 194;
        color.g = 114;
        color.b = 62;
    }
    else
    {
        color.r = 161;
        color.g = 88;
        color.b = 44;
    }
    
    m_backgroundLayer->setColor(color);
    m_backgroundLayer->setOpacity(255);
}

ArtistCell::~ArtistCell()
{
    // Destructor
}