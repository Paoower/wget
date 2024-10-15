#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>

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

struct timespec	time_diff(struct timespec time1, struct timespec time2)
{
	struct timespec	time_diff;

	if ((time2.tv_nsec - time1.tv_nsec) < 0) {
		time_diff.tv_sec = time2.tv_sec - time1.tv_sec - 1;
		time_diff.tv_nsec = time2.tv_nsec + 1e9 - time1.tv_nsec;
	} else {
		time_diff.tv_sec = time2.tv_sec - time1.tv_sec;
		time_diff.tv_nsec = time2.tv_nsec - time1.tv_nsec;
	}
	return time_diff;
}

struct timespec	get_elapsed_time(struct timespec start_time)
{
	struct timespec	current_time;

	clock_gettime(CLOCK_MONOTONIC, &current_time);
	return time_diff(start_time, current_time);
}

double	timespec_to_double(struct timespec time)
{
	return time.tv_sec + (time.tv_nsec / 1e9);
}
