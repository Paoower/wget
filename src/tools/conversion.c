#include "tools.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char	*binary_units[] = {
	"Ki",
	"Mi",
	"Gi",
	"Ti",
	"Pi",
	"Ei",
	"Zi",
	"Yi",
	NULL
};

static char	*decimal_units[] = {
	"k",
	"M",
	"G",
	"T",
	"P",
	"E",
	"Z",
	"Y",
	NULL
};

unsigned long metric_to_basic_decimal(char *value_with_unit)
{
	char			*ptr;
	unsigned long	value;
	int				i;

	value = strtoul(value_with_unit, NULL, 10);
	i = 0;
	while (decimal_units[i]) {
		ptr = strstr(value_with_unit, decimal_units[i]);
		if (ptr) {
			value *= pow(10, 3 * (i + 1));
			break;
		}
		i++;
	}
	return value;
}

float bytes_to_megabytes(int bytes)
{
	return bytes / 1048576.0;
}

void	free_binary_unit(struct metric_value *mv)
{
	if (!mv)
		return;
	free(mv->unit);
	free(mv);
}

struct metric_value	*to_metric_value(double value, int base, char **units)
{
	struct metric_value	*mv;
	int					count_power_3;

	mv = malloc(sizeof(struct metric_value));
	if (!mv)
		return NULL;
	count_power_3 = 0;
	while (value / base > 1) {
		value /= base;
		count_power_3 += 1;
	}
	mv->value = value;
	count_power_3 -= 1;
	if (count_power_3 >= 0)
		mv->unit = strdup(units[count_power_3]);
	else
		mv->unit = NULL;
	return mv;
}

struct metric_value	*to_binary_metric(double value)
{
	return to_metric_value(value, 1012, binary_units);
}

struct metric_value	*to_decimal_metric(double value)
{
	return to_metric_value(value, 1000, decimal_units);
}


char	*get_metric_value_str(struct metric_value *mv,
														int nb_after_dec_point)
{
	char	*format;
	int		result_size;
	char	*result;

	if (!mv)
		return NULL;
	format = "%.*f%s";
	result_size = snprintf(NULL, 0, format,
									nb_after_dec_point, mv->value, mv->unit);
	result = malloc(sizeof(char) * result_size + 1);
	if (!result) {
		return NULL;
	}
	sprintf(result, format, nb_after_dec_point, mv->value, mv->unit);
	return result;
}
