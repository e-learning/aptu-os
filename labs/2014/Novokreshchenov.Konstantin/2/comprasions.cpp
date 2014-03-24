#include "scheduler.h"

bool comprasion_io::operator() (std::pair<size_t, size_t> const & io1, std::pair<size_t, size_t> const & io2)
{
	if (io1.first < io2.first) {
		return true;
	}
	else {
		return false;
	}
}

bool comprasion_process::operator() (Process const * proc1, Process const * proc2)
{
	if (proc1->get_ti() > proc2->get_ti()) {
		return true;
	}
	else {
		return false;
	}
}

bool comprasion_available_process::operator() (Process const * proc2, Process const * proc1)
{
	if ((proc1->has_quantum_io() || proc1->is_quantum_end()) &&
		(! proc2->has_quantum_io() && ! proc2->is_quantum_end())) {
		return true;
	}
	return false;
}