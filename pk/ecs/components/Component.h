#pragma once

#include <cstdint>


namespace pk
{

	enum ComponentType
	{
		PK_EMPTY,
		PK_RENDERABLE_GUI,
		PK_RENDERABLE_TEXT,
		PK_RENDERABLE_SPRITE3D
	};

	class Component
	{
	protected:

		uint32_t _id = 0;
		ComponentType _type;

		bool _isActive = true;

	public:

		Component(ComponentType type) : _type(type) { }
		virtual ~Component() { }
	
		inline uint32_t getID() const { return _id; }
		inline ComponentType getType() const { return _type; }
		inline bool isActive() const { return _isActive; }
	
	};
}