#pragma once

#include "Platform/cocos2d.h"

enum class GJKeyCommand {
    None = 0,
    Jump = 1,
    Pause = 2,
    PlaceCheckpoint = 3,
    DeleteCheckpoint = 4,
    BuildMode = 5,
    EditMode = 6,
    DeleteMode = 7,
    SwipeModifier = 8,
    MoveModifier = 9,
    RotateCCW = 10,
    RotateCW = 11,
    FlipX = 12,
    FlipY = 13,
    DeleteSelected = 14,
    Undo = 15,
    Redo = 16,
    Deselect = 17,
    Copy = 18,
    Paste = 19,
    Duplicate = 20,
    Rotate = 21,
    FreeMove = 22,
    Swipe = 23,
    Snap = 24,
    Playtest = 25,
    PlaybackMusic = 26,
    NextGroup = 27,
    PreviousGroup = 28,
    ObjectLeft = 29,
    ObjectRight = 30,
    ObjectUp = 31,
    ObjectDown = 32,
    ObjectLeftSmall = 33,
    ObjectRightSmall = 34,
    ObjectUpSmall = 35,
    ObjectDownSmall = 36,
    LockPreview = 37,
    UnlockPreview = 38,
    PreviewMode = 39,
    ParticleIcons = 40,
    DebugDraw = 41,
    HideInvisible = 42,
    SaveEditorPos = 43,
    LoadEditorPos = 44,
    Zoom = 45,
    Transform = 46
};

enum class GJKeyGroup {
    Gameplay = 1,
    Editor = 2
};

class DS_Dictionary;

class KeybindingsManager : public cocos2d::CCNode {
public:
    bool commandForKeyMods(cocos2d::enumKeyCodes key, GJKeyGroup group);
    bool commandForKeyNoMods(cocos2d::enumKeyCodes key, GJKeyGroup group);
    cocos2d::CCDictionary* commandToKeyForGroup(GJKeyGroup group);
    void dataLoaded(DS_Dictionary* dict);
    void encodeDataTo(DS_Dictionary* dict);
    void firstSetup();
    GJKeyGroup groupForCommand(GJKeyCommand command);
    bool init() override;
    cocos2d::enumKeyCodes keyForCommand(GJKeyCommand command);
    cocos2d::CCDictionary* keyToCommandForGroup(GJKeyGroup group);
    static KeybindingsManager* sharedState();
    virtual ~KeybindingsManager();

private:
    cocos2d::CCDictionary* m_keyToCommandDict;
    cocos2d::CCDictionary* m_commandToKeyDict;
    
    static KeybindingsManager* s_sharedInstance;
};