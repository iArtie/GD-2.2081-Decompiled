#include "SongsLayer.h"
#include "SongObject.h"
#include "CustomListView.h"

SongsLayer* SongsLayer::create() {
    SongsLayer* returnVal = new SongsLayer();
    if (GJDropDownLayer::init("Soundtrack")) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void SongsLayer::customSetup() {
    cocos2d::CCArray* songs = cocos2d::CCArray::create();
    for (int i = 0; i <= 21; i++) {
        SongObject* song = SongObject::create(i);
        songs->addObject(song);
    }
    songs->addObject(SongObject::create(-1));

    CustomListView* listView = CustomListView::create(songs, BoomListType::Song, 220.0f, 356.0f);
    m_listLayer->addChild(listView, 6);
}

SongsLayer::~SongsLayer() {
}