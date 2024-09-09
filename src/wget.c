#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void	wget(struct parameters_t params)
{
	print_current_date("start at ");
	get_file_from_host(params.url, params.file_path, params.output_file,
									(long unsigned *)&params.rate_limit);
	print_current_date("finished at ");
}

int wget_in_background(struct parameters_t params)
{
	// Creating the fork process
	pid_t	pid;
	int		fd;

	pid = fork();
	if (pid < 0) {
		perror("Failed to fork");
		return EXIT_FAILURE;
	}
	if (pid == 0) {
		fd = open("wget-log", O_CREAT | O_WRONLY | O_TRUNC, 0644);
		// Redirect all the output & error to the file
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		close(fd);

		wget(params);
		exit(EXIT_SUCCESS);
	}
	else {
		// Wait for the child process
		wait(NULL);
		printf("Output will be written to wget-log\n");
	}
	return EXIT_SUCCESS;
}
