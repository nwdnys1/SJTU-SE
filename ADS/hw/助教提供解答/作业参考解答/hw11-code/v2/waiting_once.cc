#include "waiting_once_api.hpp"

void waiting_once::call_once_waiting(init_function f)
{
	if (!this->is_init.test())
	{
		std::lock_guard guard(this->mu);
		if (!this->is_init.test())
		{
			f();
			this->is_init.test_and_set();
		}
	}
}
