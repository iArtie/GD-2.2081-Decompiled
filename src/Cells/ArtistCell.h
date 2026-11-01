#pragma once

#include "Platform/cocos2d.h"

class SongInfoObject;
class TableViewCell;

class ArtistCell {
public:
    ArtistCell(char* charPtr, float value, float value2);
    void draw();
    bool init();
    void loadFromObject(SongInfoObject* object);
    void onNewgrounds(cocos2d::CCObject* sender);
    void onYouTube(cocos2d::CCObject* sender);
    void updateBGColor(int index);
    ~ArtistCell();

private:
    SongInfoObject* m_songInfo;
    float m_width;
    float m_height;
    TableViewCell* m_tableViewCell;
    cocos2d::CCLayer* m_mainLayer;
    cocos2d::CCLayerColor* m_backgroundLayer;
};