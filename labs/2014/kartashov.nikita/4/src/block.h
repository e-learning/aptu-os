#ifndef BLOCK_H
#define BLOCK_H

class block;

const int INT_SIZE = 4;
const int HEADER_SIZE = 3 * INT_SIZE;
block* NO_BLOCK = 0;
const int MIN_BLOCK_SIZE = HEADER_SIZE + 1;
const int FAIL = -1;
const int SUCCESS = 0;

class block
{
public:
	block(int size) : 
		m_size(size), 
		m_next(NO_BLOCK), 
		m_prev(NO_BLOCK) {}

	int available() {return is_free() ? m_size + HEADER_SIZE : 0;}
	bool is_free() {return m_size > 0;}
	int allocate() 
	{
		int result = SUCCESS;
		if (m_size > 0)
			m_size *= -1;
		else
			result = FAIL;
		return result;
	}
	int free() 
	{
		int result = SUCCESS;
		if (m_size < 0)
			m_size *= -1;
		else
			result = FAIL;
		return result;
	}
	int real_size() {return abs(m_size);}
	void merge()
	{
		if (!is_free() || m_next == NO_BLOCK || !m_next->is_free())
			return;
	
		block* ptr = m_next;
		if (m_next->m_next != NO_BLOCK)
			m_next->m_next->m_prev = this;
		m_next = m_next->m_next;
		m_size += HEADER_SIZE + ptr->m_size;
		delete ptr;
	}

	void prev_merge()
	{
		if (m_prev == NO_BLOCK)
			return;		
		m_prev->merge();
	}

	block* next() {return m_next;}
	block* prev() {return m_prev;}
	void set_prev(block* prev) {m_prev = prev;}
	void set_next(block* next) {m_next = next;}
	int size() {return m_size;}
	void set_size(int size) {m_size = size;}

private:
	int m_size;
	block* m_next;
	block* m_prev;
};

#endif /* end of include guard */
