#include "UITestScene.h"

using namespace pk;
using namespace pk::ui;


UITestScene::UITestScene()
{}

UITestScene::~UITestScene()
{}

void UITestScene::init()
{
    initBase();
    setInfoText("UITestScene", { 1, 1, 1 });

    // Testing regular panel
    vec2 testPanelScale(300, 200);
    vec2 testPanelSlotScale(100, 24);
    _testPanel.createDefault(
        this,
        _pDefaultFont,
        {
            HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL, 100.0f,
            VerticalConstraintType::PIXEL_BOTTOM, 0
        },
        testPanelScale,
        testPanelSlotScale,
        Panel::LayoutFillType::VERTICAL
    );

    _testPanel.addDefaultText("Testing");
    _testPanel.addDefaultInputField("Input", 100, nullptr);
    _testPanel.addDefaultCheckbox("Checking1");

    // Testing top bar panel
    vec2 testTopBarPanelScale(200, 175);
    _topBarPanelTest.initBase(
        this,
        _pDefaultFont,
        "Top bar testing",
        {
            HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL, 0.0f,
            VerticalConstraintType::PIXEL_TOP, 0.0f
        },
        testTopBarPanelScale,
        Panel::LayoutFillType::VERTICAL
    );
    _topBarPanelTest.addDefaultButton("Button", nullptr, 100);

    // Testing scrollable panel
    _scrollablePanel.createDefault(
        this,
        _pDefaultFont,
        {
            HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL, 350.0f,
            VerticalConstraintType::PIXEL_CENTER_VERTICAL, 0.0f
        },
        testPanelScale,
        testPanelSlotScale,
        Panel::LayoutFillType::HORIZONTAL
    );
    _scrollablePanel.addDefaultText("Testing2");
    _scrollablePanel.addDefaultButton("Testing2", nullptr, 100);
}

void UITestScene::update()
{}
