#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int background(char *url, struct parameters_t params,
                                void (*f)(char *url, struct parameters_t params))
{
    // Creating the fork process
    pid_t pid = fork();
    int fd;

    // Opening file using file descriptor
    fd = open("wget-log", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (pid < 0)
    {
        perror("Failed to fork");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        // Redirect all the output & error to the file
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);

        // Execute the function
        f(url, params);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Close file & wait for the child process
        close(fd);
        wait(NULL);
        printf("Output will be written to wget-log\n");
    }

    return EXIT_SUCCESS;
}
