#define _GNU_SOURCE

#include "download_file_from_url.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>

static float	get_percentage(struct dl_data *dld, unsigned long content_size)
{
	return (dld->total_bytes_downloaded / (double)content_size) * 100.0;
}

static char	*get_total_bytes_downloaded(struct dl_data *dld)
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

/**
 * @brief add the download speed to the dl_data structure
 */
static char	*get_download_speed(struct dl_data *dld)
{
	struct timespec			elapsed_time;
	double					elapsed_time_d;
	struct simplified_value	*sv;
	char					*result;

	elapsed_time = get_elapsed_time(dld->start_download_time);
	elapsed_time_d = timespec_to_double(elapsed_time);
	dld->download_speed = dld->total_bytes_downloaded / elapsed_time_d;
	sv = to_decimal_unit(dld->download_speed);
	result = get_simplified_value_str(sv, 2);
	if (!result) {
		free_binary_unit(sv);
		return NULL;
	}
	free_binary_unit(sv);
	return result;
}

static double	get_time_remaining(struct dl_data *dld, unsigned long content_size)
{
	unsigned long	bytes_remaining;

	bytes_remaining = content_size - dld->total_bytes_downloaded;
	return bytes_remaining / dld->download_speed;
}

void	print_bar(float percentage)
{
	int	bar_width;
	int	position;

	bar_width = 60;
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
	char			*total_bytes_downdloaded;
	double			time_remaining;

	if (!display || is_background || !content_size_str)
		return;
	content_size = atol(content_size_str);
	percentage = get_percentage(dld, content_size);
	download_speed = get_download_speed(dld);
	time_remaining = get_time_remaining(dld, content_size);
	total_bytes_downdloaded = get_total_bytes_downloaded(dld);
	print_bar(percentage);
	printf(" %.2f%%", percentage);
	printf("\n");
	if (total_bytes_downdloaded) {
		printf("   size: %sB", total_bytes_downdloaded);
		free(total_bytes_downdloaded);
	}
	if (download_speed) {
		printf("   speed: %sB/s", download_speed);
		free(download_speed);
	}
	printf("   time remaining: %.1fs", time_remaining);
	if (percentage < 100) {
		printf("      \033[A\r"); // uses space to erase 4 last char of last print
		fflush(stdout);
	} else
		printf("\n");
}
