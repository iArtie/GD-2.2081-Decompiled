#pragma once

#include <string>

class LevelRateInfoDelegate {
public:
    void rateInfoFailed(int id, int response);
    void rateInfoFinished(int id, std::string info);
};