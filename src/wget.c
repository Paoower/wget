#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void	print_current_date(char *text_before)
{
	time_t		current_time;
	struct tm	*current_time_tm;
	char		buffer[20];

	current_time = time(NULL);
	current_time_tm = localtime(&current_time);
	if (current_time_tm != NULL) {
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", current_time_tm);
		printf("%s%s\n", text_before, buffer);
	}
}

int	wget_file_mode(struct parameters_t params)
{
	(void)params;
	return 0;
}

int	wget_normal_mode(struct parameters_t params)
{
	print_current_date("start at ");
	if (get_file_from_host(params.url, params.storage_path, params.output_file,
									(long unsigned *)&params.rate_limit))
		return 1;
	print_current_date("finished at ");
	return 0;
}

int	wget(struct parameters_t params)
{
	int	normal_mode;

	normal_mode = 1;
	if (normal_mode)
		return wget_normal_mode(params);
	else
		return wget_file_mode(params);
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

		if (wget(params))
			return 1;
		exit(EXIT_SUCCESS);
	}
	else {
		// Wait for the child process
		wait(NULL);
		printf("Output will be written to wget-log\n");
	}
	return EXIT_SUCCESS;
}
