
#include "Debug.h"

#include <iostream>

#define LS_DEBUG_MESSAGE_TAG__MESSAGE		""
#define LS_DEBUG_MESSAGE_TAG__WARNING		"(Warning)"
#define LS_DEBUG_MESSAGE_TAG__ERROR			"[ERROR]"
#define LS_DEBUG_MESSAGE_TAG__FATAL_ERROR	"<!FATAL ERROR!>"


namespace pk
{
	void Debug::log(std::string message, MessageType t)
	{

		switch (t)
		{
		case pk::Debug::PK_MESSAGE:		std::cout << LS_DEBUG_MESSAGE_TAG__MESSAGE << " " << message << std::endl; break;
		case pk::Debug::PK_WARNING:		std::cout << LS_DEBUG_MESSAGE_TAG__WARNING << " " << message << std::endl; break;
		case pk::Debug::PK_ERROR:		std::cout << LS_DEBUG_MESSAGE_TAG__ERROR << " " << message << std::endl; break;
		case pk::Debug::PK_FATAL_ERROR: std::cout << LS_DEBUG_MESSAGE_TAG__FATAL_ERROR << " " << message << std::endl; break;
		default:
			break;
		}
	}

}