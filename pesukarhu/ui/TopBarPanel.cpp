#include "TopBarPanel.h"


namespace pk
{
    namespace ui
    {
        void TopBarPanel::OnClickClose::onClick(InputMouseButtonName button)
        {
            _pPanel->close();
            _pPanel->_pTopBarImg->setActive(false);
            _pPanel->_pTopBarTitle->setActive(false);
            if (_pPanel->_pCloseButton)
                _pPanel->_pCloseButton->setActive(false);
            for (GUIElement* pElem : _pPanel->_elements)
                pElem->setActive(false);

            _pPanel->_pBackgroundImg->setActive(false);
        }


        TopBarPanel::~TopBarPanel()
        {
            delete _pTopBarImg;
            delete _pTopBarTitle;
            if (_pCloseButton)
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
            bool scrollable,
            bool closeable
        )
        {
            createDefault(
                pScene,
                pFont,
                constraintProperties,
                scale,
                slotScale,
                fillType,
                false, // We create the scrollbar here differently instead of inside the inherited Panel
                _topBarHeight
            );

            // Create top bar
            GUIImage::ImgCreationProperties topBarCreationProperties;
            topBarCreationProperties.constraintProperties = constraintProperties;
            topBarCreationProperties.width = scale.x;
            topBarCreationProperties.height = _topBarHeight;
            topBarCreationProperties.color = Panel::get_base_ui_color(2).toVec3();
            topBarCreationProperties.filter = GUIFilterType::GUI_FILTER_TYPE_EMBOSS;
            _pTopBarImg = new GUIImage(topBarCreationProperties);

            // Add title text
            ConstraintProperties titleConstraintProperties = constraintProperties;
            _pTopBarTitle = new GUIText(title, *pFont, titleConstraintProperties);

            pScene->addChild(_entity, _pTopBarImg->getEntity());
            pScene->addChild(_entity, _pTopBarTitle->getEntity());

            ConstraintProperties closeButtonConstraintProperties =
            {
                constraintProperties.horizontalType,
                constraintProperties.horizontalValue + scale.x - _topBarHeight,
                constraintProperties.verticalType,
                constraintProperties.verticalValue
            };
            // Add close button
            if (closeable)
            {
                _pCloseButton = new GUIButton(
                    "X",
                    *pFont,
                    closeButtonConstraintProperties,
                    _topBarHeight - 1,
                    _topBarHeight - 2,
                    new OnClickClose(this),
                    false,
                    get_base_ui_color(2).toVec3(), // color
                    get_base_ui_color(3).toVec3(), // text color
                    get_base_ui_color(1).toVec3(), // text highlight color
                    get_base_ui_color(3).toVec3(), // background highlight color
                    GUIFilterType::GUI_FILTER_TYPE_EMBOSS // filter type
                );
            }

            // Make elements start after the top bar...
            _offsetFromPanel.y -= _topBarHeight;

            if (scrollable)
                _pScrollbar = new Scrollbar(this, _pDefaultFont, _topBarHeight);
        }

        void TopBarPanel::setLayer(int layer)
        {
            Panel::setLayer(layer);
            _pTopBarImg->getRenderable()->setLayer(layer);
            _pTopBarTitle->getRenderable()->setLayer(layer);
            if (_pCloseButton)
            {
                _pCloseButton->getImage()->getRenderable()->setLayer(layer);
                _pCloseButton->getText()->getRenderable()->setLayer(layer);
            }
        }

        void TopBarPanel::open_NEW()
        {
            _pTopBarImg->setActive(true);
            _pTopBarTitle->setActive(true);
            if (_pCloseButton)
                _pCloseButton->setActive(true);
            for (GUIElement* pElem : _elements)
                pElem->setActive(true);

            _pBackgroundImg->setActive(true);
        }

        void TopBarPanel::close_NEW()
        {
            onClose();
            _pTopBarImg->setActive(false);
            _pTopBarTitle->setActive(false);
            if (_pCloseButton)
                _pCloseButton->setActive(false);
            for (GUIElement* pElem : _elements)
                pElem->setActive(false);

            _pBackgroundImg->setActive(false);
        }

        void TopBarPanel::setComponentsActive(bool arg)
        {
            for(Component* pComponent : _pScene->getAllComponents(_entity))
                pComponent->setActive(arg);

            _pTopBarImg->setActive(arg);
            _pTopBarTitle->setActive(arg);

            if (_pCloseButton)
                _pCloseButton->setActive(arg);
        }
    }
}
