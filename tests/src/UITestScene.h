#pragma once

#include "BaseScene.h"
#include "Pesukarhu/ppk.h"
#include "Pesukarhu/ui/TopBarPanel.h"

class UITestScene : public BaseScene
{
private:
    pk::ui::Panel _testPanel;
    pk::ui::Panel _testPanel2;
    pk::ui::TopBarPanel _topBarPanelTest;

public:
    UITestScene();
    virtual ~UITestScene();
    void init();

    void update();
};
