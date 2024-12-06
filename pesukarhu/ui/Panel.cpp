#include "Panel.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/input/InputManager.h"
#include "GUIImage.h"


namespace pk
{
    namespace ui
    {
        static vec4 s_defaultUIColorsLight0[] = {
            { 201, 197, 172, 255 },
            { 215, 209, 185, 255 },
            { 180, 173, 151, 255 },
            //{ 136, 128, 99, 255 }
            { 107, 102, 85, 255 }
        };
        static vec4 s_defaultUIColorsMonochrome[] = {
            { 187, 187, 187, 255 },
            { 200, 200, 200, 255 },
            { 166, 166, 166, 255 },
            { 96, 96, 96, 255 }
        };
        static vec4 s_defaultUIColorsUIDark[] = {
            { 28, 28, 28, 255 },
            { 58, 58, 58, 255 },
            { 48, 48, 48, 255 },
            { 188, 188, 188, 255 }
        };

        std::vector<vec4> Panel::s_uiColor = {
            s_defaultUIColorsUIDark[0],
            s_defaultUIColorsUIDark[1],
            s_defaultUIColorsUIDark[2],
            s_defaultUIColorsUIDark[3]
        };

        int Panel::s_pickedPanels = 0;

        void Panel::PanelCursorPosEvent::func(int x, int y)
        {
            // check activeness by looking at "root entity's" transform's activeness
            // -> fucking stupid way of doing this but it'll do for now..
            Transform* pTransform = (Transform*)_pScene->getComponent(
                _pPanel->getEntity(),
                ComponentType::PK_TRANSFORM
            );
            if (!pTransform)
            {
                Debug::log(
                    "@Panel::PanelCursorPosEvent::func "
                    "Panel's root entity doesn't have valid Transform component!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (!pTransform->isActive())
            {
                _pPanel->_isMouseOver = false;
                return;
            }

            float fx = (float)x;
            float fy = (float)y;

            float panelX = 0.0f;
            float panelY = 0.0f;
            float panelWidth = 0.0f;
            float panelHeight = 0.0f;
            _pPanel->getRect(panelX, panelY, panelWidth, panelHeight);

            if (fx >= panelX && fx <= panelX + panelWidth && fy <= panelY && fy >= panelY - panelHeight)
            {
                _pPanel->_isMouseOver = true;
                ++s_pickedPanels;
            }
            else
            {
                _pPanel->_isMouseOver = false;
                if (s_pickedPanels - 1 >= 0)
                    --s_pickedPanels;
            }
        }


        void Panel::create(
            Scene* pScene,
            Font* pDefaultFont,
            ConstraintProperties constraintProperties,
            vec2 scale,
            LayoutFillType layoutType,
            vec3 color,
            GUIFilterType filter,
            float slotPadding,
            vec2 slotScale
        )
        {
            _pScene = pScene;
            _pDefaultFont = pDefaultFont;

            _constraintProperties = constraintProperties;

            _scale = scale;
            _layoutType = layoutType;

            _color = color;

            _slotPadding = slotPadding;
            _slotScale = slotScale;

            vec4 textureCropping(0, 0, 1, 1); // unused since no even texure atm?

            // Create panel image
            GUIImage::ImgCreationProperties imgCreationProperties;
            imgCreationProperties.constraintProperties = _constraintProperties;
            imgCreationProperties.width = _scale.x;
            imgCreationProperties.height = _scale.y;
            imgCreationProperties.color = _color;
            imgCreationProperties.filter = filter;
            imgCreationProperties.textureCropping = textureCropping;

            // Not sure should we actually keep the handle for the GUIImage... could be useful...
            GUIImage panelImg;
            panelImg.create(imgCreationProperties);

            _entity = panelImg.getEntity();

            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addCursorPosEvent(new PanelCursorPosEvent(pScene, this));
        }

        void Panel::createDefault(
            Scene* pScene,
            Font* pDefaultFont,
            ConstraintProperties constraintProperties,
            vec2 scale,
            vec2 slotScale,
            LayoutFillType fillType,
            int useColorIndex
        )
        {
            create(
                pScene,
                pDefaultFont,
                constraintProperties,
                scale,
                fillType,
                get_base_ui_color(useColorIndex).toVec3(),
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS, // filter type
                4.0f, // slot padding;
                slotScale
            );
        }

        entityID_t Panel::addText(
            std::string txt,
            ConstraintProperties constraintProperties
        )
        {
            entityID_t text = create_text(
                txt, *_pDefaultFont,
                constraintProperties,
                get_base_ui_color(3).toVec3(),
                false // bold
            );
            _pScene->addChild(_entity, text);
            // NOTE: Not sure should we actually increase slots here since explicit pos(constraint)
            ++_slotCount;

            return text;
        }

        entityID_t Panel::addText(std::string txt, vec3 color)
        {
            vec2 toAdd = calcNewSlotPos();

            ConstraintProperties useConstraintProperties = {
                _constraintProperties.horizontalType,
                _constraintProperties.horizontalValue + toAdd.x,
                _constraintProperties.verticalType,
                _constraintProperties.verticalValue + toAdd.y,
            };

            entityID_t text = create_text(
                txt, *_pDefaultFont,
                useConstraintProperties,
                color,
                false // bold
            );
            _pScene->addChild(_entity, text);
            ++_slotCount;

            return text;
        }

        entityID_t Panel::addDefaultText(std::string txt)
        {
            return addText(txt, get_base_ui_color(3).toVec3());
        }

        UIFactoryButton Panel::addDefaultButton(
            std::string txt,
            OnClickEvent* onClick,
            float width
        )
        {
            vec4 color = get_base_ui_color(1);
            Texture* pTexture = nullptr;
            vec4 textureCropping(0, 0, 1, 1);

            vec2 toAdd = calcNewSlotPos();

            ConstraintProperties useConstraintProperties =
            {
                _constraintProperties.horizontalType,
                _constraintProperties.horizontalValue + toAdd.x,
                _constraintProperties.verticalType,
                _constraintProperties.verticalValue + toAdd.y,
            };

            UIFactoryButton button = create_button(
                txt,
                *_pDefaultFont,
                useConstraintProperties,
                width, _slotScale.y, // scale
                onClick,
                false,
                color.toVec3(), // color
                get_base_ui_color(3).toVec3(), // text color
                get_base_ui_color(1).toVec3(), // text highlight color
                get_base_ui_color(3).toVec3(), // background highlight color
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS, // filter type
                pTexture,
                textureCropping
            );
            // atm fucks up because constraint and transform systems are in conflict?
            _pScene->addChild(_entity, button.rootEntity);
            ++_slotCount;
            return button;
        }

        UIFactoryButton Panel::addButton(
            std::string txt,
            OnClickEvent* onClick,
            ConstraintProperties constraintProperties,
            vec2 scale
        )
        {
            vec4 color = get_base_ui_color(2);
            Texture* pTexture = nullptr;
            vec4 textureCropping(0, 0, 1, 1);

            UIFactoryButton button = create_button(
                txt,
                *_pDefaultFont,
                constraintProperties,
                scale.x, scale.y,
                onClick,
                false,
                color.toVec3(), // color
                get_base_ui_color(3).toVec3(), // text color
                get_base_ui_color(1).toVec3(), // text highlight color
                get_base_ui_color(3).toVec3(), // background highlight color
                GUIFilterType::GUI_FILTER_TYPE_EMBOSS, // filter type
                pTexture,
                textureCropping
            );
            // atm fucks up because constraint and transform systems are in conflict?
            _pScene->addChild(_entity, button.rootEntity);
            // Atm disabling adding to slot count since this overrides the "slot" thing completely...
            //++_slotCount;
            return button;
        }

        UIFactoryInputField Panel::addDefaultInputField(
            std::string infoTxt,
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit,
            bool password
        )
        {
            vec4 color = get_base_ui_color(2);
            vec2 toAdd = calcNewSlotPos();

            ConstraintProperties useConstraintProperties = _constraintProperties;
            useConstraintProperties.horizontalValue += toAdd.x;
            useConstraintProperties.verticalValue += toAdd.y;

            UIFactoryInputField inputField = create_input_field(
                infoTxt, *_pDefaultFont,
                useConstraintProperties,
                width,
                nullptr, // on submit event
                false, // clear on submit
                color.toVec3(), // color
                get_base_ui_color(3).toVec3(), // text color
                get_base_ui_color(3).toVec3(), // text highlight color
                get_base_ui_color(1).toVec3(), // background highlight color,
                password
            );
            _pScene->addChild(_entity, inputField.rootEntity);
            ++_slotCount;

            return inputField;
        }

        UIFactoryInputField Panel::addInputField(
            std::string infoTxt,
            ConstraintProperties constraintProperties,
            int width,
            InputFieldOnSubmitEvent* onSubmitEvent,
            bool clearOnSubmit,
            bool password
        )
        {
            vec4 color = get_base_ui_color(2);
            UIFactoryInputField inputField = create_input_field(
                infoTxt, *_pDefaultFont,
                constraintProperties,
                width,
                nullptr, // on submit event
                false, // clear on submit
                color.toVec3(), // color
                get_base_ui_color(3).toVec3(), // text color
                get_base_ui_color(3).toVec3(), // text highlight color
                get_base_ui_color(1).toVec3(), // background highlight color
                password
            );
            _pScene->addChild(_entity, inputField.rootEntity);

            return inputField;
        }

        entityID_t Panel::addImage(
            ConstraintProperties constraintProperties,
            float width, float height,
            Texture* pTexture,
            vec3 color,
            vec4 textureCropping,
            GUIFilterType filter
        )
        {
            GUIImage::ImgCreationProperties imgProperties;
            imgProperties.constraintProperties = constraintProperties;
            imgProperties.width = width;
            imgProperties.height = height;
            imgProperties.color = color;
            imgProperties.filter = filter;
            imgProperties.pTexture = pTexture;
            imgProperties.textureCropping = textureCropping;

            GUIImage img;
            img.create(imgProperties);
            entityID_t imgEntity = img.getEntity();
            // NOTE: Earlier this img wasn't added as child... don't remember was there
            // a reason for it...
            _pScene->addChild(_entity, imgEntity);
            // NOTE: Not sure should _slotCount increase when adding img...
            return imgEntity;
        }

        UIFactoryCheckbox Panel::addDefaultCheckbox(std::string infoTxt)
        {
            vec4 color = get_base_ui_color(2);
            vec2 toAdd = calcNewSlotPos();

            ConstraintProperties useConstraintProperties = _constraintProperties;
            useConstraintProperties.horizontalValue += toAdd.x;
            useConstraintProperties.verticalValue += toAdd.y;

            UIFactoryCheckbox checkbox = create_checkbox(
                infoTxt,
                _pDefaultFont,
                useConstraintProperties,
                color.toVec3(), // background color
                get_base_ui_color(1).toVec3(), // background highlight color,
                get_base_ui_color(3).toVec3(), // checked indicator color
                get_base_ui_color(3).toVec3() // text color
            );
            _pScene->addChild(_entity, checkbox.rootEntity);
            ++_slotCount;
            return checkbox;
        }

        void Panel::setActive(bool arg, entityID_t entity)
        {
            if (entity == 0)
                entity = _entity;

            std::vector<Component*> components = _pScene->getComponents(entity);
            for (Component* pComponent : components)
                pComponent->setActive(arg);
            std::vector<entityID_t> children = _pScene->getChildren(entity);
            for (entityID_t child: children)
                setActive(arg, child);
        }

        void Panel::setLayer(int layer)
        {
            for (Component* pComponent : _pScene->getAllComponents(_entity))
            {
                ComponentType type = pComponent->getType();
                if (type == ComponentType::PK_RENDERABLE_GUI)
                {
                    GUIRenderable* pRenderable = (GUIRenderable*)pComponent;
                    pRenderable->setLayer(layer);
                }
                else if (type == ComponentType::PK_RENDERABLE_TEXT)
                {
                    TextRenderable* pRenderable = (TextRenderable*)pComponent;
                    pRenderable->setLayer(layer);
                }
            }
        }

        vec2 Panel::calcNewSlotPos()
        {
            vec2 useOffset = _offsetFromPanel;

            vec2 pos(0, 0);

            if (_layoutType == LayoutFillType::VERTICAL)
                pos = { 0.0f,  (_slotScale.y + _slotPadding) * _slotCount};
            else if (_layoutType == LayoutFillType::HORIZONTAL)
                pos = { (_slotScale.x + _slotPadding) * _slotCount, 0.0f };

            if (_constraintProperties.verticalType == VerticalConstraintType::PIXEL_CENTER_VERTICAL)
            {
                pos.y *= -1.0f;
                useOffset.y *= -1.0f;
            }

            return useOffset + pos;
        }

        void Panel::getRect(float& outX, float& outY, float& outWidth, float& outHeight) const
        {
            const Transform* pTransform = (const Transform*)_pScene->getComponent(
                _entity,
                ComponentType::PK_TRANSFORM
            );
            const mat4& tMat = pTransform->getTransformationMatrix();
            outX = tMat[0 + 3 * 4];
            outY = tMat[1 + 3 * 4];
            outWidth = tMat[0 + 0 * 4];
            outHeight = tMat[1 + 1 * 4];
        }

        vec4 Panel::get_base_ui_color(unsigned int colorIndex)
        {
            if (colorIndex >= UI_BASE_COLOR_COUNT)
            {
                Debug::log(
                    "@Panel::get_base_ui_color "
                    "Invalid color index: " + std::to_string(colorIndex) + " "
                    "Last available color index is " + std::to_string(UI_BASE_COLOR_COUNT - 1),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return { 0, 0, 0, 1 };
            }
            vec4 color = s_uiColor[colorIndex];
            color.x /= 255;
            color.y /= 255;
            color.z /= 255;
            color.w /= 255;
            return color;
        }

        // Values needs to be in range 0 - 255
        void Panel::set_base_ui_color(unsigned int colorIndex, int r, int g, int b, int a)
        {
            if (colorIndex >= UI_BASE_COLOR_COUNT)
            {
                Debug::log(
                    "@Panel::set_base_ui_color "
                    "Invalid color index: " + std::to_string(colorIndex) + " "
                    "Last available color index is " + std::to_string(UI_BASE_COLOR_COUNT - 1),
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            s_uiColor[colorIndex] = { (float)r, (float)g, (float)b, (float)a };
        }

        bool Panel::is_mouse_over_ui()
        {
            return s_pickedPanels > 0;
        }
    }
}
