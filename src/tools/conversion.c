#include "tools.h"
#include <stdlib.h>
#include <string.h>

char	*binary_units[] = {
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

char	*decimal_units[] = {
	"K",
	"M",
	"G",
	"T",
	"P",
	"E",
	"Z",
	"Y",
	NULL
};

float bytes_to_megabytes(int bytes)
{
	return bytes / 1048576.0;
}

void	free_binary_unit(struct simplified_value *bu)
{
	if (!bu)
		return;
	free(bu->unit);
	free(bu);
}

struct simplified_value	*to_simplified_value(double value, int base, char **units)
{
	struct simplified_value	*bu;
	int						count_power_3;

	bu = malloc(sizeof(struct simplified_value));
	if (!bu)
		return NULL;
	count_power_3 = 0;
	while (value / base > 1) {
		value /= base;
		count_power_3 += 1;
	}
	bu->value = value;
	count_power_3 -= 1;
	if (count_power_3 >= 0)
		bu->unit = strdup(units[count_power_3]);
	else
		bu->unit = NULL;
	return bu;
}

struct simplified_value	*to_binary_unit(double value)
{
	return to_simplified_value(value, 1012, binary_units);
}

struct simplified_value	*to_decimal_unit(double value)
{
	return to_simplified_value(value, 1000, decimal_units);
}


char	*get_simplified_value_str(struct simplified_value *sv,
														int nb_after_dec_point)
{
	char	*format;
	int		result_size;
	char	*result;

	if (!sv)
		return NULL;
	format = "%.*f%s";
	result_size = snprintf(NULL, 0, format,
									nb_after_dec_point, sv->value, sv->unit);
	result = malloc(sizeof(char) * result_size + 1);
	if (!result) {
		return NULL;
	}
	sprintf(result, format, nb_after_dec_point, sv->value, sv->unit);
	return result;
}
