#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::list;
using std::atoi;
using std::sort;
using std::less;
using std::priority_queue;
using std::vector;

class priority_comparer;
class process;
typedef priority_queue<process, vector<process>, priority_comparer> pqueue;
typedef priority_queue<process, vector<process>, bool (*) (process const&, process const&)> admission_queue;

struct io_operation
{
	io_operation(size_t st, size_t len) : 
		start_time(st),
		length(len) {}

	size_t start_time;
	size_t length;
};

class process
{
	public:
	process(string const& id, 
			size_t at,
			size_t wt, 
			list<io_operation> ios, 
			bool fullfilled_quantum = false) : 

		m_identifier(id), 
		m_admission_time(at),
		m_work_time(wt), 
		m_operations(ios),
		m_fullfilled_quantum(fullfilled_quantum),
		m_worked(0) {}
		
		string const& identifier() const {return m_identifier;}
		
		bool has_io() const {return m_operations.size() > 0;}

		io_operation& topmost_io() {return m_operations.front();}
		
		io_operation const& topmost_io() const {return m_operations.front();}

		void io(size_t now) 
		{
			size_t time = topmost_io().length;
			m_operations.pop_front();
			work(now, time);
		}
	
		void work(size_t now, size_t time) 
		{
			m_work_time -= time;
			m_worked += time;
			m_admission_time = now + time;
		}

		size_t work_time() {return m_work_time;}

		bool finishes_in(size_t time) const
		{
			return m_work_time <= time;
		}

		bool io_in_time(const size_t time) const
		{
			return has_io() && topmost_io().start_time <= (m_worked + time);
		}

		size_t time_to_io()
		{
			return topmost_io().start_time - m_worked;
		}

		void set_fullfilled(bool fullfilled_quantum) {m_fullfilled_quantum = fullfilled_quantum;}

		bool fullfilled_quantum() const {return m_fullfilled_quantum;}

		size_t admission_time() const {return m_admission_time;}			
		
		void out() {cout << m_identifier << " at: " << m_admission_time << " wt: " << m_work_time << " io: " << has_io() << endl; }

	private:
		string m_identifier;
		size_t m_admission_time;
		size_t m_work_time;
		list<io_operation> m_operations;
		bool m_fullfilled_quantum;
		size_t m_worked;
};

bool is_in(const char c, string const& source)
{
	return source.find(c) != string::npos;
}

size_t as_number(string const& token)
{
	return atoi(token.c_str());
}

class tokenizer
{
	public:
		tokenizer(string const& source, size_t offset = 0, string const& delimiters = " \n") : 
			m_source(source), 
			m_offset(offset),
			m_delimiters(delimiters) {}
		
		string next_token()
		{
			return next_token(m_delimiters);
		}

		string next_token(string const& delimiters)
		{
			if (has_finished())
				return "";
			
			int start = m_offset;
			int length = 0;
			while(start + length < m_source.length() && is_in(m_source[start], delimiters))
			{
				++start;
				++m_offset;
			}

			while(start + length < m_source.length() && !is_in(m_source[start + length], delimiters))
			{
				++length;
				++m_offset;				
			}

			return m_source.substr(start, length);
		}

	bool has_finished() {return m_offset == m_source.length();}

	private:
			string const& m_source;
			size_t m_offset;
			string const& m_delimiters;
};


process build_process(string const& line)
{
	tokenizer t(line);
	string identifier = t.next_token();
	
	size_t admission_time = as_number(t.next_token());
	size_t work_time = as_number(t.next_token());
	list<io_operation> io_operations;

	while (!t.has_finished())
	{
		size_t start_time = as_number(t.next_token());
		size_t length = as_number(t.next_token());
		io_operations.push_back(io_operation(start_time, length));
	}

	return process(identifier, admission_time, work_time, io_operations);
}

class priority_comparer
{
	public:
		priority_comparer(size_t quantum) : m_quantum(quantum) {}

		bool operator()(process const& right, process const& left)
		{
			if (left.finishes_in(m_quantum) == right.finishes_in(m_quantum))
				if (left.io_in_time(m_quantum) == right.io_in_time(m_quantum))
					if (left.fullfilled_quantum() == right.fullfilled_quantum())
						return less<string>()(left.identifier(), right.identifier());
					else
						return left.fullfilled_quantum();
				else
					return left.io_in_time(m_quantum);
			else
				return left.finishes_in(m_quantum);
		}

	private:
		size_t m_quantum;
};

bool admission_time_comparer(process const& right, process const& left)
{
	if (left.admission_time() == right.admission_time())
		return less<string>()(left.identifier(), right.identifier());
	return left.admission_time() < right.admission_time();
}

void idle(size_t* now, size_t time)
{
	cout << *now << " IDLE" << endl;
	*now += time;
}

void work(size_t* now, size_t time, process& proc, bool full_quantum = false)
{
	cout << *now << " " << proc.identifier() << endl;
	proc.work(*now, time);	
	*now += time;
	proc.set_fullfilled(full_quantum);
}

void move_to_available(admission_queue& processes, pqueue& available, size_t now)
{
	while (!processes.empty() && processes.top().admission_time() <= now)
	{
		process proc = processes.top();
		processes.pop();
		available.push(proc);
	}
}

void handle_processes(admission_queue& processes, size_t quantum)
{
	size_t now = 0;
	priority_comparer comparer(quantum);
	pqueue available(comparer);
	while(!processes.empty() || !available.empty())
	{
		move_to_available(processes, available, now);
		if (available.empty())
		{
			idle(&now, processes.top().admission_time() - now);
		}
		else
		{
			process proc = available.top();
			available.pop();

			if (proc.finishes_in(quantum))
			{
				work(&now, proc.work_time(), proc);	
			}
			else if (proc.io_in_time(quantum))
			{
				size_t time = proc.time_to_io(); 
				work(&now, time, proc);
				proc.io(now);
				processes.push(proc);
			}
			else
			{
				work(&now, quantum, proc, true);
				processes.push(proc);
			}
		}
	}
}

int main()
{
	size_t quantum = 0;
	cin >> quantum;

	string line;
	admission_queue processes(admission_time_comparer);
	while (getline(cin, line))
		if (!line.empty())
			processes.push(build_process(line));
	

	handle_processes(processes, quantum);
	
	return 0;
}
