#ifndef SCHEDULER
#define SCHEDULER

#include <cstdlib>
#include <string>
#include <vector>
#include <utility>
#include <queue>

class comprasion_io
{
public:
	bool operator() (std::pair<size_t, size_t> const & io1, std::pair<size_t, size_t> const & io2);
};

class Scheduler;

class Process
{
	std::string id_;
	size_t ti_;
	size_t tw_;
	std::vector< std::pair<size_t, size_t> > ios_;
	size_t nextio_;

public:
	Scheduler* ownscheduler_;

	Process(std::string const & id, size_t ti, size_t tw);
	void add_io(std::pair<size_t, size_t> const & io);
	std::string get_id() const;
	size_t get_ti() const;
	size_t get_tw() const;
	bool has_quantum_io() const;
	bool is_quantum_end() const;
	bool is_end() const;
	size_t take_quantum();
	void refresh_io(size_t time);

	void print_info();
};

class comprasion_process
{
public:
	bool operator() (Process const * proc1, Process const * proc2);
};

class comprasion_available_process
{
public:
	static size_t quantum;
	bool operator() (Process const * proc1, Process const * proc2);
};


class Scheduler
{
	size_t quantum_;
	size_t localtime_;
	std::priority_queue<Process*, std::vector<Process*>, comprasion_process> processes_;
	std::priority_queue<Process*, std::vector<Process*>, comprasion_available_process> available_processes_;

public:
	Scheduler(size_t quantum);
	void add(Process* process);
	size_t quantum_size() const;
	size_t localtime() const;
	bool isempty() const;
	void getavailable();
	void next();

	void print_report(std::string const & id) const;
	void print_procs();
};

Process* parse_process(std::string const & line);

#endif // SCHEDULER
