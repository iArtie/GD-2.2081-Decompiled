#pragma once

class GJRotationControlDelegate {
public:
    virtual void angleChangeBegin();
    virtual void angleChangeEnded();
    virtual void angleChanged(float angle);
    virtual ~GJRotationControlDelegate() = default;
};