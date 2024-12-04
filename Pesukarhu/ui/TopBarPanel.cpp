#include "TopBarPanel.h"


namespace pk
{
    namespace ui
    {
        void TopBarPanel::OnClickClose::onClick(InputMouseButtonName button)
        {
            _pPanel->close();
        }


        void TopBarPanel::initBase(
            Scene* pScene,
            Font* pFont,
            const std::string title,
            ConstraintProperties constraintProperties,
            const vec2& scale,
            LayoutFillType fillType
        )
        {
            vec2 slotScale(200, 24);
            createDefault(
                pScene,
                pFont,
                constraintProperties,
                scale,
                slotScale,
                fillType
            );

            const float topBarHeight = 20.0f;
            // Create top bar (atm just an img)
            // TODO: Maybe in the future make top bar as "button" which u can drag the panel around
            _topBarImgEntity = addImage(
                constraintProperties,
                scale.x, topBarHeight,
                nullptr, // texture
                Panel::get_base_ui_color(2).toVec3(),
                { 0, 0, 1, 1 }, // texture cropping
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS
            );
            // Add title text
            _topBarTitleEntity = addText(
                title,
                constraintProperties
            ).first;

            pScene->addChild(_entity, _topBarImgEntity);
            pScene->addChild(_entity, _topBarTitleEntity);

            ConstraintProperties closeButtonConstraintProperties =
            {
                constraintProperties.horizontalType,
                constraintProperties.horizontalValue + scale.x - topBarHeight,
                constraintProperties.verticalType,
                constraintProperties.verticalValue

            };
            // Add close button
            _topBarCloseButton = addButton(
                "X",
                new OnClickClose(this),
                closeButtonConstraintProperties,
                { topBarHeight - 1, topBarHeight - 2 } // scale
            );
        }

        void TopBarPanel::setComponentsActive(bool arg)
        {
            for(Component* pComponent : _pScene->getAllComponents(_entity))
                pComponent->setActive(arg);

            /*
            for(Component* pComponent : _pScene->getComponents(_topBarImgEntity))
                pComponent->setActive(arg);
            for(Component* pComponent : _pScene->getComponents(_topBarTitleEntity))
                pComponent->setActive(arg);

            entityID_t closeButtonEntity = _topBarCloseButton.rootEntity;
            entityID_t closeButtonImgEntity = _topBarCloseButton.imgEntity;
            entityID_t closeButtonTxtEntity = _topBarCloseButton.txtEntity;
            for(Component* pComponent : _pScene->getComponents(closeButtonEntity))
                pComponent->setActive(arg);
            for(Component* pComponent : _pScene->getComponents(closeButtonImgEntity))
                pComponent->setActive(arg);
            for(Component* pComponent : _pScene->getComponents(closeButtonTxtEntity))
                pComponent->setActive(arg);
                */
        }
    }
}
