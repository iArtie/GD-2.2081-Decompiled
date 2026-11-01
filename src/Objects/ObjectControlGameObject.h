#pragma once

#include <string>
#include "Objects/EffectGameObject.h"

class GJBaseGameLayer;

class ObjectControlGameObject : public EffectGameObject {
public:
    static ObjectControlGameObject* create();

    bool init() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;

    virtual ~ObjectControlGameObject();

protected:
    int m_targetGroupID = 0;
};