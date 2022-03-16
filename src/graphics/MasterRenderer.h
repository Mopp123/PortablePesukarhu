#pragma once


namespace pk
{

	class MasterRenderer
	{
	protected:


	public:

		MasterRenderer();
		virtual ~MasterRenderer();

		virtual void render() = 0;
	};

}