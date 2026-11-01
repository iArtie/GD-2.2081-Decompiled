#pragma once

#include <string>

class GJBaseGameLayer;

class ArtTriggerGameObject {
public:
    static ArtTriggerGameObject* create(const char* frame);
    std::string getSaveString(GJBaseGameLayer* layer);
    ~ArtTriggerGameObject();

private:
    int m_artIndex;
};