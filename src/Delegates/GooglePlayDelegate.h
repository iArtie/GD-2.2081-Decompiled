#pragma once

class GooglePlayDelegate {
public:
    virtual void googlePlaySignedIn();
    virtual ~GooglePlayDelegate() = default;
};