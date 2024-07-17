#include "session_notifier.h"

void SessionNotifier::add_awaitor(int session_id, const awaitor_shared& awaitor_ptr) override
{
    awaitors[session_id] = awaitor_ptr;
}

void SessionNotifier::rem_awaitor(int session_id) override
{
    awaitors.erase(session_id);
}

void SessionNotifier::clear() override
{
    for (auto it = awaitors.cbegin(); it != awaitors.cend() /* not hoisted */; /* no increment */)
    {
        it = awaitors.erase(it);
    }
}
