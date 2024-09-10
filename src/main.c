#include "settings.h"
#include "src.h"
#include <stddef.h>
#include <string.h>

int	main(int argc, char *argv[])
{
	struct parameters_t	parameters = {
		.background = 0,
		.convert_links = 0,
		.exclude_list = NULL,
		.storage_path = strdup("./data/"),
		.links_file = NULL,
		.rate_limit = 0,
		.mirror = 0,
		.output_file = NULL,
		.reject_list = NULL,
		.url = NULL,
	};

	handle_args(&parameters, argc, argv);
	if (parameters.background) {
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
