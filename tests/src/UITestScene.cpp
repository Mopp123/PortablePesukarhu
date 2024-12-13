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
    float verticalVal = testPanelScale.y;
    _testPanel.createDefault(
        this,
        _pDefaultFont,
        {
            HorizontalConstraintType::PIXEL_RIGHT, -testPanelScale.x,
            VerticalConstraintType::PIXEL_TOP, -120
        },
        testPanelScale,
        testPanelSlotScale,
        Panel::LayoutFillType::VERTICAL
    );

    _testPanel.addDefaultText("Testing");
    _testPanel.addDefaultButton("Button", nullptr, 150);
    _testPanel.addDefaultInputField("InputField", 150, nullptr);
    _testPanel.addDefaultCheckbox("Checkbox");

    // Testing top bar panel
    vec2 testTopBarPanelScale(200, 175);
    _topBarPanelTest.initBase(
        this,
        _pDefaultFont,
        "Top bar testing",
        {
            HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL, 0.0f,
            VerticalConstraintType::PIXEL_TOP, 0
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
            HorizontalConstraintType::PIXEL_RIGHT, -200,
            VerticalConstraintType::PIXEL_BOTTOM, 320
        },
        { 200, 320 }, // scale
        testPanelSlotScale,
        Panel::LayoutFillType::VERTICAL,
        true
    );
    for (int i = 0; i < 22; ++i)
        _scrollablePanel.addDefaultButton("Testing" + std::to_string(i), nullptr, 100);
    _scrollablePanel.addDefaultText("Scroll panel test");
    _scrollablePanel.addDefaultInputField("Input", 100, nullptr);
    _scrollablePanel.addDefaultCheckbox("Checking1");
    _scrollablePanel.addDefaultButton("Button", nullptr, 100);
}

void UITestScene::update()
{}
