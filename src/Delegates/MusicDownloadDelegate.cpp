#include "Delegates/MusicDownloadDelegate.h"
#include "Models/SongInfoObject.h"

void MusicDownloadDelegate::downloadSFXFailed(int id, GJSongError errorType)
{
}

void MusicDownloadDelegate::downloadSFXFinished(int id)
{
}

void MusicDownloadDelegate::downloadSongFailed(int id, GJSongError errorType)
{
}

void MusicDownloadDelegate::downloadSongFinished(int id)
{
}

void MusicDownloadDelegate::downloadSongStarted(int id)
{
}

void MusicDownloadDelegate::loadSongInfoFailed(int id, GJSongError errorType)
{
}

void MusicDownloadDelegate::loadSongInfoFinished(SongInfoObject* object)
{
}

void MusicDownloadDelegate::musicActionFailed(GJMusicAction action)
{
}

void MusicDownloadDelegate::musicActionFinished(GJMusicAction action)
{
}

void MusicDownloadDelegate::songStateChanged()
{
}