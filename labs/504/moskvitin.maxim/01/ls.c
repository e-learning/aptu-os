#include "ls.h"
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

int ls()
{
    DIR* cur_dir = opendir(".");
    if (cur_dir == NULL)
    {
        perror("Cannot open current directory");
        return -1;
    }


    struct strArray* entryes = createStrArray();
    if (entryes == NULL)
    {
        perror("Cannot allocate memory");
        closedir(cur_dir);
        return -1;
    }

    int last_errno = errno;
    struct dirent* entry = NULL;
    do
    {
        entry = readdir(cur_dir);
        if (entry != NULL && entry->d_name[0] != '.')
        {
            if (strArrayPush(entryes, entry->d_name))
            {
                perror("Cannot allocate memory");
                closedir(cur_dir);
                strArrayFree(entryes);
                return -1;
            }
        }
    } while (entry != NULL);

    if (last_errno != errno)
    {
        perror("Error on read directory");
        closedir(cur_dir);
        strArrayFree(entryes);
        return -1;
    }

    int code = 0;
    if (printAsTable(entryes))
        code = -1;
    closedir(cur_dir);
    strArrayFree(entryes);
    return code;
}

struct strArray* createStrArray()
{
    struct strArray* arr = malloc(sizeof(struct strArray));

    if (arr == NULL)
        return NULL;
    arr->entries = malloc(sizeof(char*));

    if (arr->entries == NULL)
    {
        free(arr);
        return NULL;
    }

    arr->entries[0] = NULL;
    arr->allocated_size = 1;
    arr->size = 0;

    return arr;
};

int strArrayPush(struct strArray *arr, const char* entry)
{
    if (arr->allocated_size == arr->size + 1)
    {
        char** newArr = malloc(sizeof(char*) * arr->allocated_size * 2);
        if (newArr == NULL)
            return -1;
        memcpy(newArr, arr->entries, (arr->size + 1) * sizeof(char*));
        arr->allocated_size *= 2;
        free(arr->entries);
        arr->entries = newArr;
    }

    size_t len = strlen(entry) + 1;
    arr->entries[arr->size] = malloc(len * sizeof(char));
    if (arr->entries[arr->size] == NULL)
        return -1;
    memcpy(arr->entries[arr->size], entry, len * sizeof(char));
    ++arr->size;
    arr->entries[arr->size] = NULL;
    return 0;
}

void strArrayFree(struct strArray *arr)
{
    char** pArr = arr->entries;
    while (*pArr)
    {
        free((void *)*(pArr++));
    }
    free((void *)arr->entries);
    free(arr);
}

int printAsTable(struct strArray *arr)
{
    if (arr->size == 0)
        return 0;

    size_t* lens = malloc(sizeof(size_t) * arr->size);
    if (lens == NULL)
        return -1;

    size_t i;
    for (i = 0; i < arr->size; ++i)
    {
        lens[i] = strlen(arr->entries[i]);
    }

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int max_col = 1;
    int rem_width = w.ws_col;

    size_t* col_width = malloc(sizeof(size_t) * (arr->size + 1));
    if (col_width == NULL)
    {
        free(lens);
        return -1;
    }

    while (rem_width > 0 && max_col <= arr->size)
    {
        ++max_col;

        for (i = 0; i < max_col; ++i)
        {
            col_width[i] = 0;
        }

        int row_count = arr->size / max_col;
        if (arr->size % max_col)
            ++row_count;

        for (i = 0; i < arr->size; ++i)
        {
            if (col_width[i / row_count] < lens[i])
                col_width[i / row_count] = lens[i];
        }

        rem_width = w.ws_col - 2 * (max_col - 1);
        for (i = 0; i < max_col; ++i)
            rem_width -= col_width[i];
    }

    max_col--;

    size_t row_count = arr->size / max_col;
    if (arr->size % max_col)
        ++row_count;

    for (i = 0; i < arr->size; ++i)
    {
        if (col_width[i / row_count] < lens[i])
            col_width[i / row_count] = lens[i];
    }

    for (i = 0; i < row_count; ++i)
    {
        size_t j = i;
        for (; j < arr->size; j += row_count)
        {
            printf("%s", arr->entries[j]);
            int rem = col_width[j / row_count] - lens[j] + 2;
            if (j + row_count >= arr->size)
                rem = 0;
            while (rem--)
                putc(' ', stdout);
        }
        putc('\n', stdout);
    }

    free(lens);
    free(col_width);
    return 0;
}