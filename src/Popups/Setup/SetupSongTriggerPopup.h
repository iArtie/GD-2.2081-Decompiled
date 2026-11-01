#pragma once

#include "Platform/cocos2d.h"
#include "Utils/GJEnums.h"
#include "Popups/Setup/SetupTriggerPopup.h"
#include "Popups/Setup/SetupAudioTriggerPopup.h"
#include "Delegates/SongPlaybackDelegate.h"
#include "Delegates/MusicDownloadDelegate.h"

class EffectGameObject;
class SongSelectNode;
class SongInfoObject;

class SetupSongTriggerPopup : public SetupAudioTriggerPopup, public SongPlaybackDelegate, public MusicDownloadDelegate {
public:
    static SetupSongTriggerPopup* create(EffectGameObject* object, cocos2d::CCArray* objects);
    bool init(EffectGameObject* object, cocos2d::CCArray* objects);

    void onClose(cocos2d::CCObject* sender);
    void onPlayback(SongInfoObject* object) override;
    void onResetSongTime(cocos2d::CCObject* sender);
    void onSavePlaybackPos(cocos2d::CCObject* sender);
    void pageChanged() override;
    void updateApplyPrepare(bool hideAll);
    void updateLength();
    void updateSongTimeSlider();
    void valueDidChange(int tag, float value) override;

    ~SetupSongTriggerPopup();

protected:
    EffectGameObject* m_gameObject = nullptr;
    SongSelectNode* m_selectNode = nullptr;
    bool m_hideAll = false;
};