#include "src.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	struct parameters_t parameters = {
		.background = 0,
		.convert_links = 0,
		.exclude_list = NULL,
		.storage_path = NULL,
		.links_file = NULL,
		.rate_limit = 0,
		.mirror = 0,
		.output_file = NULL,
		.reject_list = NULL,
		.url = NULL,
	};

	handle_args(&parameters, argc, argv);
	if (parameters.background) {
		printf("Output will be written to wget-log\n");
		if (wget_in_background(parameters)) {
			free_args(&parameters);
			return 1;
		}
	}
	else if (wget(parameters)) {
		free_args(&parameters);
		return 1;
	}
	free_args(&parameters);
	return 0;
}
