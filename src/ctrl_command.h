#ifndef __CTRL_COMMAND_H__
#define __CTRL_COMMAND_H__

#include <unordered_map>
#include <string>
#include <functional>

namespace huo_lua
{
	struct lua_debugger_context;

	using ctrl_fun = std::function<int(lua_debugger_context*)>;
	class ctrl_command
	{
	public:
		ctrl_command();
		~ctrl_command();

	private:
		std::unordered_map<std::string, ctrl_fun> m_ctrls;
	};
}

#endif