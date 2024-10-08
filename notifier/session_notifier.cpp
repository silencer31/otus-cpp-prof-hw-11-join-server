#include "session_notifier.h"

void SessionNotifier::add_awaitor(int session_id, const awaitor_shared& awaitor_ptr)
{
    awaitors[session_id] = awaitor_ptr;
}

void SessionNotifier::rem_awaitor(int session_id)
{
    awaitors.erase(session_id);
}

void SessionNotifier::clear()
{
    for (auto it = awaitors.cbegin(); it != awaitors.cend() /* not hoisted */; /* no increment */)
    {
        it = awaitors.erase(it);
    }
}

void SessionNotifier::notify(int session_id)
{
    if (!awaitors.contains(session_id)) {
        return;
    }

    awaitors[session_id]->handle_request_result();
}
