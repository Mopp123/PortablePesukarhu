#include "GUIImage.h"
#include "pesukarhu/core/Application.h"
#include "pesukarhu/core/input/InputEvent.h"
#include "pesukarhu/core/Debug.h"


namespace pk
{
    namespace ui
    {
        class ImageMouseCursorPosEvent : public CursorPosEvent
        {
        private:
            entityID_t _entity = NULL_ENTITY_ID;
            vec3 _originalColor = vec3(0, 0, 0);
            vec3 _highlightColor = vec3(1, 1, 1);

        public:
            ImageMouseCursorPosEvent(
                entityID_t entity,
                vec3 highlightColor
            ) :
                _entity(entity),
                _highlightColor(highlightColor)
            {
                const Scene* pScene = Application::get()->getCurrentScene();
                const GUIRenderable* pImg = (const GUIRenderable*)pScene->getComponent(_entity, ComponentType::PK_RENDERABLE_GUI);
                _originalColor = pImg->color;
            }

            virtual void func(int x, int y)
            {
                Scene* pScene = Application::get()->accessCurrentScene();
                GUIRenderable* pImg = (GUIRenderable*)pScene->getComponent(_entity, ComponentType::PK_RENDERABLE_GUI);
                const Transform* pTransform = (const Transform*)pScene->getComponent(_entity, ComponentType::PK_TRANSFORM);
                UIElemState* pUIState = (UIElemState*)pScene->getComponent(_entity, ComponentType::PK_UIELEM_STATE);

                const mat4& tMat = pTransform->getTransformationMatrix();

                float width = tMat[0 + 0 * 4];
                float height = tMat[1 + 1 * 4];

                float xPos = tMat[0 + 3 * 4];
                float yPos = tMat[1 + 3 * 4];

                const int imgLayer = pImg->getLayer();
                // *ignore highlight coloring if selected
                // set or remove highlight coloring
                if (pUIState->isActive() && x >= xPos && x <= xPos + width && y <= yPos && y >= yPos - height)
                {
                    // Make sure theres no overlap with other ui (layer vals check)
                    UIElemState::s_pickedLayers[_entity] = imgLayer;

                    // find highest layer value and make it selected
                    int highest = 0;
                    std::unordered_map<entityID_t, int>::const_iterator it;
                    for (it = UIElemState::s_pickedLayers.begin(); it != UIElemState::s_pickedLayers.end(); ++it)
                    {
                        highest = std::max(highest, it->second);
                    }
                    UIRenderableComponent::set_current_selected_layer(highest);

                    if (UIRenderableComponent::get_current_selected_layer() > pImg->getLayer())
                    {
                        // Reset mouseover to false if "layer switch"
                        if (!pUIState->selected)
                            pImg->color = _originalColor;

                        pUIState->mouseOver = false;
                        return;
                    }
                    UIRenderableComponent::set_current_selected_layer(pImg->getLayer());

                    if (!pUIState->selected)
                        pImg->color = _highlightColor;

                    pUIState->mouseOver = true;
                }
                else
                {
                    if (UIElemState::s_pickedLayers.find(_entity) != UIElemState::s_pickedLayers.end())
                        UIElemState::s_pickedLayers.erase(_entity);

                    if (!pUIState->selected)
                        pImg->color = _originalColor;

                    pUIState->mouseOver = false;
                }
            }
        };


        GUIImage::GUIImage(ImgCreationProperties creationProperties) :
            GUIElement(GUIElementType::PK_GUI_ELEMENT_TYPE_IMAGE)
        {
	        Application* app = Application::get();
	        Scene* currentScene = app->accessCurrentScene();

	        _entity = currentScene->createEntity();
            Transform::create(
                _entity,
                { 0,0 },
                { creationProperties.width, creationProperties.height }
            );
            GUIRenderable::create(
                _entity,
                creationProperties.pTexture,
                creationProperties.color,
                creationProperties.filter,
                creationProperties.textureCropping
            );
            ConstraintData::create(
                _entity,
                {
                    creationProperties.constraintProperties.horizontalType,
                    creationProperties.constraintProperties.horizontalValue,
                    creationProperties.constraintProperties.verticalType,
                    creationProperties.constraintProperties.verticalValue
                }
            );

            UIElemState* pUIElemState = UIElemState::create(_entity);
            pUIElemState->selectable = false;

            vec3 highlightColor = creationProperties.useHighlightColor ? creationProperties.highlightColor : creationProperties.color;
            InputManager* pInputManager = Application::get()->accessInputManager();
            pInputManager->addCursorPosEvent(
                new ImageMouseCursorPosEvent(
                    _entity,
                    highlightColor
                )
            );
        }

        GUIImage::~GUIImage()
        {}

        GUIRenderable* GUIImage::getRenderable()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIImage::getGUIRenderable "
                    "GUIImage's entity was null. Make sure GUIImage was created successfully using GUIImage::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            return (GUIRenderable*)pScene->getComponent(_entity, ComponentType::PK_RENDERABLE_GUI);
        }

        ConstraintData* GUIImage::getConstraint()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIImage::getConstraint "
                    "GUIImage's entity was null. Make sure GUIImage was created successfully using GUIImage::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            return (ConstraintData*)pScene->getComponent(
                _entity,
                ComponentType::PK_UI_CONSTRAINT
            );
        }

        Transform* GUIImage::getTransform()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIImage::getTransform "
                    "GUIImage's entity was null. Make sure GUIImage was created successfully using GUIImage::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            return (Transform*)pScene->getComponent(_entity, ComponentType::PK_TRANSFORM);
        }

        UIElemState* GUIImage::getUIElemState()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIImage::getUIElemState "
                    "GUIImage's entity was null!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            return (UIElemState*)pScene->getComponent(_entity, ComponentType::PK_UIELEM_STATE);
        }

        void GUIImage::setActive(bool arg)
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIImage::setActive "
                    "GUIImage's entity was null. Make sure GUIImage was created successfully using GUIImage::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            std::vector<Component*> components = pScene->getComponents(_entity);
            for (Component* pComponent : components)
                pComponent->setActive(arg);
        }
    }
}
