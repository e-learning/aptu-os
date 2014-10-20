#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_NUMBER_OF_RECODRS 1000
#define MAX_INPUT 100
typedef int bool;
#define true 1
#define false 0

uint64_t get_segment_of_bits(uint64_t address, size_t left, size_t right) {
	if (left > right) {
		puts("INVALID");
		exit(1);
	}
	return (address >> left) & (~(UINT64_MAX << (right - left + 1)));
}

uint64_t calculate_address(size_t gdt_size, size_t ldt_size, size_t pd_size, size_t pt_size,
		uint64_t logic_address, uint64_t segment_register, uint64_t * gdt_table,
		uint64_t * ldt_table, uint64_t * page_directory, uint64_t * page_table) {
	uint64_t * table_reference =
			get_segment_of_bits(segment_register, 2, 2) ? ldt_table : gdt_table;
	size_t table_reference_size = get_segment_of_bits(segment_register, 2, 2) ? ldt_size : gdt_size;
	uint64_t index = get_segment_of_bits(segment_register, 3, 15);
	if ((table_reference == gdt_table && index == 0) || index >= table_reference_size) {
		puts("INVALID");
		exit(1);
	}
	uint64_t segment_description = table_reference[index];
	if (!get_segment_of_bits(segment_description, 32 + 15, 32 + 15)) {
		puts("INVALID");
		exit(1);
	}
	bool granularity = get_segment_of_bits(segment_description, 32 + 23, 32 + 23);
	uint64_t limit = (get_segment_of_bits(segment_description, 0, 15))
			| (get_segment_of_bits(segment_description, 32 + 16, 32 + 19) << 16);
	limit <<= (granularity ? 12 : 0);
	if (logic_address > limit) {
		puts("INVALID");
		exit(1);
	}
	uint64_t base = (get_segment_of_bits(segment_description, 32 + 24, 32 + 31) << 24)
			| (get_segment_of_bits(segment_description, 32 + 0, 32 + 7) << 16)
			| (get_segment_of_bits(segment_description, 16, 31));

	uint64_t directory_start = get_segment_of_bits(base + logic_address, 22, 31);
	uint64_t table_start = get_segment_of_bits(base + logic_address, 12, 21);

	if (directory_start >= pd_size || table_start >= pt_size) {
		puts("INVALID");
		exit(1);
	}

	if (!get_segment_of_bits(page_directory[directory_start], 0, 0)
			|| !get_segment_of_bits(page_table[table_start], 0, 0)) {
		puts("INVALID");
		exit(1);
	}

	return ((get_segment_of_bits(page_table[table_start], 12, 31) << 12)
			| get_segment_of_bits(base + logic_address, 0, 11));
}

int main(int argc, const char *argv[]) {
	uint64_t gdt_table[MAX_NUMBER_OF_RECODRS];
	uint64_t ldt_table[MAX_NUMBER_OF_RECODRS];
	uint64_t page_directory[MAX_NUMBER_OF_RECODRS];
	uint64_t page_table[MAX_NUMBER_OF_RECODRS];
/*
	if (freopen("test2.txt", "r", stdin) == NULL) {
		return EXIT_FAILURE;
	}
*/
	char hex_string[MAX_INPUT];

	if (scanf("%s", hex_string) != 1) {
		return EXIT_FAILURE;
	}
	uint64_t logic_address = strtol(hex_string, NULL, 16);

	if (scanf("%s", hex_string) != 1) {
		return EXIT_FAILURE;
	}
	uint64_t segment_register = strtol(hex_string, NULL, 16);

	uint64_t * tables[4] = { gdt_table, ldt_table, page_directory, page_table };
	size_t table_sizes[4];
	for (int i = 0; i < 4; ++i) {
		int size;
		if (scanf("%d", &size) != 1) {
			return EXIT_FAILURE;
		}

		table_sizes[i] = size;
		for (int j = 0; j < size; j++) {
			if (scanf("%s", hex_string) != 1) {
				return EXIT_FAILURE;
			}
			tables[i][j] = strtol(hex_string, NULL, 16);
		}
	}
	uint64_t address = calculate_address(table_sizes[0], table_sizes[1], table_sizes[2],
			table_sizes[3], logic_address, segment_register, gdt_table, ldt_table, page_directory,
			page_table);
	printf("%lx\n", address);

	return EXIT_SUCCESS;
}
