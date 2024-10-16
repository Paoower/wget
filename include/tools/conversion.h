#ifndef CONVERSION_H
# define CONVERSION_H

# include <stddef.h>

struct metric_value {
	double	value;
	char	*unit;
};
unsigned long		metric_to_basic_decimal(char *value_with_unit);
float				bytes_to_megabytes(int bytes);

void				free_binary_unit(struct metric_value *mv);
struct metric_value	*to_binary_metric(double value);
struct metric_value	*to_decimal_metric(double value);
char				*get_metric_value_str(struct metric_value *mv,
														int nb_after_dec_point);

#endif
