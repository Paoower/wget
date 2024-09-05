#include "settings.h"
#include "src.h"
#include <stddef.h>

int main(int argc, char *argv[])
{
	handle_args(argc, argv);
	download_file("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg", params.file_path, params.output_file);
}
