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

    vec2 testPanelScale(100, 200);
    vec2 testPanelSlotScale(100, 24);
    _testPanel.createDefault(
        this,
        _pDefaultFont,
        {
            HorizontalConstraintType::PIXEL_RIGHT, 8.0f,
            VerticalConstraintType::PIXEL_BOTTOM, 8.0f
        },
        testPanelScale,
        testPanelSlotScale,
        Panel::LayoutFillType::VERTICAL
    );
    _testPanel.addDefaultText("Testing");

    _testPanel2.createDefault(
        this,
        _pDefaultFont,
        {
            HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL, 100.0f,
            VerticalConstraintType::PIXEL_CENTER_VERTICAL, 0.0f
        },
        testPanelScale,
        testPanelSlotScale,
        Panel::LayoutFillType::VERTICAL
    );
    _testPanel2.addDefaultText("Testing2");


    vec2 testTopBarPanelScale(200, 175);
    _topBarPanelTest.initBase(
        this,
        _pDefaultFont,
        "Top bar test",
        {
            HorizontalConstraintType::PIXEL_CENTER_HORIZONTAL, 0.0f,
            VerticalConstraintType::PIXEL_CENTER_VERTICAL, 200.0f
        },
        testTopBarPanelScale,
        Panel::LayoutFillType::VERTICAL
    );

    _topBarPanelTest.addDefaultButton("Button", nullptr, 100);
}

void UITestScene::update()
{}
