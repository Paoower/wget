#include "settings.h"
#include "src.h"
#include "get_file_from_host.h"
#include <stddef.h>

int main(int argc, char *argv[])
{
	handle_args(argc, argv);
	print_current_date("start at ");
	get_file_from_host("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg",
			params.file_path, params.output_file, NULL);
	print_current_date("finished at ");
}
