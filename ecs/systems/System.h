#pragma once


namespace pk
{
	enum SystemType
	{
		PK_SYSTEM_TYPE_STATIC,
		PK_SYSTEM_TYPE_UPDATEABLE
	};

	class System
	{
	protected:
		SystemType _type;

	public:
		System(SystemType type) : _type(type) {}
		System(const System& other) : _type(other._type) {}
		virtual ~System() {}
		inline const SystemType getType() const { return _type; }
	};
	
	class Updateable : public System
	{
	public:
		Updateable() : System(SystemType::PK_SYSTEM_TYPE_UPDATEABLE) {}
		Updateable(const Updateable& other) : System(other._type) {}
		virtual void update() = 0;
	};
}