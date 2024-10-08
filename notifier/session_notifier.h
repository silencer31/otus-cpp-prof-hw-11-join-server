#pragma once

#include "../interfaces/interface_notifier.h"

#include <map>

// ��� ��������� ������ - ���������� ����������.
using awaitors_map = std::map<int, awaitor_shared>;

/**
* @brief ����� ����������� ���������� ������, ��������� ��������� ���������� �������.
*/
class SessionNotifier final : public INotifier
{
public:
	SessionNotifier() = default;
	
	~SessionNotifier() {
		clear();
	}

	void add_awaitor(int session_id, const awaitor_shared& awaitor_ptr) override;

	void rem_awaitor(int session_id) override;

	void clear() override;

	void notify(int session_id) override;

private:
	awaitors_map awaitors; // ��������� ������, ��������� ���������� ������� � ����.
};

using notifier_shared = std::shared_ptr<INotifier>;