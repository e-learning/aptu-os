#include "scheduler.h"
#include <iostream>
#include <string>


Scheduler::Scheduler(size_t quantum) : quantum_(quantum), localtime_(0)
{}

void Scheduler::add(Process* process)
{
	process->ownscheduler_ = this;
	processes_.push(process);
}

size_t Scheduler::quantum_size() const
{
	return quantum_;
}

bool Scheduler::isempty() const
{
	return processes_.empty() && available_processes_.empty();
}

void Scheduler::getavailable()
{
		while (! processes_.empty() && processes_.top()->get_ti() <= localtime_) {
			available_processes_.push(processes_.top());
			processes_.pop();
		}
}

void Scheduler::next()
{
	getavailable();
	if (available_processes_.empty()) {
		print_report("IDLE");
		localtime_ = processes_.top()->get_ti();
		return;
	}
	else {
		Process* cur_process = available_processes_.top();
		available_processes_.pop();
		print_report(cur_process->get_id());
		size_t runtime = cur_process->take_quantum();
		localtime_ += runtime;
		if (!cur_process->is_end()) {
			processes_.push(cur_process);
		}
	}
}

void Scheduler::print_report(std::string const & id) const
{
	std::cout << localtime_ << " " << id << std::endl;
}

void Scheduler::print_procs()
{
	while (!processes_.empty()) {
		processes_.top()->print_info();
		processes_.pop();
	}
}
