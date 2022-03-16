#pragma once


namespace pk
{

	class MasterRenderer
	{
	protected:


	public:

		MasterRenderer() {}
		virtual ~MasterRenderer() {}

		virtual void render() = 0;
		virtual void resize(int w, int h) = 0;

	};

}