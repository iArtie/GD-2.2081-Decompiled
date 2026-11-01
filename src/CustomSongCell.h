#pragma once

#include "cocos2d.h"
#include "TableViewCell.h"
#include "CustomSongDelegate.h"
#include "SongInfoObject.h"
#include <string>

class CustomSongCell : public TableViewCell, public CustomSongDelegate {
public:
    static CustomSongCell* create(const char* identifier, float width, float height);
    bool init();
    
    void draw() override;
    void loadFromObject(SongInfoObject* object);
    void onDelete(cocos2d::CCObject* sender);
    void updateBGColor(int index) override;
    bool shouldReload();
    int getActiveSongID();
    void songIDChanged(int id) override;
    
    // CustomSongDelegate methods
    LevelSettingsObject* getLevelSettings() override;
    std::string getSongFileName() override;
    
    virtual ~CustomSongCell();

protected:
    SongInfoObject* m_songInfoObject;
    bool m_selected;
};