#include "command_parser.h"

DatabaseRequest CommandParser::parse_command(const std::string& command, std::string& error) override
{
	// ��������� ���������� ������ ����� \n
	std::vector<std::string> strings;
	boost::split(strings, data_read, boost::is_any_of("\n"));

	for (const std::string& str : strings)
	{
		// ����� �� ��������� ������.
		if (0 == strcmp(str.data(), "exit")) {
			join_server_ptr->close_session(session_id);
			return;
		}

		// ����� �� ��������� ������.
		if (0 == strcmp(str.data(), "shutdown")) {
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			join_server_ptr->shutdown_server(session_id);
			return;
		}

		// ���������� ������ � ��������.
		//async::receive(handle, str.data(), strlen(str.data()));
	}
}