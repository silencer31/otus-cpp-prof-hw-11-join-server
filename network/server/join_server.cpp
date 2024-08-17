#include "join_server.h"

#include <iostream>

JoinServer::JoinServer(boost::asio::io_context& io_context, const unsigned short port, const std::string& db_name)
	: parser_ptr( std::make_shared<CommandParser>() )
	, notifier_ptr(std::make_shared<SessionNotifier>())
	, req_coll_ptr(std::make_shared<RequestCollector>())
	, res_coll_ptr(std::make_shared<ResultCollector>())
	, db_manager_ptr(std::make_shared<DatabaseManager>(notifier_ptr, req_coll_ptr, res_coll_ptr, db_name))
	, acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
	// ������������� ����� � ����� ������ � ������ �, ���� ����� � ����� ��� ���.
	if ( !db_manager_ptr->connect_database()) {
		return;
	}

	// �������� ���� �������� ����������.
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
				parser_ptr,
				req_coll_ptr,
				res_coll_ptr,
				db_manager_ptr,
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

	db_manager_ptr->stop_process();

	// ��� ����������� ���������� ���� �������.
	while (db_manager_ptr->active()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
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