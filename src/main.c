#include "srcs.h"

int main(int argc, char *argv[])
{
	if (!handle_args(argc, argv))
	{
		fprintf(stderr, "No args\n");
	}
	download_file("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg", DATA_PATH);
}
