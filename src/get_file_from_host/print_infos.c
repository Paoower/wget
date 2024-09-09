#include <stdio.h>
#include <time.h>

void	print_current_date(char *text_before)
{
	time_t		current_time;
	struct tm	*current_time_tm;
	char		buffer[20];

	current_time = time(NULL);
	current_time_tm = localtime(&current_time);
	if (current_time_tm != NULL) {
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", current_time_tm);
		printf("%s%s\n", text_before, buffer);
	}
}

void	print_final_download_infos(struct timespec elapsed_time,
									long unsigned int total_bytes_downloaded)
{
	printf("elapsed time: %ld.%09ld s\n",
									elapsed_time.tv_sec, elapsed_time.tv_nsec);
	printf("bytes downloaded: %lu\n", total_bytes_downloaded);
}
