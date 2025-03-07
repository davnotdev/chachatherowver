#ifndef MAINUI_HPP
#define MAINUI_HPP

#include "ui.hpp"
#include "serialin.hpp"

class MainUI: public UI {
public:
    MainUI();
    void render(ImGuiIO& io) override;
private:
    int scanTheta;
    std::vector<SensorPoint> dataPoints;
};

#endif
