#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string>
#include <functional>
#include <stdint.h>

#define MESSAGE_NULL "null"

namespace uWS
{
	template <bool SSL>
	struct TemplatedApp;
	typedef TemplatedApp<true> SSLApp;

	template <bool SSL, bool isServer, typename USERDATA>
	struct WebSocket;
}

struct PerSocketData {
	/* Fill with user data */
};

namespace huo_lua
{
	class network final
	{
	public:
		network(const std::string &url,uint16_t port);
		~network();

		void run();
		//init callback
		void set_open_function(std::function<void(uWS::WebSocket<true, true, PerSocketData>*)> fn);
		void set_message_function(std::function<std::string(std::string_view message)> fn);
		void set_close_function(std::function<void()> fn);
		void set_functions( std::function<void(uWS::WebSocket<true, true, PerSocketData>*)> open,
							std::function<std::string(std::string_view message)> message,
							std::function<void()> close);

		network(const network&) = delete;
		network& operator=(const network&) = delete;

	private:
		uWS::SSLApp* m_app;
		std::function<void(uWS::WebSocket<true, true, PerSocketData>*)> m_open_function;
		std::function<std::string(std::string_view message)> m_message_function;
		std::function<void()> m_close_function;
	};
}

#endif
