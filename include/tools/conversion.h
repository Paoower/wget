#ifndef CONVERSION_H
# define CONVERSION_H

# include <stddef.h>

struct simplified_value {
	double	value;
	char	*unit;
};

void					free_binary_unit(struct simplified_value *bu);
struct simplified_value	*to_binary_unit(double value);
struct simplified_value	*to_decimal_unit(double value);
char					*get_simplified_value_str(struct simplified_value *sv,
														int nb_after_dec_point);
float					bytes_to_megabytes(int bytes);

#endif
