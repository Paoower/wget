#include "settings.h"
#include "src.h"
#include "get_file_from_host.h"
#include <stddef.h>

void	wget(char *url, struct parameters_t params)
{
	print_current_date("start at ");
	get_file_from_host(url, params.file_path, params.output_file,
									(long unsigned *)&params.rate_limit);
	print_current_date("finished at ");
}

int	main(int argc, char *argv[])
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
	if (parameters.background) {
		background("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg",
															parameters, wget);
	} else {
		wget("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg", parameters);
	}
	return 0;
}
