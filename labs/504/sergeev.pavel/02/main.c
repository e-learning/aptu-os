#include <stdio.h>

#include "scheduler.h"

int main(int argc, char** argv)
{
	FILE* input;
	FILE* output;
	input = fopen("input.txt", "r");
	load_tasks(input);
	fclose(input);
	output = fopen("output.txt", "w");
	run_scheduler(stdout);
	fclose(output);
	return 0;
}