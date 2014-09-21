#include <stdio.h>
#include "command_executing.h"
#include <malloc.h>

char * my_getline(void) {
    char * line = malloc(100 * sizeof(char)), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF)
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2 * sizeof(char));

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if(c == '\n')
            break;
        *line++ = c;
    }
    *line = '\0';
    return linep;
}

int main(void)
{
    char* command_line = NULL;
    bool is_not_exit;
    do
    {
        free(command_line);
        command_line = my_getline();
        if(command_line == NULL)
        {
            // When not enought mem to read line
            return -1;
        }
        is_not_exit = execute_command(command_line);
    } while(is_not_exit);
    return 0;
}

