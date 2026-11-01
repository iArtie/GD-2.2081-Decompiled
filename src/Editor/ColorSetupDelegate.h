#pragma once

class ColorSetupDelegate {
public:
    virtual ~ColorSetupDelegate() {}

    virtual void colorSetupClosed(int id);
};