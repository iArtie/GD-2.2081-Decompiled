#include "Models/GJLocalScore.h"

GJLocalScore* GJLocalScore::create(int points, int type) {
    auto score = new GJLocalScore();
    if (score && score->init(points, type)) {
        score->autorelease();
        return score;
    }
    delete score;
    return nullptr;
}

bool GJLocalScore::init(int points, int type) {
    m_points = points;
    m_type = type;
    return true;
}

GJLocalScore::~GJLocalScore() {
}