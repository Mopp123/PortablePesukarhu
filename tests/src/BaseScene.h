#pragma once

#include "pesukarhu/ppk.h"


class BaseScene : public pk::Scene
{
protected:
    pk::Font* _pDefaultFont = nullptr;
    pk::ui::GUIText* _pInfoText = nullptr;

public:
    bool loggingOut = false;

    BaseScene();
    virtual ~BaseScene();
    void initBase();

    void setInfoText(
        const std::string& txt,
        pk::vec3 color = pk::vec3(1.0f, 1.0f, 1.0f),
        float horizontalVal = 0,
        float verticalVal = 0,
        pk::HorizontalConstraintType horizontalConstraint = pk::HorizontalConstraintType::PIXEL_LEFT,
        pk::VerticalConstraintType verticalConstraint = pk::VerticalConstraintType::PIXEL_TOP
    );
};
