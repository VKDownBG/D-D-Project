//
// Created by Lenovo on 3.6.2025 г.
//

#ifndef DEFEATPANEL_H
#define DEFEATPANEL_H

#include "C:/DandD/include/UI/widgets/Button.h"

class DefeatPanel {
public:
    void Show();

    void Update();

    void Draw() const;

    bool ShouldQuit() const;

private:
    bool isActive = false;
    bool quitSelected = false;
    Button quitButton;
};

#endif //DEFEATPANEL_H
