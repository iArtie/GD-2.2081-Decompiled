#pragma once

#include "cocos2d.h"
#include "CustomSongCell.h"
#include "CustomSongDelegate.h"
#include "SongInfoObject.h"

class CustomMusicCell : public CustomSongCell, public CustomSongDelegate {
public:
    static CustomMusicCell* create(const char* identifier, float width, float height);
    bool init();
    
    void loadFromObject(SongInfoObject* object) override;
    void updateBGColor(int index) override;
    
    virtual ~CustomMusicCell();

protected:
    SongInfoObject* m_songInfoObject;
    bool m_selected;
};