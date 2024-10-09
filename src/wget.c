#include "src.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <openssl/err.h>

int	wget_classic(SSL_CTX *ctx, char *url, struct parameters_t p)
{
	struct file_data	*file_data;

	print_current_date("start at ");
	file_data = download_file_from_url(ctx, url, p.storage_path,
									p.output_file, p.rate_limit, 0, true);
	if (!file_data || !file_data->file_path) {
		free_file_data(file_data);
		return 1;
	}
	printf("Downloaded [%s]\n", file_data->file_path);
	if (p.convert_links && file_data->header_data
						&& file_data->header_data->is_html) {
		parse_links_from_html(file_data, p.reject_list,
											p.exclude_list, p.convert_links, 0);
	}
	free_file_data(file_data);
	print_current_date("finished at ");
	return 0;
}

int	wget_in_mode(SSL_CTX *ctx, int is_mirror, struct parameters_t params)
{
	if (is_mirror)
		return wget_mirror(ctx, params.url, params);
	else
		return wget_classic(ctx, params.url, params);
}

int	wget(struct parameters_t params)
{
	SSL_CTX	*ctx;
	int		result;

	ctx = init_ssl();
	if (!ctx) {
		ERR_print_errors_fp(stderr);
		return 1;
	}
	if (params.links_file)
		result = wget_from_file(ctx, params);
	else
		result = wget_in_mode(ctx, params.mirror, params);
	SSL_CTX_free(ctx);
	EVP_cleanup();
	return result;
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
