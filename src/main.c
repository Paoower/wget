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
	if (parameters.background)
		return wget_in_background(parameters);
	else
		return wget(parameters);
}
