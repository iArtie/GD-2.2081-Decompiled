#pragma once

class GJSpecialColorSelect;

class GJSpecialColorSelectDelegate {
public:
    virtual void colorSelectClosed(GJSpecialColorSelect* select, int colorID);
    virtual ~GJSpecialColorSelectDelegate() = default;
};