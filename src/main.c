#include "srcs.h"

int main(int argc, char *argv[])
{
<<<<<<< Updated upstream
	download_file("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg", DATA_PATH, NULL);
=======
	if (!handle_args(argc, argv))
	{
		return 0;
	}
	download_file("https://pbs.twimg.com/media/EMtmPFLWkAA8CIS.jpg", DATA_PATH);
>>>>>>> Stashed changes
}
