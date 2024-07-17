#include "join_server.h"

#include <iostream>

JoinServer::JoinServer(boost::asio::io_context& io_context, const unsigned short port, const char* file_path)
	: notifier_ptr(std::make_shared<SessionNotifier>())
	, parser_ptr(std::make_shared<CommandParser>())
	, acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
	//std::cout << "Bulk_Constructor" << std::endl;

	do_accept();
}

JoinServer::~JoinServer()
{
	std::cout << "Join_Destructor" << std::endl;
}

void JoinServer::do_accept()
{
	acceptor_.async_accept( // ���� ����� �������� ����������, ��������� ��������� �������.
		[this](boost::system::error_code errcode, tcp::socket socket)
		{
			if (shutdown_flag) {
				std::cout << "\n -- connection rejected. Server is shutting down!" << std::endl;
				return;
			}

			// ������� ���� �� ������ boost.
			if (errcode) {
				std::cout << "\n -- connection rejected. Boost system error: " << errcode.message() << std::endl;
				do_accept(); // ������������� �� �������� ���������� ����������.
				return;
			}

			std::cout << "\n -- connection accepted" << std::endl;

			// ������� ���������� ������ � ��������� ����� ������.
			session_shared session_ptr = std::make_shared<ClientSession>(
				shared_from_this(),
				std::move(socket),
				session_number // ������ ������ ����� ���� �����.
			);

			// ��������� ����� ������ � ��������� ������ �������.
			sessions[session_number] = session_ptr;

			// ��������� ����� ������ � ��������� ������������.
			notifier_ptr->add_awaitor(session_number, session_ptr);

			session_number++;

			// ������ ������ ����� ������ - ������ ��������� ��������� ������.
			session_ptr->start();

			// ������������� �� �������� ���������� ����������.
			do_accept();
		}
	);

	std::cout << "\nwaiting for new connection" << std::endl;
}

// �������� ������� �� ���������� �������.
void JoinServer::shutdown_server(int session_id)
{
	std::cout << "Bulk server: Exit request received from session id: " << session_id << std::endl;

	shutdown_flag = true;

	// ���������� ��������� ������.
	notifier_ptr->rem_awaitor(session_id);
	close_session(session_id);

	// � ����� ��������� ��� ������.
	for (const auto& [id, session_ptr] : sessions) {
		if (id != session_id) { // ���������� ������, �� ������� ������ ������ �� ����������.
			notifier_ptr->rem_awaitor(id);
			session_ptr->shutdown();
		}
	}

	// ��������� ����� ����� ����������.
	acceptor_.cancel();
	//acceptor_.close();
}

// �������� ������.
void JoinServer::close_session(int session_id)
{
	std::cout << "Join server: Session will be closed: " << session_id << std::endl;

	notifier_ptr->rem_awaitor(session_id);

	sessions[session_id]->shutdown();
	sessions.erase(session_id);
	session_number--;
}