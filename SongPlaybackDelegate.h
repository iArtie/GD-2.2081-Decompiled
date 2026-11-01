#pragma once

class SongInfoObject;

class SongPlaybackDelegate {
public:
    virtual void onPlayback(SongInfoObject* object);
};