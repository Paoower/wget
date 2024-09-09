#include "settings.h"
#include "src.h"
#include <stddef.h>

int main(int argc, char *argv[])
{
	struct parameters_t parameters = {
		.background = 0,
		.convert_links = 0,
		.exclude_list = "",
		.file_path = "./data/",
		.links_file = "",
		.rate_limit = 0,
		.mirror = 0,
		.output_file = "file.mp4",
		.reject_list = 0,
		.url = "",
	};

	handle_args(&parameters, argc, argv);

	if (parameters.background)
	{
		background(parameters);
		return 0;
	}
	get_file_from_host("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg",
					   parameters.file_path, parameters.output_file, (long unsigned int *)&parameters.rate_limit);
}
