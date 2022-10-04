#include "network.h"
#include "App.h"
#include "WebSocket.h"

namespace huo_lua
{
	network::network(const std::string& url, uint16_t port)
	{
		m_app = new uWS::SSLApp({
			.key_file_name = "misc/key.pem",
			.cert_file_name = "misc/cert.pem",
			.passphrase = "1234"
		});
		//init callback
		m_app->ws<PerSocketData>(url,
		{
			.compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB | uWS::DEDICATED_DECOMPRESSOR),
			.maxPayloadLength = 100 * 1024 * 1024,
			.idleTimeout = 16,
			.maxBackpressure = 100 * 1024 * 1024,
			.closeOnBackpressureLimit = false,
			.resetIdleTimeoutOnSend = false,
			.sendPingsAutomatically = true,
			/* Handlers */
			.upgrade = nullptr,
			.open = [this](auto* ws) {
				if(m_open_function) m_open_function(ws);
			},
			.message = [this](auto* ws, std::string_view message, uWS::OpCode opCode) {
				std::cout << message << std::endl;
				std::string ret_message = MESSAGE_NULL;
				if(m_message_function) ret_message = m_message_function(message);
				ws->send(ret_message, uWS::OpCode::TEXT, true);
			},
			.drain = [](auto*/*ws*/) {
			},
			.ping = [](auto*/*ws*/, std::string_view) {
			},
			.pong = [](auto*/*ws*/, std::string_view) {
			},
			.close = [this](auto* ws, int /*code*/, std::string_view /*message*/) {
				if(m_close_function) m_close_function();
			}
		});
		//listen
		m_app->listen(port, [](auto* listen_socket) {
			if (listen_socket) {
				std::cout << "Listening on port " << 9001 << std::endl;
			}
		});
	}

	network::~network()
	{
		if (m_app) delete m_app;
	}

	void network::run()
	{
		m_app->run();
	}

	void network::set_open_function(std::function<void(uWS::WebSocket<true, true, PerSocketData>*)> fn)
	{
		m_open_function = fn;
	}

	void network::set_message_function(std::function<std::string(std::string_view message)> fn)
	{
		m_message_function = fn;
	}

	void network::set_close_function(std::function<void()> fn)
	{
		m_close_function = fn;
	}

	void network::set_functions(std::function<void(uWS::WebSocket<true, true, PerSocketData>*)> open, std::function<std::string(std::string_view message)> message, std::function<void()> close)
	{
		m_open_function = open;
		m_message_function = message;
		m_close_function = close;
	}
}