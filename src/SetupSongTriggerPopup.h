#pragma once

#include "cocos2d.h"
#include "GJEnums.h"
#include "SetupTriggerPopup.h"
#include "SetupAudioTriggerPopup.h"
#include "SongPlaybackDelegate.h"
#include "MusicDownloadDelegate.h"

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