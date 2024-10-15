#define _GNU_SOURCE

#include "download_file_from_url.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>

static float	get_percentage(struct dl_data *dld, unsigned long content_size)
{
	return (dld->total_bytes_downloaded / (double)content_size) * 100.0;
}

static char	*get_total_bytes(struct dl_data *dld)
{
	struct simplified_value	*sv;
	char					*result;

	sv = to_binary_unit(dld->total_bytes_downloaded);
	result = get_simplified_value_str(sv, 2);
	if (!result) {
		free_binary_unit(sv);
		return NULL;
	}
	free_binary_unit(sv);
	return result;
}

static char	*get_download_speed(struct dl_data *dld)
{
	struct timespec			elapsed_time;
	double					elapsed_time_d;
	double					dl_speed;
	struct simplified_value	*sv;
	char					*result;

	elapsed_time = get_elapsed_time(dld->start_download_time);
	elapsed_time_d = timespec_to_double(elapsed_time);
	dl_speed = dld->total_bytes_downloaded / elapsed_time_d;
	sv = to_decimal_unit(dl_speed);
	result = get_simplified_value_str(sv, 2);
	if (!result) {
		free_binary_unit(sv);
		return NULL;
	}
	free_binary_unit(sv);
	return result;
}

void	print_bar(float percentage)
{
	int	bar_width;
	int	position;

	bar_width = 50;
	position = bar_width * percentage / 100;
	printf("[");
	for (int i = 0; i < bar_width; ++i) {
		if (i < position)
			printf("=");
		else if (i == position)
			printf(">");
		else
			printf(" ");
	}
	printf("]");
}

void	update_bar(struct dl_data *dld, char *content_size_str,
											bool display, bool is_background)
{
	float			percentage;
	unsigned long	content_size;
	char			*download_speed;
	char			*total_bytes;

	if (!display || is_background || !content_size_str)
		return;
	content_size = atol(content_size_str);
	percentage = get_percentage(dld, content_size);
	download_speed = get_download_speed(dld);
	total_bytes = get_total_bytes(dld);
	print_bar(percentage);
	printf(" %.2f%%", percentage);
	if (total_bytes) {
		printf(" %s", total_bytes);
		free(total_bytes);
	}
	if (download_speed) {
		printf(" %sB/s", download_speed);
		free(download_speed);
	}
	printf("    \r"); // uses space to erase 4 last char of last print
	fflush(stdout);
}
