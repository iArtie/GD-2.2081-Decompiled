#pragma once

class Slider;

class SliderDelegate {
public:
    virtual void sliderBegan(Slider* slider);
    virtual void sliderEnded(Slider* slider);
};