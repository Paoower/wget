#include <time.h>

struct timespec time_diff(struct timespec time1, struct timespec time2)
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
