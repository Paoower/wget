#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int background(struct parameters_t parameters)
{
    pid_t pid = fork();
    int fd;

    fd = open("wget-log.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (pid < 0)
    {
        perror("Failed to fork");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);

        get_file_from_host("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg",
                           parameters.file_path, parameters.output_file, (long unsigned int *)&parameters.rate_limit);
        exit(EXIT_SUCCESS);
    }
    else
    {
        close(fd);
        wait(NULL);
        printf("Output will be written to wget-log.txt\n");
    }

    return EXIT_SUCCESS;
}