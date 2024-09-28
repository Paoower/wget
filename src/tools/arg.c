#include <stdarg.h>

int	arg_ptr_len(va_list args, void *first)
{
	int		size;
	void	*ptr;

	ptr = first;
	size = 0;
	while (ptr) {
		size++;
		ptr = va_arg(args, void *);
	}
	return size;
}
