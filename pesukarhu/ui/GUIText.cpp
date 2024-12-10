#include "GUIText.h"
#include "pesukarhu/core/Application.h"


namespace pk
{
    namespace ui
    {
        GUIText::GUIText(
            const std::string& str,
            const Font& font,
            ConstraintProperties constraintProperties,
            vec3 color,
            bool bold
        ) :
            GUIElement(GUIElementType::PK_GUI_ELEMENT_TYPE_TEXT)
        {
            Scene* pScene = Application::get()->accessCurrentScene();

            _entity = pScene->createEntity();
            Transform* pTransform = Transform::create(_entity, { 0,0 }, { 1, 1 });

            TextRenderable::create(
                _entity,
                str,
                ((const Resource&)font).getResourceID(),
                color,
                bold
            );
            ConstraintData::create(
                _entity,
                constraintProperties
            );

            float width = 0.0f;
            for (char c : str)
            {
                const FontGlyphData * const glyph = font.getGlyph(c);
                if (glyph)
                    width += ((float)(glyph->advance >> 6)) + glyph->bearingX;
            }
            // Need to set correct scale to transform for ui constraint system to work properly!
            pTransform->accessTransformationMatrix()[0 + 0 * 4] = width;
            pTransform->accessTransformationMatrix()[1 + 1 * 4] = font.getPixelSize();
        }

        std::string GUIText::getInternalStr() const
        {
            return getStr(true);
        }

        std::string GUIText::getVisualStr() const
        {
            return getStr(false);
        }

        void GUIText::setInternalStr(const std::string& str)
        {
            setStr(str, true);
        }

        void GUIText::setVisualStr(const std::string& str)
        {
            setStr(str, false);
        }

        Transform* GUIText::getTransform()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIText::getTransform "
                    "GUIText's entity was null. Make sure GUIText was created successfully using GUIText::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            return (Transform*)pScene->getComponent(
                _entity,
                ComponentType::PK_TRANSFORM
            );
        }

        ConstraintData* GUIText::getConstraint()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIText::getConstraint "
                    "GUIText's entity was null. Make sure GUIText was created successfully using GUIText::create!",
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

        TextRenderable* GUIText::getRenderable()
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIText::getRenderable "
                    "GUIText's entity was null. Make sure GUIText was created successfully using GUIText::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return nullptr;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            return (TextRenderable*)pScene->getComponent(
                _entity,
                ComponentType::PK_RENDERABLE_TEXT
            );
        }

        void GUIText::setStr(const std::string& str)
        {
            getRenderable()->accessStr() = str;
        }

        void GUIText::setActive(bool arg)
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIText::setActive "
                    "GUIText's entity was null. Make sure GUIText was created successfully using GUIText::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            std::vector<Component*> components = pScene->getComponents(_entity);
            for (Component* pComponent : components)
                pComponent->setActive(arg);
        }

        std::string GUIText::getStr(bool getInternal) const
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIText::getStr "
                    "GUIText's entity was null. Make sure GUIText was created successfully using GUIText::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return "";
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            const TextRenderable* pRenderable = (const TextRenderable*)pScene->getComponent(
                _entity,
                ComponentType::PK_RENDERABLE_TEXT
            );
            if (!pRenderable)
            {
                Debug::log(
                    "@GUIText::getStr "
                    "GUIText's TextRenderable was nullptr!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return "";
            }
            if (getInternal)
                return pRenderable->getStr();
            else
                return pRenderable->getVisualStr();
        }

        void GUIText::setStr(const std::string& str, bool setInternal)
        {
            if (_entity == NULL_ENTITY_ID)
            {
                Debug::log(
                    "@GUIText::setStr "
                    "GUIText's entity was null. Make sure GUIText was created successfully using GUIText::create!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            Scene* pScene = Application::get()->accessCurrentScene();
            TextRenderable* pRenderable = (TextRenderable*)pScene->getComponent(_entity, ComponentType::PK_RENDERABLE_TEXT);
            if (!pRenderable)
            {
                Debug::log(
                    "@GUIText::setStr "
                    "GUIText's TextRenderable was nullptr!",
                    Debug::MessageType::PK_FATAL_ERROR
                );
                return;
            }
            if (setInternal)
                pRenderable->accessStr() = str;
            else
                pRenderable->accessVisualStr() = str;
        }
    }
}
