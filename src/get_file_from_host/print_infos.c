#include <stdio.h>
#include <time.h>

void	print_final_download_infos(struct timespec elapsed_time,
									long unsigned int total_bytes_downloaded)
{
	printf("elapsed time: %ld.%09ld s\n",
									elapsed_time.tv_sec, elapsed_time.tv_nsec);
	printf("bytes downloaded: %lu\n", total_bytes_downloaded);
}
