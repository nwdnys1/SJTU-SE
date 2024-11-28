#include "waiting_once_api.hpp"

void waiting_once::call_once_waiting(init_function f)
{
	std::lock_guard guard(this->mu);
	if (!this->is_init)
	{
		f();
		this->is_init = true;
	}
}
