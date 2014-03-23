#include "scheduler.h"
#include <sstream>
#include <string>
#include <iostream>

using std::string;

Process* parse_process(std::string const & line)
{
	std::stringstream ss(line);
	string id;
	size_t ti;
	size_t tw;

	std::getline(ss, id, ' ');
	ss >> ti;
	ss >> tw;
	Process* process = new Process(id, ti, tw);

	size_t iot;
	size_t iol;
	while ((ss >> iot) && (ss >> iol)) {
		process->add_io(std::make_pair(iot, iol));
	}

	return process;
}


Process::Process(std::string const & id, size_t ti, size_t tw) : id_(id), ti_(ti), tw_(tw), nextio_(0)
{}

void Process::add_io(std::pair<size_t, size_t> const & io)
{
	ios_.push_back(io);
}

std::string Process::get_id() const
{
	return id_;
}

size_t Process::get_ti() const
{
	return ti_;
}

size_t Process::get_tw() const
{
	return tw_;
}

bool Process::has_quantum_io () const
{
	if (nextio_ >= ios_.size()) {
		//no io
		return false;
	}
	if (ios_[nextio_].first <= ownscheduler_->quantum_size()) {
		return true;
	}
	return false;
}

bool Process::is_quantum_end() const
{
	if (tw_ <= ownscheduler_->quantum_size()) {
		return true;
	}
	return false;
}

bool Process::is_end() const
{
	return tw_ <= 0;
}

size_t Process::take_quantum()
{
	size_t runtime = 0;
	if (has_quantum_io()) {
		runtime = ios_[nextio_].first;
		ti_ = ownscheduler_->localtime() + runtime + ios_[nextio_].second;
        tw_ -= ios_[nextio_].second;
		++nextio_;
	}
	else {
		is_quantum_end() ? runtime = tw_ : runtime = ownscheduler_->quantum_size();
		ti_ = ownscheduler_->localtime() + runtime;
	}
	tw_ -= runtime;
	refresh_io(runtime);
	return runtime;
}

void Process::refresh_io(size_t time)
{
	for (size_t i = nextio_; i != ios_.size(); ++i) {
		ios_[i].first -= time;
	}
}

void Process::print_info()
{
	std::cout << "ID: " << id_ << std::endl
		<< "ti: " << ti_ << std::endl
		<< "tw: " << tw_ << std::endl;

	std::cout << std::endl;
}
