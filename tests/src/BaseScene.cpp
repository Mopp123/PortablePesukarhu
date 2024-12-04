#include "BaseScene.h"
//#include "ui/Panel.h"

using namespace pk;
using namespace ui;


BaseScene::BaseScene()
{}

BaseScene::~BaseScene()
{
}

void BaseScene::init()
{
    // Set clear color
    //Application::get()->getMasterRenderer().setClearColor(Panel::get_base_ui_color(0));
    Application::get()->getMasterRenderer().setClearColor({ 0, 0, 0, 1 });

    activeCamera = createEntity();
    Camera::create(activeCamera, { 0, 1.0f, 0 }, 0.0f, 0.0f);
    pk::Debug::log("@BaseScene::initBase Loading default font");
    pk::Application* pApp = pk::Application::get();
    pk::ResourceManager& resourceManager = pApp->getResourceManager();
    _pDefaultFont = resourceManager.createFont(
        "assets/fonts/Matamata-Regular.otf",
        20
    );

    // Create directional light
    directionalLight = createEntity();
    pk::vec3 lightDirection(0.33f, -0.33f, -0.33f);
    lightDirection.normalize();
    DirectionalLight::create(
        directionalLight,
        { 1.0f, 1.0f, 1.0f },
        lightDirection
    );

    _infoTextEntity = create_text(
        "", *_pDefaultFont,
        HorizontalConstraintType::PIXEL_LEFT, 0,
        VerticalConstraintType::PIXEL_TOP, 0
    ).first;

    setInfoText("Testing testing!");
}

void BaseScene::update()
{}

void BaseScene::setInfoText(
    const std::string& txt,
    pk::vec3 color,
    float horizontalVal,
    float verticalVal,
    pk::HorizontalConstraintType horizontalConstraint,
    pk::VerticalConstraintType verticalConstraint
)
{
    ConstraintData* pConstraintData =(ConstraintData*)getComponent(_infoTextEntity, ComponentType::PK_UI_CONSTRAINT);
    pConstraintData->horizontalType = horizontalConstraint;
    pConstraintData->horizontalValue = horizontalVal;
    pConstraintData->verticalType = verticalConstraint;
    pConstraintData->verticalValue = verticalVal;

    TextRenderable* pRenderable =(TextRenderable*)getComponent(
        _infoTextEntity,
        ComponentType::PK_RENDERABLE_TEXT
    );
    pRenderable->accessStr() = txt;
    pRenderable->color = color;
}