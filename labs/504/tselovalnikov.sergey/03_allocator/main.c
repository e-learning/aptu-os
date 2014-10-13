#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

typedef struct ptr {
	intptr_t offset;
	int size;
	struct ptr* next;
} ptr;

intptr_t max(intptr_t a, intptr_t b) {
	return a > b ? a : b;
}

int buffsize;
void* buffer;
ptr* first = NULL;

void init(int n) {
	buffer = malloc(n);
	buffsize = n;
}

ptr *alloc(int size) {
	if (!first) {
		if (size <= buffsize - sizeof(ptr)) {
			ptr* p = buffer;
			p->size = size;
			p->offset = sizeof(ptr);
			p->next = NULL;
			first = p;
			return p;
		} else {
			return NULL;
		}
	}
	if (size + sizeof(ptr) <= (void*) first - (void*) buffer) {
		ptr* p = buffer;
		p->size = size;
		p->offset = sizeof(ptr);
		p->next = first;
		first = p;
		return p;
	}
	ptr *current = first;
	while (1) {
		ptr *next = current->next;
		if (!next) {
			int remaining = buffsize - (current->size + current->offset);
			if (size + sizeof(ptr) <= remaining) {
				ptr* p = (void*) (current->size + current->offset + buffer);
				p->size = size;
				p->offset = current->size + current->offset + sizeof(ptr);
				current->next = p;
				return p;
			} else {
				return NULL;
			}
		} else {
			if (size + sizeof(ptr)
					<= (void*) next
							- ((void*) current + current->size + sizeof(ptr))) {
				// можем выделить между блоками
				ptr* p = (void*) (current->size + current->offset + buffer);
				p->size = size;
				p->offset = current->size + current->offset + sizeof(ptr);
				p->next = next;
				current->next = p;
				return p;
			} else {
				current = current->next;
			}
		}
	}
}

// return 1 - success, 0 - fail
int freealoc(intptr_t p) {
	ptr *prev = NULL;
	ptr *current = first;
	while (current) {
		if (current->offset == p) {
			if (!prev) {
				first = current->next != NULL ? current->next : NULL;
			} else {
				prev->next = current->next != NULL ? current->next : NULL;
			}
			return 1;
		}
		prev = current;
		current = current->next;
	}
	return 0;
}

void map() {
	intptr_t i = 0;
	if (!first) {
		for (i = 0; i < buffsize; i++) {
			printf("%c", 'f');
		}
		printf("\n");
		return;
	}
	ptr *current = first;
	while (current) {
		intptr_t ptr_begin = (void*) current - (void*) buffer;
		for (; i < ptr_begin; i++) {
			printf("%c", 'f');
		}
		for (; i < ptr_begin + sizeof(ptr); i++) {
			printf("%c", 'm');
		}
		for (; i < ptr_begin + sizeof(ptr) + current->size; i++) {
			printf("%c", 'u');
		}
		current = current->next;
	}
	for (; i < buffsize; i++) {
		printf("%c", 'f');
	}
	printf("\n");
}

void info() {
	int count = 0;
	intptr_t summ = 0;
	intptr_t maxx = 0;
	ptr *prev = NULL;
	ptr *current = first;
	if(first) {
		maxx = (void*)first - (void*)buffer;
	} else {
		maxx = buffsize;
	}
	while (current) {
		count++;
		summ += current->size;
		if(prev) {
			maxx = max(maxx, (void*)current - ((void*)prev + sizeof(ptr) + prev->size));
		}
		prev = current;
		current = current->next;
	}
	if(prev) {
		maxx = max(maxx, (void*)buffer + buffsize - ((void*)prev + sizeof(ptr) + prev->size));
	}
	maxx = max(0, maxx-sizeof(ptr));
	printf("%d %ld %ld\n", count, summ, maxx);
}


int main() {
	int n = 0;
	scanf("%d", &n);
	init(n);

	while (1) {
		char command[64];
		scanf("%s", command);

		if (strcmp(command, "ALLOC") == 0) {
			int size;
			scanf("%d", &size);
			ptr *p = alloc(size);
			if (p == NULL) {
				printf("-1\n");
			} else {
				printf("+%ld\n", p->offset);
			}
		} else if (strcmp(command, "FREE") == 0) {
			int p;
			scanf("%d", &p);
			if (freealoc(p)) {
				printf("+\n");
			} else {
				printf("-\n");
			}
		} else if (strcmp(command, "INFO") == 0) {
			info();
		} else if (strcmp(command, "MAP") == 0) {
			map();
		}
	}

	return 0;
}
