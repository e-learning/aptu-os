#include <iostream>
#include <memory>
#include <string>

using std::cout;
using std::endl;
using std::cin;

using std::string;
using std::to_string;

struct beg{
	char tag;
	size_t size;
};

struct end{
	size_t size;
	char tag;
};

void init(beg* b, char tag, size_t size){
	b->tag = tag;
	b->size = size;
}

void init(end* e, char tag, size_t size){
	e->tag = tag;
	e->size = size;
}

static const size_t MIN_MEM_SIZE = sizeof(beg)+sizeof(end) + 1;

end* prev_end(beg * b){
	char* bc = reinterpret_cast<char* >(b);
	char* nc = bc - sizeof(end);
	return reinterpret_cast<end*>(nc);
}
end* my_end(beg * b){
	char * bc = reinterpret_cast<char * >(b);
	char * nc = bc + sizeof(beg) + b->size;
	return reinterpret_cast<end*>(nc);
}
beg * prev(beg * b){
	end* e = prev_end(b);
	char *bc = reinterpret_cast<char*>(b);
	char *nc = bc - sizeof(beg) - sizeof(end) - e->size;
	return reinterpret_cast<beg*> (nc);
}
beg * next(beg * b){
	char * bc = reinterpret_cast<char*>(b);
	char * nc = bc + sizeof(beg)+sizeof(end)+b->size;
	return reinterpret_cast<beg*>(nc);
}

void init(char * mem, size_t mem_size){
	size_t user_size = mem_size - sizeof(beg) - sizeof(end);
	beg * a = reinterpret_cast<beg *> (mem);
	init(a, '-', user_size);
	end * b = reinterpret_cast<end *> (mem + mem_size - sizeof(end));
	init(b, '-', user_size);
}

beg* alloc_impl(beg *b, size_t size){
	if (b->size - size < MIN_MEM_SIZE){
		size = b->size;
	}
	if(b->size == size){
		b->tag = '+';
		return b;
	}
	size_t free_size = b->size - size - sizeof(beg) - sizeof(end);
	end* e = my_end(b);

	init(b, '+', size);
	end* ne = my_end(b);
	init(ne, '+', size);
	
	beg* nb = next(b);	
	init(nb, '-',free_size);
	
	init(e, '-', free_size);
	return b;
}
void mem_alloc(char * mem, size_t mem_size, size_t size){
	if (size == 0){
		cout << "-" << endl;
		return;
	}
	beg * b = reinterpret_cast<beg *>(mem);
	
	while(reinterpret_cast<char*>(b) < mem + mem_size){
		if (b->tag == '+'){
			b = next(b);
		}else{
			if (b->size >= size){
				alloc_impl(b, size);
				cout << "+ " << reinterpret_cast<char*>(b) - mem + sizeof(beg) <<endl;
				return;
			}
		}
	}
	cout << '-' << endl;
}
void join(beg* b1, beg* b2){
	b1->size = b1->size + b2->size + sizeof(beg)+sizeof(end);
	end* e2 = my_end(b2);
	e2->size = b1->size;
}
void mem_free(char * mem, size_t mem_size, size_t pnt){
	pnt -= sizeof(beg);
	if (pnt > mem_size - MIN_MEM_SIZE){
		cout << '-' << endl;
		return;
	}
	if (mem[pnt] != '+'){
		cout << '-' << endl;
		return;
	}
	beg * b = reinterpret_cast<beg*> (mem+pnt);
	end * e = my_end(b);
	b->tag = '-';
	e->tag = '-';
	if (next(b)->tag == '-'){
		join(b, next(b));
	}
	if (pnt != 0 && prev_end(b)->tag == '-'){
		join(prev(b), b);
	}
	cout << '+' << endl; 
}
void mem_info(char * mem, size_t mem_size){
	size_t user_blocks = 0;
	size_t user_mem = 0;
	size_t max_block = 0;
	beg * b = reinterpret_cast<beg*>(mem);
	while(reinterpret_cast<char*>(b) < mem+mem_size){
		if (b->tag == '-'){
			if (b->size > max_block)
				max_block = b->size;
		}else{
			user_blocks++;
			user_mem += b->size;
		}
		b = next(b);
	}
	cout << user_blocks << endl;
	cout << user_mem << endl;
	cout << max_block << endl;
}

void print_n_times(char c, size_t times){
	for (size_t i = 0; i!= times; ++i ){
		cout << c;
	}
}

void mem_map(char* mem, size_t mem_size){
    char * p = mem;
	while (p < mem + mem_size){
		beg* b = reinterpret_cast<beg* >(p);
		print_n_times('m', sizeof(beg));
		if (b->tag == '+'){
			print_n_times('u', b->size);
		}else{
			print_n_times('f', b->size);
		}
		print_n_times('m',sizeof(end));
		p = reinterpret_cast<char*>(next(b));
	}
	cout << endl;
}

int main(){
	size_t mem_size = 0;
	cin >> mem_size;
	if (mem_size == 0){
		return 1;
	}
	std::unique_ptr<char[]> mem_manager (new char[mem_size]);
	char * mem = mem_manager.get();
	init(mem, mem_size);
	while(true){
		string command;
		cin >> command;
		if (command == "ALLOC"){
			size_t size = 0;
			cin >> size;
			mem_alloc(mem, mem_size, size);
		}else if (command == "FREE"){
			size_t pnt = 0;
			cin >> pnt;
			mem_free(mem, mem_size, pnt);
		}else if (command == "INFO"){
			mem_info(mem, mem_size);
		}else if (command == "MAP"){
			mem_map(mem, mem_size);
		}else{
			cout << "Bad command. " << endl;
		}
	}
	
	
	return 0;
}
