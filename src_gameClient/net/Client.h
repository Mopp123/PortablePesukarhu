#pragma once

#include <string>
#include "requests/Request.h"

namespace net
{
	class Client
	{
	private:

		// actually its the complete url to server...
		std::string _hostname;

		static Client* s_pInstance;

	public:

		void init(const std::string& host);
		
		Client(const Client& other) = delete;
		~Client();

		void sendRequest(Request* req);

		inline const std::string& getHostname() const { return _hostname; }

		static Client* get_instance();

	private:

		Client();

	};
}