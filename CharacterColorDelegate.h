#pragma once

enum class UnlockType;

class CharacterColorDelegate {
public:
    virtual ~CharacterColorDelegate() {}

    virtual void playerColorChanged();
    virtual void showUnlockPopup(int id, UnlockType type);
};