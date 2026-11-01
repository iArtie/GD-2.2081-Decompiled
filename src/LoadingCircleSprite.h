#pragma once

class LoadingCircleSprite {
public:
    static LoadingCircleSprite * create(float spinSpeed);
    void hideCircle();
    bool init(float spinSpeed);
    ~LoadingCircleSprite();

private:
    void* setBlendFunc; // tipo no inferido (nunca se copio a una local con nombre)
    void* setOpacity; // tipo no inferido (nunca se copio a una local con nombre)
    void* setVisible; // tipo no inferido (nunca se copio a una local con nombre)
};
