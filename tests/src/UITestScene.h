#pragma once

#include "BaseScene.h"
#include "pesukarhu/ppk.h"
#include "pesukarhu/ui/Select.h"
#include "pesukarhu/ui/TopBarPanel.h"

class UITestScene : public BaseScene
{
private:
    pk::ui::Panel _testPanel;
    pk::ui::TopBarPanel _topBarPanelTest;
    pk::ui::Panel _scrollablePanel;

public:
    UITestScene();
    virtual ~UITestScene();
    void init();

    void update();
};
