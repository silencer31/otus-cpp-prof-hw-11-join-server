#pragma once

#include "../interfaces/interface_notifier.h"

#include <map>

using awaitors_map = std::map<int, awaitor_shared>; // Для коллекции сессий - ожидателей результата.

class SessionNotifier final : public INotifier
{
public:
	//SessionNotifier() = default;
	
	~SessionNotifier() {
		clear();
	}

	void add_awaitor(int session_id, const awaitor_shared& awaitor_ptr) override;

	void rem_awaitor(int session_id) override;

	void clear() override;

private:
	awaitors_map awaitors; // Коллекция ожидающих выполнения запроса к базе.
};