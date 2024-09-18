#include "progress_bar.h"
#include <stdio.h>
#include <stdlib.h> 

void update_bar(unsigned long total_bytes_downloaded, char *content_size)
{
    int content_size_f = atof(content_size);
    int bar_width = 50;
    float percentage = ((float)total_bytes_downloaded / content_size_f) * 100;

    int position = bar_width * percentage / 100;
    printf("[");
    for (int i = 0; i < bar_width; ++i)
    {
        if (i < position)
        {
            printf("=");
        }
        else if (i == position)
        {
            printf(">");
        }
        else
        {
            printf(" ");
        }
    }
    printf("] %f%%\r", percentage);
    fflush(stdout);
}