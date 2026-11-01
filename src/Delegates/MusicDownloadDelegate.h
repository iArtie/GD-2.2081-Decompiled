#pragma once

#include "Utils/GJEnums.h"

class SongInfoObject;

class MusicDownloadDelegate {
public:
    void downloadSFXFailed(int id, GJSongError errorType);
    void downloadSFXFinished(int id);
    void downloadSongFailed(int id, GJSongError errorType);
    void downloadSongFinished(int id);
    void downloadSongStarted(int id);
    void loadSongInfoFailed(int id, GJSongError errorType);
    void loadSongInfoFinished(SongInfoObject* object);
    void musicActionFailed(GJMusicAction action);
    void musicActionFinished(GJMusicAction action);
    void songStateChanged();
};