#include "TopBarPanel.h"


namespace pk
{
    namespace ui
    {
        void TopBarPanel::OnClickClose::onClick(InputMouseButtonName button)
        {
            _pPanel->close();
        }


        TopBarPanel::~TopBarPanel()
        {
            delete _pTopBarImg;
            delete _pTopBarTitle;
            delete _pCloseButton;
        }

        void TopBarPanel::initBase(
            Scene* pScene,
            Font* pFont,
            const std::string title,
            ConstraintProperties constraintProperties,
            const vec2& scale,
            LayoutFillType fillType,
            vec2 slotScale,
            bool scrollable
        )
        {
            createDefault(
                pScene,
                pFont,
                constraintProperties,
                scale,
                slotScale,
                fillType,
                false
            );

            const float topBarHeight = 20.0f;
            // Create top bar (atm just an img)
            // TODO: Maybe in the future make top bar as "button" which u can drag the panel around
            GUIImage::ImgCreationProperties topBarCreationProperties;
            topBarCreationProperties.constraintProperties = constraintProperties;
            topBarCreationProperties.width = scale.x;
            topBarCreationProperties.height = topBarHeight;
            topBarCreationProperties.color = Panel::get_base_ui_color(2).toVec3();
            topBarCreationProperties.filter = GUIFilterType::GUI_FILTER_TYPE_EMBOSS;
            _pTopBarImg = new GUIImage(topBarCreationProperties);

            // Add title text
            _pTopBarTitle = new GUIText(title, *pFont, constraintProperties);

            pScene->addChild(_entity, _pTopBarImg->getEntity());
            pScene->addChild(_entity, _pTopBarTitle->getEntity());

            ConstraintProperties closeButtonConstraintProperties =
            {
                constraintProperties.horizontalType,
                constraintProperties.horizontalValue + scale.x - topBarHeight,
                constraintProperties.verticalType,
                constraintProperties.verticalValue

            };
            // Add close button
            _pCloseButton = new GUIButton(
                "X",
                *pFont,
                closeButtonConstraintProperties,
                topBarHeight - 1,
                topBarHeight - 2,
                new OnClickClose(this),
                false,
                get_base_ui_color(2).toVec3(), // color
                get_base_ui_color(3).toVec3(), // text color
                get_base_ui_color(1).toVec3(), // text highlight color
                get_base_ui_color(3).toVec3(), // background highlight color
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS // filter type
            );

            // Make elements start after the top bar...
             _offsetFromPanel.y += topBarHeight;
            if (scrollable)
                _pScrollbar = new Scrollbar(this, _pDefaultFont);
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
