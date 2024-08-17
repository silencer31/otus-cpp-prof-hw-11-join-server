#pragma once

#include "../interfaces/interface_notifier.h"

#include <map>

// Для коллекции сессий - ожидателей результата.
using awaitors_map = std::map<int, awaitor_shared>;

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
	awaitors_map awaitors; // Коллекция сессий, ожидающих выполнения запроса к базе.
};

using notifier_shared = std::shared_ptr<INotifier>;