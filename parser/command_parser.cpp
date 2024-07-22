#include "command_parser.h"

DatabaseRequest CommandParser::parse_command(const std::string& command, std::string& error) override
{
	// Разделяем полученные данные через \n
	std::vector<std::string> strings;
	boost::split(strings, data_read, boost::is_any_of("\n"));

	for (const std::string& str : strings)
	{
		// Нужно ли завершить сессию.
		if (0 == strcmp(str.data(), "exit")) {
			join_server_ptr->close_session(session_id);
			return;
		}

		// Нужно ли выключить сервер.
		if (0 == strcmp(str.data(), "shutdown")) {
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			join_server_ptr->shutdown_server(session_id);
			return;
		}

		// Отправляем данные в контекст.
		//async::receive(handle, str.data(), strlen(str.data()));
	}
}