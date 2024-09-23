#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int	wget_classic(char *url, const char *storage_dir_path,
								char *file_name, unsigned long bytes_per_sec)
{
	char	*file_path;

	print_current_date("start at ");
	file_path = download_file_from_url(url, storage_dir_path,
												file_name, bytes_per_sec, 0);
	if (!file_path)
		return 1;
	printf("Downloaded [%s]\n", file_path);
	free(file_path);
	print_current_date("finished at ");
	return 0;
}

int	wget_in_mode(int is_mirror, struct parameters_t params)
{
	if (is_mirror)
		return wget_mirror(params.url, params);
	else
		return wget_classic(params.url, params.storage_path,
										params.output_file, params.rate_limit);
}

int	wget(struct parameters_t params)
{
	if (params.links_file)
		return wget_from_file(params);
	else
		return wget_in_mode(params.mirror, params);
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
