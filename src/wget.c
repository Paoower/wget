#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int	wget_classic(struct parameters_t params)
{
	char	*file_path;

	print_current_date("start at ");
	file_path = download_file_from_url(params.url, params.storage_path,
						params.output_file, params.rate_limit, params.mirror);
	if (!file_path)
		return 1;
	printf("Downloaded [%s]\n", file_path);
	free(file_path);
	print_current_date("finished at ");
	return 0;
}

int	wget(struct parameters_t params)
{
	if (params.links_file)
		return wget_from_file(params);
	else if (params.mirror)
		return wget_mirror(params);
	else
		return wget_classic(params);
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
