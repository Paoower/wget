#include "settings.h"
#include "src.h"
#include <stddef.h>

int	main(int argc, char *argv[])
{
	struct parameters_t	parameters = {
		.background = 0,
		.convert_links = 0,
		.exclude_list = "",
		.file_path = "./data/",
		.links_file = "",
		.rate_limit = 0,
		.mirror = 0,
		.output_file = NULL,
		.reject_list = 0,
		.url = "",
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
